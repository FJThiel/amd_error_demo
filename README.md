# AMD OpenGL Vertex Shader/Buffer issue
Minimal (not-) working demo, demonstrating unexpected behaviour on AMD cards. 

## Issue:
When color values are passed as RGB888 to the Shader it only passes
the first 4096 correct. After that, the renderer will use random values
and the image will flicker violently.

It only occurs on our AMD System (specs below), Systems with Nvidia GPUs
are all fine.  
Only occurs when a core profile is selected, compatibility profiles worke as expected.

## Specs
Windows 10 Pro 64x  
Intel Core i5-2400  
Radeon R7 370 4GB  
Current Driver (18.3.4 at the moment, but problem persists for a few versions now)  

## Building
Pull repo, run cmake and build the target.  
GLFW3 required  

## Steps to reproduce
No further interaction than starting the program required.  
You will see a white dot in the middle of the window.

### Expected Result
The point remains white for as long the program runs.

### Actual Result
The point flickers
