#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>

//* c++
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// SxImGui namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace SxImGui {

	//=========================================================================================
	// methods
	//=========================================================================================

	bool DragVector3(const char* label, float v[3], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	bool DragFloat(const char* label, float* v, float v_speed = 1.0f, const std::optional<float>& v_min = std::nullopt, const std::optional<float>& v_max = std::nullopt, const char* format = "%.3f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	template <class T>
	bool RadioButton(const char* label, T* v, T v_button);

	bool SelectImageButton(const char* id, ImTextureID texture_id, const ImVec2& image_size, bool is_selected);

	bool IsDoubleClick();

	//=========================================================================================
	// option
	//=========================================================================================

}

////////////////////////////////////////////////////////////////////////////////////////////
// SxImGui namespace template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
bool SxImGui::RadioButton(const char* label, T* v, T v_button) {
	const bool pressed = ImGui::RadioButton(label, (*v == v_button));
	if (pressed) {
		*v = v_button;
	}

	return pressed;
}
