# KEX-23



## Install and Run

The project is built using OpenCV 4.7.0 for C++ using Visual Studio 2022. "Debug" and "Release" modes for x64 are configured, 
with the only assumption being that the OpenCV installation is in `C:\opencv`.

For new configurations:

* Add `C:\opencv\build\x64\vc16\bin` as environmental variable for PATH.

* In project properties:

	* Go `VC++ Directories` -> `Include Directories` and add `C:\opencv\build\include`.

	* Go `VC++ Directories` -> `Library Directories` and add `C:\opencv\build\x64\vc16\lib`.

	* Go `Linker` -> `Input` -> `Additional Dependencies` and add `opencv_world470d.lib` (for Debug mode) or `opencv_world470.lib` (for Release mode).

Instructions taken from [Youtube](https://youtu.be/trXs2r6xSnI).
