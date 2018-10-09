
#ifndef __MATERIAL_LOADER_H__
#define __MATERIAL_LOADER_H__
#include "OpenGL.h"
#include "DevIL\include\il.h"
#include "DevIL\include\ilu.h"
#include "DevIL\include\ilut.h"
#include "Module.h"

#pragma comment( lib, "../Source/DevIL/lib/DevIL.lib")
#pragma comment( lib, "../Source/DevIL/lib/ILU.lib")
#pragma comment( lib, "../Source/DevIL/lib/ILUT.lib")

class ComponentMaterial;
class MaterialLoader
{
public:

	MaterialLoader();
	bool Start();
	virtual ~MaterialLoader();
	ComponentMaterial* LoadPNG(const char* file_name);
	void Render();
	void CleanUp();

public:
	char* unique_path;
	
};
#endif // !__MATERIAL_LOADER_H__