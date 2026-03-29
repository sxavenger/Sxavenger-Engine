#include "SxGuiInternal.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Configuration/Configuration.h>

//* externals
#include <imgui_internal.h>

////////////////////////////////////////////////////////////////////////////////////////////
// (anonymous namespace)
////////////////////////////////////////////////////////////////////////////////////////////
namespace {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Constants
	////////////////////////////////////////////////////////////////////////////////////////////

	static const float DRAGDROP_HOLD_TO_OPEN_TIMER = 0.70f; // Time for drag-hold to activate items accepting the ImGuiButtonFlags_PressedOnDragDropHold button behavior.
	static const float DRAG_MOUSE_THRESHOLD_FACTOR = 0.50f; // Multiplier for the default value of io.MouseDragThreshold to make DragFloat/DragInt react faster to mouse drags.

	static const ImU32 GDefaultRgbaColorMarkers[4] = {
		IM_COL32(203, 38, 0, 255), IM_COL32(103, 169, 0, 255), IM_COL32(44, 126, 237, 255), IM_COL32(140, 140, 140, 255)
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// ImVec operators
	////////////////////////////////////////////////////////////////////////////////////////////

#ifndef IMGUI_DEFINE_MATH_OPERATORS
	// ImVec2 operators
	static inline ImVec2  operator*(const ImVec2& lhs, const float rhs) { return ImVec2(lhs.x * rhs, lhs.y * rhs); }
	static inline ImVec2  operator/(const ImVec2& lhs, const float rhs) { return ImVec2(lhs.x / rhs, lhs.y / rhs); }
	static inline ImVec2  operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
	static inline ImVec2  operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }
	static inline ImVec2  operator*(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y); }
	static inline ImVec2  operator/(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x / rhs.x, lhs.y / rhs.y); }
	static inline ImVec2  operator-(const ImVec2& lhs) { return ImVec2(-lhs.x, -lhs.y); }
	static inline ImVec2& operator*=(ImVec2& lhs, const float rhs) { lhs.x *= rhs; lhs.y *= rhs; return lhs; }
	static inline ImVec2& operator/=(ImVec2& lhs, const float rhs) { lhs.x /= rhs; lhs.y /= rhs; return lhs; }
	static inline ImVec2& operator+=(ImVec2& lhs, const ImVec2& rhs) { lhs.x += rhs.x; lhs.y += rhs.y; return lhs; }
	static inline ImVec2& operator-=(ImVec2& lhs, const ImVec2& rhs) { lhs.x -= rhs.x; lhs.y -= rhs.y; return lhs; }
	static inline ImVec2& operator*=(ImVec2& lhs, const ImVec2& rhs) { lhs.x *= rhs.x; lhs.y *= rhs.y; return lhs; }
	static inline ImVec2& operator/=(ImVec2& lhs, const ImVec2& rhs) { lhs.x /= rhs.x; lhs.y /= rhs.y; return lhs; }
	static inline bool    operator==(const ImVec2& lhs, const ImVec2& rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }
	static inline bool    operator!=(const ImVec2& lhs, const ImVec2& rhs) { return lhs.x != rhs.x || lhs.y != rhs.y; }
	// ImVec4 operators
	static inline ImVec4  operator*(const ImVec4& lhs, const float rhs) { return ImVec4(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs); }
	static inline ImVec4  operator/(const ImVec4& lhs, const float rhs) { return ImVec4(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs); }
	static inline ImVec4  operator+(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); }
	static inline ImVec4  operator-(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w); }
	static inline ImVec4  operator*(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w); }
	static inline ImVec4  operator/(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w); }
	static inline ImVec4  operator-(const ImVec4& lhs) { return ImVec4(-lhs.x, -lhs.y, -lhs.z, -lhs.w); }
	static inline bool    operator==(const ImVec4& lhs, const ImVec4& rhs) { return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w; }
	static inline bool    operator!=(const ImVec4& lhs, const ImVec4& rhs) { return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z || lhs.w != rhs.w; }
#endif

}

////////////////////////////////////////////////////////////////////////////////////////////
// SxGui namespace methods
////////////////////////////////////////////////////////////////////////////////////////////

