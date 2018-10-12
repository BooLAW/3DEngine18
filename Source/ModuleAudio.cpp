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
	LoadFx("Assets/Audio/Sound/light_button_press.ogg");
	
	App->profiler.SaveInitData("Audio");

	return ret;
}

// Called before quitting
bool ModuleAudio::CleanUp()
{
	CONSOLE_LOG("Freeing sound FX, closing Mixer and Audio subsystem",INFO_LOG);
	
	for (int i = 0; i < fx.size(); i++)   Mix_FreeChunk(fx[i]);
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

bool ModuleAudio::PlayFx(unsigned int id, std::vector<BoolList> tick_array, uint volume, int repeat)
{
	
	if (id < fx.size() && tick == false)
	{
		tick = true;
		bool tmp = false;
		for (std::list<SFXList>::iterator it = blackList.begin(); it != blackList.end(); it++)
			if (*it == id && *it != LIGHT_BUTTON_CLICK) { tmp = true; }

		if (!tmp)
		{
			blackList.push_back((SFXList)id);
			Mix_VolumeChunk(fx[id], volume*sfxVolumeModifier);
			Mix_PlayChannel(-1, fx[id], repeat);
			return true;
		}
	}

	return false;
}

// Load WAV
void ModuleAudio::LoadFx(const char* path)
{
	if (Mix_Chunk* chunk = Mix_LoadWAV(path))
		fx.push_back(chunk);
}