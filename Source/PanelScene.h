#ifndef __PanelScene_H__
#define __PanelScene_H__
#include "Panel.h"
class PanelScene : public Panel
{
public:

	PanelScene();
	virtual ~PanelScene();
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
