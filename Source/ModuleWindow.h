#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"

class Application;

class ModuleWindow : public Module
{
public:
	ModuleWindow(bool start_enabled = true);
	virtual ~ModuleWindow();

	bool Init();
	bool CleanUp();

	void SetTitle(const char* title);
	void DrawModuleConfig()override;
	void BroadcastEvent(SDL_Event & event)override;

	bool Save(Document& config_w, FileWriteStream& os);
	bool Load(Document* config_r);
	void Resize(int w, int h);

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screen_surface;

	//Windows config
	int width;
	int height;

	bool fullscreen = false;
	bool borderless = false;
	bool size_modified = false;
};

#endif // __ModuleWindow_H__