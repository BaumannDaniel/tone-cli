#include "device_facade.h"

#include <utility>

void tone::DeviceFacade::addPlayer(std::shared_ptr<Player> player) {
    auto player_id = player->getPlayerInfo()->getId();
    player->init();
    players.insert(
        {
            player_id,
            std::move(player)
        });
}

void tone::DeviceFacade::removePlayer(boost::uuids::uuid player_id) {
    if (players.contains(player_id)) {
        players.at(player_id)->unInit();
        players.erase(player_id);
    }
}


void tone::DeviceFacade::startPlayer(boost::uuids::uuid playerId) const {
    if (players.contains(playerId)) {
        players.at(playerId)->start();
    }
}

void tone::DeviceFacade::stopPlayer(boost::uuids::uuid playerId) const {
    if (players.contains(playerId)) {
        players.at(playerId)->stop();
    }
}

void tone::DeviceFacade::changePlayerFile(boost::uuids::uuid player_id, std::string file_name) const {
    if (players.contains(player_id)) {
        players.at(player_id)->changeFile(std::move(file_name));
    }
}


std::vector<std::shared_ptr<tone::PlayerInfo> > tone::DeviceFacade::getPlayersInfo() const {
    auto players_info = std::vector<std::shared_ptr<PlayerInfo> >{};
    for (const auto &player_pair: players) {
        players_info.push_back(player_pair.second->getPlayerInfo());
    }
    return players_info;
}
