#include "ClearScene.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Component/Components/Transform/RectTransformComponent.h>
#include <Engine/Component/Components/SpriteRenderer/SpriteRendererComponent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ClearScene class methods
///////////////////////////////////////////////////////////////////////////////////////////

void ClearScene::Init() {

	fade_ = std::make_unique<MonoBehaviour>();

	auto transform = fade_->AddComponent<RectTransformComponent>();
	transform->GetTransform().scale = kMainWindowSize;
	transform->GetTransform().pivot = { 0.0f, 0.0f };

	fade_->AddComponent<SpriteRendererComponent>();
}

void ClearScene::Update() {

	timer_.AddDeltaTime();

	float alpha = (timer_.time / time_.time);

	auto sprite = fade_->GetComponent<SpriteRendererComponent>();
	sprite->SetColor({ 0.0f, 0.0f, 0.0f, alpha });

	if (timer_ >= time_) {
		Transition transition = {};
		transition.destruction = Transition::Destruction::All;
		transition.names       = { "Game", "Title" };

		BaseScene::SetTransition(transition);
	}

}
