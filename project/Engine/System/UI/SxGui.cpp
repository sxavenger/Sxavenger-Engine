#include "SxGui.h"
SXAVENGER_ENGINE_USING

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
	// Json [Serialize / Deserialize] functions
	////////////////////////////////////////////////////////////////////////////////////////////

	//* serialize *//

	static json Serialize(const float value) {
		return JsonSerializeFormatter<float>::Serialize(value);
	}

	static json Serialize(const ImVec2& value) {
		Vector2f v{ value.x, value.y };
		return JsonSerializeFormatter<Vector2f>::Serialize(v);
	}

	static json Serialize(const ImGuiDir value) {
		std::string name = std::string(magic_enum::enum_name<ImGuiDir>(value));
		return JsonSerializeFormatter<std::string>::Serialize(name);
	}

	static json Serialize(const ImColor& value) {
		Color4ui c = {
			static_cast<uint32_t>(value.Value.x * 255.0f),
			static_cast<uint32_t>(value.Value.y * 255.0f),
			static_cast<uint32_t>(value.Value.z * 255.0f),
			static_cast<uint32_t>(value.Value.w * 255.0f)
		};

		return JsonSerializeFormatter<Color4ui>::Serialize(c);
	}

	//* deserialize *//

	static void Deserialize(float& value, const json& data, const std::string& label) {
		if (data.contains(label)) {
			value = JsonSerializeFormatter<float>::Deserialize(data.at(label));
		}
	}

	static void Deserialize(ImVec2& value, const json& data, const std::string& label) {
		if (data.contains(label)) {
			Vector2f v = JsonSerializeFormatter<Vector2f>::Deserialize(data.at(label));
			value      = ImVec2{ v.x, v.y };
		}
	}

	static void Deserialize(ImGuiDir& value, const json& data, const std::string& label) {
		if (data.contains(label)) {
			std::string name = JsonSerializeFormatter<std::string>::Deserialize(data.at(label));
			value            = magic_enum::enum_cast<ImGuiDir>(name).value_or(ImGuiDir_None);
		}
	}

	static void Deserialize(ImVec4& value, const json& data, const std::string& label) {
		if (data.contains(label)) {
			Color4ui c = JsonSerializeFormatter<Color4ui>::Deserialize(data.at(label));
			value      = ImColor{
				static_cast<float>(c.r) / 255.0f,
				static_cast<float>(c.g) / 255.0f,
				static_cast<float>(c.b) / 255.0f,
				static_cast<float>(c.a) / 255.0f
			};
		}
	}

}

////////////////////////////////////////////////////////////////////////////////////////////
// SxGui namespace methods
////////////////////////////////////////////////////////////////////////////////////////////

void SxGui::Setting() {

	//!< imguiの設定
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	// imgui dockingブランチを参照...

	{ //!< fontの変更 English
		std::filesystem::path filepath = kPackagesDirectory / "font" / "FiraMono-Regular.ttf";
		io.Fonts->AddFontFromFileTTF(filepath.generic_string().c_str(), 14.0f);
	}

	{ //!< fontの変更 Japanese

		ImFontConfig config = {};
		config.MergeMode = true;

		static const ImWchar ranges[] = {
			static_cast<ImWchar>(0x0020), static_cast<ImWchar>(0x00FF),   //!< Basic Latin + Latin-1 Supplement
			static_cast<ImWchar>(0x3000), static_cast<ImWchar>(0x30FF),   //!< CJK Symbols and Punctuation + Hiragana + Katakana
			static_cast<ImWchar>(0x4E00), static_cast<ImWchar>(0x9FAF),   //!< CJK Unified Ideographs
			static_cast<ImWchar>(0xFF00), static_cast<ImWchar>(0xFFEF),   //!< Half-width characters
			static_cast<ImWchar>(0x1F000), static_cast<ImWchar>(0x1F02F), //!< Emoticons
			static_cast<ImWchar>(0)
		};

		std::filesystem::path filepath = kPackagesDirectory / "font" / "MPLUSRounded1c-Regular.ttf";
		io.Fonts->AddFontFromFileTTF(filepath.generic_string().c_str(), 16.0f, &config, ranges);
	}

	{ //!< imguiの書き込み, 読み込みを手動に変更

		io.IniFilename = NULL;

		//!< iniの読み込み
#ifdef _DEVELOPMENT
		if (!std::filesystem::exists(kGuiLayoutFilepath) && std::filesystem::exists(kGuiSampleLayoutFilepath)) {
			//!< iniファイルが存在しない場合はコピー
			std::filesystem::copy(kGuiLayoutFilepath, kGuiSampleLayoutFilepath, std::filesystem::copy_options::overwrite_existing);
			StreamLogger::EngineLog("[SxGui]: imgui layout copied.");
		}
#endif

		ImGui::LoadIniSettingsFromDisk(kGuiLayoutFilepath.generic_string().c_str());
	}

	{ //!< styleの設定

		// todo: fileをcopyする処理を追加する?

		SxGui::LoadStyle();

	}

}

