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

	//* imgui option *//

	bool DragFloat(const char* label, float* value, float speed = 1.0f, float min = 0.0f, float max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	bool EditColor3(const char* label, float color[3], ImGuiColorEditFlags flags = ImGuiColorEditFlags_None);

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

	void SetStash(const char* label, float* value, int32_t components);
	//void GetStash();

};