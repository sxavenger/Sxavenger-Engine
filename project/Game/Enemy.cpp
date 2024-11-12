#include "Enemy.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Game/SxavengerGame.h>

//* Lib
#include <Lib/Adapter/Json.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Enemy class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Enemy::Init(Model* model, const Vector3f& position, const Vector3f& velocity, int32_t point) {

	SetName(std::format("enemy {:p}", reinterpret_cast<void*>(this)));

	model_ = model;
	model_->ApplyRaytracing();
	CreateRaytracingRecorder();

	transform_.transform.scale     = { 0.2f, 0.2f, 0.2f };
	transform_.transform.translate = position;
	transform_.UpdateMatrix();

	velocity_ = velocity;
	point_    = point;

	SetColliderBoundingSphere({0.4f});
	Collider::position_ = transform_.GetWorldPosition();

	ModelBehavior::renderingFlag_ = kBehaviorRender_Raytracing | kBehaviorRender_Systematic;

	Collider::typeId_       = kColliderType_Enemy;
	Collider::targetTypeId_ = kColliderType_PlayerBullet;
}

void Enemy::Term() {
}

void Enemy::Update() {

	transform_.transform.rotate    *= MakeAxisAngle({ 0.0f, 1.0f, 0.0f }, 0.02f);
	transform_.transform.translate += velocity_ * Performance::GetDeltaTime(s).time;
	transform_.UpdateMatrix();

	Collider::position_ = transform_.GetWorldPosition();

}

void Enemy::SetAttributeImGui() {

	if (ImGui::Button("delete")) {
		isDelete_ = true;
	}
}

void Enemy::OnCollisionEnter(_MAYBE_UNUSED Collider* const other) {
	isDelete_ = true;
}

////////////////////////////////////////////////////////////////////////////////////////////
// EnemyCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void EnemyCollection::Init() {

	SetName("enemy collection");

	enemyModel_[kEnemyType_Cube] = SxavengerGame::LoadModel("resources/model", "enemy.obj");
	enemyModel_[kEnemyType_Star] = SxavengerGame::LoadModel("resources/model", "enemy_star.obj");

	CreateEnemyPopCommand({ 2.0f }, kEnemyType_Star,{ 2.0f, 2.0f, 2.0f }, { 0.0f, -0.2f, 0.0f });
	CreateEnemyPopCommand({ 4.0f }, kEnemyType_Cube, { 0.0f, 3.0f, 10.0f }, { 0.2f, 0.0f, 0.0f });
	CreateEnemyPopCommand({ 8.0f }, kEnemyType_Star, { -2.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.2f });
	CreateEnemyPopCommand({ 16.0f }, kEnemyType_Cube, { 0.0f, 2.0f, 10.0f }, { -0.2f, 0.0f, 0.0f });

	Json root = Json::object();

	root["command"] = Json::array();

}

void EnemyCollection::Term() {
}

void EnemyCollection::Update() {

	// enemies update
	enemies_.remove_if([](const std::unique_ptr<Enemy>& enemy) {
		return enemy->IsDelete();
	});

	for (auto& enemy : enemies_) {
		enemy->Update();
	}

	// command update
	popTimer_.AddDeltaTime();

	for (auto commandIt = commands_.begin(); commandIt != commands_.end();) {

		const auto& command = (*commandIt);

		if (command.popTime <= popTimer_) {

			CreateEnemy(command.type, command.position, command.velocity);

			// commandから削除
			commandIt = commands_.erase(commandIt);
			continue;
		}

		++commandIt;
	}

}

void EnemyCollection::SetAttributeImGui() {

	if (ImGui::Button("create")) {
		CreateEnemy(kEnemyType_Cube, {});
	}

	if (ImGui::Button("repop")) {
		CreateEnemyPopCommand({ 2.0f }, kEnemyType_Star, { 2.0f, 2.0f, 2.0f }, { 0.0f, -0.2f, 0.0f });
		CreateEnemyPopCommand({ 4.0f }, kEnemyType_Star, { 0.0f, 3.0f, 10.0f }, { 0.2f, 0.0f, 0.0f });
		CreateEnemyPopCommand({ 8.0f }, kEnemyType_Star, { -2.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.2f });
		CreateEnemyPopCommand({ 16.0f }, kEnemyType_Star, { 0.0f, 2.0f, 10.0f }, { -0.2f, 0.0f, 0.0f });
	}
}

void EnemyCollection::CreateEnemy(EnemyType type, const Vector3f& position, const Vector3f& velocity) {
	std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();
	newEnemy->Init(enemyModel_[type], position, velocity);

	SetChild(newEnemy.get());
	enemies_.emplace_back(std::move(newEnemy));
}

void EnemyCollection::CreateEnemyPopCommand(DeltaTimePoint popTime, EnemyType type, const Vector3f& position, const Vector3f& velocity) {

	EnemyPopCommand command = {};
	command.popTime  = popTime;
	command.type     = type;
	command.velocity = velocity;
	command.position = position;

	commands_.emplace_back(command);

}
