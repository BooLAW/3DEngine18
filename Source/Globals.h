#pragma once

// Warning disabled ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 )

#include <windows.h>
#include <stdio.h>
#include <list>
#include <vector>
#include <string>

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);
#define CONSOLE_LOG_DEBUG(format, ...) console_log_debug(__FILE__, __LINE__, format, __VA_ARGS__);
#define CONSOLE_LOG_WARNING(format, ...) console_log_warning(__FILE__, __LINE__, format, __VA_ARGS__);
#define CONSOLE_LOG_ERROR(format, ...) console_log_error(__FILE__, __LINE__, format, __VA_ARGS__);
#define CONSOLE_LOG_INFO(format, ...) console_log_info(__FILE__, __LINE__, format, __VA_ARGS__);


void log(const char file[], int line, const char* format, ...);
void console_log_debug(const char file[], int line, const char* format, ...);
void console_log_warning(const char file[], int line, const char* format, ...);
void console_log_error(const char file[], int line, const char* format, ...);
void console_log_info(const char file[], int line, const char* format, ...);


// Deletes a buffer
#define RELEASE( x )\
    {\
       if( x != nullptr )\
       {\
         delete x;\
	     x = nullptr;\
       }\
    }

// Deletes an array of buffers
#define RELEASE_ARRAY( x )\
	{\
       if( x != nullptr )\
       {\
           delete[] x;\
	       x = nullptr;\
		 }\
	 }

#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define HALF_PI  1.57079632679
#define PI 3.14159265359

#define WINDOW_OFFSET_X 100
#define WINDOW_OFFSET_Y 100

typedef unsigned int uint;

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};
enum log_types
{
	DEB_LOG= 0,
	WARN_LOG,
	ERR_LOG,
	INFO_LOG
};
// Configuration -----------

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 1024
#define SCREEN_SIZE 1
#define WIN_FULLSCREEN false
#define WIN_RESIZABLE true
#define WIN_BORDERLESS false
#define WIN_FULLSCREEN_DESKTOP false
#define TITLE "Living Worlds"
#define ORGANIZATION "UPC CITM"



