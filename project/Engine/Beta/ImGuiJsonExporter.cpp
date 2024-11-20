#include "ImGuiJsonExporter.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Logger.h>

//=========================================================================================
// static variables
//=========================================================================================

const std::string ImGuiJsonExporter::kDirectory_ = "ImGuiJsonExporter/";

////////////////////////////////////////////////////////////////////////////////////////////
// ImGuiJsonExporter class methods
////////////////////////////////////////////////////////////////////////////////////////////

bool ImGuiJsonExporter::TryLoad(const std::string& filename) {
	std::string filepath = kDirectory_;

	if (filename.empty()) {
		filepath += name_ + ".json";

	} else {
		filepath += filename;
	}

	return JsonAdapter::TryLoadJson(filepath, stash_);
}

void ImGuiJsonExporter::Output(const std::string& filename) {
	std::string filepath = kDirectory_;

	if (filename.empty()) {
		filepath += name_ + ".json";

	} else {
		filepath += filename;
	}

	JsonAdapter::WriteJson(filepath, stash_);
}

void ImGuiJsonExporter::Clear() {
	stash_.clear();
}

void ImGuiJsonExporter::SetToStash(const char* label, float* value, int32_t components) {

	auto& element = stash_[label];

	if (stash_[label].empty()) {
		element = Json::array();
		element.get_ref<Json::array_t&>().resize(components);
	}

	Assert(element.is_array(), "element is not json::array. label: " + std::string(label));

	for (int32_t i = 0; i < components; ++i) {
		element.at(i) = value[i];
	}
}

void ImGuiJsonExporter::GetFromStash(const char* label, float* out, int32_t components) {
	if (!stash_.contains(label)) {
		return;
	}

	auto& element = stash_[label];

	Assert(element.is_array() && element.size() >= components, "element not the same type. label: " + std::string(label));

	for (int32_t i = 0; i < components; ++i) {
		out[i] = element.at(i);
	}
}

bool ImGuiJsonExporter::DragFloatN(const char* label, float* value, int32_t components, float speed, float min, float max, const char* format, ImGuiSliderFlags flags) {
	GetFromStash(label, value, components);

	bool result = ImGui::DragScalarN(label, ImGuiDataType_Float, value, components, speed, &min, &max, format, flags);
	SetToStash(label, value, components);

	return result;
}

bool ImGuiJsonExporter::DragFloat(const char* label, float* value, float speed, float min, float max, const char* format, ImGuiSliderFlags flags) {
	return DragFloatN(label, value, 1, speed, min, max, format, flags);
}

bool ImGuiJsonExporter::DragFloat2(const char* label, float* value, float speed, float min, float max, const char* format, ImGuiSliderFlags flags) {
	return DragFloatN(label, value, 2, speed, min, max, format, flags);
}

bool ImGuiJsonExporter::DragFloat3(const char* label, float* value, float speed, float min, float max, const char* format, ImGuiSliderFlags flags) {
	return DragFloatN(label, value, 3, speed, min, max, format, flags);
}

bool ImGuiJsonExporter::DragFloat4(const char* label, float* value, float speed, float min, float max, const char* format, ImGuiSliderFlags flags) {
	return DragFloatN(label, value, 4, speed, min, max, format, flags);
}

bool ImGuiJsonExporter::SliderFloatN(const char* label, float* value, int components, float min, float max, const char* format, ImGuiSliderFlags flags) {
	GetFromStash(label, value, components);

	bool result = ImGui::SliderScalarN(label, ImGuiDataType_Float, value, components, &min, &max, format, flags);
	SetToStash(label, value, components);

	return result;
}

bool ImGuiJsonExporter::SliderFloat(const char* label, float* value, float min, float max, const char* format, ImGuiSliderFlags flags) {
	return SliderFloatN(label, value, 1, min, max, format, flags);
}

bool ImGuiJsonExporter::SliderFloat2(const char* label, float* value, float min, float max, const char* format, ImGuiSliderFlags flags) {
	return SliderFloatN(label, value, 2, min, max, format, flags);
}

bool ImGuiJsonExporter::SliderFloat3(const char* label, float* value, float min, float max, const char* format, ImGuiSliderFlags flags) {
	return SliderFloatN(label, value, 3, min, max, format, flags);
}

bool ImGuiJsonExporter::SliderFloat4(const char* label, float* value, float min, float max, const char* format, ImGuiSliderFlags flags) {
	return SliderFloatN(label, value, 4, min, max, format, flags);
}
