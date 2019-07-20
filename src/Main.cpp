
#include "JuceHeader.h"
#include "MainComponent.h"
#include "LookAndFeel.h"
#include "Controller.h"

namespace vmc {

class Application  : public JUCEApplication
{
public:
    Application() { }

    const String getApplicationName() override       { return ProjectInfo::projectName; }
    const String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override       { return false; }

    void initialise (const String& commandLine) override
    {
        setupGlobals();
        LookAndFeel::setDefaultLookAndFeel (&look);
        mainWindow.reset (new MainWindow (getApplicationName(), *controller));
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

    void anotherInstanceStarted (const String& commandLine) override
    {
    }

    class MainWindow : public DocumentWindow
    {
    public:
        MainWindow (String name, Controller& vc)
            : DocumentWindow (name, Desktop::getInstance().getDefaultLookAndFeel()
                                        .findColour (ResizableWindow::backgroundColourId), DocumentWindow::allButtons),
              controller (vc)
        {
            setUsingNativeTitleBar (true);
            setBackgroundColour (kv::LookAndFeel_KV1::widgetBackgroundColor.darker());
            setContentOwned (new MainComponent (vc), true);
            centreWithSize (640, 120);
            setContentComponentSize (640, 120);
            setResizable (false, false);

            if (auto* props = controller.getSettings().getUserSettings())
            {
                const auto state = props->getValue ("windowPosition", String());
                if (state.isNotEmpty())
                    restoreWindowStateFromString (state);
            }
            
            setVisible (true);
        }

        ~MainWindow()
        {
            setConstrainer (nullptr);
        }

        void savePersistentData()
        {
            if (auto* props = controller.getSettings().getUserSettings())
            {
                props->setValue ("windowPosition", getWindowStateAsString());
            }

            if (auto* const comp = dynamic_cast<MainComponent*> (getContentComponent()))
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
        Controller& controller;
        ComponentBoundsConstrainer constrain;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    vmc::LookAndFeel look;
    std::unique_ptr<MainWindow> mainWindow;
    std::unique_ptr<Controller> controller;

    void setupGlobals()
    {
        
        controller.reset (new Controller());
        controller->initializeAudioDevice();
    }

    void shutdownGui()
    {
        if (mainWindow != nullptr)
        {
            mainWindow->savePersistentData();
            mainWindow = nullptr;
        }

        LookAndFeel::setDefaultLookAndFeel (nullptr);
    }
};

}

START_JUCE_APPLICATION (vmc::Application)
