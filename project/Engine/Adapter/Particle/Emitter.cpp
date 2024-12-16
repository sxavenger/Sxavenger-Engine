#include "Emitter.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------

//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Emitter class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Emitter::Init() {
}

void Emitter::Term() {
}

void Emitter::Update() {

	emitTimer_.AddDeltaTime();

	if (emitTimer_ >= emitTime_) {
		emitTimer_ = emitTimer_.Mod(emitTime_);

		if (particle_ != nullptr) {
			for (uint32_t i = 0; i < emitCount_; ++i) {
				particle_->Emit(GetPosition());
			}
		}
	}
}

void Emitter::SetAttributeImGui() {
	if (ImGui::DragFloat3("min", &min_.x, 0.01f)) {
		Min(min_, max_);
	}

	if (ImGui::DragFloat3("max", &max_.x, 0.01f)) {
		Max(min_, max_);
	}

	ImGui::DragFloat("time",  &emitTime_.time, 0.01f, 0.0f, FLT_MAX);
	ImGui::SliderFloat("timer", &emitTimer_.time, 0.0f, emitTime_.time);

	static const uint32_t step = 1;
	ImGui::InputScalar("count", ImGuiDataType_U32, &emitCount_, &step);
}
