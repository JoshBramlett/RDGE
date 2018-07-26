#include <rdge/gameobjects/types.hpp>
#include <rdge/util/strings.hpp>

#include <sstream>
#include <cstring> // strrchr

namespace rdge {

std::ostream&
operator<< (std::ostream& os, ActionType value)
{
    return os << rdge::to_string(value);
}

std::string
to_string (ActionType value)
{
    switch (value)
    {
#define CASE(X) case X: return (strrchr(#X, ':') + 1); break;
        CASE(ActionType::NONE)
        CASE(ActionType::DIALOG)
        CASE(ActionType::SCENE_PUSH)
        CASE(ActionType::SCENE_POP)
        CASE(ActionType::SCENE_SWAP)
        default: break;
#undef CASE
    }

    std::ostringstream ss;
    ss << "UNKNOWN[" << static_cast<uint32>(value) << "]";
    return ss.str();
}

bool
try_parse (const std::string& test, ActionType& out)
{
    std::string s = rdge::to_lower(test);
    if      (s == "none")       { out = ActionType::NONE;       return true; }
    else if (s == "dialog")     { out = ActionType::DIALOG;     return true; }
    else if (s == "scene_push") { out = ActionType::SCENE_PUSH; return true; }
    else if (s == "scene_pop")  { out = ActionType::SCENE_POP;  return true; }
    else if (s == "scene_swap") { out = ActionType::SCENE_SWAP; return true; }

    return false;
}

} // namespace rdge