bool SxGui::CheckInsertText(const char* label) {
	const char* label_end = ImGui::FindRenderedTextEnd(label);
	return label != label_end;
}

bool SxGui::DragScalarNImpl(const char* label, ImGuiDataType data_type, void* p_data, int components, float v_speed, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags) {

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems) {
		return false;
	}

	const char* label_end = ImGui::FindRenderedTextEnd(label);

	if (label == label_end) {
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
	}
	
	ImGuiContext& g = *GImGui;
	bool value_changed = false;
	ImGui::BeginGroup();
	ImGui::PushID(label);
	ImGui::PushMultiItemsWidths(components, ImGui::CalcItemWidth());
	size_t type_size = ImGui::DataTypeGetInfo(data_type)->Size;
	for (int i = 0; i < components; i++) {
		ImGui::PushID(i);

		if (i > 0) {
			ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
		}
			
		if (flags & ImGuiSliderFlags_ColorMarkers) {
			ImGui::SetNextItemColorMarker(GDefaultRgbaColorMarkers[i]);
		}
		
		value_changed |= ImGui::DragScalar("", data_type, p_data, v_speed, p_min, p_max, format, flags);
		ImGui::PopID();
		ImGui::PopItemWidth();
		p_data = (void*)((char*)p_data + type_size);
	}
	ImGui::PopID();

	if (label != label_end) {
		ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
		ImGui::TextEx(label, label_end);

	} else {
		ImGui::PopItemWidth();
	}

	ImGui::EndGroup();
	return value_changed;
}

bool SxGui::SliderScalarNImpl(const char* label, ImGuiDataType data_type, void* v, int components, const void* v_min, const void* v_max, const char* format, ImGuiSliderFlags flags) {

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems) {
		return false;
	}

	const char* label_end = ImGui::FindRenderedTextEnd(label);
	if (label == label_end) {
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
	}

	ImGuiContext& g = *GImGui;
	bool value_changed = false;
	ImGui::BeginGroup();
	ImGui::PushID(label);
	ImGui::PushMultiItemsWidths(components, ImGui::CalcItemWidth());
	size_t type_size = ImGui::DataTypeGetInfo(data_type)->Size;
	for (int i = 0; i < components; i++) {
		ImGui::PushID(i);

		if (i > 0) {
			ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
		}
			
		if (flags & ImGuiSliderFlags_ColorMarkers) {
			ImGui::SetNextItemColorMarker(GDefaultRgbaColorMarkers[i]);
		}
			
		value_changed |= ImGui::SliderScalar("", data_type, v, v_min, v_max, format, flags);
		ImGui::PopID();
		ImGui::PopItemWidth();
		v = (void*)((char*)v + type_size);
	}
	ImGui::PopID();

	if (label != label_end) {
		ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
		ImGui::TextEx(label, label_end);

	} else {
		ImGui::PopItemWidth();
	}

	ImGui::EndGroup();
	return value_changed;
}

bool SxGui::InputScalarNImpl(const char* label, ImGuiDataType data_type, void* p_data, int components, const void* p_step, const void* p_step_fast, const char* format, ImGuiInputTextFlags flags) {

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems) {
		return false;
	}

	const char* label_end = ImGui::FindRenderedTextEnd(label);
	if (label == label_end) {
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
	}

	ImGuiContext& g = *GImGui;
	bool value_changed = false;
	ImGui::BeginGroup();
	ImGui::PushID(label);
	ImGui::PushMultiItemsWidths(components, ImGui::CalcItemWidth());
	size_t type_size = ImGui::DataTypeGetInfo(data_type)->Size;
	for (int i = 0; i < components; i++) {
		ImGui::PushID(i);

		if (i > 0) {
			ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
		}
			
		value_changed |= ImGui::InputScalar("", data_type, p_data, p_step, p_step_fast, format, flags);
		ImGui::PopID();
		ImGui::PopItemWidth();
		p_data = (void*)((char*)p_data + type_size);
	}
	ImGui::PopID();

	
	if (label != label_end) {
		ImGui::SameLine(0.0f, g.Style.ItemInnerSpacing.x);
		ImGui::TextEx(label, label_end);

	} else {
		ImGui::PopItemWidth();
	}

	ImGui::EndGroup();
	return value_changed;
}
