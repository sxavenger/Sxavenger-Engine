#include "Score.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Game/SxavengerGame.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Score class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Score::Init() {
	SetName("score");

	BaseBehavior::renderingFlag_ = kBehaviorRender_Adaptive;

	texture_ = Sxavenger::LoadTexture("resources/textures/number_small.png");
}

void Score::Term() {
}

void Score::SetAttributeImGui() {
	ImGui::Text("score: %d", score);

	static float step = 8.0f;
	
	ImGui::InputScalarN("margin", ImGuiDataType_Float, &margin_.x, 2, &step);
}

void Score::DrawAdaptive(_MAYBE_UNUSED const Camera3D* camera) {

	std::string str = std::to_string(score);

	for (size_t i = 0; i < str.size(); ++i) {
		int n = str[i] - '0';

		Vector2f size = texture_->GetSize();
		size.x /= 10.0f;

		Vector2f position = {};
		position.x = size.x * i;

		SxavengerGame::DrawSpriteClip(
			margin_ + position, size, { 0.1f * n, 0.0f }, { -0.9f, 0.0f }, texture_->GetGPUHandleSRV()
		);
	}

	
	
}
