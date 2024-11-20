#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Game/Behavior/ModelBehavior.h>

//* engine
#include <Engine/Beta/ImGuiJsonExporter.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Plane class
////////////////////////////////////////////////////////////////////////////////////////////
class Plane
	: public ModelBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Plane()  = default;
	~Plane() { Term(); }

	void Init();

	void Term();

	void SetAttributeImGui() override;

private:

	float value_ = 0.0f;

	ImGuiJsonExporter imguiJsonExporter_;

};