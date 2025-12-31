/*
    This file is part of Virtual MIDI Controller
    Copyright (c) 2019  Kushview, LLC.  All rights reserved.
*/

#include <juce_gui_basics/juce_gui_basics.h>

#include "device.hpp"
#include "maincomponent.hpp"
#include "lookandfeel.hpp"
#include "controller.hpp"

using namespace juce;

namespace vmc {
class Application : public JUCEApplication {
public:
    Application() {}

    const String getApplicationName() override { return "Virtual MIDI Controller"; }
    const String getApplicationVersion() override { return VMC_VERSION_STRING; }
    bool moreThanOneInstanceAllowed() override { return false; }

    void initialise (const String& commandLine) override
    {
        ignoreUnused (commandLine);
        setupGlobals();

        LookAndFeel::setDefaultLookAndFeel (&look);
        mainWindow.reset (new MainWindow (getApplicationName(), *controller));
        tooltipWindow.reset (new TooltipWindow (mainWindow.get()));
    }

    void shutdown() override
    {
        controller->saveSettings();
        shutdownGui();

        controller->shutdown();
        controller.reset();
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted (const String& commandLine) override
    {
        ignoreUnused (commandLine);
    }

    class MainWindow : public DocumentWindow,
                        public Controller::Listener {
    public:
        MainWindow (String name, Controller& vc)
            : DocumentWindow (name, Desktop::getInstance().getDefaultLookAndFeel().findColour (ResizableWindow::backgroundColourId),
                              DocumentWindow::closeButton | DocumentWindow::minimiseButton),
              controller (vc)
        {
            controller.addListener (this);
#if JUCE_LINUX
            setUsingNativeTitleBar (false);
#else
            setUsingNativeTitleBar (true);
#endif
            setContentOwned (new MainComponent (vc), true);

            if (auto* props = controller.getSettings().getUserSettings()) {
                const auto state = props->getValue ("windowPosition", String());
                if (state.isNotEmpty())
                    restoreWindowStateFromString (state);
            }

            setSize (VMC_WIDTH, VMC_HEIGHT);
            setContentComponentSize (VMC_WIDTH, VMC_HEIGHT);
            setResizable (false, false);

            deviceChanged();

            setVisible (true);
        }

        ~MainWindow() override
        {
            controller.removeListener (this);
            clearContentComponent();
            setConstrainer (nullptr);
        }

        void savePersistentData()
        {
            if (auto* props = controller.getSettings().getUserSettings()) {
                props->setValue ("windowPosition", getWindowStateAsString());
            }

            if (auto* const comp = dynamic_cast<MainComponent*> (getContentComponent())) {
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

        void deviceChanged() override
        {
            auto name = controller.deviceFile().getFileNameWithoutExtension();
            setName ("Virtual MIDI Controller - " + name);
        }

    private:
        Controller& controller;
        ComponentBoundsConstrainer constrain;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    vmc::LookAndFeel look;
    std::unique_ptr<MainWindow> mainWindow;
    std::unique_ptr<Controller> controller;
    std::unique_ptr<TooltipWindow> tooltipWindow; // Add TooltipWindow instance

    void setupGlobals()
    {
        controller.reset (new Controller());
        controller->restoreSettings();
        controller->initializeAudioDevice();
    }

    void shutdownGui()
    {
        tooltipWindow = nullptr; // Clean up the tooltip window
        if (mainWindow != nullptr) {
            mainWindow->savePersistentData();
            mainWindow = nullptr;
        }

        LookAndFeel::setDefaultLookAndFeel (nullptr);
    }
};
} // namespace vmc

START_JUCE_APPLICATION (vmc::Application)
