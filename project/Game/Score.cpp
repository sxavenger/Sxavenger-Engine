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

	texture_ = Sxavenger::LoadTexture("resources/textures/score_numbres.png");

	Sxavenger::LoadTexture("resources/white1x1.png");
}

void Score::Term() {
}

void Score::Update() {

	Vector2f proj = ToProjection();
	Vector3f position = Matrix::Transform({ proj.x, proj.y, 0.6f }, camera_->GetViewProjMatrix().Inverse());

	transform_.transform.translate = position;
	transform_.transform.scale     = { 0.1f, 0.1f, 0.1f };
	transform_.transform.rotate   *= MakeAxisAngle({0.0f, 1.0f, 0.0f}, 0.02f);
	transform_.UpdateMatrix();

	if (destroyTimer_.time <= 0.0f) {
		return;
	}

	destroyTimer_.SubtractDeltaTime();


	if (destroyTimer_.time <= 0.0f) {
		model_ = nullptr;
	}
}

void Score::SetAttributeImGui() {
	ImGui::Text("score: %d", score);

	static float step = 8.0f;
	
	ImGui::InputScalarN("margin", ImGuiDataType_Float, &margin_.x, 2, &step);
}

void Score::DrawAdaptive(_MAYBE_UNUSED const Camera3D* camera) {
	ModelBehavior::DrawAdaptive(camera);

	uint32_t displayScore = std::min<uint32_t>(score, 9999);
	std::string str = std::to_string(displayScore);

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

	/*SxavengerGame::DrawSprite(
		margin_, { 240, 80 }, Sxavenger::GetTextureHandleGPU("resources/white1x1.png")
	);*/
	
}

void Score::AddScore(uint32_t addScore, Model* destroyEnemyModel) {

	score += addScore;

	model_ = destroyEnemyModel;
	destroyTimer_ = { 10.0f };

	transform_.transform.rotate = Quaternion::Identity();
	transform_.UpdateMatrix();
}

Vector2f Score::ToProjection() {
	return {
		(displayDestroyModelPos_.x / kWindowSize.x) * 2.0f - 1.0f,
		1.0f - (displayDestroyModelPos_.y / kWindowSize.y) * 2.0f
	};
}
