#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "config.h" // NOLINT
#include "main_input_processor.h"

#include <boost/uuid/random_generator.hpp>

#include "device_id_manager.h"
#include "player.h"
#include "../util/device_facade_mock.h"
#include "../util/logger_mock.h"

namespace tone::test {
    class MainInputProcessorTest : public testing::Test {
    protected:
        const std::string test_file_1 = "/testPath/test1.mp3";
        const std::string player_1_ui_id = "a";
        const boost::uuids::uuid player_1_device_id = boost::uuids::random_generator()();

        std::shared_ptr<ui::DeviceIdManager> device_id_manager;
        std::shared_ptr<MockLogger> logger;
        std::shared_ptr<MockDeviceFacade> device_facade;
        ui::MainInputProcessor main_input_processor;

        MainInputProcessorTest()
            : device_id_manager(std::make_shared<ui::DeviceIdManager>()),
              logger(std::make_shared<MockLogger>()),
              device_facade(std::make_shared<MockDeviceFacade>()),
              main_input_processor(
                  ui::MainInputProcessor(device_facade, device_id_manager, logger)
              ) {
        }
    };

    TEST_F(MainInputProcessorTest, process$addsPlayerToDeviceFacade) {
        const std::string input = std::format("add player {}", test_file_1);
        const auto mock_device_facade = device_facade.get();
        EXPECT_CALL(
            *mock_device_facade,
            addPlayer(testing::Truly([&](const std::shared_ptr<Player>& player) {
                auto correct_file = player->getPlayerInfo()->getFileName() == test_file_1;
                auto correct_state = player->getDeviceState() == DeviceState::UN_INIT;
                return correct_file && correct_state;
                }))
        ).Times(1);
        main_input_processor.process(input);
    }

    TEST_F(MainInputProcessorTest, process$startsPlayer) {
        device_id_manager->addIdMapping(player_1_ui_id, player_1_device_id, ui::PLAYER);
        const std::string input = std::format("start {}", player_1_ui_id);
        const auto mock_device_facade = device_facade.get();
        EXPECT_CALL(
            *mock_device_facade,
            startPlayer(player_1_device_id)
        );
        main_input_processor.process(input);
    }

    TEST_F(MainInputProcessorTest, process$playsMp3File) {
        const std::string input = std::format("play {}", test_file_1);
        const auto mock_device_facade = device_facade.get();
        boost::uuids::uuid player_device_id;
        testing::InSequence seq;
        EXPECT_CALL(
            *mock_device_facade,
            addPlayer(testing::Truly([&](const std::shared_ptr<Player>& player) {
                    player_device_id = player->getPlayerInfo()->getId();
                    auto correct_file = player->getPlayerInfo()->getFileName() == test_file_1;
                    auto correct_state = player->getDeviceState() == DeviceState::UN_INIT;
                    return correct_file && correct_state;
                    }
                )
            )
        );
        EXPECT_CALL(
            *mock_device_facade,
            startPlayer(testing::Truly([&](const boost::uuids::uuid &id) {
                    return id == player_device_id;
                    }
                )
            )
        );
        main_input_processor.process(input);
    }
}
