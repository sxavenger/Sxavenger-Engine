#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>
#include <magic_enum.hpp>

//* c++
#include <concepts>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// SxGui namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace SxGui {

	////////////////////////////////////////////////////////////////////////////////////////////
	// internal concepts
	////////////////////////////////////////////////////////////////////////////////////////////

	template <typename T>
	concept ScalarConcept
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

	template <ScalarConcept T>
	constexpr ImGuiDataType GetImGuiDataType();

	////////////////////////////////////////////////////////////////////////////////////////////
	// Widget internal methods
	////////////////////////////////////////////////////////////////////////////////////////////

	bool CheckInsertText(const char* label);

	//* DragScalarN [Impl]

	bool DragScalarNImpl(const char* label, ImGuiDataType data_type, void* p_data, int components, float v_speed, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags);

	//* SliderScalarN [Impl]

	bool SliderScalarNImpl(const char* label, ImGuiDataType data_type, void* v, int components, const void* v_min, const void* v_max, const char* format, ImGuiSliderFlags flags);

	//* InputScalarN [Impl]

	bool InputScalarNImpl(const char* label, ImGuiDataType data_type, void* p_data, int components, const void* p_step, const void* p_step_fast, const char* format, ImGuiInputTextFlags flags);

}

////////////////////////////////////////////////////////////////////////////////////////////
// SxGui [Internal] namespace template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <SxGui::ScalarConcept T>
constexpr ImGuiDataType SxGui::GetImGuiDataType() {
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
