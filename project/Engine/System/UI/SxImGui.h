#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>

//* c++
#include <optional>
#include <string>
#include <functional>

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

	bool ImageButton(const char* id, ImTextureID texture_id, const ImVec2& image_size, const ImVec4& tint_col = ImVec4(1, 1, 1, 1));

	bool IsDoubleClickItem();

	void TextClipped(const char* text, float width);

	void TextClippedEx(const char* text, const char* end_text, float width);

	bool InputText(const char* label, std::string& buf, std::string& dst, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
	void InputTextFunc(const char* label, std::string& buf, const std::function<void(const std::string&)>& func, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);

	void PlotHistogramFunc(const char* label, const std::function<float(int32_t idx)>& func, int32_t values_count, int32_t values_offset = 0, const char* overlay_text = NULL, const std::optional<float>& scale_min = std::nullopt, const std::optional<float>& scale_max = std::nullopt, ImVec2 graph_size = ImVec2(0, 0));

	bool BeginHoveredTooltip(ImGuiHoveredFlags flags = ImGuiHoveredFlags_None);

	void EndHoveredTooltip();

	bool IsMouseClickedRect(const ImVec2& min, const ImVec2& max, ImGuiMouseButton button = ImGuiMouseButton_Left);

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
