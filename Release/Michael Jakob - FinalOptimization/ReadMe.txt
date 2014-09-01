/////////////////////////////////////////////////////
/// Program:	FinalOptimization
/// Tools used:	Direct3d 11 and C++11
/// Compiler:	cl.exe
/// Subsystem:	32-bit
/// Author: 	Michael Jakob
/////////////////////////////////////////////////////


/// What is FinalOptimization?
FinalOptimization is the name I gave to the Direct3d-framework I created.
This program will demonstrate the basic functionality, that is possible.
Note, that everything has been created from scratch: there is no second
engine this system is built upon, it's pure Direct3d combined with elegant C++11.


/// Requirements
- A DirectX 11 capable GPU
- Windows Vista or higher
- Being ready to have some fun


/// Start
Double click either
 - "start albedo-huge.cmd"	...to start "FinalOptimization.exe" with loading a huge-sized terrain (1024x1024)
 - "start albedo.cmd"		...to start "FinalOptimization.exe" with loading a normal-sized terrain (512x512)
 - "start flatland.cmd"		...to start "FinalOptimization.exe" with loading a small-sized terrain (256x256)


/// Navigation
//Key//		//Functionality//
W		Move forwards
A		Move left
S		Move backwards
D		Move right
Q		Move upwards	(makes only sense in the flying-mode)
E		Move downwards	(makes only sense in the flying-mode)
M		Toggle flying-mode. If this mode is enabled, you are no longer sticked to the ground.
		You can literally go *everywhere*.

/// Background information
- Terrains are stored in height-maps (grayscale-images), namely in the "raw"-format
- "Sticking on the floor" has been realized by (bilinear) interpolation between the surrounding vertices
- Vector- and matrixdata is stored using 128-bit registers of your CPU for optimal performance


