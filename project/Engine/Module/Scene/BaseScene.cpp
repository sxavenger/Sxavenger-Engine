#include "BaseScene.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseScene class methods
////////////////////////////////////////////////////////////////////////////////////////////

bool BaseScene::IsTransition() const {
	return transition_.has_value();
}

const BaseScene::Transition& BaseScene::GetTransition() const {
	// transition_が存在しない場合は例外を投げる
	StreamLogger::AssertA(transition_.has_value(), "BaseScene::GetTransition", "No transition available.");
	return transition_.value();
}

void BaseScene::SetTransition(const Transition& transition) {
	transition_ = transition;
}
