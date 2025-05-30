#include <ftxui/component/component.hpp>
#include <utility>
#include <boost/algorithm/string.hpp>

#include "player_component.h"
#include "formatting.h"


namespace tone::ui {
    class PlayerComponentBase : public ftxui::ComponentBase {
        std::string ui_id;
        std::shared_ptr<IPlayerInfo> player_info;

    public:
        PlayerComponentBase(
            std::string ui_id,
            std::shared_ptr<IPlayerInfo> player_info
        );

        ftxui::Element Render() override;
    };
}

tone::ui::PlayerComponentBase::PlayerComponentBase(
    std::string ui_id,
    std::shared_ptr<IPlayerInfo> player_info
) : ui_id(std::move(ui_id)),
    player_info(std::move(player_info)) {
}

ftxui::Element tone::ui::PlayerComponentBase::Render() {
    const auto formatted_player_id = std::format("Player {} ", boost::to_upper_copy(ui_id));
    const auto formatted_player_file_name = std::format(" {} ", player_info->getFileName());
    const auto player_sample_rate = player_info->getSampleRate();
    const auto file_length_seconds = player_info->getNumberOfPcmFrames() / player_sample_rate;
    const auto current_file_position = player_info->getCurrentPcmFrameNumber() / player_sample_rate;
    const auto show_hours = file_length_seconds >= 60 * 60;
    const auto formatted_position = secondsToDisplayFormat(current_file_position, show_hours);
    const auto formatted_length = secondsToDisplayFormat(file_length_seconds, show_hours);
    const auto formatted_progress = std::format(" {} / {}", formatted_position, formatted_length);
    return vbox(
               hbox(
                   ftxui::text(formatted_player_id),
                   ftxui::separator(),
                   ftxui::text(formatted_player_file_name),
                   ftxui::separator(),
                   ftxui::text(formatted_progress)
               )
           ) | ftxui::border;
}

ftxui::Component tone::ui::createPlayerComponent(
    const std::string& ui_id,
    const std::shared_ptr<IPlayerInfo>& player_info
) {
    return ftxui::Make<PlayerComponentBase>(ui_id, player_info);
}
