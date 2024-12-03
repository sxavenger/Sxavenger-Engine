#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Module/Behavior/ModelBehavior.h>

//* engine
#include <Engine/Module/VisualLayer/VisualDoF.h>

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

private:

	//=========================================================================================
	// public methods
	//=========================================================================================

	std::unique_ptr<Model> model_;

};