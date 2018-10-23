#include "PanelScene.h"
#include "ModuleCamera3D.h"
#include "Application.h"
#include "TextureMSAA.h"
#include "ComponentCamera.h"
#include "Camera.h"

PanelScene::PanelScene():Panel("Scene")
{
	active = true;
}


PanelScene::~PanelScene()
{
}

void PanelScene::Draw()
{
	ComponentCamera* aux = (ComponentCamera*)App->camera->editor_camera->GetComponent(ComponentType::CAMERA);

	if (ImGui::BeginDock("Scene", &active))
	{
		ImVec2 size = ImGui::GetContentRegionAvail();
		if (aux->cam->SceneMSAA() != nullptr)
		{
			ImGui::Image((void*)aux->cam->SceneMSAA()->GetTextureID() ,size, ImVec2(0, 1), ImVec2(1, 0));
		}

	}
	ImGui::EndDock();

	if (App->camera->editor_camera->GetCamera()->SceneMSAA() != nullptr)
	{
		App->camera->editor_camera->GetCamera()->SceneMSAA()->Render();
		App->camera->editor_camera->GetCamera()->SceneMSAA()->Unbind();
	}


}
