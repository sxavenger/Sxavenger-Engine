#include "Enemy.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Game/SxavengerGame.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Enemy class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Enemy::Init(Model* model, const Vector3f& position) {

	SetName(std::format("enemy {:p}", reinterpret_cast<void*>(this)));

	model_ = model;

	transform_.transform.scale     = { 0.2f, 0.2f, 0.2f };
	transform_.transform.translate = position;
	transform_.UpdateMatrix();

	SetColliderBoundingSphere({0.4f});
	Collider::position_ = transform_.GetWorldPosition();
}

void Enemy::Term() {
}

void Enemy::Update() {

	transform_.transform.rotate *= MakeAxisAngle({ 0.0f, 1.0f, 0.0f }, 0.02f);
	transform_.UpdateMatrix();

	Collider::position_ = transform_.GetWorldPosition();

}

void Enemy::SetAttributeImGui() {

	if (ImGui::Button("delete")) {
		isDelete_ = true;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// EnemyCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void EnemyCollection::Init() {

	SetName("enemy collection");

	enemyModel_ = SxavengerGame::LoadModel("resources/model", "enemy.obj");

	CreateEnemy({2.0f, 2.0f, 2.0f});
	CreateEnemy({4.0f, 1.0f, 4.0f});
	CreateEnemy({-2.0f, 1.0f, 1.0f});
}

void EnemyCollection::Term() {
}

void EnemyCollection::Update() {

	enemies_.remove_if([](const std::unique_ptr<Enemy>& enemy) {
		return enemy->IsDelete();
	});

	for (auto& enemy : enemies_) {
		enemy->Update();
	}

}

void EnemyCollection::SetAttributeImGui() {

	if (ImGui::Button("create")) {
		CreateEnemy({});
	}

}

void EnemyCollection::CreateEnemy(const Vector3f& position) {
	std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();
	newEnemy->Init(enemyModel_, position);

	SetChild(newEnemy.get());
	enemies_.emplace_back(std::move(newEnemy));
}
