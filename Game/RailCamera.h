#pragma once
#define NOMINMAX

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Beta/BaseBehavior.h>
#include <Engine/Beta/ModelBehavior.h>

//* engine
#include <Engine/Beta/CineCamera.h>

//* lib
#include <Lib/Geometry/Vector3.h>

//* c++
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// Rail class
////////////////////////////////////////////////////////////////////////////////////////////
class RailCamera
	: public ModelBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	RailCamera()  = default;
	~RailCamera() = default;

	void Init();

	void Term();

	void Update();

	void Draw();

	void SetAttributeImGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	CineCamera* camera_ = nullptr;

	//* member *//

	float t_ = 0.0f;

	//* parameter *//

	std::vector<Vector3f> points_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void DrawCatmullrom(uint32_t kSubdivision);

};