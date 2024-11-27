#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Game/Behavior/ModelBehavior.h>


//* c++
#include <memory>

//* game
#include <Game/Plane.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Teapot class
////////////////////////////////////////////////////////////////////////////////////////////
class Teapot
	: public ModelBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void SetAttributeImGui() override;

	void DrawSystematic(_MAYBE_UNUSED const Camera3D* camera) override;
	void DrawAdaptive(_MAYBE_UNUSED const Camera3D* camera) override;

private:

	//=========================================================================================
	// private varibles
	//=========================================================================================

	Vector2f pos_ = { 0.0f, 0.0f };
	Vector2f size_ = { 128.0f, 128.0f };
	float rotate_ = 0.0f;

	static const uint32_t kIntanceCount_ = 2;

	std::unique_ptr<DxObject::BufferResource<TransformationMatrix>> matrix_;

	std::unique_ptr<Plane> plane_;


};