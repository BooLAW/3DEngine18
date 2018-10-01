#include "Panel.h"

Panel::Panel(const char * _name)
{
	name = _name;
}

Panel::~Panel()
{
}


void Panel::Activate()
{
	active = !active;
}

bool Panel::IsActive()
{
	return active;
}

void Panel::Draw()
{

}


