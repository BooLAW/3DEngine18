
#ifndef __TextureMSAA_H__
#define __TextureMSAA_H__

#include "Globals.h"
// Tutorial LINK https://sites.google.com/site/john87connor/framebuffer/tutorial-10-6-framebuffer-multisample

class TextureMSAA
{
public:
	TextureMSAA();
	~TextureMSAA();

	bool Create(uint width, uint height, int msa_lvl); 
	void Destroy();
	void Bind(); 
	void Render(); 
	void Unbind(); 

	void SetTextureID();
	unsigned int GetTextureID() const;

	void SetWidth();
	unsigned int GetWidth() const;

	void SetHeight();
	unsigned int GetHeight() const;

	void LoadToMemory();
	void UnloadFromMemory();
	void Clear();

private:

	uint fbo_id;
	uint fbo_msaa_id;
	uint texture_id;
	uint rbo_id;
	uint rbo_color_id;
	uint rbo_depth_id;

	unsigned int width;
	unsigned int height;

	int max_msaa_samples;
	int current_msaa_samples;
};
#endif

