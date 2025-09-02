#include "SxImGui.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>
#include <imgui_internal.h>

//* c++
#include <limits>
#include <algorithm>

#ifndef IMGUI_DEFINE_MATH_OPERATORS
namespace {
// ImVec2 operators
static inline ImVec2  operator*(const ImVec2& lhs, const float rhs)     { return ImVec2(lhs.x * rhs, lhs.y * rhs); }
static inline ImVec2  operator/(const ImVec2& lhs, const float rhs)     { return ImVec2(lhs.x / rhs, lhs.y / rhs); }
static inline ImVec2  operator+(const ImVec2& lhs, const ImVec2& rhs)   { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2  operator-(const ImVec2& lhs, const ImVec2& rhs)   { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }
static inline ImVec2  operator*(const ImVec2& lhs, const ImVec2& rhs)   { return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y); }
static inline ImVec2  operator/(const ImVec2& lhs, const ImVec2& rhs)   { return ImVec2(lhs.x / rhs.x, lhs.y / rhs.y); }
static inline ImVec2  operator-(const ImVec2& lhs)                      { return ImVec2(-lhs.x, -lhs.y); }
static inline ImVec2& operator*=(ImVec2& lhs, const float rhs)          { lhs.x *= rhs; lhs.y *= rhs; return lhs; }
static inline ImVec2& operator/=(ImVec2& lhs, const float rhs)          { lhs.x /= rhs; lhs.y /= rhs; return lhs; }
static inline ImVec2& operator+=(ImVec2& lhs, const ImVec2& rhs)        { lhs.x += rhs.x; lhs.y += rhs.y; return lhs; }
static inline ImVec2& operator-=(ImVec2& lhs, const ImVec2& rhs)        { lhs.x -= rhs.x; lhs.y -= rhs.y; return lhs; }
static inline ImVec2& operator*=(ImVec2& lhs, const ImVec2& rhs)        { lhs.x *= rhs.x; lhs.y *= rhs.y; return lhs; }
static inline ImVec2& operator/=(ImVec2& lhs, const ImVec2& rhs)        { lhs.x /= rhs.x; lhs.y /= rhs.y; return lhs; }
static inline bool    operator==(const ImVec2& lhs, const ImVec2& rhs)  { return lhs.x == rhs.x && lhs.y == rhs.y; }
static inline bool    operator!=(const ImVec2& lhs, const ImVec2& rhs)  { return lhs.x != rhs.x || lhs.y != rhs.y; }
// ImVec4 operators
static inline ImVec4  operator*(const ImVec4& lhs, const float rhs)     { return ImVec4(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs); }
static inline ImVec4  operator/(const ImVec4& lhs, const float rhs)     { return ImVec4(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs); }
static inline ImVec4  operator+(const ImVec4& lhs, const ImVec4& rhs)   { return ImVec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); }
static inline ImVec4  operator-(const ImVec4& lhs, const ImVec4& rhs)   { return ImVec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w); }
static inline ImVec4  operator*(const ImVec4& lhs, const ImVec4& rhs)   { return ImVec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w); }
static inline ImVec4  operator/(const ImVec4& lhs, const ImVec4& rhs)   { return ImVec4(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w); }
static inline ImVec4  operator-(const ImVec4& lhs)                      { return ImVec4(-lhs.x, -lhs.y, -lhs.z, -lhs.w); }
static inline bool    operator==(const ImVec4& lhs, const ImVec4& rhs)  { return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w; }
static inline bool    operator!=(const ImVec4& lhs, const ImVec4& rhs)  { return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z || lhs.w != rhs.w; }
}
#endif // IMGUI_DEFINE_MATH_OPERATORS

////////////////////////////////////////////////////////////////////////////////////////////
// SxImGui namespace methods
////////////////////////////////////////////////////////////////////////////////////////////

