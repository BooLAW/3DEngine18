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
		glEnable(GL_TEXTURE_2D);
		if (aux->cam->SceneMSAA() != nullptr)
		{
			ImGui::Image((void*)aux->cam->SceneMSAA()->GetTextureID() ,size, ImVec2(0, 1), ImVec2(1, 0));
		}
		glDisable(GL_TEXTURE_2D);

	}
	ImGui::EndDock();

	if (aux->cam->SceneMSAA() != nullptr)
	{
		aux->cam->SceneMSAA()->Render();
		aux->cam->SceneMSAA()->Unbind();
	}
}
