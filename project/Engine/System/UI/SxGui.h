#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* SxGui
#include "SxGuiInternal.h"

//* engine
#include <Engine/System/Configuration/Configuration.h>
#include <Engine/System/Utility/StreamLogger.h>

//* lib
#include <Lib/Adapter/Json/JsonHandler.h>
#include <Lib/Adapter/Json/JsonSerializer.h>

//* external
#include <imgui.h>
#include <magic_enum.hpp>

//* c++
#include <string>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// SxGui namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace SxGui {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Icon enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//!< [Google Material Icon](https://fonts.google.com/icons)
	enum class Icon : uint32_t {
		Home               = 0xE88A,
		DeployedCode       = 0xF720,
		DeployedCodeUpdate = 0xF5F4,
		ChessPawn          = 0xF3B6,
		AccountTree        = 0xE97A,
		Menu               = 0xE5D2,
		MenuOpen           = 0xE9BD,
		Layers             = 0xE53B,
		Window             = 0xF088,
		Folder             = 0xE2C7,
		Files              = 0xEA85,
		Terminal           = 0xEB8E,
		Timer              = 0xE425,
		Schedule           = 0xE8B5,
		Stack              = 0xF500,
		Visibility         = 0xE8F4,
		VisibilityOff      = 0xE8F5,
		Delete             = 0xE872,
		Equal              = 0xF77B,
		Nearby             = 0xE6B7,
		NearbyError        = 0xF03B,
		Dialog             = 0xE99F,
		ShortText          = 0xE261,
		Videocam           = 0xE04B,
		LightMode          = 0xE518,
		LightBlob          = 0xE0F0,
		Light              = 0xF02A,
		BacklightHigh      = 0xF7ED,
		Search             = 0xE8B6,
		Label              = 0xE892,

		Cube             = DeployedCode,
		CubeUpdate       = DeployedCodeUpdate,
		Hierarchy        = AccountTree,
		Camera           = Videocam,
		DirectionalLight = LightMode,
		PointLight       = LightBlob,
		SpotLight        = Light,
		RectLight        = BacklightHigh,

		/* なぜか以下は使えない
		Warning       = 0xE002,
		Error         = 0xE000,
		*/
	};

	void TextCharToUtf8(char out[5], uint32_t c); // return output UTF-8 bytes count

	//=========================================================================================
	// variables
	//=========================================================================================

	static inline const std::filesystem::path kGuiLayoutFilepath       = "imgui.ini";
	static inline const std::filesystem::path kGuiSampleLayoutFilepath = SXAVENGER_ENGINE kPackagesDirectory / "gui" / kGuiLayoutFilepath;

	static inline const std::filesystem::path kGuiStyleFilepath  = "imgui_style.style";

	//=========================================================================================
	// methods
	//=========================================================================================

	void Setting();

	void SaveIni(const std::filesystem::path& filepath = kGuiLayoutFilepath);

	void LoadStyle(const std::filesystem::path& filepath = kGuiStyleFilepath);

	void SaveStyle(const std::filesystem::path& filepath = kGuiStyleFilepath);

	//=========================================================================================
	// widget methods
	//=========================================================================================

	//-----------------------------------------------------------------------------------------
	// [DragScalar]
	//-----------------------------------------------------------------------------------------

	template <ScalarConcept T, int32_t Component>
	bool DragScalarN(const char* label, T v[Component], float v_speed = 1.0f, const std::optional<T>& v_min = std::nullopt, const std::optional<T>& v_max = std::nullopt, const char* format = NULL, ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	template <ScalarConcept T>
	bool DragScalar(const char* label, T* v, float v_speed = 1.0f, const std::optional<T>& v_min = std::nullopt, const std::optional<T>& v_max = std::nullopt, const char* format = NULL, ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	template <ScalarConcept T, int32_t Component>
	bool DragVectorN(const char* label, T v[Component], float v_speed = 1.0f, const std::optional<T>& v_min = std::nullopt, const std::optional<T>& v_max = std::nullopt, const char* format = NULL, ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	//-----------------------------------------------------------------------------------------
	// [SliderScalar]
	//-----------------------------------------------------------------------------------------

	template <ScalarConcept T, int32_t Component>
	void SliderScalarN(const char* label, T v[Component], const T v_min, const T v_max, const char* format = NULL, ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	template <ScalarConcept T>
	void SliderScalar(const char* label, T* v, const T v_min, const T v_max, const char* format = NULL, ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	//-----------------------------------------------------------------------------------------
	// [InputText]
	//-----------------------------------------------------------------------------------------

	bool InputText(const char* label, std::string& str, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None);
	bool InputText(const char* label, std::string& str, const char* emptyText, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None);

	//-----------------------------------------------------------------------------------------
	// [Image]
	//-----------------------------------------------------------------------------------------

	void Image(ImTextureRef handle, const ImVec2& resolution);

	void ImageLabel(const char* label, ImTextureRef handle, const ImVec2& resolution);

	//-----------------------------------------------------------------------------------------
	// [Dummy]
	//-----------------------------------------------------------------------------------------

	void DummySpace(const ImVec2& size);

	//-----------------------------------------------------------------------------------------
	// [Selectable]
	//-----------------------------------------------------------------------------------------

	bool Selectable(const char* label, bool isSelect, ImGuiSelectableFlags flags = ImGuiSelectableFlags_None);


	//-----------------------------------------------------------------------------------------
	// [ComboEnum]
	//-----------------------------------------------------------------------------------------

	template <typename T> requires std::is_enum_v<T>
	bool ComboEnum(const char* label, T* v, ImGuiComboFlags flags = ImGuiComboFlags_None);

	////////////////////////////////////////////////////////////////////////////////////////////
	// Hierarchy namespace
	////////////////////////////////////////////////////////////////////////////////////////////
	namespace Hierarchy {

		//=========================================================================================
		// methods
		//=========================================================================================

		bool Begin(ImGuiTableFlags flags = ImGuiTableFlags_None, int32_t column = 1);

		void End();

		void NextRow();

		void SetColumnIndex(int32_t column);

		bool TreeNode(const std::string& label, bool isSelect, bool isLeaf, ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None, int32_t column = 0);

		void TreePop();

		bool IsClicked();

		bool IsDoubleClicked();

		//=========================================================================================
		// internal methods
		//=========================================================================================

		void TreeNodeStoreStackData(ImGuiTreeNodeFlags flags, float x1);

		bool TreeNodeBehavior(ImGuiID id, ImGuiTreeNodeFlags flags, const char* label, const char* label_end = NULL);

		bool TreeNodeEx(const char* label, ImGuiTreeNodeFlags flags);

	}

	////////////////////////////////////////////////////////////////////////////////////////////
	// Table namespace
	////////////////////////////////////////////////////////////////////////////////////////////
	namespace Table {

		////////////////////////////////////////////////////////////////////////////////////////////
		// Column enum class
		////////////////////////////////////////////////////////////////////////////////////////////
		enum class Column : uint8_t {
			Label,
			Widget,

			Count,
		};

		////////////////////////////////////////////////////////////////////////////////////////////
		// Table methods
		////////////////////////////////////////////////////////////////////////////////////////////

		bool Begin(const std::string& label);

		void End();

		void NextRow();

		void SetColumnIndex(Column column);

		////////////////////////////////////////////////////////////////////////////////////////////
		// TreeNode methods
		////////////////////////////////////////////////////////////////////////////////////////////

		bool TreeNode(const std::string& label);

		void TreePop();

		////////////////////////////////////////////////////////////////////////////////////////////
		// Widget methods
		////////////////////////////////////////////////////////////////////////////////////////////

		bool CheckBox(const std::string& label, bool* v);

		template <ScalarConcept T, int32_t Component>
		bool DragScalarN(const std::string& label, T v[Component], float v_speed = 1.0f, const std::optional<T>& v_min = std::nullopt, const std::optional<T>& v_max = std::nullopt, const char* format = NULL, ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// formatter
////////////////////////////////////////////////////////////////////////////////////////////
template <>
struct std::formatter<SxGui::Icon, char> {

	constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

	template <typename FormatContext>
	auto format(const SxGui::Icon& icon, FormatContext& ctx) const {
		char utf8[5] = {};
		SxGui::TextCharToUtf8(utf8, static_cast<uint32_t>(icon));
		return std::format_to(ctx.out(), "{}", utf8);
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// SxGui namespace template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <SxGui::ScalarConcept T, int32_t Component>
bool SxGui::DragScalarN(const char* label, T v[Component], float v_speed, const std::optional<T>& v_min, const std::optional<T>& v_max, const char* format, ImGuiSliderFlags flags) {
	const std::pair<T, T> range = { v_min.value_or(std::numeric_limits<T>::lowest()), v_max.value_or(std::numeric_limits<T>::max()) };
	return SxGui::DragScalarNInternal(label, SxGui::GetImGuiDataType<T>(), v, Component, v_speed, &range.first, &range.second, format, flags);;
}

template <SxGui::ScalarConcept T>
bool SxGui::DragScalar(const char* label, T* v, float v_speed, const std::optional<T>& v_min, const std::optional<T>& v_max, const char* format, ImGuiSliderFlags flags) {
	return SxGui::DragScalarN<T, 1>(label, v, v_speed, v_min, v_max, format, flags);
}

template <SxGui::ScalarConcept T, int32_t Component>
bool SxGui::DragVectorN(const char* label, T v[Component], float v_speed, const std::optional<T>& v_min, const std::optional<T>& v_max, const char* format, ImGuiSliderFlags flags) {
	return SxGui::DragScalarN<T, Component>(label, v, v_speed, v_min, v_max, format, flags | ImGuiSliderFlags_ColorMarkers);
}

template <SxGui::ScalarConcept T, int32_t Component>
void SxGui::SliderScalarN(const char* label, T v[Component], const T v_min, const T v_max, const char* format, ImGuiSliderFlags flags) {
	return SxGui::SliderScalarNInternal(label, SxGui::GetImGuiDataType<T>(), v, Component, &v_min, &v_max, format, flags);
}

template <SxGui::ScalarConcept T>
void SxGui::SliderScalar(const char* label, T* v, const T v_min, const T v_max, const char* format, ImGuiSliderFlags flags) {
	return SxGui::SliderScalarN<T, 1>(label, v, v_min, v_max, format, flags);
}

template <typename T> requires std::is_enum_v<T>
bool SxGui::ComboEnum(const char* label, T* v, ImGuiComboFlags flags) {
	bool changed = false;

	if (ImGui::BeginCombo(label, magic_enum::enum_name(*v).data(), flags)) {
		for (const auto& [value, name] : magic_enum::enum_entries<T>()) {
			if (SxGui::Selectable(name.data(), *v == value)) {
				*v = value;
				changed |= true;
			}
		}
		ImGui::EndCombo();
	}

	return changed;
}

////////////////////////////////////////////////////////////////////////////////////////////
// SxGui Table namespace template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <SxGui::ScalarConcept T, int32_t Component>
bool SxGui::Table::DragScalarN(const std::string& label, T v[Component], float v_speed, const std::optional<T>& v_min, const std::optional<T>& v_max, const char* format, ImGuiSliderFlags flags) {

	bool changed = false;

	SxGui::Table::NextRow();

	const std::string id = "## " + label;

	SxGui::Table::SetColumnIndex(Column::Widget);
	changed = SxGui::DragScalarN<T, Component>(id.c_str(), v, v_speed, v_min, v_max, format, flags);

	SxGui::Table::SetColumnIndex(Column::Label);
	ImGui::Text(label.c_str());

	return changed;
	
}
