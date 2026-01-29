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

	////////////////////////////////////////////////////////////////////////////////////////////
	// Hierarchy namespace
	////////////////////////////////////////////////////////////////////////////////////////////
	namespace Hierarchy {

		//=========================================================================================
		// methods
		//=========================================================================================

		bool Begin();

		void End();

		bool TreeNode(const std::string& label, bool isSelect, bool isLeaf, ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None);

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
		// Tree Table
		////////////////////////////////////////////////////////////////////////////////////////////

		bool Begin(const std::string& label);

		void End();

		////////////////////////////////////////////////////////////////////////////////////////////
		// widget
		////////////////////////////////////////////////////////////////////////////////////////////

		bool CheckBox(const std::string& label, bool* v);

		template <ScalerConcept T, int32_t Component>
		bool DragScalarN(const std::string& label, T v[Component], float v_speed = 1.0f, const std::optional<T>& v_min = std::nullopt, const std::optional<T>& v_max = std::nullopt, const char* format = GetImGuiFormat<T>(), ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	}
}

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
bool SxGui::Table::DragScalarN(const std::string& label, T v[Component], float v_speed, const std::optional<T>& v_min, const std::optional<T>& v_max, const char* format, ImGuiSliderFlags flags) {
	ImGui::TableNextRow();

	const std::string id = "## " + label;

	ImGui::TableSetColumnIndex(static_cast<int>(Column::Label));
	ImGui::Text(label.c_str());

	ImGui::TableSetColumnIndex(static_cast<int>(Column::Widget));
	std::pair<T, T> range = { v_min.value_or(std::numeric_limits<T>::lowest()), v_max.value_or(std::numeric_limits<T>::max()) };
	return ImGui::DragScalarN(id.c_str(), GetImGuiDataType<T>(), v, Component, v_speed, &range.first, &range.second, format, flags);
}
