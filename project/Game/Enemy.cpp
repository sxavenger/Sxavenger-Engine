#include "Enemy.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Game/SxavengerGame.h>

//* Lib
#include <Lib/MyMath.h>

//* Game
#include <Game/Player.h>

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
	//transform_.transform.rotate    = ToQuaternion(CalculateEuler(Normalize(velocity)));
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

	//transform_.transform.rotate    *= MakeAxisAngle(Normalize(velocity_), 0.02f);
	transform_.transform.translate += velocity_ * Performance::GetDeltaTime(s).time;
	transform_.UpdateMatrix();

	Collider::position_ = transform_.GetWorldPosition();

	deleteTime_.AddDeltaTime();

	if (deleteTime_ >= deleteTimer_) {
		isDelete_ = true;
	}
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
	enemyModel_[kEnemyType_Star] = SxavengerGame::LoadModel("resources/model", "star.obj");
	enemyModel_[kEnemyType_Rocket] = SxavengerGame::LoadModel("resources/model", "enemy_rocket.obj");
	enemyModel_[kEnemyType_Ballun] = SxavengerGame::LoadModel("resources/model", "balloon.obj");

	for (uint32_t i = 0; i < kCountOfEnemyType; ++i) {
		enemyModel_[i]->ApplyRaytracing();
	}

	PushCommand();

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

	while (!commands_.empty()) {
		const EnemyPopCommand& command = commands_.top();

		if (command.point <= player_->GetPoint()) {
			CreateEnemy(command);

			commands_.pop();
			continue;
		}

		break;
	}
}

void EnemyCollection::SetAttributeImGui() {

	if (ImGui::Button("create")) {
		CreateEnemy(kEnemyType_Cube, {});
	}

	if (ImGui::Button("repop")) {
		PushCommand();
	}
}

void EnemyCollection::CreateEnemy(EnemyType type, const Vector3f& position, const Vector3f& velocity) {
	std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();
	newEnemy->Init(enemyModel_[type], position, velocity);

	SetChild(newEnemy.get());
	enemies_.emplace_back(std::move(newEnemy));
}

void EnemyCollection::CreateEnemy(const EnemyPopCommand& command) {

	Vector3f base = player_->CatmullRomPosition(command.point);

	std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();
	newEnemy->Init(enemyModel_[command.type], command.offset + base, command.velocity);

	SetChild(newEnemy.get());
	enemies_.emplace_back(std::move(newEnemy));
}

void EnemyCollection::CreateEnemyPopCommand(DeltaTimePoint popTime, EnemyType type, const Vector3f& offset, const Vector3f& velocity) {

	EnemyPopCommand command = {};
	command.point    = popTime;
	command.type     = type;
	command.velocity = velocity;
	command.offset   = offset;

	commands_.emplace(command);

}

void EnemyCollection::PushCommand() {

	CreateEnemyPopCommand({ 0.6f }, kEnemyType_Ballun, { 0.2f, 3.0f, 2.0f }, { 0.0f, 0.2f, 0.0f });
	CreateEnemyPopCommand({ 0.4f }, kEnemyType_Ballun, { 0.4f, 2.0f, 0.2f }, { 0.0f, 0.22f, 0.0f });
	CreateEnemyPopCommand({ 1.0f }, kEnemyType_Ballun, { -0.5f, 2.0f, 1.0f }, { 0.0f, 0.3f, 0.0f });

	CreateEnemyPopCommand({ 5.0f }, kEnemyType_Rocket, { -2.0f, 1.0f, 10.0f }, { -0.8f, 0.0f, 0.0f });
	CreateEnemyPopCommand({ 5.0f }, kEnemyType_Rocket, { -2.0f, 1.2f, 12.0f }, { -0.8f, 0.0f, 0.0f });

	CreateEnemyPopCommand({ 8.0f }, kEnemyType_Star, { -1.0f, 0.1f, 2.0f }, { 0.0f, -0.2f, 0.0f });
	CreateEnemyPopCommand({ 8.2f }, kEnemyType_Star, { -2.0f, 0.1f, 2.0f }, { 0.0f, -0.2f, 0.0f });
	CreateEnemyPopCommand({ 8.4f }, kEnemyType_Star, { -3.0f, 0.1f, 2.0f }, { 0.0f, -0.2f, 0.0f });

	CreateEnemyPopCommand({ 13.0f }, kEnemyType_Cube, { 8.0f, 2.0f, -2.0f }, { 1.0f, 0.0f, 0.0f });
	CreateEnemyPopCommand({ 14.2f }, kEnemyType_Cube, { 8.0f, 2.0f, -2.0f }, { 1.0f, 0.0f, -2.0f });
	CreateEnemyPopCommand({ 15.3f }, kEnemyType_Cube, { 8.0f, 2.0f, -2.0f }, { 1.0f, 0.0f, 2.0f });


	CreateEnemyPopCommand({ 17.0f }, kEnemyType_Ballun, { 3.0f, 0.2f, -0.0f }, { 0.0f, 0.2f, 0.0f });
	CreateEnemyPopCommand({ 17.2f }, kEnemyType_Ballun, { 4.2f, 0.2f, -0.2f }, { 0.0f, 0.4f, 0.0f });
	CreateEnemyPopCommand({ 17.4f }, kEnemyType_Ballun, { 5.4f, 0.2f, -0.0f }, { 0.0f, 0.8f, 0.0f });

	CreateEnemyPopCommand({ 25.0f }, kEnemyType_Star, { -4.0f, 0.2f, -10.0f }, { 0.0f, 0.2f, 0.0f });
	CreateEnemyPopCommand({ 25.0f }, kEnemyType_Star, { -4.0f, 0.2f, -11.0f }, { 0.0f, 0.3f, 0.1f });
	CreateEnemyPopCommand({ 25.0f }, kEnemyType_Star, { -4.0f, 0.2f, -12.0f }, { 0.0f, 0.4f, -0.1f });
}
