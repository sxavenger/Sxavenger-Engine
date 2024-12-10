#include "Emitter.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Module/SxavengerGraphics/SxavGraphicsFrame.h>
#include <Engine/Module/SxavengerModule.h>

//* lib
#include <Lib/Adapter/Random/Random.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Emitter class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Emitter::Init() {
	SetName("emitter");
	TransformBehavior::SetRenderingFlag(BehaviorRenderingType::kLateAdaptive);

	TransformBehavior::GetTransform().translate.y = 4.0f;
	TransformBehavior::UpdateMatrix();
}

void Emitter::Term() {
}

void Emitter::Update() {

	emitTimer_.AddDeltaTime();

	if (emitTimer_ >= emitTime_) {
		emitTimer_.time = std::fmod(emitTimer_.time, emitTime_.time);

		if (particle_ != nullptr) {
			for (int32_t i = 0; i < emitCount_; ++i) {
				particle_->Emit(GetEmitPosition());
			}
		}
	}
}

void Emitter::SetAttributeImGui() {
	ImGui::DragFloat("emit time", &emitTime_.time, 0.01f, 0.0f, 2.0f);
	ImGui::InputInt("emit count", &emitCount_);
}

void Emitter::DrawLateAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame) {
	if (frame->GetType() == SxavGraphicsFrameType::kDebug) {
		SxavengerModule::DrawBox(GetPosition() + min_, GetPosition() + max_, { 1.0f, 1.0f, 0.0f, 1.0f });
	}
}

const Vector3f Emitter::GetEmitPosition() const {
	Vector3f position = GetPosition();

	position.x += Random::Generate(min_.x, max_.x);
	position.y += Random::Generate(min_.y, max_.y);
	position.z += Random::Generate(min_.z, max_.z);

	return position;
}
