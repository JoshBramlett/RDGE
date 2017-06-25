#include <rdge/physics/collision_graph.hpp>

#include <algorithm> // remove_if

namespace rdge {
namespace physics {

namespace {

ContactFilter s_defaultContactFilter;
GraphListener s_defaultGraphListener;


constexpr float VELOCITY_THRESHOLD = 1.f;
constexpr float BAUMGARTE = 0.2f;
constexpr float MAX_LINEAR_CORRECTION = 0.2f;
constexpr float MAX_TRANSLATION = 0.2f;
constexpr float MAX_TRANSLATION_SQAURED = MAX_TRANSLATION * MAX_TRANSLATION;
constexpr float MAX_ROTATION = 0.5f * math::PI;
constexpr float MAX_ROTATION_SQUARED = MAX_ROTATION * MAX_ROTATION;

struct island_solver
{
    struct solver_body_data
    {
        RigidBody* body;
        math::vec2 pos;         //!< body.sweep.pos_n
        math::vec2 linear_vel;  //!< body.linear.velocity
        float      angle;       //!< body.sweep.angle_n
        float      angular_vel; //!< body.angular.velocity
    };

    struct solver_contact_data
    {
        Contact* contact;
        size_t body_index[2];
        float inv_mass[2];
        float inv_mmoi[2];

        struct velocity_constraint_point
        {
            math::vec2 rel_point[2];   //!< manifold.point - world body center
            float normal_impulse = 0.f;
            float tangent_impulse = 0.f;
            float normal_mass;
            float tangent_mass;
            float velocity_bias;
        };

        velocity_constraint_point points[2];
    };

    std::vector<solver_body_data>    body_data;
    std::vector<solver_contact_data> contact_data;

    math::vec2 gravity;
    float dt;
    size_t velocity_iterations = 8;
    size_t position_iterations = 3;

    void init (float delta_time, size_t body_count, size_t contact_count)
    {
        dt = delta_time;

        body_data.reserve(body_count);
        contact_data.reserve(contact_count);
    }

    void add (RigidBody* b)
    {
        // Store positions for continuous collision
        b->sweep.angle_0 = b->sweep.angle_n;
        b->sweep.pos_0 = b->sweep.pos_n;

        solver_body_data data = {
            b,
            b->sweep.pos_n,
            b->linear.velocity,
            b->sweep.angle_n,
            b->angular.velocity
        };

        if (b->GetType() == RigidBodyType::DYNAMIC)
        {
            // Perform initial velocity integration and apply damping

            auto linear_acc = (b->gravity_scale * gravity) +
                              (b->linear.force * b->linear.inv_mass);
            auto angular_acc = (b->angular.torque * b->angular.inv_mmoi);

            data.linear_vel += linear_acc * dt;
            data.angular_vel += angular_acc * dt;

            // From Box2D regarding damping:
            // ODE: dv/dt + c * v = 0
            // Solution: v(t) = v0 * exp(-c * t)
            // Time step: v(t + dt)
            //            = v0 * exp(-c * (t + dt))
            //            = v0 * exp(-c * t) * exp(-c * dt) = v * exp(-c * dt)
            // v2 = exp(-c * dt) * v1
            // Pade approximation:
            // v2 = v1 * 1 / (1 + c * dt)
            data.linear_vel *= 1.f / (1.f + dt * b->linear.damping);
            data.angular_vel *= 1.f / (1.f + dt * b->angular.damping);
        }

        b->solver_index = body_data.size();
        body_data.push_back(data);
    }

    void add (Contact* c)
    {
        solver_contact_data data;
        data.contact = c;
        contact_data.push_back(data);
    }

    void clear (void)
    {
        body_data.clear();
        contact_data.clear();
    }

