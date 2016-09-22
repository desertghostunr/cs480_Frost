# PA3

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

Pressing the left mouse button or the up arrow key will toggle the planet's direction of orbit.

Pressing the right mouse button or the down arrow key will toggle the planet's direction of rotation.

Pressing the X1 mouse button or the right arrow key will toggle if the planet's orbit is paused.

Pressing the X2 mouse button or the left arrow key will toggle if the planet's rotation is paused.

Pressing the middle mouse button or the p key will toggle the pause state of all of the planet's movement.

Pressing the the w key will toggle the moon's direction of orbit.

Pressing the the s key will toggle the moon's direction of rotation.

Pressing the the d key or the o key will toggle if the moon's orbit is paused.

Pressing the the a key or the r key will toggle if the moon's rotation is paused.

Pressing the the m key will toggle the pause state of all of the moon's movement.

Pressing the space bar will toggle the pause state of the planet and the moon.

#### Quick Reference

Any of the following keys can be used for their specified purpose.

##### Toggle Planet Orbit 
###### left mouse button
###### up arrow key

##### Toggle Planet Rotation
###### right mouse button
###### down arrow key

##### Toggle Planet Oribt Paused
###### X1 mouse button
###### right arrow key

##### Toggle Planet Rotation Paused
###### X2 mouse button
###### left arrow key

##### Toggle Planet All Paused
###### middle mouse button
###### p key

##### Toggle Moon Orbit 
###### w key

##### Toggle Moon Rotation
###### s key

##### Toggle Moon Oribt Paused
###### d key
###### o key

##### Toggle Moon Rotation Paused
###### a key
###### r key

##### Toggle Moon All Paused
###### m key

##### Toggle Moon and Planet All Paused
###### space bar


## Ubuntu.cse.unr.edu
OpenGL 3.3 will run on the [ubuntu.cse.unr.edu](https://ubuntu.cse.unr.edu/) website. To do so follow the build instructions, but when running the Tutorial executable use this line to execute.
```bash
/usr/NX/scripts/vgl/vglrun ./Tutorial -v shaders/cubeVertexShader.glsl -f shaders/cubeFragmentShader.glsl
```
