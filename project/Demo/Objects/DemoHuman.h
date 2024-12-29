#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Module/Behavior/AnimationBehavior.h>

//* engine
#include <Engine/Asset/SxavengerAsset.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DemoHuman class
////////////////////////////////////////////////////////////////////////////////////////////
class DemoHuman
	: public AnimationBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DemoHuman()  = default;
	~DemoHuman() = default;

	void Init();

	void Update();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	AssetObserver<Model> model_;
	AssetObserver<Animator> animator1_;
	AssetObserver<Animator> animator2_;

	std::unique_ptr<SkeletonMesh> skeleton_;

	DeltaTimePoint<TimeUnit::s> time_ = {};

	bool flag = true;

};
