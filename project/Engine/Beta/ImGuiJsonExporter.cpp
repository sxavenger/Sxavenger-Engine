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

bool ImGuiJsonExporter::SliderFloatN(const char* label, float* value, int32_t components, float min, float max, const char* format, ImGuiSliderFlags flags) {
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

bool ImGuiJsonExporter::DragIntN(const char* label, int32_t* value, int32_t components, float speed, int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
	GetFromStash(label, value, components);

	bool result = ImGui::DragScalarN(label, ImGuiDataType_S32, value, components, speed, &min, &max, format, flags);
	SetToStash(label, value, components);

	return result;
}

bool ImGuiJsonExporter::DragInt(const char* label, int32_t* value, float speed, int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
	return DragIntN(label, value, 1, speed, min, max, format, flags);
}

bool ImGuiJsonExporter::DragInt2(const char* label, int32_t* value, float speed, int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
	return DragIntN(label, value, 2, speed, min, max, format, flags);
}

bool ImGuiJsonExporter::DragInt3(const char* label, int32_t* value, float speed, int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
	return DragIntN(label, value, 3, speed, min, max, format, flags);
}

bool ImGuiJsonExporter::DragInt4(const char* label, int32_t* value, float speed, int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
	return DragIntN(label, value, 4, speed, min, max, format, flags);
}

bool ImGuiJsonExporter::SliderIntN(const char* label, int32_t* value, int32_t components, int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
	GetFromStash(label, value, components);

	bool result = ImGui::SliderScalarN(label, ImGuiDataType_S32, value, components, &min, &max, format, flags);
	SetToStash(label, value, components);

	return result;
}

bool ImGuiJsonExporter::SliderInt(const char* label, int32_t* value, int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
	return SliderIntN(label, value, 1, min, max, format, flags);
}

bool ImGuiJsonExporter::SliderInt2(const char* label, int32_t* value, int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
	return SliderIntN(label, value, 2, min, max, format, flags);
}

bool ImGuiJsonExporter::SliderInt3(const char* label, int32_t* value, int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
	return SliderIntN(label, value, 3, min, max, format, flags);
}

bool ImGuiJsonExporter::SliderInt4(const char* label, int32_t* value, int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
	return SliderIntN(label, value, 4, min, max, format, flags);
}

bool ImGuiJsonExporter::DragUintN(const char* label, uint32_t* value, int32_t components, float speed, uint32_t min, uint32_t max, const char* format, ImGuiSliderFlags flags) {
	GetFromStash(label, value, components);

	bool result = ImGui::DragScalarN(label, ImGuiDataType_U32, value, components, speed, &min, &max, format, flags);
	SetToStash(label, value, components);

	return result;
}

bool ImGuiJsonExporter::DragUint(const char* label, uint32_t* value, float speed, uint32_t min, uint32_t max, const char* format, ImGuiSliderFlags flags) {
	return DragUintN(label, value, 1, speed, min, max, format, flags);
}

bool ImGuiJsonExporter::DragUint2(const char* label, uint32_t* value, float speed, uint32_t min, uint32_t max, const char* format, ImGuiSliderFlags flags) {
	return DragUintN(label, value, 2, speed, min, max, format, flags);
}

bool ImGuiJsonExporter::DragUint3(const char* label, uint32_t* value, float speed, uint32_t min, uint32_t max, const char* format, ImGuiSliderFlags flags) {
	return DragUintN(label, value, 3, speed, min, max, format, flags);
}

bool ImGuiJsonExporter::DragUint4(const char* label, uint32_t* value, float speed, uint32_t min, uint32_t max, const char* format, ImGuiSliderFlags flags) {
	return DragUintN(label, value, 4, speed, min, max, format, flags);
}

bool ImGuiJsonExporter::SliderUintN(const char* label, uint32_t* value, int32_t components, uint32_t min, uint32_t max, const char* format, ImGuiSliderFlags flags) {
	GetFromStash(label, value, components);

	bool result = ImGui::SliderScalarN(label, ImGuiDataType_U32, value, components, &min, &max, format, flags);
	SetToStash(label, value, components);

	return result;
}

bool ImGuiJsonExporter::SliderUint(const char* label, uint32_t* value, uint32_t min, uint32_t max, const char* format, ImGuiSliderFlags flags) {
	return SliderUintN(label, value, 1, min, max, format, flags);
}

bool ImGuiJsonExporter::SliderUint2(const char* label, uint32_t* value, uint32_t min, uint32_t max, const char* format, ImGuiSliderFlags flags) {
	return SliderUintN(label, value, 2, min, max, format, flags);
}

bool ImGuiJsonExporter::SliderUint3(const char* label, uint32_t* value, uint32_t min, uint32_t max, const char* format, ImGuiSliderFlags flags) {
	return SliderUintN(label, value, 3, min, max, format, flags);
}

bool ImGuiJsonExporter::SliderUint4(const char* label, uint32_t* value, uint32_t min, uint32_t max, const char* format, ImGuiSliderFlags flags) {
	return SliderUintN(label, value, 4, min, max, format, flags);
}
