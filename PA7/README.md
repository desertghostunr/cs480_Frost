# PA7 Solar System

# Contributors
* Andy Frost
* J.J. Sibley

# Dependencies, Building, and Running

## Dependency Instructions
To run this project installation of these three programs are required [GLEW](http://glew.sourceforge.net/), [GLM](http://glm.g-truc.net/0.9.7/index.html), [SDL2](https://wiki.libsdl.org/Tutorials), and [ASSIMP](http://www.assimp.org/) and [OPENCV](http://opencv.org/).

This project uses OpenGL 3.3.

This project supports OpenCV 2.4.9.1 and OpenCV 3.1.

This project requires CMake 2.8 +. If this is not available, please use the makefile instead.

### Ubuntu/Linux
```bash
sudo apt-get install libglew-dev libglm-dev libsdl2-dev libassimp-dev libopencv-dev
```

## Building and Running
Please build this project using CMake 2.8 +. It is recommended to create a build directory and to build the project there.

Running the make in a separate directory will allow easy cleanup of the build data, and an easy way to prevent unnecessary data to be added to the git repository.

Note: If CMake fails for any reason, the makefile is also kept up to date. Again, CMake is the preferred build method.

### CMake Instructions

The minimum required version of CMake is 2.8. 

The building of the project is done using CMake, installation with apt-get may be necessary.

Example building and running with CMake:

```bash
mkdir build
cd build
cmake ..
make
./SolarSystem -c config/SolarSystemConfig.xml
```

### makefile Instructions

Example building and running with the makefile:
```bash
mkdir build
cd build
cp ../makefile .
make
./SolarSystem -c config/SolarSystemConfig.xml
```

### Specific Instructions on Running
Running the program requires specifying a vertex shader and a fragment shader. Shaders will be found under a directory named shaders in the directory where the program was built. This must be included in the path name when specifying the shaders to be used.

The program has two options that can be used: -c, --h.

-c specifies the configuration file

--h brings up a series of instructions for help using the program.

When running the program both a vertex shader and a fragment shader must be specified. Failure to specify both shaders will cause the program to terminate. Failure to specify the model will cause the program to terminate. The obj file and mtl file should be in the models folder.

The file path for the specified shader must come directly after the option identifier. If one of the option identifiers is found where a file path is expected the program will terminate.

This program is run in the following format:

./SolarSystem -[CONFIG_OPTION] [CONFIG_FILE_PATH]

An example of running this program:

```bash
./SolarSystem -c config/SolarSystemConfig.xml
```

## Controls

### Camera Controls:
* Press 1: set the camera to a top down view
* Press 2: Set the camera to a Side by side view

* Press w: To zoom in *unless already zoomed in past sun then becomes a zoom out
* Press s: To zoom out *unless zoomed past sun then becomes a zoom in

* *Following controls become inverted if zoomed past the sun:
* Press Left arrow key: Moves camera to the left
* Press Right arrow key: Moves camera to the right
* Press Up arrow key: Moves Up
* Press Down arrow key: Moves down

### Speed Controls
* Press a: to slow the entire simulation.
* Press d: to speed up the entire simulation.
* Press x: to reset the speed.
* Press ,: to slow the speed of orbit.
* Press .: to increase the speed of orbit.
* Press o: to reset the speed of rotation.
* Press k: to decrease the speed of rotation.
* Press l: to increase the speed of rotation.
* Press r: to reset the speed of rotation.
* Press space: to pause the entire simulation

## Ubuntu.cse.unr.edu
OpenGL 3.3 will run on the [ubuntu.cse.unr.edu](https://ubuntu.cse.unr.edu/) website. To do so follow the build instructions, but when running the Tutorial executable use this line to execute.
```bash
/usr/NX/scripts/vgl/vglrun ./SolarSystem -c config/SolarSystemConfig.xml
```
