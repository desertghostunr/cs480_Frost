# PA1: Spinning Cube

# Dependencies, Building, and Running

## Dependency Instructions
To run this project installation of these three programs are required [GLEW](http://glew.sourceforge.net/), [GLM](http://glm.g-truc.net/0.9.7/index.html), and [SDL2](https://wiki.libsdl.org/Tutorials).

This project uses OpenGL 3.3.

### Ubuntu/Linux
```bash
sudo apt-get install libglew-dev libglm-dev libsdl2-dev
```

## Building and Running
Please build this project using CMake. It is recommended to create a build directory and to build the project there.

Running the make in a separate directory will allow easy cleanup of the build data, and an easy way to prevent unnecessary data to be added to the git repository.

### CMake Instructions
The building of the project is done using CMake, installation with apt-get may be necessary.

Example building and running:

```bash
mkdir build
cd build
cmake ..
make
./Tutorial -v shaders/cubeVertexShader.glsl -f shaders/cubeFragmentShader.glsl
```

### Specific Instructions on Running
Running the program requires specifying a vertex shader and a fragment shader. Shaders will be found under a directory named shaders in the directory where the program was built. This must be included in the path name when specifying the shaders to be used.

The program has three options that can be used: -v, -f, --h.

-v specifies the file path of the vertex shader.

-f specifies the file path of the fragment shader.

--h brings up a series of instructions for help using the program.

When running the program both a vertex shader and a fragment shader must be specified. Failure to specify both shaders will cause the program to terminate.

The file path for the specified shader must come directly after the option identifier. If one of the option identifiers is found where a file path is expected the program will terminate.

This program is run in the following format:

./Tutorial -[SHADER_OPTION] [SHADER_FILE_PATH] -[SHADER_OPTION] [SHADER_FILE_PATH] 


An example of running this program:

```bash
./Tutorial -v shaders/cubeVertexShader.glsl -f shaders/cubeFragmentShader.glsl
```

### Controls

The controls used to augment the cube's orbit and rotation are as follows:

Pressing the left mouse button, the w key, or the up key will toggle the cube's direction of orbit.

Pressing the right mouse button, the s key, or the down key will toggle the cube's direction of rotation.

Pressing the X1 mouse button, the d key, the o key, or the right key will toggle if the cube's orbit is paused.

Pressing the X2 mouse button, the a key, the r key, or the left key will toggle if the cube's rotation is paused.

Pressing the middle mouse button, the space bar, or the p key will toggle the pause state of all of the cube's movement.

#### Quick Reference

Any of the following keys can be used for their specified purpose.

##### Toggle Orbit 
###### left mouse button
###### w key
###### up arrow key

##### Toggle Rotation
###### right mouse button
###### s key
###### down arrow key

##### Toggle Oribt Paused
###### X1 mouse button
###### d key
###### o key
###### right arrow key

##### Toggle Rotation Paused
###### X2 mouse button
###### a key
###### r key
###### left arrow key

##### Toggle All Paused
###### middle mouse button
###### space bar
###### p key


## Ubuntu.cse.unr.edu
OpenGL 3.3 will run on the [ubuntu.cse.unr.edu](https://ubuntu.cse.unr.edu/) website. To do so follow the build instructions, but when running the Tutorial executable use this line to execute.
```bash
/usr/NX/scripts/vgl/vglrun ./Tutorial -v shaders/cubeVertexShader.glsl -f shaders/cubeFragmentShader.glsl
```
