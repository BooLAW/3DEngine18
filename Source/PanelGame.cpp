#include "PanelGame.h"
#include "ModuleCamera3D.h"
#include "Application.h"
#include "TextureMSAA.h"
#include "ComponentCamera.h"
#include "Camera.h"


PanelGame::PanelGame() :Panel("Game")
{
	active = true;
}


PanelGame::~PanelGame()
{
}

void PanelGame::Draw()
{
	if (ImGui::BeginDock("Game", &active))
	{
		//TODO
		ImVec2 new_size = ImGui::GetContentRegionAvail();
		if (size.x != new_size.x || size.y != new_size.y) {
			//App->renderer3D->OnResize(size.x, size.y);
			size.x = new_size.x;
			size.y = new_size.y;
		}
		pos = ImGui::GetWindowPos();
	
		ImVec2 size = ImGui::GetContentRegionAvail();

		if (App->camera->GetCurrentCam()->SceneMSAA() != nullptr)
		{
			ImGui::Image((void*)App->camera->GetCurrentCam()->SceneMSAA()->GetTextureID(), size, ImVec2(0, 1), ImVec2(1, 0));
			App->camera->GetCurrentCam()->SceneMSAA()->Render();
			App->camera->GetCurrentCam()->SceneMSAA()->Unbind();

		}
		isOver = ImGui::IsMouseHoveringWindow();
	}
	ImGui::EndDock();

	if (App->camera->GetCurrentCam()->SceneMSAA() != nullptr)
	{
		}
}

bool PanelGame::MouseOver() const
{
	return isOver;
}

ImVec2 PanelGame::GetSize() const
{
	return size;
}

ImVec2 PanelGame::GetPos() const
{
	return pos;
}
