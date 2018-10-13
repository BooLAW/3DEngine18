#ifndef __PANEL_CONSOLE_H__
#define __PANEL_CONSOLE_H__
#include "Panel.h"

struct typeLog {
	std::string text;
	int type;
};
class PanelConsole : public Panel
{
public:
	PanelConsole();
	virtual ~PanelConsole();


	void Draw()override;
	bool CheckDisplayFlag(int log);
	void LogToConsole(const std::string text,int type);
	void ClearConsole();
	const ImVec4 GetColorFromLogType(typeLog log);
public:

	std::vector<typeLog> text_buffer;

	std::vector<std::string> text_debug_buffer;
	std::vector<std::string> text_warning_buffer;
	std::vector<std::string> text_err_buffer;
	std::vector<std::string> text_info_buffer;

	bool to_show = false;

	bool show_debug_text = true;
	bool show_warning_text = true;
	bool show_err_text = true;
	bool show_info_text = true;

	ImVec4 Blue = { 0,0,1,1 };
	ImVec4 Red = { 1,0,0,1 };
	ImVec4 Yellow = { 1,1,0,1 };
	ImVec4 White = { 1,1,1,1 };

};
#endif
