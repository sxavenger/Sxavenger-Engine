#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>

//* lib
#include <Lib/Adapter/Json/Json.h>
#include <Lib/Geometry/Vector4.h>

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

	void SetToStash(const char* label, float* value, int32_t components);

	void GetFromStash(const char* label, float* out, int32_t components);

	//* imgui option *//

	bool DragFloatN(const char* label, float* value, int32_t components, float speed = 1.0f, float min = 0.0f, float max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool DragFloat(const char* label, float* value, float speed = 1.0f, float min = 0.0f, float max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool DragFloat2(const char* label, float* value, float speed = 1.0f, float min = 0.0f, float max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool DragFloat3(const char* label, float* value, float speed = 1.0f, float min = 0.0f, float max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool DragFloat4(const char* label, float* value, float speed = 1.0f, float min = 0.0f, float max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	bool SliderFloatN(const char* label, float* value, int components, float min, float max, const char* format = "%.3f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool SliderFloat(const char* label, float* value, float min, float max, const char* format = "%.3f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool SliderFloat2(const char* label, float* value, float min, float max, const char* format = "%.3f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool SliderFloat3(const char* label, float* value, float min, float max, const char* format = "%.3f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool SliderFloat4(const char* label, float* value, float min, float max, const char* format = "%.3f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);

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