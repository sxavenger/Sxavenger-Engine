#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Module/Behavior/AnimationBehavior.h>

//* engine
#include <Engine/Asset/AssetLibrary/Model/AssetModel.h>
#include <Engine/Content/Animation/AnimationGroup.h>
#include <Engine/Content/Animation/SkeletonMesh.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AnimationDemo class
////////////////////////////////////////////////////////////////////////////////////////////
class AnimationDemo
	: public AnimationBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AnimationDemo()  = default;
	~AnimationDemo() { Term(); }

	void Init();

	void Term();

	void Update();

	virtual void SetAttributeImGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::shared_ptr<AssetModel> model_;

	AnimationGroup group_;
	SkeletonMesh   mesh_;

	DeltaTimePoint<TimeUnit::s> time_ = {};

};