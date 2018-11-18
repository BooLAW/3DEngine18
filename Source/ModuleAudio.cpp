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
	CONSOLE_LOG_INFO("Loading Audio Mixer");
	bool ret = true;
	SDL_Init(SDL_WINDOW_SHOWN);

	if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		CONSOLE_LOG_ERROR("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", Mix_GetError());
		ret = false;
	}

	// load support for the OGG format
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if((init & flags) != flags)
	{
		CONSOLE_LOG_ERROR("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		ret = false;
	}

	//Initialize SDL_mixer
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		CONSOLE_LOG_ERROR("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		ret = false;
	}
	LoadFx("Assets/Audio/Sound/light_button_press.ogg");
	
	for (int i = 0; i < 100;i++)
	{
		tick_arr.push_back(TRUEBOOL);
		audio_tick_arr.push_back(TRUEBOOL);
		renderer_tick_arr.push_back(TRUEBOOL);
		camera_tick_arr.push_back(TRUEBOOL);
		window_tick_arr.push_back(TRUEBOOL);
		config_tick_arr.push_back(TRUEBOOL);
		input_tick_arr.push_back(TRUEBOOL);
		menu_tick_arr.push_back(TRUEBOOL);
	}

	App->profiler.SaveInitData("Audio");

	return ret;
}

// Called before quitting
bool ModuleAudio::CleanUp()
{
	CONSOLE_LOG_INFO("Freeing sound FX, closing Mixer and Audio subsystem");
	
	for (int i = 0; i < fx.size(); i++)   Mix_FreeChunk(fx[i]);
	fx.clear();

	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
	return true;
}

void ModuleAudio::DrawModuleConfig()
{
	if (ImGui::CollapsingHeader("Audio"))
	{
		App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->audio_tick_arr[0]);
		ImGui::Checkbox("Mute Sound", &mute_sound);
		if (mute_sound == false)
		{
			
			App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->audio_tick_arr[1]);
			if (ImGui::SliderInt("Volume", &sound_volume, 0, 128))
			{
				App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->audio_tick_arr[2]);
				App->audio->audio_tick_arr[2] = FALSEBOOL;
			}


		}
		else
			App->audio->audio_tick_arr[1] = FALSEBOOL;			
		ImGui::Checkbox("Mute Music", &mute_music);
		if (mute_music == false)
		{
			App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->audio_tick_arr[3]);
			if (ImGui::SliderInt("sVolume", &music_volume, 0, 128))
			{
				App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->audio_tick_arr[4]);
				App->audio->audio_tick_arr[4] = FALSEBOOL;
			}
		}
		else
			App->audio->audio_tick_arr[3] = FALSEBOOL;
	}
	else
		App->audio->audio_tick_arr[0] = FALSEBOOL;
}

bool ModuleAudio::PlayFx(unsigned int id, BoolList* tick_array, uint volume, int repeat)
{
	unsigned int ret = 0;
	if (mute_sound)
	{
		volume = 0;
	}
	else
		volume = sound_volume;
	
	if (id < fx.size() && *tick_array == FALSEBOOL)
	{
		*tick_array = TRUEBOOL;
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

bool ModuleAudio::Save(Document & config_w, FileWriteStream & os)
{
	Document::AllocatorType& allocator = config_w.GetAllocator();

	Value test(kObjectType);


	test.AddMember("MuteAudio", mute_sound, allocator);
	test.AddMember("SoundVolume", sound_volume, allocator);
	test.AddMember("MuteMusic", mute_music, allocator);
	test.AddMember("MusicVolume", music_volume, allocator);
	

	config_w.AddMember("audio", test, allocator);

	return false;
}

bool ModuleAudio::Load(Document * config_r)
{
	Document ret;
	ret.Parse(App->loadBuf);
	ret.IsObject();
	mute_sound = ret["audio"]["MuteAudio"].GetBool();
	sound_volume = ret["audio"]["SoundVolume"].GetInt();
	mute_music = ret["audio"]["MuteMusic"].GetBool();
	music_volume = ret["audio"]["MusicVolume"].GetInt();
	return false;
}

// Load WAV
void ModuleAudio::LoadFx(const char* path)
{
	if (Mix_Chunk* chunk = Mix_LoadWAV(path))
	{
		if (chunk == NULL)
		{
			CONSOLE_LOG_ERROR("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
		}
		else
			fx.push_back(chunk);
	}
		
}
