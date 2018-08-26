#include <chrono/scenes/util/contact_handler.hpp>
#include <chrono/util/tilemap_helpers.hpp>
#include <chrono/entities/player.hpp>

#include <rdge/physics/contact.hpp>

namespace {

bool
SortToPlayer (fixture_user_data** a, fixture_user_data** b)
{
    if (*a == nullptr || *b == nullptr)
    {
        return false;
    }
    else if (a->type & fixture_user_data_player_sensor_all)
    {
        return true;
    }
    else if (b->type & fixture_user_data_player_sensor_all)
    {
        std::swap(*a, *b);
        return true;
    }

    return false;
}

} // anonymous namespace

void
ProcessContactStart (rdge::physics::Contact* contact)
{
    if (c->HasSensor())
    {
        auto child = static_cast<fixture_user_data*>(c->fixture_a->user_data);
        auto sibling = static_cast<fixture_user_data*>(c->fixture_b->user_data);
        if (SortToPlayer(child, sibling))
        {
            if (sibling->type & fixture_user_data_action_trigger)
            {
                auto& trigger = sibling->action_trigger;
                if (trigger.invoke_required)
                {
                    Player* player = Player::Extract(child);
                    player->pending_actions.Add(c, child, sibling);
                }
                else
                {
                    QueueCustomEvent(g_game.custom_events[trigger.action_type], 0);
                }
            }
        }
    }
}

void
ProcessContactEnd (rdge::physics::Contact* contact)
{
    if (c->HasSensor())
    {
        auto child = static_cast<fixture_user_data*>(c->fixture_a->user_data);
        auto sibling = static_cast<fixture_user_data*>(c->fixture_b->user_data);
        if (SortToPlayer(child, sibling))
        {
            if (sibling->type & fixture_user_data_action_trigger)
            {
                auto& trigger = sibling->action_trigger;
                if (trigger.invoke_required)
                {
                    Player* player = Player::Extract(child);
                    player->pending_actions.Remove(c);
                }
            }
        }
    }
}

