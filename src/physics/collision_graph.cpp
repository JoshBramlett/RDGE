#include <rdge/physics/collision_graph.hpp>
#include <rdge/physics/joints/revolute_joint.hpp>

#ifdef RDGE_DEBUG
#include <rdge/debug/renderer.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/physics/aabb.hpp>
#include <rdge/util/profiling.hpp>
#include <rdge/util/memory/alloc.hpp>

#include <imgui/imgui.h>
#endif

#include <algorithm> // remove_if

namespace rdge {
namespace physics {

namespace {

ContactFilter s_defaultContactFilter;
GraphListener s_defaultGraphListener;

} // anonymous namespace

CollisionGraph::CollisionGraph (const math::vec2& g)
    : custom_filter(&s_defaultContactFilter)
    , listener(&s_defaultGraphListener)
    , m_solver(&m_step)
    , m_flags(CLEAR_FORCES)
{
    m_dirtyProxies.reserve(128);
    m_solver.gravity = g;
}

CollisionGraph::~CollisionGraph (void) noexcept
{
    m_bodies.for_each([=](auto* b) {
        block_allocator.Delete<RigidBody>(b);
    });

    m_contacts.for_each([=](auto* c) {
        block_allocator.Delete<Contact>(c);
    });
}

RigidBody*
CollisionGraph::CreateBody (const rigid_body_profile& profile)
{
    if (IsLocked())
    {
        SDL_assert(false);
        return nullptr;
    }

    RigidBody* result = block_allocator.New<RigidBody>(profile, this);
    m_bodies.push_back(result);

    return result;
}

void
CollisionGraph::DestroyBody (RigidBody* body)
{
    if (IsLocked())
    {
        SDL_assert(false);
        return;
    }

    body->joint_edges.for_each([=](auto* edge) {
        DestroyJoint(edge->joint);
    });

    body->contact_edges.for_each([=](auto* edge) {
        DestroyContact(edge->contact);
    });

    body->fixtures.for_each([=](auto* f) {
        body->DestroyFixture(f);
    });

    m_bodies.remove(body);
    block_allocator.Delete<RigidBody>(body);
}

RevoluteJoint*
CollisionGraph::CreateRevoluteJoint (RigidBody* a, RigidBody* b, math::vec2 anchor)
{
    if (IsLocked())
    {
        SDL_assert(false);
        return nullptr;
    }

    RevoluteJoint* result = block_allocator.New<RevoluteJoint>(a, b, anchor);
    m_joints.push_back(result);
    a->joint_edges.push_back(&result->edge_a);
    b->joint_edges.push_back(&result->edge_b);

    return result;
}

void
CollisionGraph::DestroyJoint (BaseJoint* joint)
{
    if (IsLocked())
    {
        SDL_assert(false);
    }

    RigidBody* body_a = joint->body_a;
    RigidBody* body_b = joint->body_b;

    body_a->WakeUp();
    body_b->WakeUp();

    // If the joint was preventing collisions, flag fixtures for filtering
    if (!joint->ShouldCollide())
    {
        body_a->contact_edges.for_each([=](auto* edge) {
            if (edge->other == body_b)
            {
                edge->contact->fixture_a->FlagFilterDirty();
                edge->contact->fixture_b->FlagFilterDirty();
            }
        });
    }

    m_joints.remove(joint);
    body_a->joint_edges.remove(&joint->edge_a);
    body_b->joint_edges.remove(&joint->edge_b);

    // TODO update once more joints are added
    block_allocator.Delete<RevoluteJoint>(static_cast<RevoluteJoint*>(joint));
}

void
CollisionGraph::Step (float dt)
{
    m_flags |= LOCKED;

    SDL_assert(dt > 0.f);
    m_step.dt = dt;
    m_step.inv = 1.f / dt;
    m_step.ratio = m_step.inv_0 * dt;

    // 1) update contact list
    {
        // find new contacts for any added proxies
        if (!m_dirtyProxies.empty())
        {
            ScopeProfiler<> p(&debug_profile.create_contacts);
            auto pairs = m_tree.Query<fixture_proxy>(m_dirtyProxies);
            for (auto& p : pairs)
            {
                CreateContact(p.first, p.second);
            }

            m_dirtyProxies.clear();
        }

        // remove all contacts that are not colliding
        ScopeProfiler<> p(&debug_profile.purge_contacts);
        PurgeContacts();
    }

    {
        // NOTE: Island flags must be reset prior to solving.  To avoid another
        //       iteration to reset the flags, they are performed at:
        //   - bodies reset at the end of the step
        //   - contacts reset during contact purging
        //   - joints done here

        // TODO Remove this when physics engine gets more mileage.

#ifdef RDGE_DEBUG
        m_bodies.for_each([](auto* body) {
            SDL_assert((body->m_flags & RigidBody::ON_ISLAND) == 0);
        });

        m_contacts.for_each([](auto* contact) {
            SDL_assert((contact->m_flags & Contact::ON_ISLAND) == 0);
        });
#endif

        m_joints.for_each([](auto* joint) {
            joint->m_flags &= ~BaseJoint::ON_ISLAND;
        });
    }

    // 2) integration and contact solving
    {
        ScopeProfiler<> p(&debug_profile.solve);
        static std::vector<RigidBody*> body_stack;
        body_stack.reserve(m_bodies.count);

        m_solver.Initialize(m_bodies.count, m_contacts.count, m_joints.count);
        m_bodies.for_each([&](auto* body) {
            if ((body->m_flags & RigidBody::ON_ISLAND) ||
                !body->IsSimulating() ||
                !body->IsAwake())
            {
                return;
            }

            body_stack.clear();
            m_solver.Clear();

            body->m_flags |= RigidBody::ON_ISLAND;
            body_stack.push_back(body);
            while (!body_stack.empty())
            {
                RigidBody* b = body_stack.back();
                body_stack.pop_back();

                m_solver.Add(b);

                // to keep islands small do not propogate past static bodies
                if (b->m_type == RigidBodyType::STATIC)
                {
                    continue;
                }

                b->contact_edges.for_each([&](auto* edge) {
                    Contact* c = edge->contact;

                    // TODO could be simplified to m_flags != 0 (except sensor test),
                    //      but for future proofing should remain as is.  Look into
                    //      IsTouching to see where it's used.
                    if ((c->m_flags & Contact::ON_ISLAND) ||
                        !c->IsTouching() ||
                        !c->IsEnabled() ||
                        (c->fixture_a->IsSensor() || c->fixture_b->IsSensor()))
                    {
                        return;
                    }

                    c->m_flags |= Contact::ON_ISLAND;
                    m_solver.Add(c);

                    if ((edge->other->m_flags & RigidBody::ON_ISLAND) == 0)
                    {
                        edge->other->m_flags |= RigidBody::ON_ISLAND;
                        body_stack.push_back(edge->other);
                    }
                });

                b->joint_edges.for_each([&](auto* edge) {
                    BaseJoint* j = edge->joint;

                    // TODO could be simplified to m_flags != 0 (except sensor test),
                    //      but for future proofing should remain as is.  Look into
                    //      IsTouching to see where it's used.
                    if (j->m_flags & BaseJoint::ON_ISLAND)
                    {
                        return;
                    }

                    if (!j->body_a->IsSimulating() || !j->body_b->IsSimulating())
                    {
                        return;
                    }

                    j->m_flags |= BaseJoint::ON_ISLAND;
                    m_solver.Add(j);

                    if ((edge->other->m_flags & RigidBody::ON_ISLAND) == 0)
                    {
                        edge->other->m_flags |= RigidBody::ON_ISLAND;
                        body_stack.push_back(edge->other);
                    }
                });
            }

            m_solver.Solve();
            m_solver.ProcessPostSolve(*this);
        });
    }

    {
        ScopeProfiler<> p(&debug_profile.synchronize);
        m_bodies.for_each([=](auto* body) {
            // If a body was not in an island then it did not move.
            if (body->m_flags & RigidBody::ON_ISLAND)
            {
                if (body->m_type != RigidBodyType::STATIC)
                {
                    body->SyncFixtures();

                    if (m_flags & CLEAR_FORCES)
                    {
                        body->linear.force = { 0.f, 0.f };
                        body->angular.torque = 0.f;
                    }
                }

                // Remove flag for the next iteration
                body->m_flags &= ~RigidBody::ON_ISLAND;
            }
        });
    }

    m_step.dt_0 = m_step.dt;
    m_step.inv_0 = m_step.inv;

    m_flags &= ~LOCKED;
}

void
CollisionGraph::CreateContact (fixture_proxy* a, fixture_proxy* b)
{
    RigidBody* body_a = a->fixture->body;
    RigidBody* body_b = b->fixture->body;

    if (!body_a->ShouldCollide(body_b))
    {
        return;
    }

    if (custom_filter && !custom_filter->ShouldCollide(a->fixture, b->fixture))
    {
        return;
    }

    if (body_a->HasEdge(a->fixture, b->fixture))
    {
        return;
    }

    Contact* contact = block_allocator.New<Contact>(a->fixture, b->fixture);
    m_contacts.push_back(contact);
    body_a->contact_edges.push_back(&contact->edge_a);
    body_b->contact_edges.push_back(&contact->edge_b);

    if (!contact->fixture_a->IsSensor() &&
        !contact->fixture_b->IsSensor())
    {
        body_a->WakeUp();
        body_b->WakeUp();
    }
}

void
CollisionGraph::DestroyContact (Contact* contact)
{
    RigidBody* body_a = contact->fixture_a->body;
    RigidBody* body_b = contact->fixture_b->body;

    if (contact->IsTouching())
    {
        if (listener)
        {
            listener->OnContactEnd(contact);
        }

        if (!contact->fixture_a->IsSensor() &&
            !contact->fixture_b->IsSensor())
        {
            body_a->WakeUp();
            body_b->WakeUp();
        }
    }

    m_contacts.remove(contact);
    body_a->contact_edges.remove(&contact->edge_a);
    body_b->contact_edges.remove(&contact->edge_b);

    block_allocator.Delete<Contact>(contact);
}

void
CollisionGraph::PurgeContacts (void)
{
    m_contacts.for_each([this](auto* contact) {
        contact->m_flags &= ~Contact::ON_ISLAND;

        Fixture* a = contact->fixture_a;
        Fixture* b = contact->fixture_b;
        if (a->IsFilterDirty() || b->IsFilterDirty())
        {
            if (a->body->ShouldCollide(b->body) == false)
            {
                DestroyContact(contact);
                return;
            }

            if (custom_filter && custom_filter->ShouldCollide(a, b) == false)
            {
                DestroyContact(contact);
                return;
            }

            a->FlagFilterClean();
            b->FlagFilterClean();
        }

        // skip intersecion tests for sleeping bodies
        if (!a->body->IsAwake() && !b->body->IsAwake())
        {
            return;
        }

        // purge non-intersecting contacts.  check is on the enlarged AABBs
        if (!m_tree.Intersects(a->proxy->handle, b->proxy->handle))
        {
            DestroyContact(contact);
            return;
        }

        contact->Update(listener);
    });
}

int32
CollisionGraph::RegisterProxy (fixture_proxy* proxy)
{
    int32 handle = m_tree.CreateProxy(proxy->box, proxy);
    m_dirtyProxies.push_back(handle);

    return handle;
}

void
CollisionGraph::UnregisterProxy (const fixture_proxy* proxy)
{
    int32 handle = proxy->handle;
    if (handle == fixture_proxy::INVALID_HANDLE)
    {
        return;
    }

    m_tree.DestroyProxy(handle);
    m_dirtyProxies.erase(std::remove_if(m_dirtyProxies.begin(),
                                        m_dirtyProxies.end(),
                                        [=](int32 h) { return h == handle; }));
}

void
CollisionGraph::MoveProxy (const fixture_proxy* proxy, const math::vec2& displacement)
{
    m_tree.MoveProxy(proxy->handle, proxy->box, displacement);
    m_dirtyProxies.push_back(proxy->handle);
}

void
CollisionGraph::TouchProxy (const fixture_proxy* proxy)
{
    m_dirtyProxies.push_back(proxy->handle);
}

#ifdef RDGE_DEBUG
void
CollisionGraph::Debug_UpdateWidget (bool* p_open)
{
    if (p_open && !*p_open)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(210.f, 410.f), ImGuiSetCond_FirstUseEver);
    if (!ImGui::Begin("Physics", p_open))
    {
        ImGui::End();
        return;
    }

