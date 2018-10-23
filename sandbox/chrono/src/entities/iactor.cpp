#include <chrono/entities/iactor.hpp>
#include <chrono/types.hpp>

#include <rdge/physics/rigid_body.hpp>
#include <rdge/physics/fixture.hpp>
#include <rdge/util/strings.hpp>
#include <rdge/debug/assert.hpp>

#include <sstream>
#include <cstring> // strrchr

using namespace rdge;

/* static */ IActor*
IActor::Extract (const fixture_user_data* user_data)
{
    RDGE_ASSERT(user_data);
    RDGE_ASSERT(user_data->fixture);

    auto body = user_data->fixture->body;
    RDGE_ASSERT(body);
    RDGE_ASSERT(body->user_data);

    return static_cast<IActor*>(body->user_data);
}

namespace perch {

std::ostream&
operator<< (std::ostream& os, ActorType value)
{
    return os << perch::to_string(value);
}

std::string
to_string (ActorType value)
{
    switch (value)
    {
#define CASE(X) case X: return (strrchr(#X, ':') + 1); break;
        CASE(ActorType::NONE)
        CASE(ActorType::PLAYER)
        CASE(ActorType::STATIC)
        CASE(ActorType::SIGN)
        CASE(ActorType::CONTAINER)
        CASE(ActorType::DEBUTANTE)
        default: break;
#undef CASE
    }

    std::ostringstream ss;
    ss << "UNKNOWN[" << static_cast<uint32>(value) << "]";
    return ss.str();
}

bool
try_parse (const std::string& test, ActorType& out)
{
    std::string s = rdge::to_lower(test);
    if      (s == "none")      { out = ActorType::NONE;      return true; }
    else if (s == "player")    { out = ActorType::PLAYER;    return true; }
    else if (s == "static")    { out = ActorType::STATIC;    return true; }
    else if (s == "sign")      { out = ActorType::SIGN;      return true; }
    else if (s == "container") { out = ActorType::CONTAINER; return true; }
    else if (s == "debutante") { out = ActorType::DEBUTANTE; return true; }

    return false;
}

} // namespace perch
