#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
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
#include <concepts>
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
		Home          = 0xE88A,
		Cube          = 0xF720,
		ChessPawn     = 0xF3B6,
		Hierarchy     = 0xE97A,
		Menu          = 0xE5D2,
		MenuOpen      = 0xE9BD,
		Layers        = 0xE53B,
		Window        = 0xF088,
		Folder        = 0xE2C7,
		Terminal      = 0xEB8E,
		Timer         = 0xE425,
		Stack         = 0xF500,
		Visibility    = 0xE8F4,
		VisibilityOff = 0xE8F5,
		Delete        = 0xE872,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// internal concepts
	////////////////////////////////////////////////////////////////////////////////////////////

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

	template <ScalerConcept T, int32_t Component>
	bool DragScalarN(const char* label, T v[Component], float v_speed = 1.0f, const std::optional<T>& v_min = std::nullopt, const std::optional<T>& v_max = std::nullopt, const char* format = GetImGuiFormat<T>(), ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	template <ScalerConcept T>
	bool DragScalar(const char* label, T* v, float v_speed = 1.0f, const std::optional<T>& v_min = std::nullopt, const std::optional<T>& v_max = std::nullopt, const char* format = GetImGuiFormat<T>(), ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	bool DragVector2(const char* label, float v[2], float v_speed = 1.0f, const std::optional<float>& v_min = std::nullopt, const std::optional<float>& v_max = std::nullopt, const char* format = "%.3f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	bool DragVector3(const char* label, float v[3], float v_speed = 1.0f, const std::optional<float>& v_min = std::nullopt, const std::optional<float>& v_max = std::nullopt, const char* format = "%.3f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	void ImageLabel(const char* label, ImTextureRef handle, const ImVec2& size);

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
		// widget methods
		////////////////////////////////////////////////////////////////////////////////////////////

		bool CheckBox(const std::string& label, bool* v);

		template <ScalerConcept T, int32_t Component>
		bool DragScalarN(const std::string& label, T v[Component], float v_speed = 1.0f, const std::optional<T>& v_min = std::nullopt, const std::optional<T>& v_max = std::nullopt, const char* format = GetImGuiFormat<T>(), ImGuiSliderFlags flags = ImGuiSliderFlags_None);

		bool DragVector2(const std::string& label, float v[2], float v_speed = 1.0f, const std::optional<float>& v_min = std::nullopt, const std::optional<float>& v_max = std::nullopt, const char* format = "%.3f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);

		bool DragVector3(const std::string& label, float v[3], float v_speed = 1.0f, const std::optional<float>& v_min = std::nullopt, const std::optional<float>& v_max = std::nullopt, const char* format = "%.3f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);

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

template <SxGui::ScalerConcept T>
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

template <SxGui::ScalerConcept T>
constexpr const char* SxGui::GetImGuiFormat() {
	if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, int16_t> || std::is_same_v<T, int32_t>)         return "%d";
	else if constexpr (std::is_same_v<T, uint8_t> || std::is_same_v<T, uint16_t> || std::is_same_v<T, uint32_t>) return "%u";
	else if constexpr (std::is_same_v<T, int64_t>)                                                               return "%lld";
	else if constexpr (std::is_same_v<T, uint64_t>)                                                              return "%llu";
	else if constexpr (std::is_same_v<T, float>)                                                                 return "%.3f";
	else if constexpr (std::is_same_v<T, double>)                                                                return "%.6f";
	else static_assert(false, "Unsupported type for ImGui format.");
}

template <SxGui::ScalerConcept T, int32_t Component>
bool SxGui::DragScalarN(const char* label, T v[Component], float v_speed, const std::optional<T>& v_min, const std::optional<T>& v_max, const char* format, ImGuiSliderFlags flags) {
	std::pair<T, T> range = { v_min.value_or(std::numeric_limits<T>::lowest()), v_max.value_or(std::numeric_limits<T>::max()) };
	return ImGui::DragScalarN(label, SxGui::GetImGuiDataType<T>(), v, Component, v_speed, &range.first, &range.second, format, flags);
}

template <SxGui::ScalerConcept T>
bool SxGui::DragScalar(const char* label, T* v, float v_speed, const std::optional<T>& v_min, const std::optional<T>& v_max, const char* format, ImGuiSliderFlags flags) {
	return SxGui::DragScalarN<T, 1>(label, v, v_speed, v_min, v_max, format, flags);
}

template <SxGui::ScalerConcept T, int32_t Component>
bool SxGui::Table::DragScalarN(const std::string& label, T v[Component], float v_speed, const std::optional<T>& v_min, const std::optional<T>& v_max, const char* format, ImGuiSliderFlags flags) {

	bool changed = false;

	SxGui::Table::NextRow();

	const std::string id = "## " + label;

	SxGui::Table::SetColumnIndex(Column::Widget);
	std::pair<T, T> range = { v_min.value_or(std::numeric_limits<T>::lowest()), v_max.value_or(std::numeric_limits<T>::max()) };
	changed = ImGui::DragScalarN(id.c_str(), GetImGuiDataType<T>(), v, Component, v_speed, &range.first, &range.second, format, flags);

	SxGui::Table::SetColumnIndex(Column::Label);
	ImGui::Text(label.c_str());

	return changed;
}