    ImGui::Text("Graph");
    ImGui::Spacing();
    ImGui::Indent(15.f);
    ImGui::Text("bodies:   %zu", m_bodies.size());
    ImGui::Text("contacts: %zu", m_contacts.size());
    ImGui::Unindent(15.f);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("SmallBlockAllocator");
    ImGui::Spacing();
    ImGui::Indent(15.f);
    ImGui::Text("claimed:         %llu b", this->block_allocator.usage.claimed);
    ImGui::Text("slack:           %llu b", this->block_allocator.usage.slack);
    ImGui::Text("large_allocs:    %zu", this->block_allocator.usage.large_allocs);
    ImGui::Unindent(15.f);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("BVH Tree");
    ImGui::Spacing();
    ImGui::Indent(15.f);
    ImGui::Text("height:          %d", m_tree.Height());
    ImGui::Text("nodes:           %zu", m_tree.m_nodes.size());
    ImGui::Unindent(15.f);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Profiling (us)");
    ImGui::Spacing();
    ImGui::Indent(15.f);
    ImGui::Text("create contacts: %lld", this->debug_profile.create_contacts);
    ImGui::Text("purge contacts:  %lld", this->debug_profile.purge_contacts);
    ImGui::Text("solve:           %lld", this->debug_profile.solve);
    ImGui::Text("synchronize:     %lld", this->debug_profile.synchronize);
    ImGui::Text("---------------------");
    ImGui::Text("total:           %lld", this->debug_profile.create_contacts +
                                         this->debug_profile.purge_contacts +
                                         this->debug_profile.solve +
                                         this->debug_profile.synchronize);
    ImGui::Unindent(15.f);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    bool prevent_sleep = IsSleepPrevented();
    ImGui::Text("Properties");
    ImGui::Spacing();
    ImGui::Indent(15.f);
    ImGui::Checkbox("Prevent Sleep", &prevent_sleep);
    ImGui::Unindent(15.f);

