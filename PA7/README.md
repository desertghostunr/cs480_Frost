# PA7 Solar System

# Things to do:

## Create Configuration File Loading Function
* Add a function to main for loading a configuration file.
* The order of components in the configuration file should not matter.
* The object number, parent number, and model path are mandatory for an object.
* The other parameters are optional and will be filled with defaults if not provided.
* Add settings to adjust the location of the view point.

### The configuration file is structured as follows:

-v [vertex shader complete path]

-f [fragment shader complete path]

--h display help

-p -c -m -s -r -o -l -t [planet information]

#### Parts of a planet
-p [planet/object number (sun is zero)]

-c [child object's number (can be more than one)]

-m [model for object path]

-s [three floating point values of the objects size relative to earth]

-r [the rotation rate]

-o [three values expressing the planets orbital radius in the x, y, z]

-l [location in local space]

-t [three floating point values of the tilt of the planet]


## Load object update information in the graphics initialization function

* This requires the set and update functions to set the parameters passed in

## Models and Textures

* Pretty self explanatory

## View point

* Something new to learn

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
./SolarSystem -v shaders/textureVertexShader.glsl -f shaders/textureFragmentShader.glsl -m models/BoxT.obj
```

### makefile Instructions

Example building and running with the makefile:
```bash
mkdir build
cd build
cp ../makefile .
make
./SolarSystem -v shaders/textureVertexShader.glsl -f shaders/textureFragmentShader.glsl -m models/BoxT.obj
```

### Specific Instructions on Running
Running the program requires specifying a vertex shader and a fragment shader. Shaders will be found under a directory named shaders in the directory where the program was built. This must be included in the path name when specifying the shaders to be used.

The program has four options that can be used: -v, -f, -m, --h.

-v specifies the file path of the vertex shader.

-f specifies the file path of the fragment shader.

-m specifies the file path of the obj file of the model.

--h brings up a series of instructions for help using the program.

When running the program both a vertex shader and a fragment shader must be specified. Failure to specify both shaders will cause the program to terminate. Failure to specify the model will cause the program to terminate. The obj file and mtl file should be in the models folder.

The file path for the specified shader must come directly after the option identifier. If one of the option identifiers is found where a file path is expected the program will terminate.

This program is run in the following format:

./SolarSystem -[SHADER_OPTION] [SHADER_FILE_PATH] -[SHADER_OPTION] [SHADER_FILE_PATH] -[MODEL_OPTION] [MODEL_FILE_PATH]


An example of running this program:

```bash
./SolarSystem -v shaders/textureVertexShader.glsl -f shaders/textureFragmentShader.glsl -m models/BoxT.obj
```

## Ubuntu.cse.unr.edu
OpenGL 3.3 will run on the [ubuntu.cse.unr.edu](https://ubuntu.cse.unr.edu/) website. To do so follow the build instructions, but when running the Tutorial executable use this line to execute.
```bash
/usr/NX/scripts/vgl/vglrun ./SolarSystem -v shaders/textureVertexShader.glsl -f shaders/textureFragmentShader.glsl -m models/BoxT.obj
```
