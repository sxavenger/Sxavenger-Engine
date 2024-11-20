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

bool ImGuiJsonExporter::DragFloat(const char* label, float* value, float speed, float min, float max, const char* format, ImGuiSliderFlags flags) {
	if (stash_.contains(label)) {
		*value = stash_.at(label);
	}

	bool result = ImGui::DragFloat(label, value, speed, min, max, format, flags);
	SetStash(label, value, 1);

	return result;
}

void ImGuiJsonExporter::SetStash(const char* label, float* value, int32_t components) {
	if (stash_[label].empty()) {
		auto& element = stash_[label] = Json::array();
		element.get_ref<Json::array_t&>().resize(components);
	}

	auto element = stash_[label].begin();

	for (int32_t i = 0; i < components; ++i) {
		element->at(i) = value[i];
	}
}