    if (prevent_sleep)
    {
        m_flags |= PREVENT_SLEEP;
    }
    else
    {
        m_flags &= ~PREVENT_SLEEP;
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Debug Drawing");
    ImGui::Spacing();
    ImGui::Indent(15.f);
    ImGui::Checkbox("Show Fixtures", &this->debug_draw_fixtures);
    ImGui::Checkbox("Show Proxy AABBs", &this->debug_draw_proxy_aabbs);
    ImGui::Checkbox("Show Joints", &this->debug_draw_joints);
    ImGui::Checkbox("Show Center of Mass", &this->debug_draw_center_of_mass);
    ImGui::Checkbox("Show BVH Nodes", &this->debug_draw_bvh_nodes);
    ImGui::Unindent(15.f);

    ImGui::End();
}

void
CollisionGraph::Debug_Draw (float pixel_ratio)
{
    if (this->debug_draw_bvh_nodes)
    {
        m_tree.DebugDraw(pixel_ratio);
    }

    if (this->debug_draw_fixtures ||
        this->debug_draw_proxy_aabbs ||
        this->debug_draw_center_of_mass)
    {
        m_bodies.for_each([=](auto* body) {
            body->fixtures.for_each([=](auto* f) {
                if (this->debug_draw_fixtures)
                {
                    if (!body->IsSimulating())
                    {
                        debug::DrawWireFrame(f, color(127, 127, 76), pixel_ratio);
                    }
                    else if (body->GetType() == RigidBodyType::STATIC)
                    {
                        debug::DrawWireFrame(f, color(127, 230, 127), pixel_ratio);
                    }
                    else if (body->GetType() == RigidBodyType::KINEMATIC)
                    {
                        debug::DrawWireFrame(f, color(127, 127, 230), pixel_ratio);
                    }
                    if (!body->IsAwake())
                    {
                        debug::DrawWireFrame(f, color(152, 152, 152), pixel_ratio);
                    }
                    else
                    {
                        debug::DrawWireFrame(f, color(230, 178, 178), pixel_ratio);
                    }
                }

                if (this->debug_draw_proxy_aabbs)
                {
                    debug::DrawWireFrame(f->proxy->box, color(230, 76, 230), pixel_ratio);
                }

                if (this->debug_draw_center_of_mass)
                {

                }
            });
        });

        if (this->debug_draw_joints)
        {
            m_joints.for_each([=](auto* joint) {
                debug::DrawLine(joint->body_a->GetPosition(), joint->AnchorA());
                debug::DrawLine(joint->body_b->GetPosition(), joint->AnchorB());
            });
        }
    }
}
#endif

} // namespace physics
} // namespace rdge
