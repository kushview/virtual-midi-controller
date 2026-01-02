on run argv
    tell application "Finder"
        tell disk (item 1 of argv)
            open
            
            set current view of container window to icon view
            set toolbar visible of container window to false
            set statusbar visible of container window to false
            set the bounds of container window to {400, 100, 1000, 600}
            
            set theViewOptions to the icon view options of container window
            set arrangement of theViewOptions to not arranged
            set icon size of theViewOptions to 140
            set background picture of theViewOptions to file ".background:background.png"
            
            -- Position the application icon
            set position of item "Virtual MIDI Controller.app" of container window to {125, 220}
            
            -- Position the Applications symlink
            set position of item "Applications" of container window to {475, 220}
            
            -- Update the window and close
            update without registering applications
            delay 2
            close
        end tell
    end tell
end run
