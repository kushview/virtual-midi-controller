/*
    This file is part of Virtual MIDI Controller
    Copyright (c) 2019  Kushview, LLC.  All rights reserved.
*/

#include "juce.hpp"
#include <juce_events/juce_events.h>

#include "maincomponent.hpp"
#include "lookandfeel.hpp"
#include "controller.hpp"

#ifndef VMC_WIDTH
#define VMC_WIDTH 900
#endif

#ifndef VMC_HEIGHT
#define VMC_HEIGHT 320
#endif

namespace vmc
{
    class Application : public JUCEApplication
    {
    public:
        Application() {}

        const String getApplicationName() override { return "Virtual MIDI Controller"; }
        const String getApplicationVersion() override { return "1.0.0"; }
        bool moreThanOneInstanceAllowed() override { return false; }

        void initialise(const String &commandLine) override
        {
            ignoreUnused(commandLine);
            setupGlobals();
            LookAndFeel::setDefaultLookAndFeel(&look);
            mainWindow.reset(new MainWindow(getApplicationName(), *controller));
        }

        void shutdown() override
        {
            shutdownGui();
            controller->saveSettings();
            controller->shutdown();
            controller.reset();
        }

        void systemRequestedQuit() override
        {
            quit();
        }

        void anotherInstanceStarted(const String &commandLine) override
        {
            ignoreUnused(commandLine);
        }

        class MainWindow : public DocumentWindow
        {
        public:
            MainWindow(String name, Controller &vc)
                : DocumentWindow(name, Desktop::getInstance().getDefaultLookAndFeel().findColour(ResizableWindow::backgroundColourId), 
                    DocumentWindow::closeButton | DocumentWindow::minimiseButton ),
                  controller(vc)
            {
#if JUCE_LINUX
                setUsingNativeTitleBar(false);
#else
                setUsingNativeTitleBar(true);
#endif
                setContentOwned(new MainComponent(vc), true);

                if (auto *props = controller.getSettings().getUserSettings())
                {
                    const auto state = props->getValue("windowPosition", String());
                    if (state.isNotEmpty())
                        restoreWindowStateFromString(state);
                }

                setSize(VMC_WIDTH, VMC_HEIGHT);
                setContentComponentSize(VMC_WIDTH, VMC_HEIGHT);
                setResizable(false, false);

                setVisible(true);
            }

            ~MainWindow()
            {
                setConstrainer(nullptr);
            }

            void savePersistentData()
            {
                if (auto *props = controller.getSettings().getUserSettings())
                {
                    props->setValue("windowPosition", getWindowStateAsString());
                }

                if (auto *const comp = dynamic_cast<MainComponent *>(getContentComponent()))
                {
                }
            }

            void closeButtonPressed() override
            {
                JUCEApplication::getInstance()->systemRequestedQuit();
            }

            void maximiseButtonPressed() override
            {
                return;
            }

        private:
            Controller &controller;
            ComponentBoundsConstrainer constrain;
            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
        };

    private:
        vmc::LookAndFeel look;
        std::unique_ptr<MainWindow> mainWindow;
        std::unique_ptr<Controller> controller;

        void setupGlobals()
        {
            controller.reset(new Controller());
            controller->initializeAudioDevice();
        }

        void shutdownGui()
        {
            if (mainWindow != nullptr)
            {
                mainWindow->savePersistentData();
                mainWindow = nullptr;
            }

            LookAndFeel::setDefaultLookAndFeel(nullptr);
        }
    };

}

START_JUCE_APPLICATION(vmc::Application)
