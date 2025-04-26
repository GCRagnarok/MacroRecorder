#pragma once

#include <glfw3.h>
#include <iostream>
#include <functional>
#include "../ImGui/imgui.h"
#include "../include/MacroManager.h"

class MacroManager; // Forward declaration

class ImGuiApp
{
public:
	ImGuiApp();
	~ImGuiApp();

	void Init();
	void Run();
	void Clean();

private:
	void Render();

	void RecordMacroButton();
	void PlayMacroButton();

	void CenteredText(const std::string& text);
	void CenteredButton(const char* label, std::function<void()> onClick);

	GLFWwindow* m_Window;

	MacroManager* m_MacroManager;

	ImVec4 m_TextColorRed;
	ImVec4 m_TextColorGreen;
	ImVec4 m_TextColorYellow;
};
