#include "Player.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyEngine.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Player class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Player::Init() {

	// transform
	transform_.translate = { 0.0f, 3.0f, -12.0f };
	transform_.rotate = { 0.06f, 0.0f, 0.0f };

	worldMatrix_ = transform_.GetMatrix();

	// IA
	model_ = std::make_unique<Model>("./Resources/model", "player.obj");
	
	blas_ = std::make_unique<DxrObject::BottomLevelAS>();
	blas_->Create(
		model_->GetMesh(0).vertexResource.get(), model_->GetMesh(0).indexResource.get(),
		L"player"
	);

	SetThisAttribute("player");

}

void Player::Term() {
}

void Player::Update() {

	Vector3f direction = {  };
	const float speed = 0.04f;

	if (MyEngine::IsPressKey(DIK_W)) {
		direction.z += speed;
	}

	if (MyEngine::IsPressKey(DIK_S)) {
		direction.z -= speed;
	}

	if (MyEngine::IsPressKey(DIK_A)) {
		direction.x -= speed;
	}

	if (MyEngine::IsPressKey(DIK_D)) {
		direction.x += speed;
	}

	if (MyEngine::IsPressKey(DIK_LSHIFT)) {
		direction *= 2.0f;
	}

	Vector3f velocity = Matrix::Transform(direction, Matrix::MakeRotateY(transform_.rotate.y));
	transform_.translate += velocity;

	worldMatrix_ = transform_.GetMatrix();

}

void Player::SetAttributeImGui() {

	ImGui::DragFloat3("scale",     &transform_.scale.x,     0.01f);
	ImGui::DragFloat3("rotate",    &transform_.rotate.x,    0.01f);
	ImGui::DragFloat3("translate", &transform_.translate.x, 0.01f);

	ImGui::Text("// todo: key config");

}