bool SxImGui::DragVector2(const char* label, float v[2], float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems) {
		return false;
	}

	bool isChanged = false;

	// 全体のウィジェット幅は通常の DragFloat2 と同じ
	float widgetWidth = ImGui::CalcItemWidth();
	float setWidth = widgetWidth / 2.0f; // 各軸のセル幅

	// 角丸四角形のサイズ
	float rectWidth = 2.0f;
	float rectHeight = ImGui::GetFrameHeight();
	float rounding = ImGui::GetStyle().FrameRounding;
	float dragWidth = setWidth - rectWidth; // 各 DragFloat の幅

	static const ImU32 rectColors[2] = {
		IM_COL32(203, 38, 0, 255),
		IM_COL32(103, 169, 0, 255),
	};

	{ //!< group
		ImGui::PushID(label);
		ImGui::BeginGroup();

		for (uint32_t i = 0; i < 2; ++i) {
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
			if (i < 1) {
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

bool SxImGui::DragFloat2(const char* label, float v[2], float v_speed, const std::optional<float>& v_min, const std::optional<float>& v_max, const char* format, ImGuiSliderFlags flags) {
	return ImGui::DragFloat2(label, v, v_speed, v_min.value_or(std::numeric_limits<float>::min()), v_max.value_or(std::numeric_limits<float>::max()), format, flags);
}

bool SxImGui::SelectImageButton(const char* id, ImTextureID texture_id, const ImVec2& image_size, bool is_selected) {
	ImVec4 col = is_selected ? ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] : ImVec4{ 0, 0, 0, 0 };
	return ImGui::ImageButton(id, texture_id, image_size, { 0, 0 }, { 1, 1 }, col, { 1, 1, 1, 1 });
}

bool SxImGui::ImageButton(const char* id, ImTextureID texture_id, const ImVec2& image_size, const ImVec4& tint_col) {
	return ImGui::ImageButton(id, texture_id, image_size, { 0, 0 }, { 1, 1 }, { 0, 0, 0, 0 }, tint_col);
}

bool SxImGui::IsDoubleClickItem() {
	return ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
}

void SxImGui::TextClipped(const char* text, float width) {
	const ImVec2 beginPos = ImGui::GetCursorScreenPos();
	const ImVec2 endPos = { beginPos.x + width, beginPos.y + ImGui::GetTextLineHeight() };

	ImGui::RenderTextClipped(beginPos, endPos, text, nullptr, nullptr, {}, nullptr);
	ImGui::Dummy({ width, ImGui::GetTextLineHeight() });
}

void SxImGui::TextClippedEx(const char* text, const char* end_text, float width) {

	ImVec2 mainTextSize = ImGui::CalcTextSize(text);

	if (mainTextSize.x < width) { //!< end_textが必要ない場合

		// textの描画
		const ImVec2 mainTextBeginPos = ImGui::GetCursorScreenPos();
		const ImVec2 mainTextEndPos = { mainTextBeginPos.x + width, mainTextBeginPos.y + ImGui::GetTextLineHeight() };

		ImGui::RenderTextClipped(mainTextBeginPos, mainTextEndPos, text, nullptr, nullptr, {}, nullptr);

	} else {

		ImVec2 endTextSize = ImGui::CalcTextSize(end_text);
		endTextSize.x = std::clamp(endTextSize.x, 0.0f, width);

		// textの描画
		const ImVec2 mainTextBeginPos = ImGui::GetCursorScreenPos();
		const ImVec2 mainTextEndPos = { mainTextBeginPos.x + width - endTextSize.x, mainTextBeginPos.y + ImGui::GetTextLineHeight() };

		ImGui::RenderTextClipped(mainTextBeginPos, mainTextEndPos, text, nullptr, nullptr, {}, nullptr);

		// end_textの描画
		const ImVec2 endTextBeginPos = { mainTextEndPos.x, mainTextBeginPos.y };
		const ImVec2 endTextEndPos = { mainTextEndPos.x + endTextSize.x, mainTextEndPos.y + endTextSize.y };

		ImGui::RenderTextClipped(endTextBeginPos, endTextEndPos, end_text, nullptr, nullptr, {}, nullptr);
	}
	
	ImGui::Dummy({ width, ImGui::GetTextLineHeight() });
}

bool SxImGui::InputText(const char* label, std::string& buf, std::string& dst, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data) {
	bool isChanged = ImGui::InputText(label, buf.data(), buf.size(), flags, callback, user_data);

	if (isChanged) {
		size_t pos = buf.find('\0');

		if (pos != std::string::npos) {
			dst = buf.substr(0, pos);

		} else {
			dst = buf;
		}
	}

	return isChanged;
}

void SxImGui::InputTextFunc(const char* label, std::string& buf, const std::function<void(const std::string&)>& func, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data) {
	if (buf.size() < 128) {
		buf.resize(128); // 初期サイズを設定
	}

	if (ImGui::InputText(label, buf.data(), buf.size(), flags, callback, user_data)) {
		size_t pos = buf.find('\0');

		if (pos != std::string::npos) {
			func(buf.substr(0, pos));

		} else {
			func(buf);
		}
	}
}

bool SxImGui::InputText(const char* label, std::string& dst, ImGuiInputTextFlags flags) {
	static std::unordered_map<const char*, std::string> buffers;

	if (!buffers.contains(label)) {
		buffers.emplace(label, std::string(128, '\0'));
	}

	std::string& buf = buffers[label];

	bool isChanged = ImGui::InputText(label, buf.data(), buf.size(), flags);

	if (!ImGui::IsItemActive()) {
		buf = dst;
		buf.resize(128, '\0'); // バッファのサイズをリセット
	}

	if (isChanged) {
		size_t pos = buf.find('\0');

		if (pos != std::string::npos) {
			dst = buf.substr(0, pos);
		} else {
			dst = buf;
		}
	}

	return isChanged;
}

void SxImGui::PlotHistogramFunc(const char* label, const std::function<float(int32_t idx)>& func, int32_t values_count, int32_t values_offset, const char* overlay_text, const std::optional<float>& scale_min, const std::optional<float>& scale_max, ImVec2 graph_size) {

	ImGuiContext& g     = *GImGui;
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems) {
		return;
	}

	const ImGuiStyle& style = g.Style;
	const ImGuiID id        = window->GetID(label);

	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
	const ImVec2 frame_size = ImGui::CalcItemSize(graph_size, ImGui::CalcItemWidth(), label_size.y + style.FramePadding.y * 2.0f);

	const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + frame_size);
	const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
	const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0));

	ImGui::ItemSize(total_bb, style.FramePadding.y);

	if (!ImGui::ItemAdd(total_bb, id, &frame_bb, ImGuiItemFlags_NoNav)) {
		return;
	}

	bool hovered;
	ImGui::ButtonBehavior(frame_bb, id, &hovered, NULL);

	// Determine scale from values if not specified

	std::optional<float> v_scale_min = scale_min;
	std::optional<float> v_scale_max = scale_max;

	if (!v_scale_min.has_value() || !v_scale_max.has_value()) {
		float v_min = FLT_MAX;
		float v_max = -FLT_MAX;

		for (int32_t i = 0; i < values_count; i++) {
			const float v = func(i);
			if (v != v) {
				continue;
			}
				
			v_min = ImMin(v_min, v);
			v_max = ImMax(v_max, v);
		}

		if (!v_scale_min.has_value()) {
			v_scale_min = v_min;
		}
			
		if (!v_scale_max.has_value()) {
			v_scale_max = v_max;
		}
	}

	ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

	const int32_t values_count_min = 1;
	int idx_hovered = -1;
	if (values_count >= values_count_min) {
		int res_w      = ImMin((int)frame_size.x, values_count);
		int item_count = values_count;

		// Tooltip on hover
		if (hovered && inner_bb.Contains(g.IO.MousePos)) {
			const float t = ImClamp((g.IO.MousePos.x - inner_bb.Min.x) / (inner_bb.Max.x - inner_bb.Min.x), 0.0f, 0.9999f);
			const int v_idx = (int)(t * item_count);
			IM_ASSERT(v_idx >= 0 && v_idx < values_count);

			const float v0 = func((v_idx + values_offset) % values_count);

			ImGui::SetTooltip("%d: %8.4g", v_idx, v0);
				
			idx_hovered = v_idx;
		}

		const float t_step = 1.0f / (float)res_w;
		const float inv_scale = ((*v_scale_min) == (*v_scale_max)) ? 0.0f : (1.0f / ((*v_scale_max) - (*v_scale_min)));

		float v0 = func((0 + values_offset) % values_count);
		float t0 = 0.0f;
		ImVec2 tp0 = ImVec2(t0, 1.0f - ImSaturate((v0 - (*v_scale_min)) * inv_scale));                       // Point in the normalized space of our target rectangle
		float histogram_zero_line_t = ((*v_scale_min) * (*v_scale_max) < 0.0f) ? (1 + (*v_scale_min) * inv_scale) : ((*v_scale_min) < 0.0f ? 0.0f : 1.0f);   // Where does the zero line stands

		const ImU32 col_base    = ImGui::GetColorU32(ImGuiCol_PlotHistogram);
		const ImU32 col_hovered = ImGui::GetColorU32(ImGuiCol_PlotHistogramHovered);

		for (int32_t n = 0; n < res_w; n++) {
			const float t1 = t0 + t_step;
			const int v1_idx = (int)(t0 * item_count + 0.5f);
			IM_ASSERT(v1_idx >= 0 && v1_idx < values_count);
			const float v1   = func((v1_idx + values_offset + 1) % values_count);
			const ImVec2 tp1 = ImVec2(t1, 1.0f - ImSaturate((v1 - (*v_scale_min)) * inv_scale));

			// NB: Draw calls are merged together by the DrawList system. Still, we should render our batch are lower level to save a bit of CPU.
			ImVec2 pos0 = ImLerp(inner_bb.Min, inner_bb.Max, tp0);
			ImVec2 pos1 = ImLerp(inner_bb.Min, inner_bb.Max, ImVec2(tp1.x, histogram_zero_line_t));

			if (pos1.x >= pos0.x + 2.0f) {
				pos1.x -= 1.0f;
			}
			window->DrawList->AddRectFilled(pos0, pos1, idx_hovered == v1_idx ? col_hovered : col_base);

			t0 = t1;
			tp0 = tp1;
		}
	}

	// Text overlay
	if (overlay_text) {
		ImGui::RenderTextClipped(ImVec2(frame_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), frame_bb.Max, overlay_text, NULL, NULL, ImVec2(0.5f, 0.0f));
	}
		

	if (label_size.x > 0.0f) {
		ImGui::RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, inner_bb.Min.y), label);
	}
}