void SxGui::SaveIni(const std::filesystem::path& filepath) {
	ImGui::SaveIniSettingsToDisk(filepath.generic_string().c_str());
}

void SxGui::LoadStyle(const std::filesystem::path& filename) {

	ImGui::StyleColorsDark();

	json data;
	if (!JsonHandler::LoadFromJson(kPackagesDirectory / "gui" / filename, data)) {
		return;
	}

	ImGuiStyle& style = ImGui::GetStyle();

	//* main
	Deserialize(style.WindowPadding,     data, "WindowPadding");
	Deserialize(style.FramePadding,      data, "FramePadding");
	Deserialize(style.ItemSpacing,       data, "ItemSpacing");
	Deserialize(style.TouchExtraPadding, data, "TouchExtraPadding");
	Deserialize(style.IndentSpacing,     data, "IndentSpacing");
	Deserialize(style.GrabMinSize,       data, "GrabMinSize");

	//* border
	Deserialize(style.WindowBorderSize, data, "WindowBorderSize");
	Deserialize(style.ChildBorderSize,  data, "ChildBorderSize");
	Deserialize(style.PopupBorderSize,  data, "PopupBorderSize");
	Deserialize(style.FrameBorderSize,  data, "FrameBorderSize");

	//* rounding
	Deserialize(style.WindowRounding, data, "WindowRounding");
	Deserialize(style.ChildRounding,  data, "ChildRounding");
	Deserialize(style.FrameRounding,  data, "FrameRounding");
	Deserialize(style.PopupRounding,  data, "PopupRounding");
	Deserialize(style.GrabRounding,   data, "GrabRounding");

	//* scrollbar
	Deserialize(style.ScrollbarSize,     data, "ScrollbarSize");
	Deserialize(style.ScrollbarRounding, data, "ScrollbarRounding");
	Deserialize(style.ScrollbarPadding,  data, "ScrollbarPadding");

	//* tab
	Deserialize(style.TabBorderSize,      data, "TabBorderSize");
	Deserialize(style.TabBarBorderSize,   data, "TabBarBorderSize");
	Deserialize(style.TabBarOverlineSize, data, "TabBarOverlineSize");
	Deserialize(style.TabMinWidthBase,    data, "TabMinWidthBase");
	Deserialize(style.TabRounding,        data, "TabRounding");

	//* tables
	Deserialize(style.CellPadding, data, "CellPadding");

	//* widget
	Deserialize(style.ColorButtonPosition, data, "ColorButtonPosition");

	//* docking
	Deserialize(style.DockingSeparatorSize, data, "DockingSeparatorSize");

	//* color
	for (size_t i = 0; i < ImGuiCol_COUNT; ++i) {
		std::string name = std::string(magic_enum::enum_name(static_cast<ImGuiCol_>(i)));
		Deserialize(style.Colors[i], data["Colors"], name);
	}
}

