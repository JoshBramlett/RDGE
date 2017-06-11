#include "test.hpp"

#include <rdge/assets.hpp>
#include <rdge/system.hpp>
#include <rdge/util.hpp>

#include <rdge/debug.hpp>

#include <SDL_assert.h>

using namespace rdge;
using namespace rdge::math;
using namespace rdge::physics;

TestScene::TestScene (void)
    : collision_graph({ 0.f, 9.8f })
{

}

void
TestScene::Initialize (void)
{ }

void
TestScene::Terminate (void)
{ }

void
TestScene::Hibernate (void)
{ }

void
TestScene::Activate (void)
{ }

void
TestScene::OnEvent (const Event& event)
{
    Unused(event);
}

void
TestScene::OnUpdate (const delta_time& dt)
{
    Unused(dt);
}

void
TestScene::OnRender (void)
{
    polygon::PolygonData data;
    data[0] = {0.f, 0.f};
    data[1] = {150.f, 300.f};
    data[2] = {0.f, 300.f};
    auto p = polygon(data, 3);
    debug::DrawWireFrame(p);

    //debug::DrawWireFrame(circle(vec2(100.f, 100.f), 50.f), color::BLUE);
    //auto c = circle(vec2(100.f, 100.f), 50.f);
    //debug::DrawWireFrame(c);

    math::vec2 axis({ 0.707106781187f, 0.707106781187f });
    math::vec2 trans = { -200.f, 200.f };
    auto proj = p.project(axis);
    auto start = axis * proj.x;
    auto end = axis * proj.y;

    //start.x -= 200.f;
    //start.y += 200.f;
    //end.x -= 200.f;
    //end.y += 200.f;
    //debug::DrawLine(start, end);
    debug::DrawLine(trans + start, trans + end);
    //std::cout << "proj=" << proj << std::endl
              //<< "  start=" << (d * proj.x) << std::endl
              //<< "  end=" << (d * proj.y) << std::endl;


    camera.Translate({ 0.f, 0.f });
    camera.Update();
    debug::SetProjection(camera.combined);
}
