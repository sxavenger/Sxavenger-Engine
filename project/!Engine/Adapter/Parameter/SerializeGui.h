#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* parameter
#include "SerializeParameter.h"

//* engine
#include <Engine/System/UI/SxImGui.h>

//* externals
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SerializeGuiFormatter structure
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
struct SerializeGuiFormatter { //!< 特殊化必須
public:
	static_assert(false, "SerializeGuiFormatter<T> is not specialized.");
};

////////////////////////////////////////////////////////////////////////////////////////////
// SerializeGuiFormatter structure for bool
////////////////////////////////////////////////////////////////////////////////////////////
template <>
struct SerializeGuiFormatter<bool> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static bool Checkbox(SerializeParameter<bool>& param) {

		std::string label = "## " + param.Label();

		bool changed = ImGui::Checkbox(label.c_str(), &param.Get());

		ImGui::SameLine();

		if (ImGui::Button(param.Label().c_str())) {
			param.Save();
		}

		return changed;
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// SerializeGuiFormatter structure for scalar
////////////////////////////////////////////////////////////////////////////////////////////
template <SxImGui::ScalerConcept T>
struct SerializeGuiFormatter<T> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* Drag Scalar N *//

	static bool DragScalar(SerializeParameter<T>& param, float v_speed = 1.0f, const std::optional<T>& v_min = std::nullopt, const std::optional<T>& v_max = std::nullopt, const char* format = SxImGui::GetImGuiFormat<T>(), ImGuiSliderFlags flags = ImGuiSliderFlags_None) {
		std::string label = "## " + param.Label();

		bool changed = SxImGui::DragScalarN<T, 1>(label.c_str(), &param.Get(), v_speed, v_min, v_max, format, flags);

		ImGui::SameLine();

		if (ImGui::Button(param.Label().c_str())) {
			param.Save();
		}

		return changed;
	}

	static bool DragScalar2(SerializeParameter<Vector2<T>>& param, float v_speed = 1.0f, const std::optional<T>& v_min = std::nullopt, const std::optional<T>& v_max = std::nullopt, const char* format = SxImGui::GetImGuiFormat<T>(), ImGuiSliderFlags flags = ImGuiSliderFlags_None) {
		std::string label = "## " + param.Label();

		bool changed = SxImGui::DragScalarN<T, 2>(label.c_str(), &param.Get().x, v_speed, v_min, v_max, format, flags);

		ImGui::SameLine();

		if (ImGui::Button(param.Label().c_str())) {
			param.Save();
		}

		return changed;
	}

	static bool DragScalar3(SerializeParameter<Vector3<T>>& param, float v_speed = 1.0f, const std::optional<T>& v_min = std::nullopt, const std::optional<T>& v_max = std::nullopt, const char* format = SxImGui::GetImGuiFormat<T>(), ImGuiSliderFlags flags = ImGuiSliderFlags_None) {
		std::string label = "## " + param.Label();

		bool changed = SxImGui::DragScalarN<T, 3>(label.c_str(), &param.Get().x, v_speed, v_min, v_max, format, flags);

		ImGui::SameLine();

		if (ImGui::Button(param.Label().c_str())) {
			param.Save();
		}

		return changed;
	}

	//* Slider Scalar N *//

	static bool SliderScalar(SerializeParameter<T>& param, const T v_min, const T v_max, const char* format = SxImGui::GetImGuiFormat<T>(), ImGuiSliderFlags flags = ImGuiSliderFlags_None) {

		std::string label = "## " + param.Label();

		bool changed = SxImGui::SliderScalarN<T, 1>(label.c_str(), &param.Get(), v_min, v_max, format, flags);

		ImGui::SameLine();

		if (ImGui::Button(param.Label().c_str())) {
			param.Save();
		}

		return changed;
	}

	static bool SliderScalar2(SerializeParameter<Vector2<T>>& param, const T v_min, const T v_max, const char* format = SxImGui::GetImGuiFormat<T>(), ImGuiSliderFlags flags = ImGuiSliderFlags_None) {

		std::string label = "## " + param.Label();

		bool changed = SxImGui::SliderScalarN<T, 2>(label.c_str(), &param.Get().x, v_min, v_max, format, flags);

		ImGui::SameLine();

		if (ImGui::Button(param.Label().c_str())) {
			param.Save();
		}

		return changed;
	}

	static bool SliderScalar3(SerializeParameter<Vector3<T>>& param, const T v_min, const T v_max, const char* format = SxImGui::GetImGuiFormat<T>(), ImGuiSliderFlags flags = ImGuiSliderFlags_None) {

		std::string label = "## " + param.Label();

		bool changed = SxImGui::SliderScalarN<T, 3>(label.c_str(), &param.Get().x, v_min, v_max, format, flags);

		ImGui::SameLine();

		if (ImGui::Button(param.Label().c_str())) {
			param.Save();
		}

		return changed;
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// SerializeGuiFormatter structure for vector scalar
////////////////////////////////////////////////////////////////////////////////////////////
template <SxImGui::ScalerConcept T>
struct SerializeGuiFormatter<std::vector<T>> {
public:

	static bool DragScalars3(SerializeParameter<std::vector<Vector3<T>>>& param, float v_speed = 1.0f, const std::optional<T>& v_min = std::nullopt, const std::optional<T>& v_max = std::nullopt, const char* format = SxImGui::GetImGuiFormat<T>(), ImGuiSliderFlags flags = ImGuiSliderFlags_None) {

		std::string label = "## " + param.Label();

		// パラメータが空の場合の処理
		if (param.Get().empty()) {
			ImGui::Text("No elements to display for %s", label.c_str());
			return false;
		}

		bool changed = false;

		// 各要素に対する UI を生成
		for (size_t i = 0; i < param.Get().size(); ++i) {
			// DragScalarN の呼び出し
			changed |= SxImGui::DragScalarN<T, 3>((label + std::to_string(i)).c_str(), &param.Get()[i].x, v_speed, v_min, v_max, format, flags);

			
			ImGui::SameLine();

			// ボタンの配置
			if (ImGui::Button(std::format("{}[{}]", param.Label(), i).c_str())) {
				param.Save();
			}

			// 各要素の間にスペースを追加
			ImGui::Spacing();
		}

		return changed;

	}

};
