#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Beta/BaseBehavior.h>
#include <Engine/Beta/ModelInstanceBehavior.h>

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

	Rail()  = default;
	~Rail() = default;

	void Init();

	void Term();

	void Draw();

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