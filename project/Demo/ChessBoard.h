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
// ChessBoard class
////////////////////////////////////////////////////////////////////////////////////////////
class ChessBoard
	: public ModelBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ChessBoard()  = default;
	~ChessBoard() = default;

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

	std::unique_ptr<Collider> collider_;

};