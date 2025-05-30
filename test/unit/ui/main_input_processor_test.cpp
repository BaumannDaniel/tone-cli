#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <boost/uuid/random_generator.hpp>

#include "config.h" // NOLINT
#include "main_input_processor.h"
#include "device_id_manager.h"
#include "player.h"
#include "../util/app_state_holder_mock.h"

#include "../util/device_repository_mock.h"
#include "../util/logger_mock.h"

/**
 * NOTE: Use parameterized tests to test input sanitation, when resolved:
 * https://youtrack.jetbrains.com/issue/CPP-36051/Google-test-TESTP-in-CLion-Nova-No-run-button
 */
namespace tone::test {
    class MainInputProcessorTest : public testing::Test {
    protected:
        const std::string test_file_1 = "/testPath/test1.mp3";
        const std::string player_1_ui_id = "a";
        const boost::uuids::uuid player_1_device_id = boost::uuids::random_generator()();

        std::shared_ptr<ui::DeviceIdManager> device_id_manager;
        std::shared_ptr<MockLogger> logger;
        std::shared_ptr<MockDeviceRepository> device_repository;
        std::shared_ptr<MockAppStateHolder> app_state_holder;
        ui::MainInputProcessor main_input_processor;

        MainInputProcessorTest()
            : device_id_manager(std::make_shared<ui::DeviceIdManager>()),
              logger(std::make_shared<MockLogger>()),
              device_repository(std::make_shared<MockDeviceRepository>()),
              app_state_holder(std::make_shared<MockAppStateHolder>()),
              main_input_processor(
                  ui::MainInputProcessor(device_repository, device_id_manager, app_state_holder, logger)
              ) {
        }
    };

    TEST_F(MainInputProcessorTest, process$invokesExitOnAppStateHolder) {
        const std::string input = "exit";
        const auto mock_app_state_holder = app_state_holder.get();
        EXPECT_CALL(
            *mock_app_state_holder,
            exit
        ).Times(1);
        main_input_processor.process(input);
    }

    TEST_F(MainInputProcessorTest, process$addsPlayerToDeviceFacade) {
        const std::string input = std::format("add player {}", test_file_1);
        const auto mock_device_repository = device_repository.get();
        EXPECT_CALL(
            *mock_device_repository,
            addPlayer(testing::Truly([&](const std::shared_ptr<IPlayer>& player) {
                auto correct_file = player->getPlayerInfo()->getFileName() == test_file_1;
                auto correct_state = player->getDeviceState() == DeviceState::UN_INIT;
                return correct_file && correct_state;
                }))
        ).Times(1);
        main_input_processor.process(input);
    }

    TEST_F(MainInputProcessorTest, process$addsRecorderToDeviceFacade) {
        const std::string input = std::format("add recorder {}", test_file_1);
        const auto mock_device_repository = device_repository.get();
        EXPECT_CALL(
            *mock_device_repository,
            addRecorder(testing::Truly([&](const std::shared_ptr<IRecorder>& player) {
                auto correct_file = player->getRecorderInfo()->getFileName() == test_file_1;
                return correct_file;
                }))
        ).Times(1);
        main_input_processor.process(input);
    }

    TEST_F(MainInputProcessorTest, process$startsPlayer) {
        device_id_manager->addIdMapping(player_1_ui_id, player_1_device_id, ui::PLAYER);
        const std::string input = std::format("start {}", player_1_ui_id);
        const auto mock_device_repository = device_repository.get();
        EXPECT_CALL(
            *mock_device_repository,
            startDevice(player_1_device_id)
        );
        main_input_processor.process(input);
    }

    TEST_F(MainInputProcessorTest, process$playsMp3File) {
        const std::string input = std::format("play {}", test_file_1);
        const auto mock_device_repository = device_repository.get();
        boost::uuids::uuid player_device_id;
        testing::InSequence seq;
        EXPECT_CALL(
            *mock_device_repository,
            addPlayer(testing::Truly([&](const std::shared_ptr<IPlayer>& player) {
                    player_device_id = player->getPlayerInfo()->getId();
                    auto correct_file = player->getPlayerInfo()->getFileName() == test_file_1;
                    auto correct_state = player->getDeviceState() == DeviceState::UN_INIT;
                    return correct_file && correct_state;
                    }
                )
            )
        );
        EXPECT_CALL(
            *mock_device_repository,
            startDevice(testing::Truly([&](const boost::uuids::uuid &id) {
                    return id == player_device_id;
                    }
                )
            )
        );
        main_input_processor.process(input);
    }

    TEST_F(MainInputProcessorTest, process$stopsPlayer) {
        device_id_manager->addIdMapping(player_1_ui_id, player_1_device_id, ui::PLAYER);
        const std::string input = std::format("stop {}", player_1_ui_id);
        const auto mock_device_repository = device_repository.get();
        EXPECT_CALL(
            *mock_device_repository,
            stopDevice(player_1_device_id)
        );
        main_input_processor.process(input);
    }

    TEST_F(MainInputProcessorTest, process$removesPlayer) {
        device_id_manager->addIdMapping(player_1_ui_id, player_1_device_id, ui::PLAYER);
        const std::string input = std::format("remove {}", player_1_ui_id);
        const auto mock_device_repository = device_repository.get();
        EXPECT_CALL(
            *mock_device_repository,
            removeDevice(player_1_device_id)
        );
        main_input_processor.process(input);
    }

    TEST_F(MainInputProcessorTest, process$changesFileOnPlayer) {
        device_id_manager->addIdMapping(player_1_ui_id, player_1_device_id, ui::PLAYER);
        const std::string input = std::format("file {} {}", player_1_ui_id, test_file_1);
        const auto mock_device_repository = device_repository.get();
        EXPECT_CALL(
            *mock_device_repository,
            changeDeviceFile(player_1_device_id, test_file_1)
        );
        main_input_processor.process(input);
    }
}
