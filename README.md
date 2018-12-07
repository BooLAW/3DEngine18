
# Living Worlds
It's a 3D engine made for degree of Videogames Programming and Design in the subject Engines. Our main goal is to learn how to make a simple 3D engine using third party tools mentioned below.

Link to our repository: [link](https://github.com/BooLAW/3DEngine18)

Engine Features

# Josep Pi Serra
- Created our own file format
- Serialization of:
	- Game Objects and Components
	- Scene
- Resource Manager:
	- Library is loaded each time the engine starts if there is none.
	- Storing all the meshes in resources
	
## Innovation
- Sound 

# Pau Bonet Vall Llebrera
- Mouse Picking
	- ImGuizmo
- Space Partitioning (with Octree)
- Camera 
- Engine Structure
	- Components & Childs Structure
	- Deleting & Clearing GameObjects
- Time Management
	- Clocks
	- Game Panel

# Controls
Editor Camera Controls
- W = Forward
- A = Left
- S = Backwards
- D = Right
- Q = Down
- E = Up

Other Controls
- Escape = Quit Program
- F1 = Physics Wireframe





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
