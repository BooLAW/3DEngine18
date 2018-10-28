#ifndef __PanelScene_H__
#define __PanelScene_H__
#include "Panel.h"
class PanelScene : public Panel
{
public:
	PanelScene();
	virtual ~PanelScene();
	void Draw()override;
};
#endif