void SxGui::SaveStyle(const std::filesystem::path& filename) {

	json data = json::object();

	const ImGuiStyle& style = ImGui::GetStyle();

	//* main
	data["WindowPadding"]     = Serialize(style.WindowPadding);
	data["FramePadding"]      = Serialize(style.FramePadding);
	data["ItemSpacing"]       = Serialize(style.ItemSpacing);
	data["TouchExtraPadding"] = Serialize(style.TouchExtraPadding);
	data["IndentSpacing"]     = Serialize(style.IndentSpacing);
	data["GrabMinSize"]       = Serialize(style.GrabMinSize);

	//* border
	data["WindowBorderSize"] = Serialize(style.WindowBorderSize);
	data["ChildBorderSize"]  = Serialize(style.ChildBorderSize);
	data["PopupBorderSize"]  = Serialize(style.PopupBorderSize);
	data["FrameBorderSize"]  = Serialize(style.FrameBorderSize);

	//* rounding
	data["WindowRounding"] = Serialize(style.WindowRounding);
	data["ChildRounding"]  = Serialize(style.ChildRounding);
	data["FrameRounding"]  = Serialize(style.FrameRounding);
	data["PopupRounding"]  = Serialize(style.PopupRounding);
	data["GrabRounding"]   = Serialize(style.GrabRounding);

	//* scrollbar
	data["ScrollbarSize"]     = Serialize(style.ScrollbarSize);
	data["ScrollbarRounding"] = Serialize(style.ScrollbarRounding);
	data["ScrollbarPadding"]  = Serialize(style.ScrollbarPadding);

	//* tab
	data["TabBorderSize"]      = Serialize(style.TabBorderSize);
	data["TabBarBorderSize"]   = Serialize(style.TabBarBorderSize);
	data["TabBarOverlineSize"] = Serialize(style.TabBarOverlineSize);
	data["TabMinWidthBase"]    = Serialize(style.TabMinWidthBase);
	data["TabRounding"]        = Serialize(style.TabRounding);

	//* tables
	data["CellPadding"] = Serialize(style.CellPadding);

	//* widget
	data["ColorButtonPosition"] = Serialize(style.ColorButtonPosition);

	//* docking
	data["DockingSeparatorSize"] = Serialize(style.DockingSeparatorSize);

	//* color
	for (size_t i = 0; i < ImGuiCol_COUNT; ++i) {
		std::string name     = std::string(magic_enum::enum_name(static_cast<ImGuiCol_>(i)));
		data["Colors"][name] = Serialize(ImColor(style.Colors[i]));
	}

	JsonHandler::WriteToJson(kPackagesDirectory / "gui" / filename, data);

}

////////////////////////////////////////////////////////////////////////////////////////////
// Hierarchy namespace methods
////////////////////////////////////////////////////////////////////////////////////////////

bool SxGui::Hierarchy::Begin() {

	const ImGuiTableFlags kFlags = ImGuiTableFlags_RowBg;

	bool isOpen = ImGui::BeginTable("## Hierarchy", 1, kFlags);

	if (isOpen) {
		//!< styleの設定
		ImGui::PushStyleVarY(ImGuiStyleVar_CellPadding, 0.0f);
	}

	return isOpen;
}

void SxGui::Hierarchy::End() {
	ImGui::EndTable();

	//!< styleのリセット
	ImGui::PopStyleVar();
}

bool SxGui::Hierarchy::TreeNode(const std::string& label, bool isSelect, bool isLeaf, ImGuiTreeNodeFlags flags) {

	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);

	const ImGuiTreeNodeFlags kDefault
		= ImGuiTreeNodeFlags_OpenOnDoubleClick
		| ImGuiTreeNodeFlags_OpenOnArrow
		| ImGuiTreeNodeFlags_FramePadding
		| ImGuiTreeNodeFlags_SpanAllColumns
		| ImGuiTreeNodeFlags_DrawLinesToNodes;

	flags |= kDefault;

	if (isSelect) {
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	if (isLeaf) {
		flags |= ImGuiTreeNodeFlags_Leaf;
	}

	bool isOpen = SxGui::Hierarchy::TreeNodeEx(label.c_str(), flags);

	return isOpen;
}

void SxGui::Hierarchy::TreePop() {
	ImGui::TreePop();
}

