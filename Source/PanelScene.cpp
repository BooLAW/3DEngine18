#include "PanelScene.h"
#include "ModuleCamera3D.h"
#include "Application.h"
#include "TextureMSAA.h"

PanelScene::PanelScene():Panel("Scene")
{
	active = true;
}


PanelScene::~PanelScene()
{
}

void PanelScene::Draw()
{
	if (ImGui::BeginDock("Scene", &active))
	{
		ImVec2 size = ImGui::GetContentRegionAvail();
		//glEnable(GL_TEXTURE_2D);
		if (App->camera->SceneMSAA() != nullptr)
		{
			ImGui::Image((void*)App->camera->SceneMSAA()->GetTextureID(),size, ImVec2(0, 1), ImVec2(1, 0));
		}
		//glDisable(GL_TEXTURE_2D);
	}
	ImGui::EndDock();

	if (App->camera->SceneMSAA() != nullptr)
	{
		App->camera->SceneMSAA()->Render();
		App->camera->SceneMSAA()->Unbind();
	}
}
