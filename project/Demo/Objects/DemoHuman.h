#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Module/Behavior/AnimationBehavior.h>

//* engine
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Module/Sprite/Sprite.h>

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

	void DrawLateAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	AssetObserver<Model> model_;
	AssetObserver<Animator> animator1_;
	AssetObserver<Animator> animator2_;

	std::unique_ptr<SkeletonMesh> skeleton_;


	TimePointf<TimeUnit::second> time_;

	bool flag = true;

	Sprite sprite_;

};