    void solve_velocity (void)
    {
        for (auto& data : contact_data)
        {
            auto& bdata_a = body_data[data.body_index[0]];
            auto& bdata_b = body_data[data.body_index[1]];

            const auto& mf = data.contact->manifold;
            math::vec2 tangent = mf.normal.perp_ccw();
            float tangent_speed = data.contact->tangent_speed;
            float friction = data.contact->friction;

            for (size_t i = 0; i < mf.count; i++)
            {
                // Solve tangent constraints first b/c non-penetration is more
                // important than friction

                auto& vcp = data.points[i];

                // relative velocity along direction of contact tangent
                math::vec2 vel_a = bdata_a.linear_vel +
                                   (vcp.rel_point[0].perp() * bdata_a.angular_vel);
                math::vec2 vel_b = bdata_b.linear_vel +
                                   (vcp.rel_point[1].perp() * bdata_b.angular_vel);
                float rtv = math::dot(tangent, vel_b - vel_a) - tangent_speed;

                // Compute tangent force
                float lambda = vcp.tangent_mass * (-rtv);

                // clamp the accumulated force
                float max_friction = friction * vcp.normal_impulse;
                float new_impulse = math::clamp(vcp.tangent_impulse + lambda,
                                                -max_friction, max_friction);
                lambda = new_impulse - vcp.tangent_impulse;
                vcp.tangent_impulse = new_impulse;

                // apply contact impulse
                math::vec2 impulse = tangent * lambda;

                bdata_a.linear_vel -= data.inv_mass[0] * impulse;
                bdata_a.angular_vel -= data.inv_mmoi[0] *
                                       math::perp_dot(vcp.rel_point[0], impulse);

                bdata_b.linear_vel += data.inv_mass[1] * impulse;
                bdata_b.angular_vel += data.inv_mmoi[1] *
                                       math::perp_dot(vcp.rel_point[1], impulse);
            }

            for (size_t i = 0; i < mf.count; i++)
            {
                // Solve normal constraints

                auto& vcp = data.points[i];

                // relative velocity along direction of contact normal
                math::vec2 vel_a = bdata_a.linear_vel +
                                   (vcp.rel_point[0].perp() * bdata_a.angular_vel);
                math::vec2 vel_b = bdata_b.linear_vel +
                                   (vcp.rel_point[1].perp() * bdata_b.angular_vel);
                float rnv = math::dot(mf.normal, vel_b - vel_a);

                // Compute normal force
                float lambda = -vcp.normal_mass * (rnv - vcp.velocity_bias);

                // clamp the accumulated force
                float new_impulse = std::max(vcp.normal_impulse + lambda, 0.f);
                lambda = new_impulse - vcp.normal_impulse;
                vcp.normal_impulse = new_impulse;

                // apply contact impulse
                math::vec2 impulse = mf.normal * lambda;

                bdata_a.linear_vel -= data.inv_mass[0] * impulse;
                bdata_a.angular_vel -= data.inv_mmoi[0] *
                                       math::perp_dot(vcp.rel_point[0], impulse);

                bdata_b.linear_vel += data.inv_mass[1] * impulse;
                bdata_b.angular_vel += data.inv_mmoi[1] *
                                       math::perp_dot(vcp.rel_point[1], impulse);
            }
        }
    }

    bool correct_position (void)
    {
        float max_depth = 0.f;

        for (auto& data : contact_data)
        {
            auto& bdata_a = body_data[data.body_index[0]];
            auto& bdata_b = body_data[data.body_index[1]];

            const auto& mf = data.contact->manifold;
            math::vec2 normal = mf.flip_dominant ? -mf.normal : mf.normal;

            for (size_t i = 0; i < mf.count; i++)
            {
                auto& vcp = data.points[i];

                // positional correction
                //Vec2 correction = max( penetration - k_slop, 0.0f ) /
                                  //(A.inv_mass + B.inv_mass) * percent * n
                if (mf.depths[i] > max_depth)
                {
                    max_depth = mf.depths[i];
                }

                float C = math::clamp(BAUMGARTE * (-mf.depths[i] + LINEAR_SLOP),
                                      -MAX_LINEAR_CORRECTION, 0.f);

                float rel_normal_a = math::perp_dot(vcp.rel_point[0], normal);
                float rel_normal_b = math::perp_dot(vcp.rel_point[1], normal);

                // effective mass on the normal
                float enm = data.inv_mass[0] +
                            data.inv_mass[1] +
                            (data.inv_mmoi[0] * math::square(rel_normal_a)) +
                            (data.inv_mmoi[1] * math::square(rel_normal_b));
                float normal_mass = (enm > 0.f) ? (-C / enm) : 0.f;
                auto impulse = normal * normal_mass;

                bdata_a.pos -= data.inv_mass[0] * impulse;
                bdata_a.angle -= data.inv_mmoi[0] *
                                 math::perp_dot(vcp.rel_point[0], impulse);

                bdata_b.pos += data.inv_mass[1] * impulse;
                bdata_b.angle += data.inv_mmoi[1] *
                                 math::perp_dot(vcp.rel_point[1], impulse);
            }
        }

        return max_depth <= LINEAR_SLOP * 3.f;
    }

