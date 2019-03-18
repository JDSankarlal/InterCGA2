#include "UI.h"
#include "IO.h"

bool UI::isInit = false;

void UI::InitImGUI()
{
	TTK::internal::imguiInit();
	isInit = true;
}

void UI::Start(int windowWidth, int windowHeight)
{
	if (isInit)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize.x = static_cast<float>(windowWidth);
		io.DisplaySize.y = static_cast<float>(windowHeight);
		ImGui::NewFrame();
	}
	else
	{
		SAT_DEBUG_LOG_ERROR("IMGUI NOT INITIALIZED");
	}	
}

void UI::End()
{
	if (isInit)
	{
		ImGui::Render();
	}
	else
	{
		SAT_DEBUG_LOG_ERROR("IMGUI NOT INITIALIZED");
	}
}
