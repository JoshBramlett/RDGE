#pragma once

//!@{ Forward declarations
namespace rdge {
namespace physics {
class Contact;
} // namespace physics
} // namespace rdge
//!@}

namespace perch {

//!@{ Collision contact processing
void ProcessContactStart (rdge::physics::Contact* contact);
void ProcessContactEnd (rdge::physics::Contact* contact);
//!@}

} // namespace perch
