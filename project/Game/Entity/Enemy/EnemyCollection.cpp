#include "EnemyCollection.h"

////////////////////////////////////////////////////////////////////////////////////////////
// EnemyCollection class
////////////////////////////////////////////////////////////////////////////////////////////

void EnemyCollection::Init() {

	for (size_t i = 0; i < enemies_.size(); ++i) {
		enemies_[i] = std::make_unique<Enemy>();

		QuaternionTransform transform = {};
		transform.rotate    = MakeAxisAngle({ 0.0f, 1.0f, 0.0f }, pi_v);
		transform.translate = { static_cast<float>(i) * 2 - 4.0f, 0.0f, 2.0f};

		enemies_[i]->Init(transform);


		enemies_[i]->SetName(std::format("enemy ## ptr: {:p}", reinterpret_cast<const void*>(enemies_[i].get())));
		SetChild(enemies_[i].get());
	}

	SetName("enemy collection");
}

void EnemyCollection::Update() {
	std::for_each(enemies_.begin(), enemies_.end(), [](auto& enemy) { enemy->Update(); });
}

void EnemyCollection::Draw() {
}

void EnemyCollection::SetAttributeImGui() {
}
