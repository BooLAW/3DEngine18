#ifndef __MATERIAL_H__
#define __MATERIAL_H__
#include "Globals.h"
class Material
{
public:
	Material();
	virtual ~Material();
public:
	uint textures_id = 0;
	uint width = 0;
	uint height = 0;

	
};
#endif
