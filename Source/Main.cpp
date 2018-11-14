#include <stdlib.h>
#include "Application.h"
#include "Globals.h"

#ifdef _DEBUG
#define TEST_MEMORY_MANAGER
#include "mmgr/mmgr.h"
#endif

#include "SDL/include/SDL.h"
#pragma comment( lib, "SDL/libx86/SDL2.lib" )
#pragma comment( lib, "SDL/libx86/SDL2main.lib" )

enum main_states
{
	MAIN_CREATION,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};
Application* App = nullptr;

int main(int argc, char ** argv)
{
	CONSOLE_LOG_INFO("Starting game '%s'...", TITLE);

	int main_return = EXIT_FAILURE;
	main_states state = MAIN_CREATION;
		
	int fps = 0;

	while (state != MAIN_EXIT)
	{
		switch (state)
		{
		case MAIN_CREATION:

			App = new Application();
			CONSOLE_LOG_INFO("-------------- Application Creation --------------");
			state = MAIN_START;
			break;

		case MAIN_START:

			CONSOLE_LOG_INFO("-------------- Application Init --------------");
			if (App->Init() == false)
			{
				CONSOLE_LOG_ERROR("Application Init exits with ERROR");
				state = MAIN_EXIT;
			}
			else
			{
				state = MAIN_UPDATE;
				CONSOLE_LOG_INFO("-------------- Application Update --------------");
			}

			break;

		case MAIN_UPDATE:
		{
			int update_return = App->Update();

			if (update_return == UPDATE_ERROR)
			{
				CONSOLE_LOG_ERROR("Application Update exits with ERROR");
				state = MAIN_EXIT;
			}

			if (update_return == UPDATE_STOP)
				state = MAIN_FINISH;

		}
			break;

		case MAIN_FINISH:

			CONSOLE_LOG_INFO("-------------- Application CleanUp --------------");
			if (App->CleanUp() == false)
			{
				CONSOLE_LOG_ERROR("Application CleanUp exits with ERROR");
			}
			else
				main_return = EXIT_SUCCESS;

			state = MAIN_EXIT;

			break;

		}
	}

	delete App;
	//CONSOLE_LOG("Exiting game '%s'...\n", TITLE);
	return main_return;
}