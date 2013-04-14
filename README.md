InstancingTest
==============

This is a test case built to replicate my failed attempt at producing an instanced 2d renderer with OpenGL, without all the messy framework stuff I've got in the testbed codebase I ran into the issue in.
I'm new to versioning on github properly, so this is a bit of a mess.
It runs on Windows and I've included GLFW.DLL and GLEW32d.DLL, though I cannot guarantee these acutually are enough. You'll probably want to get the latest stable versions of GLFW, GLEW and GLM to build it.
A built .exe is also included, so you can run it and see what happens. Make sure you pull the dll files and run it on Windows.
Tested on Windows 7 64-bit, but it's a 32-bit application so it should be fine either way.
Probably runs on any Windows version that supports OpenGL 3.3 or newer.

License and copyright stuff can be found in LICENSE.txt, and GLEW and GLM's license files are available in GLEW_COPY.txt and GLM_COPY.txt respectively.