void SxGui::Hierarchy::TreeNodeStoreStackData(ImGuiTreeNodeFlags flags, float x1) {
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;

	g.TreeNodeStack.resize(g.TreeNodeStack.Size + 1);
	ImGuiTreeNodeStackData* tree_node_data = &g.TreeNodeStack.Data[g.TreeNodeStack.Size - 1];
	tree_node_data->ID = g.LastItemData.ID;
	tree_node_data->TreeFlags = flags;
	tree_node_data->ItemFlags = g.LastItemData.ItemFlags;
	tree_node_data->NavRect = g.LastItemData.NavRect;

	// Initially I tried to latch value for GetColorU32(ImGuiCol_TreeLines) but it's not a good trade-off for very large trees.
	const bool draw_lines = (flags & (ImGuiTreeNodeFlags_DrawLinesFull | ImGuiTreeNodeFlags_DrawLinesToNodes)) != 0;
	tree_node_data->DrawLinesX1 = draw_lines ? (x1 + g.FontSize * 0.5f + g.Style.FramePadding.x) : +FLT_MAX;
	tree_node_data->DrawLinesTableColumn = (draw_lines && g.CurrentTable) ? (ImGuiTableColumnIdx)g.CurrentTable->CurrentColumn : -1;
	tree_node_data->DrawLinesToNodesY2 = -FLT_MAX;
	window->DC.TreeHasStackDataDepthMask |= (1 << window->DC.TreeDepth);
	if (flags & ImGuiTreeNodeFlags_DrawLinesToNodes)
		window->DC.TreeRecordsClippedNodesY2Mask |= (1 << window->DC.TreeDepth);
}

