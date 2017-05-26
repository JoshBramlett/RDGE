#include <rdge/physics/contact.hpp>
#include <rdge/physics/fixture.hpp>
#include <rdge/physics/rigid_body.hpp>

namespace rdge {
namespace physics {

Contact::Contact (Fixture* a, Fixture* b)
    : fixture_a(a)
    , fixture_b(b)
{

}

Contact::~Contact (void) noexcept
{

}

void
Contact::Update (ContactListener* listener)
{
    bool was_touching = IsTouching();
    bool is_touching = false;

    if (listener)
    {
        if (was_touching == false && is_touching == true)
        {
            listener->BeginContact(this);
        }

        if (was_touching == true && is_touching == false)
        {
            listener->EndContact(this);
        }
    }
}

bool
ContactFilter::ShouldCollide (Fixture* a, Fixture* b) const noexcept
{
    if (a->filter.group && a->filter.group == b->filter.group)
    {
        return (a->filter.group > 0);
    }

    return (a->filter.mask & b->filter.category) &&
           (b->filter.mask & a->filter.category);
}

} // namespace physics
} // namespace rdge
