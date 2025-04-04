#pragma once
#include <memory>
#include <string>

#include "device_facade.h"
#include "device_id_manager.h"
#include "logging.h"

class ToneLogger;

namespace tone::ui {
    struct InputCommands {
        static constexpr std::string ADD = "add";
        static constexpr std::string REMOVE = "remove";
        static constexpr std::string PLAY = "play";
        static constexpr std::string START = "start";
        static constexpr std::string STOP = "stop";
        static constexpr std::string RESET = "reset";
        static constexpr std::string FILE = "file";
    };

    struct InputSubjects {
        static constexpr std::string PLAYER = "player";
    };

    class MainInputProcessor {
        std::shared_ptr<ToneLogger> logger;
        std::shared_ptr<DeviceFacade> device_facade;
        std::shared_ptr<DeviceIdManager> device_id_mapper;

        boost::uuids::uuid add_player(std::string file_name) const;

    public:
        MainInputProcessor(
            std::shared_ptr<DeviceFacade> device_facade,
            std::shared_ptr<DeviceIdManager> device_id_mapper,
            std::shared_ptr<ToneLogger> tone_logger
        );

        void process(std::string input) const;
    };
}
