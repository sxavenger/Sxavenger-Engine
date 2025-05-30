#include "SxImGui.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>
#include <imgui_internal.h>

//* c++
#include <limits>

////////////////////////////////////////////////////////////////////////////////////////////
// SxImGui namespace methods
////////////////////////////////////////////////////////////////////////////////////////////

bool SxImGui::DragVector3(const char* label, float v[3], float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems) {
		return false;
	}

	bool isChanged = false;

	// 全体のウィジェット幅は通常の DragFloat3 と同じ
	float widgetWidth = ImGui::CalcItemWidth();
	float setWidth    = widgetWidth / 3.0f; // 各軸のセル幅

	// 角丸四角形のサイズ
	float rectWidth  = 2.0f;
	float rectHeight = ImGui::GetFrameHeight();
	float rounding   = ImGui::GetStyle().FrameRounding;
	float dragWidth  = setWidth - rectWidth; // 各 DragFloat の幅
 
	static const ImU32 rectColors[3] = {
		IM_COL32(203, 38, 0, 255),
		IM_COL32(103, 169, 0, 255),
		IM_COL32(44, 126, 237, 255)
	};

	{ //!< group
		ImGui::PushID(label);
		ImGui::BeginGroup();

		for (uint32_t i = 0; i < 3; ++i) {
			ImGui::PushID(i);
			// 角丸四角形のダミー領域を描画
			ImGui::Dummy(ImVec2(rectWidth, rectHeight));
			ImVec2 rectPos = ImGui::GetItemRectMin();
			ImVec2 rectMax(rectPos.x + rectWidth, rectPos.y + rectHeight);
			ImGui::GetWindowDrawList()->AddRectFilled(rectPos, rectMax, rectColors[i], rounding, ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersBottomLeft);
 
			// 角丸四角形と DragFloat の間に隙間を作らない
			ImGui::SameLine(0, 0);

			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
			
			ImGui::SetNextItemWidth(dragWidth - 2);
			isChanged |= ImGui::DragFloat("", &v[i], v_speed, v_min, v_max, format, flags);
			ImGui::PopID();

			ImGui::PopStyleVar(); // `FrameRounding` の変更を元に戻す
 
			// 各セットの後に `SameLine()` を適用して 1 行に並ぶようにする
			if (i < 2) {
				ImGui::SameLine(0, 4); // 間隔を調整
			}
		}

		ImGui::EndGroup();
		ImGui::PopID();
	}
	
 
	// 右側に通常通りラベルを表示
	ImGui::SameLine();
	ImGui::Text("%s", label);
 
	return isChanged;
}

bool SxImGui::DragFloat(const char* label, float* v, float v_speed, const std::optional<float>& v_min, const std::optional<float>& v_max, const char* format, ImGuiSliderFlags flags) {
	return ImGui::DragFloat(label, v, v_speed, v_min.value_or(std::numeric_limits<float>::min()), v_max.value_or(std::numeric_limits<float>::max()), format, flags);
}
