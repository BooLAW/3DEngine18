#ifndef __PANEL_GAME_H__
#define __PANEL_GAME_H__

#include "Panel.h"

class PanelGame : public Panel
{
public:
	PanelGame();
	virtual ~PanelGame();
	void Draw()override;
	bool MouseOver()const;
	ImVec2 GetSize()const;
	ImVec2 GetPos()const;

private:

	ImVec2 size;
	ImVec2 pos;
	bool isOver;
};

#endif