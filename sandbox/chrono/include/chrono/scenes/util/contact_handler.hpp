#pragma once

namespace rdge {
namespace physics {
class Contact;
} // namespace physics
} // namespace rdge

void ProcessContactStart (rdge::physics::Contact* contact);
void ProcessContactEnd (rdge::physics::Contact* contact);
