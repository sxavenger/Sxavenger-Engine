#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Game/Behavior/ModelInstanceBehavior.h>
//#include <Engine/Game/Behavior/BaseBehavior.h>

//* lib
#include <Lib/Geometry/Vector3.h>

//* c++
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// Rail class
////////////////////////////////////////////////////////////////////////////////////////////
class Rail
	: public ModelInstanceBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Rail() = default;
	~Rail() = default;

	void Init();

	void Term();

	void DrawAdaptive(_MAYBE_UNUSED const Camera3D* camera) override;

	void SetAttributeImGui() override;

	//* getter *//

	Vector3f LoopCatmullRomPosition(float t);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* rail *//

	std::vector<Vector3f> points_;

	//* behavior *//

	static const uint32_t kInstanceSize_ = 128;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void DrawCatmullrom(uint32_t kSubdivision);


};