# XDinput
DirectInput wrapper for XInput controllers.

# Features
Not too many worth mentioning, but:
- Maps Left and Right triggers to Sliders 1 and 2 and buttons 11 and 12 when 80% down (configurable later).
- Enables vibration for games that support Force Feedback.
- Displays the state of the sticks, buttons, triggers and vibration motors for all controllers, selectable by clicking the controller's quadrant on the status window.
- Stick calibration/centering in-game by pressing LB+RB+Back+Start. For those old sticks that have seen better days.

# Planned features/ideas
- Per game button mapping for those games that think "Left Stick" means "pause".
- Stick sensitivity and mapping to different axes. Same for triggers.
- Disable vibration (even if some games already have that option).
- Set vibration strength (same as above but just in case).
- Global settings. Prossibly stored in the user's documents.
- A GUI to modify said global/specific settings. No more fiddling with INI files and no more "what were the allowed values again?".

# Running/Debugging
Download everything and open the Solution File (XDinput.sln) in Visual C++ Express 2010. Should be readable by more recent VS packages after a conversion but I haven't tested that. Command names will be those of VCE2010.

Make sure you set the target program and working directory in Project > Properties (Alt+F7) > Debugging, browse for the game's EXE you want to test and hit Run. VCE should first run the `install.bat` file that helps determine which version of DirectInput the game/program uses and copy the output DLL (XDinput.dll) to the target's directory with the right name (dinput.dll or dinput8.dll).
