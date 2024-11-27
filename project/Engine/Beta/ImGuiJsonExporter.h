#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>

//* engine
#include <Engine/System/Logger.h>

//* lib
#include <Lib/Adapter/Json/Json.h>

//* c++
#include <concepts>


////////////////////////////////////////////////////////////////////////////////////////////
// ImGuiJsonExporter class
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
concept ImGuiJsonExportTable = std::is_arithmetic_v<T>;

////////////////////////////////////////////////////////////////////////////////////////////
// ImGuiJsonExporter class
////////////////////////////////////////////////////////////////////////////////////////////
class ImGuiJsonExporter {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ImGuiJsonExporter()  = default;
	~ImGuiJsonExporter() = default;

	void SetName(const std::string& name) { name_ = name; }

	//* stash option *//

	bool TryLoad(const std::string& filename = "");

	void Output(const std::string& filename = "");

	void Clear();

	const Json& GetStash() const { return stash_; }
	Json& GetStash() { return stash_; }

	template <ImGuiJsonExportTable T>
	void SetToStash(const char* label, T* value, int32_t components);

	template <ImGuiJsonExportTable T>
	void GetFromStash(const char* label, T* out, int32_t components);

	//* imgui option *//
	//* drag float
	bool DragFloatN(const char* label, float* value, int32_t components, float speed = 1.0f, float min = 0.0f, float max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool DragFloat(const char* label, float* value, float speed = 1.0f, float min = 0.0f, float max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool DragFloat2(const char* label, float* value, float speed = 1.0f, float min = 0.0f, float max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool DragFloat3(const char* label, float* value, float speed = 1.0f, float min = 0.0f, float max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool DragFloat4(const char* label, float* value, float speed = 1.0f, float min = 0.0f, float max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	//* slider float
	bool SliderFloatN(const char* label, float* value, int32_t components, float min, float max, const char* format = "%.3f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool SliderFloat(const char* label, float* value, float min, float max, const char* format = "%.3f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool SliderFloat2(const char* label, float* value, float min, float max, const char* format = "%.3f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool SliderFloat3(const char* label, float* value, float min, float max, const char* format = "%.3f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool SliderFloat4(const char* label, float* value, float min, float max, const char* format = "%.3f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	//* drag int32_t
	bool DragIntN(const char* label, int32_t* value, int32_t components, float speed = 1.0f, int32_t min = 0, int32_t max = 0, const char* format = "%d", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool DragInt(const char* label, int32_t* value, float speed = 1.0f, int32_t min = 0, int32_t max = 0, const char* format = "%d", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool DragInt2(const char* label, int32_t* value, float speed = 1.0f, int32_t min = 0, int32_t max = 0, const char* format = "%d", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool DragInt3(const char* label, int32_t* value, float speed = 1.0f, int32_t min = 0, int32_t max = 0, const char* format = "%d", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool DragInt4(const char* label, int32_t* value, float speed = 1.0f, int32_t min = 0, int32_t max = 0, const char* format = "%d", ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	//* slider int32_t
	bool SliderIntN(const char* label, int32_t* value, int32_t components, int32_t min, int32_t max, const char* format = "%d", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool SliderInt(const char* label, int32_t* value, int32_t min, int32_t max, const char* format = "%d", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool SliderInt2(const char* label, int32_t* value, int32_t min, int32_t max, const char* format = "%d", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool SliderInt3(const char* label, int32_t* value, int32_t min, int32_t max, const char* format = "%d", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool SliderInt4(const char* label, int32_t* value, int32_t min, int32_t max, const char* format = "%d", ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	//* drag uint32_t
	bool DragUintN(const char* label, uint32_t* value, int32_t components, float speed = 1.0f, uint32_t min = 0, uint32_t max = 0, const char* format = "%u", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool DragUint(const char* label, uint32_t* value, float speed = 1.0f, uint32_t min = 0, uint32_t max = 0, const char* format = "%u", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool DragUint2(const char* label, uint32_t* value, float speed = 1.0f, uint32_t min = 0, uint32_t max = 0, const char* format = "%u", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool DragUint3(const char* label, uint32_t* value, float speed = 1.0f, uint32_t min = 0, uint32_t max = 0, const char* format = "%u", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool DragUint4(const char* label, uint32_t* value, float speed = 1.0f, uint32_t min = 0, uint32_t max = 0, const char* format = "%u", ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	//* slider uint32_t
	bool SliderUintN(const char* label, uint32_t* value, int32_t components, uint32_t min = 0, uint32_t max = 0, const char* format = "%u", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool SliderUint(const char* label, uint32_t* value, uint32_t min = 0, uint32_t max = 0, const char* format = "%u", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool SliderUint2(const char* label, uint32_t* value, uint32_t min = 0, uint32_t max = 0, const char* format = "%u", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool SliderUint3(const char* label, uint32_t* value, uint32_t min = 0, uint32_t max = 0, const char* format = "%u", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool SliderUint4(const char* label, uint32_t* value, uint32_t min = 0, uint32_t max = 0, const char* format = "%u", ImGuiSliderFlags flags = ImGuiSliderFlags_None);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* parameter *//

	std::string name_;
	Json stash_;

	//* config *//

	static const std::string kDirectory_;

	//=========================================================================================
	// private methods
	//=========================================================================================
};

////////////////////////////////////////////////////////////////////////////////////////////
// ImGuiJsonExporter class methods
////////////////////////////////////////////////////////////////////////////////////////////

template<ImGuiJsonExportTable T>
inline void ImGuiJsonExporter::SetToStash(const char* label, T* value, int32_t components) {
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

template<ImGuiJsonExportTable T>
inline void ImGuiJsonExporter::GetFromStash(const char* label, T* out, int32_t components) {
	if (!stash_.contains(label)) {
		return;
	}

	auto& element = stash_[label];

	Assert(element.is_array() && element.size() >= components, "element not the same type. label: " + std::string(label));

	for (int32_t i = 0; i < components; ++i) {
		out[i] = element.at(i);
	}
}
