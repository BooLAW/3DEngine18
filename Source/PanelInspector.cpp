#include "PanelInspector.h"
#include "ModuleSceneIntro.h"
#include "Component.h"
#include "ComponentMesh.h"



PanelInspector::PanelInspector(const char* name): Panel(name)
{

}


PanelInspector::~PanelInspector()
{
}

void PanelInspector::MeshComponentInfo(GameObject * active_GO)
{
	//ComponentMesh* m_cmp = (ComponentMesh*) active_GO->Get
}
