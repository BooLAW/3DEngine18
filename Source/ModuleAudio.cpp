#include "Globals.h"
#include "Application.h"
#include "ModuleAudio.h"

#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

ModuleAudio::ModuleAudio( bool start_enabled) : Module(start_enabled), music(NULL)
{}

// Destructor
ModuleAudio::~ModuleAudio()
{}

// Called before render is available
bool ModuleAudio::Init()
{
	App->profiler.StartTimer("Audio");
	CONSOLE_LOG("Loading Audio Mixer", INFO_LOG);
	bool ret = true;
	SDL_Init(SDL_WINDOW_SHOWN);
	

	if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		//CONSOLE_LOG("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n",WARN_LOG);
		ret = false;
	}

	// load support for the OGG format
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if((init & flags) != flags)
	{
		//CONSOLE_LOG("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		ret = false;
	}

	//Initialize SDL_mixer
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		//CONSOLE_LOG("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		ret = false;
	}

	for (int i = 0; i < 100; i++)
	{
		id_light_button_press.push_back(LoadFx("Assets/Audio/Sound/light_button_press.ogg"));
	}

	
	App->profiler.SaveInitData("Audio");

	return ret;
}

// Called before quitting
bool ModuleAudio::CleanUp()
{
	CONSOLE_LOG("Freeing sound FX, closing Mixer and Audio subsystem",INFO_LOG);

	if(music != NULL)
	{
		Mix_FreeMusic(music);
	}

	std::vector<Mix_Chunk*>* item;

	for(std::vector<Mix_Chunk*>::iterator item = fx.begin(); item != fx.end(); item++)
	{

		Mix_FreeChunk((*item));
	}

	fx.clear();
	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
	return true;
}

void ModuleAudio::DrawModuleConfig()
{
}

// Play a music file
bool ModuleAudio::PlayMusic(const char* path, float fade_time)
{
	bool ret = true;
	
	if(music != NULL)
	{
		if(fade_time > 0.0f)
		{
			Mix_FadeOutMusic((int) (fade_time * 1000.0f));
		}
		else
		{
			Mix_HaltMusic();
		}

		// this call blocks until fade out is done
		Mix_FreeMusic(music);
	}
	
	music = Mix_LoadMUS(path);

	if(music == NULL)
	{
		//CONSOLE_LOG("Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());
		ret = false;
	}
	else
	{
		if(fade_time > 0.0f)
		{
			if(Mix_FadeInMusic(music, -1, (int) (fade_time * 1000.0f)) < 0)
			{
				//CONSOLE_LOG("Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
		else
		{
			if(Mix_PlayMusic(music, -1) < 0)
			{
				//CONSOLE_LOG("Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
	}

	//CONSOLE_LOG("Successfully playing %s", path);
	return ret;
}

// Load WAV
unsigned int ModuleAudio::LoadFx(const char* path)
{
	unsigned int ret = 0;

	Mix_Chunk* chunk = Mix_LoadWAV(path);

	if(chunk == NULL)
	{
		//CONSOLE_LOG("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
	}
	else
	{
		fx.push_back(chunk);
		ret = fx.size();
	}

	return ret;
}

// Play WAV
bool ModuleAudio::PlayFx(unsigned int id, bool restart)
{
	bool ret = false;

	Mix_Chunk* chunk = NULL;
	int i = 0;

	for (std::vector<Mix_Chunk*>::iterator item = fx.begin(); item != fx.end(); item++)
	{
		if (i == id - 1 && (*item)->has_played_once == false && restart == false)
		{
			chunk = (*item);
			Mix_PlayChannel(-1, chunk, 0);
			ret = true;
			(*item)->has_played_once = true;
			break;
		}
		else if (restart == true)
		{
			(*item)->has_played_once = false;
			ret = false;
			break;
		}

		i++;
	}
	return ret;
}