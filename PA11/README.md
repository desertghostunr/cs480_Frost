# PA11 Broadside

# Contributors
* Andy Frost
* J.J. Sibley

# Dependencies, Building, and Running

## Dependency Instructions
To run this project installation of these three programs are required [GLEW](http://glew.sourceforge.net/), [GLM](http://glm.g-truc.net/0.9.7/index.html), [SDL2](https://wiki.libsdl.org/Tutorials), [ASSIMP](http://www.assimp.org/), [OPENCV](http://opencv.org/), and [BULLET](http://bulletphysics.org/wordpress/).

This project uses OpenGL 3.3.

This project supports OpenCV 2.4.9.1 and OpenCV 3.1.

This project requires CMake 2.8 +. If this is not available, please use the makefile instead.

### Ubuntu/Linux
```bash
sudo apt-get install libglew-dev libglm-dev libsdl2-dev libassimp-dev libopencv-dev libbullet-dev
```

## Building and Running
Please build this project using CMake 2.8 + or the makefile. It is recommended to create a build directory and to build the project there.

Running the make in a separate directory will allow easy cleanup of the build data, and an easy way to prevent unnecessary data to be added to the git repository.

Note: If CMake fails for any reason, the makefile is also kept up to date.

### CMake Instructions

The minimum required version of CMake is 2.8. 

The building of the project is done using CMake, installation with apt-get may be necessary.

Example building and running with CMake:

```bash
mkdir build
cd build
cmake ..
make
./Broadside -c config/Bullet.xml
```

### makefile Instructions

Example building and running with the makefile:
```bash
mkdir build
cd build
cp ../makefile .
make
./Broadside -c config/Bullet.xml
```

### Specific Instructions on Running
Running the program requires specifying a vertex shader and a fragment shader. Shaders will be found under a directory named shaders in the directory where the program was built. This must be included in the path name when specifying the shaders to be used.

The program has two options that can be used: -c, --h.

-c specifies the configuration file

--h brings up a series of instructions for help using the program.

When running the program both a vertex shader and a fragment shader must be specified. Failure to specify both shaders will cause the program to terminate. Failure to specify the model will cause the program to terminate. The obj file and mtl file should be in the models folder.

The file path for the specified shader must come directly after the option identifier. If one of the option identifiers is found where a file path is expected the program will terminate.

This program is run in the following format:

./Pinball -[CONFIG_OPTION] [CONFIG_FILE_PATH]

An example of running this program:

```bash
./Broadside -c config/Bullet.xml
```

## Controls

### Movement Controls:
* Press the right mouse button to show the cursor.

* Press the left mouse button to hide the cursor.

* Press w: moves player 1's ship forward

* Press d: moves player 1's ship right

* Press a: moves player 1's ship left

* Press f: fires player 1's guns based on the view

* Press q: toggles player 1's view left

* Press e: toggles player 1's view right

* Press c: moves player 1's view forward

* Press up arrow: moves player 2's ship forward

* Press right arrow: moves player 2's ship right

* Press left arrow: moves player 2's ship left

* Press \: fires player 2's guns based on the view

* Press [: toggles player 2's view left

* Press ]: toggles player 2's view right

* Press enter: moves player 2's view forward

* Press space bar: starts the game.

* Press p: pauses the game.

### Lighting and View Controls
* Note: the objects selected will be printed out in the terminal.

* Press x: toggles the shader program used.

* Press 1: select adjusting ambient brightness.

* Press 2: select adjusting objects specular brightness.

* Press 3: select adjusting spotlight brightness and size.

* Press v: to use the arrow keys to move the camera.

* Press shift: to reset the camera position.

* Press r: to only adjust the red channel.

* Press g: to only adjust the green channnel.

* Press b: to only adjust the blue channel.

* Press o: to adjust all channels.

* Press Left arrow key: 
	* Option 2: switches to the previous object.
	* Option 3: decreases spotlight size.

* Press Right arrow key: 
	* Option 2: switches to the right object
	* Option 3: increases spotlight size.

* Press Up arrow key: increases brightness.

* Press Down arrow key: decreases brightness.


## Ubuntu.cse.unr.edu
OpenGL 3.3 will run on the [ubuntu.cse.unr.edu](https://ubuntu.cse.unr.edu/) website. To do so follow the build instructions, but when running the Tutorial executable use this line to execute.
```bash
/usr/NX/scripts/vgl/vglrun ./Broadside -c config/Bullet.xml
```