bool SxImGui::BeginHoveredTooltip(ImGuiHoveredFlags flags) {
	if (ImGui::IsItemHovered(flags)) {
		ImGui::BeginTooltip();
		return true;
	}

	return false;
}

void SxImGui::EndHoveredTooltip() {
	ImGui::EndTooltip();
}

bool SxImGui::IsMouseClickedRect(const ImVec2& min, const ImVec2& max, ImGuiMouseButton button) {
	return ImGui::IsMouseClicked(button) && ImGui::IsMouseHoveringRect(min, max, false);
}

void SxImGui::HelpMarker(const char* label, const char* text, bool isSameline) {
	if (isSameline) {
		ImGui::SameLine();
	}

	ImGui::TextDisabled(label);
	if (ImGui::BeginItemTooltip()) {
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(text);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void SxImGui::Image(ImTextureRef handle, const ImVec2& size) {
	
	ImVec2 region = ImGui::GetContentRegionAvail();

	// 画像アス比と分割したWindowアス比の計算
	float textureAspectRatio = static_cast<float>(size.x) / static_cast<float>(size.y);
	float windowAspectRatio  = region.x / region.y;

	// 出力する画像サイズの設定
	ImVec2 displayTextureSize = region;

	// 画像サイズの調整
	if (textureAspectRatio <= windowAspectRatio) {
		displayTextureSize.x *= textureAspectRatio / windowAspectRatio;

	} else {
		displayTextureSize.y *= windowAspectRatio / textureAspectRatio;
	}

	ImGui::Image(handle, displayTextureSize);
}
