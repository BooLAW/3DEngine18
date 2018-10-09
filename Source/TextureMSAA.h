#pragma once

#include "Globals.h"

class TextureMSAA
{
public:

	TextureMSAA();
	~TextureMSAA();

	bool Create(uint width, uint height);

	void Bind();
	void Render();
	void Unbind();

	uint GetTextureID();

private:

	uint framebuffer_id;
	uint texture_id;
	uint depthbuffer_id;

	/*uint rbo_id;
	uint rbo_color_id;*/

	unsigned int width;
	unsigned int height;

	/*int max_msaa_samples;
	int current_msaa_samples;*/
};

