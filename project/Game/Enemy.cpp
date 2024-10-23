#include "Enemy.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Enemy class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Enemy::Init(Model* model, const Vector3f& position) {

	SetName(std::format("enemy {:p}", reinterpret_cast<void*>(this)));

	model_ = model;

	transform_.transform.translate = position;
	transform_.UpdateMatrix();
}

void Enemy::Term() {
}

void Enemy::Update() {

	transform_.transform.rotate *= MakeAxisAngle({ 0.0f, 1.0f, 0.0f }, 0.02f);
	transform_.UpdateMatrix();

}

////////////////////////////////////////////////////////////////////////////////////////////
// EnemyCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void EnemyCollection::Init() {

	SetName("enemy collection");


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



}

void EnemyCollection::CreateEnemy(const Vector3f& position) {
	std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();
	newEnemy->Init(enemyModel_, position);

	enemies_.emplace_back(std::move(newEnemy));
}
