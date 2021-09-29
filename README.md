# tremolo
A Real time audio processing plugin in the Apple format, AudioUnit, version 2 API using the Core Audio Utility classes. 
The plugin processes incoming audio and applys a Tremolo effect, by manipulating the volume of the incoming audio samples, with either a Sine or Square wave modulation.
All code for the plugin is written in C++.

This plugin can be hosted in Logic X, currently working with Logic X 10.6.3, running on macOS 11.5.2 (Big Sur). I have not tested this with other hosts yet such as Ableton Live, Digital Performer etc. 
It currently does not provide a custom CocoaUI GUI object, instead Logic will linstantiate a generic GUI for access to the control parameters.

Control parameters include;
- Tremolo Speed, in hertz.
- Tremolo Depth, in percentage.
- Modulation Wave (either Sine or Square), provided in a drop down menu.

To install, download the project files, build the target execultable, and move the derived .component folder into /Library/Audio/Plug-Ins/Components/. Open Logic to scan for the new PlugIn, it should then appear in the plugin folders under 'DAVE'.

Enjoy!

<img width="1920" alt="Tremolo Audio Unit screenshot" src="https://user-images.githubusercontent.com/67363039/135184252-0edfd516-5dbc-477f-8d56-ef5fd1dde4da.png">
