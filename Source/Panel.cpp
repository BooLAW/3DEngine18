#include "Panel.h"

Panel::Panel(const char * _name)
{
	panel_name = _name;
}

Panel::~Panel()
{
}


void Panel::Activate()
{
	active = true;
}

void Panel::DeActivate()
{
	active = false;
}

bool Panel::IsActive()const
{
	return active;
}

void Panel::Draw()
{

}


