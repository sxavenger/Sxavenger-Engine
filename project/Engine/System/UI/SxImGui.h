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
#include <type_traits>

////////////////////////////////////////////////////////////////////////////////////////////
// SxImGui namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace SxImGui {

	//=========================================================================================
	// template scaler methods
	//=========================================================================================

	template <typename T>
	concept ScalerConcept
		= std::is_same_v<T, int8_t>
		|| std::is_same_v<T, uint8_t>
		|| std::is_same_v<T, int16_t>
		|| std::is_same_v<T, uint16_t>
		|| std::is_same_v<T, int32_t>
		|| std::is_same_v<T, uint32_t>
		|| std::is_same_v<T, int64_t>
		|| std::is_same_v<T, uint64_t>
		|| std::is_same_v<T, float>
		|| std::is_same_v<T, double>;
	//!< ImGuiで扱えるスカラー型

	template <ScalerConcept T>
	constexpr ImGuiDataType GetImGuiDataType();

	template <ScalerConcept T>
	constexpr const char* GetImGuiFormat();

	template <ScalerConcept T, int32_t N>
	bool DragScalarN(const char* label, T* v, float v_speed = 1.0f, const std::optional<T>& v_min = std::nullopt, const std::optional<T>& v_max = std::nullopt, const char* format = GetImGuiFormat<T>(), ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	template <ScalerConcept T, int32_t N>
	bool SliderScalarN(const char* label, T* v, const T v_min, const T v_max, const char* format = GetImGuiFormat<T>(), ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	template <ScalerConcept T, int32_t N>
	bool InputScalarN(const char* label, T* v, const char* format = GetImGuiFormat<T>(), ImGuiInputTextFlags flags = ImGuiInputTextFlags_None);

	template <ScalerConcept T>
	bool InputScalar(const char* label, T* v, const char* format = GetImGuiFormat<T>(), ImGuiInputTextFlags flags = ImGuiInputTextFlags_None);

	template <typename T>
	bool RadioButton(const char* label, T* v, T v_button);

	template <typename T>
	bool CheckBoxFlags(const char* label, T* flags, T flags_value);

	//=========================================================================================
	// methods
	//=========================================================================================

	bool DragVector2(const char* label, float v[2], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool DragVector3(const char* label, float v[3], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	bool DragFloat(const char* label, float* v, float v_speed = 1.0f, const std::optional<float>& v_min = std::nullopt, const std::optional<float>& v_max = std::nullopt, const char* format = "%.3f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool DragFloat2(const char* label, float v[2], float v_speed = 1.0f, const std::optional<float>& v_min = std::nullopt, const std::optional<float>& v_max = std::nullopt, const char* format = "%.3f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	bool SelectImageButton(const char* id, ImTextureID texture_id, const ImVec2& image_size, bool is_selected);

	bool ImageButton(const char* id, ImTextureID texture_id, const ImVec2& image_size, const ImVec4& tint_col = ImVec4(1, 1, 1, 1));

	bool IsDoubleClickItem();

	void TextClipped(const char* text, float width);

	void TextClippedEx(const char* text, const char* end_text, float width);

	bool InputText(const char* label, std::string& buf, std::string& dst, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
	void InputTextFunc(const char* label, std::string& buf, const std::function<void(const std::string&)>& func, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);

	bool InputText(const char* label, std::string& dst, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None);
	bool InputTextFunc(const char* label, const std::string& dst, const std::function<void(const std::string&)>& func, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None);

	bool MultilineInputTextFunc(const char* label, const std::string& dst, const std::function<void(const std::string&)>& func, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = ImGuiInputTextFlags_None);

	void PlotHistogramFunc(const char* label, const std::function<float(int32_t idx)>& func, int32_t values_count, int32_t values_offset = 0, const char* overlay_text = NULL, const std::optional<float>& scale_min = std::nullopt, const std::optional<float>& scale_max = std::nullopt, ImVec2 graph_size = ImVec2(0, 0));

	bool BeginHoveredTooltip(ImGuiHoveredFlags flags = ImGuiHoveredFlags_None);

	void EndHoveredTooltip();

	bool IsMouseClickedRect(const ImVec2& min, const ImVec2& max, ImGuiMouseButton button = ImGuiMouseButton_Left);

	void HelpMarker(const char* label, const char* text, bool isSameline = true);

	void Image(ImTextureRef handle, const ImVec2& size);

}

////////////////////////////////////////////////////////////////////////////////////////////
// SxImGui namespace template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <SxImGui::ScalerConcept T>
constexpr ImGuiDataType SxImGui::GetImGuiDataType() {
	if constexpr (std::is_same_v<T, int8_t>)        return ImGuiDataType_S8;
	else if constexpr (std::is_same_v<T, uint8_t>)  return ImGuiDataType_U8;
	else if constexpr (std::is_same_v<T, int16_t>)  return ImGuiDataType_S16;
	else if constexpr (std::is_same_v<T, uint16_t>) return ImGuiDataType_U16;
	else if constexpr (std::is_same_v<T, int32_t>)  return ImGuiDataType_S32;
	else if constexpr (std::is_same_v<T, uint32_t>) return ImGuiDataType_U32;
	else if constexpr (std::is_same_v<T, int64_t>)  return ImGuiDataType_S64;
	else if constexpr (std::is_same_v<T, uint64_t>) return ImGuiDataType_U64;
	else if constexpr (std::is_same_v<T, float>)    return ImGuiDataType_Float;
	else if constexpr (std::is_same_v<T, double>)   return ImGuiDataType_Double;
	else static_assert(false, "Unsupported type for ImGuiDataType");
}

template <SxImGui::ScalerConcept T>
constexpr const char* SxImGui::GetImGuiFormat() {
	if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, int16_t> || std::is_same_v<T, int32_t> || std::is_same_v<T, int>) {
		return "%d";

	} else if constexpr (std::is_same_v<T, uint8_t> || std::is_same_v<T, uint16_t> || std::is_same_v<T, uint32_t> || std::is_same_v<T, unsigned int>) {
		return "%u";

	} else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, long long>) {
		return "%lld";

	} else if constexpr (std::is_same_v<T, uint64_t> || std::is_same_v<T, unsigned long long>) {
		return "%llu";

	} else if constexpr (std::is_same_v<T, float>) {
		return "%.3f";

	} else if constexpr (std::is_same_v<T, double>) {
		return "%.6f";

	} else {
		static_assert(!std::is_same_v<T, T>, "Unsupported type for ImGui format.");
	}
}

template <SxImGui::ScalerConcept T, int32_t N>
bool SxImGui::DragScalarN(const char* label, T* v, float v_speed, const std::optional<T>& v_min, const std::optional<T>& v_max, const char* format, ImGuiSliderFlags flags) {
	std::pair<T, T> range = { v_min.value_or(std::numeric_limits<T>::min()), v_max.value_or(std::numeric_limits<T>::max()) };
	return ImGui::DragScalarN(label, SxImGui::GetImGuiDataType<T>(), v, N, v_speed, &range.first, &range.second, format, flags);
}

template <SxImGui::ScalerConcept T, int32_t N>
bool SxImGui::SliderScalarN(const char* label, T* v, const T v_min, const T v_max, const char* format, ImGuiSliderFlags flags) {
	return ImGui::SliderScalarN(label, SxImGui::GetImGuiDataType<T>(), v, N, &v_min, &v_max, format, flags);
}

template <SxImGui::ScalerConcept T, int32_t N>
bool SxImGui::InputScalarN(const char* label, T* v, const char* format, ImGuiInputTextFlags flags) {
	return ImGui::InputScalarN(label, SxImGui::GetImGuiDataType<T>(), v, N, NULL, NULL, format, flags);
}

template <SxImGui::ScalerConcept T>
bool SxImGui::InputScalar(const char* label, T* v, const char* format, ImGuiInputTextFlags flags) {
	return ImGui::InputScalar(label, SxImGui::GetImGuiDataType<T>(), v, NULL, NULL, format, flags);
}

template <typename T>
bool SxImGui::RadioButton(const char* label, T* v, T v_button) {
	const bool pressed = ImGui::RadioButton(label, (*v == v_button));
	if (pressed) {
		*v = v_button;
	}

	return pressed;
}

template <typename T>
bool SxImGui::CheckBoxFlags(const char* label, T* flags, T flags_value) {

	bool all_on = (*flags & flags_value) == flags_value;
	//bool any_on = (*flags & flags_value) != 0;
	bool pressed = false;

	pressed = ImGui::Checkbox(label, &all_on);

	if (pressed) {
		if (all_on) {
			*flags |= flags_value;

		} else {
			*flags &= ~flags_value;
		}
	}

	return pressed;
}
