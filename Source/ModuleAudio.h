#ifndef __ModuleAudio_H__
#define __ModuleAudio_H__

#include "Module.h"
#include "SDL_mixer/include/SDL_mixer.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f
enum SFXList
{
	//Peasant SFX
	LIGHT_BUTTON_CLICK,
};
enum BoolList
{
	TRUEBOOL,
	FALSEBOOL
};
class ModuleAudio : public Module
{
public:

	ModuleAudio(bool start_enabled = true);
	~ModuleAudio();

	bool Init();
	bool CleanUp();
	void DrawModuleConfig()override;

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory
	void LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(unsigned int id, std::vector<BoolList> tick_array, uint volume = 128, int repeat = 0);

private:

	Mix_Music*					music;
	

	float						musicVolumeModifier = 1;
	float						sfxVolumeModifier = 1;

public:
	std::list<SFXList>			blackList;
	std::vector<Mix_Chunk*>		fx;
	bool button_up = false;
	bool tick = false;
	std::vector<BoolList> tick_arr;

};

#endif // __ModuleAudio_H__