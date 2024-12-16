#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Module/Behavior/ModelBehavior.h>

//* engine
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Module/Collider/Collider.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Ground class
////////////////////////////////////////////////////////////////////////////////////////////
class Ground
	: public ModelBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Ground()  = default;
	~Ground() = default;

	void Init();

	void Term();

	void Update();

	void DrawSystematic(_MAYBE_UNUSED const SxavGraphicsFrame* frame) override;

	void Wait() { model_->WaitCompleted(); }

	void SetAttributeImGui() override;

private:

	//=========================================================================================
	// public methods
	//=========================================================================================

	std::shared_ptr<AssetModel> model_;

};
