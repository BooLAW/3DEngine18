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
	
	if (ImGui::BeginDock("Scene", &active))
	{
		ImVec2 new_size = ImGui::GetContentRegionAvail();
		if (size.x != new_size.x || size.y != new_size.y) {
			//App->renderer3D->OnResize(size.x, size.y);
			size.x = new_size.x;
			size.y = new_size.y;
		}
		pos = ImGui::GetWindowPos();
		if (ImGui::Button("ClearScene"))
			App->scene_intro->ClearScene();
		ImVec2 size = ImGui::GetContentRegionAvail();
		if (App->camera->editor_cam->SceneMSAA() != nullptr)
		{
			ImGui::Image((void*)App->camera->editor_cam->SceneMSAA()->GetTextureID() ,size, ImVec2(0, 1), ImVec2(1, 0));
		}
		isOver = ImGui::IsMouseHoveringWindow();
	}
	ImGui::EndDock();

	if (App->camera->editor_cam->SceneMSAA() != nullptr)
	{
		App->camera->editor_cam->SceneMSAA()->Render();
		App->camera->editor_cam->SceneMSAA()->Unbind();
	}


}

bool PanelScene::MouseOver() const
{
	return isOver;
}

ImVec2 PanelScene::GetSize() const
{
	return size;
}

ImVec2 PanelScene::GetPos() const
{
	return pos;
}
