#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl.h"

class UI
{
public:
	static void InitImGUI();
	static void Start(int windowWidth, int windowHeight);
	static void End();
	static bool isInit;
private:
};