bool SxGui::Hierarchy::TreeNodeBehavior(ImGuiID id, ImGuiTreeNodeFlags flags, const char* label, const char* label_end) {

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const bool display_frame = (flags & ImGuiTreeNodeFlags_Framed) != 0;
	const ImVec2 padding = (display_frame || (flags & ImGuiTreeNodeFlags_FramePadding)) ? style.FramePadding : ImVec2(style.FramePadding.x, ImMin(window->DC.CurrLineTextBaseOffset, style.FramePadding.y));

	if (!label_end)
		label_end = ImGui::FindRenderedTextEnd(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, label_end, false);

	const float text_offset_x = g.FontSize + (display_frame ? padding.x * 3 : padding.x * 2);   // Collapsing arrow width + Spacing
	const float text_offset_y = ImMax(padding.y, window->DC.CurrLineTextBaseOffset);            // Latch before ItemSize changes it
	const float text_width = g.FontSize + label_size.x + padding.x * 2;                         // Include collapsing arrow

	// We vertically grow up to current line height up the typical widget height.
	const float frame_height = ImMax(ImMin(window->DC.CurrLineSize.y, g.FontSize + style.FramePadding.y * 2), label_size.y + padding.y * 2);
	const bool span_all_columns = (flags & ImGuiTreeNodeFlags_SpanAllColumns) != 0 && (g.CurrentTable != NULL);
	const bool span_all_columns_label = (flags & ImGuiTreeNodeFlags_LabelSpanAllColumns) != 0 && (g.CurrentTable != NULL);
	ImRect frame_bb;
	frame_bb.Min.x = span_all_columns ? window->ParentWorkRect.Min.x : (flags & ImGuiTreeNodeFlags_SpanFullWidth) ? window->WorkRect.Min.x : window->DC.CursorPos.x;
	frame_bb.Min.y = window->DC.CursorPos.y;
	frame_bb.Max.x = span_all_columns ? window->ParentWorkRect.Max.x : (flags & ImGuiTreeNodeFlags_SpanLabelWidth) ? window->DC.CursorPos.x + text_width + padding.x : window->WorkRect.Max.x;
	frame_bb.Max.y = window->DC.CursorPos.y + frame_height;
	if (display_frame) {
		const float outer_extend = IM_TRUNC(window->WindowPadding.x * 0.5f); // Framed header expand a little outside of current limits
		frame_bb.Min.x -= outer_extend;
		frame_bb.Max.x += outer_extend;
	}

	ImVec2 text_pos(window->DC.CursorPos.x + text_offset_x, window->DC.CursorPos.y + text_offset_y);
	ImGui::ItemSize(ImVec2(text_width, frame_height), padding.y);

	// For regular tree nodes, we arbitrary allow to click past 2 worth of ItemSpacing
	ImRect interact_bb = frame_bb;
	if ((flags & (ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_SpanLabelWidth | ImGuiTreeNodeFlags_SpanAllColumns)) == 0)
		interact_bb.Max.x = frame_bb.Min.x + text_width + (label_size.x > 0.0f ? style.ItemSpacing.x * 2.0f : 0.0f);

	// Compute open and multi-select states before ItemAdd() as it clear NextItem data.
	ImGuiID storage_id = (g.NextItemData.HasFlags & ImGuiNextItemDataFlags_HasStorageID) ? g.NextItemData.StorageId : id;
	bool is_open = ImGui::TreeNodeUpdateNextOpen(storage_id, flags);

	bool is_visible;
	if (span_all_columns || span_all_columns_label) {
		// Modify ClipRect for the ItemAdd(), faster than doing a PushColumnsBackground/PushTableBackgroundChannel for every Selectable..
		const float backup_clip_rect_min_x = window->ClipRect.Min.x;
		const float backup_clip_rect_max_x = window->ClipRect.Max.x;
		window->ClipRect.Min.x = window->ParentWorkRect.Min.x;
		window->ClipRect.Max.x = window->ParentWorkRect.Max.x;
		is_visible = ImGui::ItemAdd(interact_bb, id);
		window->ClipRect.Min.x = backup_clip_rect_min_x;
		window->ClipRect.Max.x = backup_clip_rect_max_x;
	} else {
		is_visible = ImGui::ItemAdd(interact_bb, id);
	}
	g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HasDisplayRect;
	g.LastItemData.DisplayRect = frame_bb;

	// If a NavLeft request is happening and ImGuiTreeNodeFlags_NavLeftJumpsToParent enabled:
	// Store data for the current depth to allow returning to this node from any child item.
	// For this purpose we essentially compare if g.NavIdIsAlive went from 0 to 1 between TreeNode() and TreePop().
	// It will become tempting to enable ImGuiTreeNodeFlags_NavLeftJumpsToParent by default or move it to ImGuiStyle.
	bool store_tree_node_stack_data = false;
	if ((flags & ImGuiTreeNodeFlags_DrawLinesMask_) == 0)
		flags |= g.Style.TreeLinesFlags;
	const bool draw_tree_lines = (flags & (ImGuiTreeNodeFlags_DrawLinesFull | ImGuiTreeNodeFlags_DrawLinesToNodes)) && (frame_bb.Min.y < window->ClipRect.Max.y) && (g.Style.TreeLinesSize > 0.0f);
	if (!(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
		store_tree_node_stack_data = draw_tree_lines;
		if ((flags & ImGuiTreeNodeFlags_NavLeftJumpsToParent) && !g.NavIdIsAlive)
			if (g.NavMoveDir == ImGuiDir_Left && g.NavWindow == window && ImGui::NavMoveRequestButNoResultYet())
				store_tree_node_stack_data = true;
	}

	const bool is_leaf = (flags & ImGuiTreeNodeFlags_Leaf) != 0;
	if (!is_visible) {
		if ((flags & ImGuiTreeNodeFlags_DrawLinesToNodes) && (window->DC.TreeRecordsClippedNodesY2Mask & (1 << (window->DC.TreeDepth - 1)))) {
			ImGuiTreeNodeStackData* parent_data = &g.TreeNodeStack.Data[g.TreeNodeStack.Size - 1];
			parent_data->DrawLinesToNodesY2 = ImMax(parent_data->DrawLinesToNodesY2, window->DC.CursorPos.y); // Don't need to aim to mid Y position as we are clipped anyway.
			if (frame_bb.Min.y >= window->ClipRect.Max.y)
				window->DC.TreeRecordsClippedNodesY2Mask &= ~(1 << (window->DC.TreeDepth - 1)); // Done
		}
		if (is_open && store_tree_node_stack_data)
			TreeNodeStoreStackData(flags, text_pos.x - text_offset_x); // Call before TreePushOverrideID()
		if (is_open && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
			ImGui::TreePushOverrideID(id);
		IMGUI_TEST_ENGINE_ITEM_INFO(g.LastItemData.ID, label, g.LastItemData.StatusFlags | (is_leaf ? 0 : ImGuiItemStatusFlags_Openable) | (is_open ? ImGuiItemStatusFlags_Opened : 0));
		return is_open;
	}

	if (span_all_columns || span_all_columns_label) {
		ImGui::TablePushBackgroundChannel();
		g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HasClipRect;
		g.LastItemData.ClipRect = window->ClipRect;
	}

	ImGuiButtonFlags button_flags = ImGuiTreeNodeFlags_None;
	if ((flags & ImGuiTreeNodeFlags_AllowOverlap) || (g.LastItemData.ItemFlags & ImGuiItemFlags_AllowOverlap))
		button_flags |= ImGuiButtonFlags_AllowOverlap;
	if (!is_leaf)
		button_flags |= ImGuiButtonFlags_PressedOnDragDropHold;

	// We allow clicking on the arrow section with keyboard modifiers held, in order to easily
	// allow browsing a tree while preserving selection with code implementing multi-selection patterns.
	// When clicking on the rest of the tree node we always disallow keyboard modifiers.
	const float arrow_hit_x1 = (text_pos.x - text_offset_x) - style.TouchExtraPadding.x;
	const float arrow_hit_x2 = (text_pos.x - text_offset_x) + (g.FontSize + padding.x * 2.0f) + style.TouchExtraPadding.x;
	const bool is_mouse_x_over_arrow = (g.IO.MousePos.x >= arrow_hit_x1 && g.IO.MousePos.x < arrow_hit_x2);

	const bool is_multi_select = (g.LastItemData.ItemFlags & ImGuiItemFlags_IsMultiSelect) != 0;
	if (is_multi_select) // We absolutely need to distinguish open vs select so _OpenOnArrow comes by default
		flags |= (flags & ImGuiTreeNodeFlags_OpenOnMask_) == 0 ? ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick : ImGuiTreeNodeFlags_OpenOnArrow;

	// Open behaviors can be altered with the _OpenOnArrow and _OnOnDoubleClick flags.
	// Some alteration have subtle effects (e.g. toggle on MouseUp vs MouseDown events) due to requirements for multi-selection and drag and drop support.
	// - Single-click on label = Toggle on MouseUp (default, when _OpenOnArrow=0)
	// - Single-click on arrow = Toggle on MouseDown (when _OpenOnArrow=0)
	// - Single-click on arrow = Toggle on MouseDown (when _OpenOnArrow=1)
	// - Double-click on label = Toggle on MouseDoubleClick (when _OpenOnDoubleClick=1)
	// - Double-click on arrow = Toggle on MouseDoubleClick (when _OpenOnDoubleClick=1 and _OpenOnArrow=0)
	// It is rather standard that arrow click react on Down rather than Up.
	// We set ImGuiButtonFlags_PressedOnClickRelease on OpenOnDoubleClick because we want the item to be active on the initial MouseDown in order for drag and drop to work.
	if (is_mouse_x_over_arrow)
		button_flags |= ImGuiButtonFlags_PressedOnClick;
	else if (flags & ImGuiTreeNodeFlags_OpenOnDoubleClick)
		button_flags |= ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnDoubleClick;
	else
		button_flags |= ImGuiButtonFlags_PressedOnClickRelease;
	if (flags & ImGuiTreeNodeFlags_NoNavFocus)
		button_flags |= ImGuiButtonFlags_NoNavFocus;

	bool selected = (flags & ImGuiTreeNodeFlags_Selected) != 0;
	const bool was_selected = selected;

	// Multi-selection support (header)
	if (is_multi_select) {
		// Handle multi-select + alter button flags for it
		ImGui::MultiSelectItemHeader(id, &selected, &button_flags);
		if (is_mouse_x_over_arrow)
			button_flags = (button_flags | ImGuiButtonFlags_PressedOnClick) & ~ImGuiButtonFlags_PressedOnClickRelease;
	} else {
		if (window != g.HoveredWindow || !is_mouse_x_over_arrow)
			button_flags |= ImGuiButtonFlags_NoKeyModsAllowed;
	}

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(interact_bb, id, &hovered, &held, button_flags);
	bool toggled = false;
	if (!is_leaf) {
		if (pressed && g.DragDropHoldJustPressedId != id) {
			if ((flags & ImGuiTreeNodeFlags_OpenOnMask_) == 0 || (g.NavActivateId == id && !is_multi_select))
				toggled = true; // Single click
			if (flags & ImGuiTreeNodeFlags_OpenOnArrow)
				toggled |= is_mouse_x_over_arrow && !g.NavHighlightItemUnderNav; // Lightweight equivalent of IsMouseHoveringRect() since ButtonBehavior() already did the job
			if ((flags & ImGuiTreeNodeFlags_OpenOnDoubleClick) && g.IO.MouseClickedCount[0] == 2)
				toggled = true; // Double click
		} else if (pressed && g.DragDropHoldJustPressedId == id) {
			IM_ASSERT(button_flags & ImGuiButtonFlags_PressedOnDragDropHold);
			if (!is_open) // When using Drag and Drop "hold to open" we keep the node highlighted after opening, but never close it again.
				toggled = true;
			else
				pressed = false; // Cancel press so it doesn't trigger selection.
		}

		if (g.NavId == id && g.NavMoveDir == ImGuiDir_Left && is_open) {
			toggled = true;
			ImGui::NavClearPreferredPosForAxis(ImGuiAxis_X);
			ImGui::NavMoveRequestCancel();
		}
		if (g.NavId == id && g.NavMoveDir == ImGuiDir_Right && !is_open) // If there's something upcoming on the line we may want to give it the priority?
		{
			toggled = true;
			ImGui::NavClearPreferredPosForAxis(ImGuiAxis_X);
			ImGui::NavMoveRequestCancel();
		}

		if (toggled) {
			is_open = !is_open;
			window->DC.StateStorage->SetInt(storage_id, is_open);
			g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_ToggledOpen;
		}
	}

	// Multi-selection support (footer)
	if (is_multi_select) {
		bool pressed_copy = pressed && !toggled;
		ImGui::MultiSelectItemFooter(id, &selected, &pressed_copy);
		if (pressed)
			ImGui::SetNavID(id, window->DC.NavLayerCurrent, g.CurrentFocusScopeId, interact_bb);
	}

	if (selected != was_selected)
		g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_ToggledSelection;

	// Render
	{
		const ImU32 text_col = ImGui::GetColorU32(ImGuiCol_Text);
		ImGuiNavRenderCursorFlags nav_render_cursor_flags = ImGuiNavRenderCursorFlags_Compact;
		if (is_multi_select)
			nav_render_cursor_flags |= ImGuiNavRenderCursorFlags_AlwaysDraw; // Always show the nav rectangle
		if (display_frame) {
			// Framed type
			const ImU32 bg_col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_TabActive : hovered ? ImGuiCol_TabHovered : ImGuiCol_TabSelectedOverline);
			ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, bg_col, true, style.FrameRounding);
			ImGui::RenderNavCursor(frame_bb, id, nav_render_cursor_flags);
			if (span_all_columns && !span_all_columns_label)
				ImGui::TablePopBackgroundChannel();
			if (flags & ImGuiTreeNodeFlags_Bullet)
				ImGui::RenderBullet(window->DrawList, ImVec2(text_pos.x - text_offset_x * 0.60f, text_pos.y + g.FontSize * 0.5f), text_col);
			else if (!is_leaf)
				ImGui::RenderArrow(window->DrawList, ImVec2(text_pos.x - text_offset_x + padding.x, text_pos.y), text_col, is_open ? ((flags & ImGuiTreeNodeFlags_UpsideDownArrow) ? ImGuiDir_Up : ImGuiDir_Down) : ImGuiDir_Right, 1.0f);
			else // Leaf without bullet, left-adjusted text
				text_pos.x -= text_offset_x - padding.x;
			if (flags & ImGuiTreeNodeFlags_ClipLabelForTrailingButton)
				frame_bb.Max.x -= g.FontSize + style.FramePadding.x;
			if (g.LogEnabled)
				ImGui::LogSetNextTextDecoration("###", "###");
		} else {
			// Unframed typed for tree nodes
			if (hovered || selected) {
				const ImU32 bg_col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_TabActive : hovered ? ImGuiCol_TabHovered : ImGuiCol_TabSelectedOverline);
				ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, bg_col, false);
			}
			ImGui::RenderNavCursor(frame_bb, id, nav_render_cursor_flags);
			if (span_all_columns && !span_all_columns_label)
				ImGui::TablePopBackgroundChannel();
			if (flags & ImGuiTreeNodeFlags_Bullet)
				ImGui::RenderBullet(window->DrawList, ImVec2(text_pos.x - text_offset_x * 0.5f, text_pos.y + g.FontSize * 0.5f), text_col);
			else if (!is_leaf)
				ImGui::RenderArrow(window->DrawList, ImVec2(text_pos.x - text_offset_x + padding.x, text_pos.y + g.FontSize * 0.15f), text_col, is_open ? ((flags & ImGuiTreeNodeFlags_UpsideDownArrow) ? ImGuiDir_Up : ImGuiDir_Down) : ImGuiDir_Right, 0.70f);
			if (g.LogEnabled)
				ImGui::LogSetNextTextDecoration(">", NULL);
		}

		if (draw_tree_lines)
			ImGui::TreeNodeDrawLineToChildNode(ImVec2(text_pos.x - text_offset_x + padding.x, text_pos.y + g.FontSize * 0.5f));

		// Label
		if (display_frame)
			ImGui::RenderTextClipped(text_pos, frame_bb.Max, label, label_end, &label_size);
		else
			ImGui::RenderText(text_pos, label, label_end, false);

		if (span_all_columns_label)
			ImGui::TablePopBackgroundChannel();
	}

	if (is_open && store_tree_node_stack_data)
		TreeNodeStoreStackData(flags, text_pos.x - text_offset_x); // Call before TreePushOverrideID()
	if (is_open && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
		ImGui::TreePushOverrideID(id); // Could use TreePush(label) but this avoid computing twice

	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | (is_leaf ? 0 : ImGuiItemStatusFlags_Openable) | (is_open ? ImGuiItemStatusFlags_Opened : 0));
	return is_open;
	
}

