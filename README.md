# OrbitalVector
A space simulator using real orbital physics to explore transportation, communication, and combat based on near-future technologies.

More information can be found on http://orbitalvelocity.github.io/

Getting started:

Install the following dependencies:
GLEW
GLFW3
CMake

#Downloading
1. git clone https://github.com/orbitalVelocity/OrbitalVector
2. cd OrbitalVector
3. git submodule init
4. git submodule update

#Command line build instructions (Mac/Linux)
1. mkdir build
2. cd build
3. cmake ..
4. make -j 4 (if you have 4 cores)
5. cp ../assets/* .
6. cp ../glsl/* .
7. ./orbitalvector

#Xcode build instructions (start in Terminal)
1. mkdir build
2. cd build
3. cmake -G Xcode
4. open OrbitalVector.xcodeproj/
5. Now in Xcode, add include/library path to GLEW and GLFW, and glm
6. In build phase, add a move files phase following steps

#Windows/Visual Studio build instructions
1. someone get to work on it
