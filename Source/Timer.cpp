// ----------------------------------------------------
// Timer.cpp
// Body for CPU Tick Timer class
// ----------------------------------------------------

#include "Timer.h"

// ---------------------------------------------
Timer::Timer()
{
	paused = false;
	amount = 0;
	Start();

}

// ---------------------------------------------
void Timer::Start()
{
	running = true;
	started_at = SDL_GetTicks();
	amount = 0;

}

// ---------------------------------------------
void Timer::Stop()
{
	running = false;
	stopped_at = SDL_GetTicks();
}

void Timer::Pause()
{
	paused = true;

	stopped_at = SDL_GetTicks();

	this->amount += (stopped_at - started_at);
	running = false;
}

void Timer::Resume()
{
	if (paused == false)
		return;

	started_at = SDL_GetTicks();
	running = true;

}

// ---------------------------------------------
Uint32 Timer::Read()
{
	if (running == true && paused == false)
		return SDL_GetTicks() - started_at;

	else if (running == false && paused == false)
		return stopped_at - started_at;

	else if (paused == true && running == true)
		return amount + (SDL_GetTicks() - started_at);

	else if (paused == true && running == false)
		return amount;

}

