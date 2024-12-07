#include "Emitter.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Module/SxavengerGraphics/SxavGraphicsFrame.h>
#include <Engine/Module/SxavengerModule.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Emitter class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Emitter::Init() {
	SetName("emitter");
	TransformBehavior::renderingFlag_ = kBehaviorRendering_LateAdaptive;
}

void Emitter::Term() {
}

void Emitter::DrawLateAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame) {
	if (frame->GetType() == SxavGraphicsFrameType::kDebug) {
		SxavengerModule::DrawBox(GetPosition() + min_, GetPosition() + max_, { 1.0f, 1.0f, 0.0f, 1.0f });
	}
}