    void solve (void)
    {
        //b2ContactSolver contactSolver(&contactSolverDef);
        //contactSolver.InitializeVelocityConstraints();

        //for (int32 i = 0; i < step.velocityIterations; ++i)
        //{
            //contactSolver.SolveVelocityConstraints();
        //}

        //contactSolver.StoreImpulses();

        //IntegratePositions();

        //for (int32 i = 0; i < step.positionIterations; ++i)
        //{
            //contactSolver.SolvePositionConstraints();
        //}

        //CopyBuffersBackToBodies();
        //MarkBodiesAsSleeping();

        // !!! contactSolver.InitializeVelocityConstraints();
        for (auto& data : contact_data)
        {
            // populate relevant body data
            const auto body_a = data.contact->fixture_a->body;
            const auto body_b = data.contact->fixture_b->body;

            data.body_index[0] = body_a->solver_index;
            data.inv_mass[0] = body_a->linear.inv_mass;
            data.inv_mmoi[0] = body_a->angular.inv_mmoi;

            data.body_index[1] = body_b->solver_index;
            data.inv_mass[1] = body_b->linear.inv_mass;
            data.inv_mmoi[1] = body_b->angular.inv_mmoi;

            // build the velocity constraint points
            auto& bdata_a = body_data[data.body_index[0]];
            auto& bdata_b = body_data[data.body_index[1]];

            const auto& mf = data.contact->manifold;
            math::vec2 tangent = mf.normal.perp_ccw();
            float restitution = data.contact->restitution;

            for (size_t i = 0; i < mf.count; i++)
            {
                auto& vcp = data.points[i];

                vcp.rel_point[0] = mf.contacts[i] - bdata_a.pos;
                vcp.rel_point[1] = mf.contacts[i] - bdata_b.pos;

                float rel_normal_a = math::perp_dot(vcp.rel_point[0], mf.normal);
                float rel_normal_b = math::perp_dot(vcp.rel_point[1], mf.normal);

                // effective mass on the normal
                float enm = data.inv_mass[0] +
                            data.inv_mass[1] +
                            (data.inv_mmoi[0] * math::square(rel_normal_a)) +
                            (data.inv_mmoi[1] * math::square(rel_normal_b));
                vcp.normal_mass = (enm > 0.f) ? (1.f / enm) : 0.f;

                float rel_tangent_a = math::perp_dot(vcp.rel_point[0], tangent);
                float rel_tangent_b = math::perp_dot(vcp.rel_point[1], tangent);

                // effective mass on the tangent
                float etm = data.inv_mass[0] +
                            data.inv_mass[1] +
                            (data.inv_mmoi[0] * math::square(rel_tangent_a)) +
                            (data.inv_mmoi[1] * math::square(rel_tangent_b));
                vcp.tangent_mass = (etm > 0.f) ? (1.f / etm) : 0.f;

                // relative velocity at contact normal
                math::vec2 rvel_a = bdata_a.linear_vel +
                                    (vcp.rel_point[0].perp() * bdata_a.angular_vel);
                math::vec2 rvel_b = bdata_b.linear_vel +
                                    (vcp.rel_point[1].perp() * bdata_b.angular_vel);
                float rnv = math::dot(mf.normal, rvel_b - rvel_a);

                vcp.velocity_bias = 0.f;
                if (rnv < -VELOCITY_THRESHOLD)
                {
                    vcp.velocity_bias = rnv * -restitution;
                }
            }
        }

        for (size_t iter = 0; iter < velocity_iterations; iter++)
        {
            solve_velocity();
        }

        //for (auto& data : body_data)
        //{
            //auto t = data.linear_vel * dt;
            //if (t.self_dot() > MAX_TRANSLATION_SQAURED)
            //{
                //data.linear_vel *= MAX_TRANSLATION / t.length();
            //}

            //auto r = data.angular_vel * dt;
            //if (math::square(r) > MAX_ROTATION_SQUARED)
            //{
                //data.angular_vel *= MAX_ROTATION / math::abs(r);
            //}

            //data.pos += data.linear_vel * dt;
            //data.angle += data.angular_vel * dt;
        //}

        for (size_t iter = 0; iter < position_iterations; iter++)
        {
            if (correct_position())
            {
                break;
            }
        }

        for (auto& data : body_data)
        {
            auto body = data.body;
            body->sweep.pos_n = data.pos + (data.linear_vel * dt);
            body->sweep.angle_n = data.angle + (data.angular_vel * dt);
            body->linear.velocity = data.linear_vel;
            body->angular.velocity = data.angular_vel;

            body->world_transform.set_angle(body->sweep.angle_n);
            body->world_transform.pos = body->sweep.pos_n -
                                        body->world_transform.rot.rotate(body->sweep.local_center);
        }
    }

} s_island;

} // anonymous namespace

CollisionGraph::CollisionGraph (const math::vec2& g)
    : gravity(g)
    , custom_filter(&s_defaultContactFilter)
    , listener(&s_defaultGraphListener)
{
    m_dirtyProxies.reserve(128);

    // TODO this will be a problem if there are multiple instances
    s_island.gravity = g;
}

CollisionGraph::~CollisionGraph (void) noexcept
{
    bodies.for_each([=](auto* b) {
        block_allocator.Delete<RigidBody>(b);
    });

    contacts.for_each([=](auto* c) {
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
    bodies.push_back(result);

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

    body->fixtures.for_each([=](auto* f) {
        body->DestroyFixture(f);
    });

    body->contact_edges.for_each([=](auto* edge) {
        DestroyContact(edge->contact);
    });

    bodies.remove(body);
    block_allocator.Delete<RigidBody>(body);
}

void
CollisionGraph::Step (float dt)
{
    m_flags |= LOCKED;

    // 1) update contact list
    {
        // find new contacts for any added proxies
        if (!m_dirtyProxies.empty())
        {
            auto pairs = m_tree.Query<fixture_proxy>(m_dirtyProxies);
            for (auto& p : pairs)
            {
                CreateContact(p.first, p.second);
            }

            m_dirtyProxies.clear();
        }

        // remove all contacts that are not colliding
        PurgeContacts();
    }

    // 2) integration and contact solving
    {
        // clear island flags

        // TODO we iterate through the body list three times in this step.  Clean this up.
        bodies.for_each([](auto* body) {
            body->m_flags &= ~RigidBody::ON_ISLAND;
        });

        contacts.for_each([](auto* contact) {
            contact->m_flags &= ~Contact::ON_ISLAND;
        });

        static std::vector<RigidBody*> body_stack;
        body_stack.reserve(bodies.count);

        s_island.init(dt, bodies.count, contacts.count);
        bodies.for_each([&](auto* body) {
            if ((body->m_flags & RigidBody::ON_ISLAND) ||
                !body->IsSimulating() ||
                !body->IsAwake())
            {
                return;
            }

            body_stack.clear();
            s_island.clear();

            body_stack.push_back(body);
            while (!body_stack.empty())
            {
                RigidBody* b = body_stack.back();
                body_stack.pop_back();

                s_island.add(b);
                body->m_flags |= RigidBody::ON_ISLAND;

                // Box2D: Make sure the body is awake (without resetting sleep timer).
                // TODO ??? Why?
                body->m_flags |= RigidBody::AWAKE;

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
                    s_island.add(c);

                    if ((edge->other->m_flags & RigidBody::ON_ISLAND) == 0)
                    {
                        body_stack.push_back(edge->other);
                    }
                });
            }

            s_island.solve();

            // TODO Box2D iterates through the island and removes the island from
            //      all static bodies.  I think this step can be removed simply
            //      by having an OnIsland getter return false if static
        });

        bodies.for_each([](auto* body) {
            // If a body was not in an island then it did not move.
            if (((body->m_flags & RigidBody::ON_ISLAND) == 0) ||
                (body->m_type == RigidBodyType::STATIC))
            {
                return;
            }

            body->SyncFixtures();
        });
    }

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
    contacts.push_back(contact);
    body_a->contact_edges.push_back(&contact->edge_a);
    body_b->contact_edges.push_back(&contact->edge_b);

    if (!contact->fixture_a->IsSensor() &&
        !contact->fixture_b->IsSensor())
    {
        body_a->WakeUp();
        body_b->WakeUp();
    }

    std::cout << "Contact created" << std::endl;
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

    contacts.remove(contact);
    body_a->contact_edges.remove(&contact->edge_a);
    body_b->contact_edges.remove(&contact->edge_b);

    block_allocator.Delete<Contact>(contact);

    std::cout << "Contact destroyed" << std::endl;
}

void
CollisionGraph::PurgeContacts (void)
{
    contacts.for_each([this](auto* contact) {
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

    //std::cout << "register: " << m_tree.Dump() << std::endl;

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

    //std::cout << "unregister: " << m_tree.Dump() << std::endl;
}

void
CollisionGraph::MoveProxy (const fixture_proxy* proxy, const math::vec2& displacement)
{
    m_tree.MoveProxy(proxy->handle, proxy->box, displacement);
    m_dirtyProxies.push_back(proxy->handle);

    //std::cout << "move: " << m_tree.Dump() << std::endl;
}

void
CollisionGraph::TouchProxy (const fixture_proxy* proxy)
{
    m_dirtyProxies.push_back(proxy->handle);
}

} // namespace physics
} // namespace rdge