bool SxGui::Hierarchy::TreeNodeEx(const char* label, ImGuiTreeNodeFlags flags) {
	return SxGui::Hierarchy::TreeNodeBehavior(ImGui::GetID(label), flags, label, NULL);
}

////////////////////////////////////////////////////////////////////////////////////////////
// Table namespace methods
////////////////////////////////////////////////////////////////////////////////////////////

bool SxGui::Table::Begin(const std::string& label) {

	const ImGuiTreeNodeFlags kTreeNodeFlags
		= ImGuiTreeNodeFlags_CollapsingHeader
		| ImGuiTreeNodeFlags_DefaultOpen;

	bool isOpen = ImGui::TreeNodeEx(label.c_str(), kTreeNodeFlags);

	if (!isOpen) { //!< 開かれていない.
		return false;
	}

	ImGui::PushID(label.c_str());

	const ImGuiTableFlags kTableFlags
		= ImGuiTableFlags_Resizable
		| ImGuiTableFlags_BordersInner;

	// TODO: CellPaddingの調整.

	ImVec2 region = ImGui::GetContentRegionAvail();

	ImGui::BeginTable(label.c_str(), static_cast<int>(Column::Count), kTableFlags, ImVec2{ region.x, 0.0f });

	return true;
}

void SxGui::Table::End() {
	ImGui::EndTable();
	ImGui::PopID();
}

bool SxGui::Table::CheckBox(const std::string& label, bool* v) {
	ImGui::TableNextRow();

	const std::string id = "## " + label;

	ImGui::TableSetColumnIndex(static_cast<int>(Column::Label));
	ImGui::Text(label.c_str());

	ImGui::TableSetColumnIndex(static_cast<int>(Column::Widget));
	return ImGui::Checkbox(id.c_str(), v);
}
