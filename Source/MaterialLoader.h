
#ifndef __MATERIAL_LOADER_H__
#define __MATERIAL_LOADER_H__
#include "OpenGL.h"
#include "DevIL\include\il.h"
#include "DevIL\include\ilu.h"
#include "DevIL\include\ilut.h"
#include "Module.h"

//#pragma comment( lib, "../Source/DevIL/libx86/DevIL.lib")
//#pragma comment( lib, "../Source/DevIL/libx86/ILU.lib")
//#pragma comment( lib, "../Source/DevIL/libx86/ILUT.lib")


class MaterialLoader
{
public:

	MaterialLoader();
	bool Start();
	virtual ~MaterialLoader();
	//GLuint LoadPNG(std::string& file_name);
	void Render();
	void CleanUp();

public:
	
};
#endif // !__MATERIAL_LOADER_H__