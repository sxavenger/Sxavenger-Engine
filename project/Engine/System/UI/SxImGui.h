#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Geometry/Vector3.h>

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

	//=========================================================================================
	// option
	//=========================================================================================



}
