
# Living Worlds
It's a 3D engine made for degree of Videogames Programming and Design in the subject Engines. Our main goal is to learn how to make a simple 3D engine using third party tools mentioned below.

Link to our repository: [link](https://github.com/BooLAW/3DEngine18)

# Engine Features

## Josep Pi Serra
- Physics
	- Implemented Physics Bullet library
	- Shooting Balls Mechanic
	- Component Colliders:		
		- Cube: Dimensions and Offset
		- Sphere: Radius and Offset
	- Component Rigid Body:
		- Set Gravity
		- Set Mass
	- Physics Body Class
	- Improved Primitives
	- Basic Primitives
		- Cubes
		- Sphere
	- Module Physics
	- Save/Load Scene improved for Physics

## Pau Bonet Vall Llebrera
- Polish Engine
	- Correct old transform bugs (still some to solve)
- Physics
	- Component Player Controller
	- Research on NvidiaPhysX(not using it, but there is a PhysX branch where you can see that there is job done on it)
	- Main Camera Controlls


# Controls

### Editor Camera Controls

- W = Forward
- A = Left
- S = Backwards
- D = Right
- Q = Down
- E = Up

### Game Camera Controls(need to be in Play Mode)

- W = Forward
- A = Left
- S = Backwards
- D = Right
- R = Down
- F = Up
- LeftClick = shoot bullet
- Shift + Mouse = rotate(like in an FPS)

### Other Controls
- Escape = Quit Program
- F1 = Physics Wireframe.
- Z: change guizmo to translate mode.
- X: change guizmo to rotate mode.
- C: change guizmo to scale mode.

## Explanation & GIFS
Detailed explanation of last assignment sub-system, containing two animated gif that shows editor
and in-game functionality.

<iframe width="560" height="315" src="https://www.youtube.com/embed/z0CdAuWSDC0" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

	
	

## Video
One minute video speed-up that shows the process of authoring scenes in your editor, especially
with your core technology from the last assignment (captured from the computer, not mobile).

<iframe width="560" height="315" src="https://www.youtube.com/embed/z0CdAuWSDC0" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

## Last Release
[link]()

## License
MIT License

Copyright (c) 2018 Pau Bonet Vall Llebrera & Josep Pi Serra

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

## Libraries Used


--> [MathGeolib](https://github.com/juj/MathGeoLib): This library is used to calculate simple geometry math

--> [PCG](http://www.pcg-random.org/download.html): Good random number generator

--> [SDL](https://www.libsdl.org/download-2.0.php): To handle the assertions and help with OpenGL initialization.

--> [mmgr](https://github.com/ConfettiFX/The-Forge/tree/master/Common_3/ThirdParty/OpenSource/FluidStudios/MemoryManager): Memory Allocation & Leak Detection

--> [STL](http://www.cplusplus.com/reference/stl/): containers(vector,array,...)

--> [ImGui](https://github.com/ocornut/imgui): UI interface. We also used the [docking branch](https://github.com/ocornut/imgui/tree/docking).

--> [DevIL](http://openil.sourceforge.net/download.php): For Texture and dds rendering for the models.

--> [Assimp](https://github.com/assimp/assimp/releases/tag/v4.1.0/): For reading data from the .fbx format

--> [OpenGL](https://www.opengl.org/documentation/): For the render of 3D models and textures.

--> [Dirent](https://github.com/tronkko/dirent): For iterating thought the files of the working directory.

--> [ThomasThelen](https://github.com/ThomasThelen/Convex-Hull): For Creating a ConvexHull for the Physics bodies of the meshes.
