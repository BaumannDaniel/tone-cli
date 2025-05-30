#include "main_screen.h"

#include <utility>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>

namespace tone::ui {
    class MainScreenBase : public ftxui::ComponentBase {
        std::shared_ptr<ILogger> logger;
        std::shared_ptr<MainInputProcessor> main_input_processor;
        ftxui::Component editor_screen;
        std::string main_input_value;
        ftxui::InputOption main_input_option;
        ftxui::Component main_input;
        int tab_selected = 0;
        const std::vector<std::string> tab_values{
            MainScreenTabs::MEDIA,
            MainScreenTabs::SETTINGS,
        };
        ftxui::Component tab_toggle;
        ftxui::Component settings_container;
        ftxui::Component tab_container{};
        ftxui::Component root_container;

    public:
        MainScreenBase(
            const ftxui::Component& editor_screen,
            std::shared_ptr<MainInputProcessor> main_input_processor,
            std::shared_ptr<ILogger> logger
        );

        bool OnEvent(ftxui::Event event) override;

        ftxui::Element Render() override;
    };
}

tone::ui::MainScreenBase::MainScreenBase(
    const ftxui::Component& editor_screen,
    std::shared_ptr<MainInputProcessor> main_input_processor,
    std::shared_ptr<ILogger> logger
) : logger(std::move(logger)),
    main_input_processor(std::move(main_input_processor)),
    editor_screen(editor_screen) {
    main_input_option = {
        .multiline = false,
        .on_enter = [&] {
            this->main_input_processor->process(main_input_value);
            main_input_value.clear();
        }
    };
    main_input = Input(&main_input_value, "Type your command here!", main_input_option);
    tab_toggle = ftxui::Toggle(&tab_values, &tab_selected);
    settings_container = ftxui::Container::Vertical({});
    tab_container = ftxui::Container::Tab(
        {
            editor_screen,
            Renderer(
                settings_container,
                [&] { return ftxui::text(std::to_string(std::time(nullptr))); }
            )
        },
        &tab_selected
    );
    root_container = ftxui::Container::Vertical({
        main_input,
        tab_toggle,
        tab_container
    });
    Add(main_input);
    Add(tab_toggle);
    Add(tab_container);
}

bool tone::ui::MainScreenBase::OnEvent(ftxui::Event event) {
    ComponentBase::OnEvent(event);
    return true;
}


ftxui::Element tone::ui::MainScreenBase::Render() {
    return vbox(
        main_input->Render(),
        ftxui::separator(),
        tab_toggle->Render(),
        ftxui::separator(),
        tab_container->Render()
    );
}

ftxui::Component tone::ui::createMainScreen(
    const ftxui::Component& editor_screen,
    const std::shared_ptr<MainInputProcessor>& main_input_processor,
    const std::shared_ptr<ILogger>& logger
) {
    return ftxui::Make<MainScreenBase>(editor_screen, main_input_processor, logger);
}
