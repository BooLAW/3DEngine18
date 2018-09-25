#ifndef __TIMER_H__
#define __TIMER_H__

#include "Globals.h"
#include "SDL/include/SDL.h"

class Timer
{
public:

	// Constructor
	Timer();

	void Start();
	void Stop();

	void Pause();
	void Resume();

	Uint32 Read();

private:


	bool	paused = false;
	bool	running = false;

	Uint32	started_at;
	Uint32	stopped_at;
	// In case of having in count pre and post update
	Uint32  amount;
};

#endif //__TIMER_H__