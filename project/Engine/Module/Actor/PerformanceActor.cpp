#include "PerformanceActor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/System.h>
#include <Engine/Assets/Content/ContentStorage.h>
#include <Engine/Components/Component/Transform/RectTransformComponent.h>
#include <Engine/Components/Component/CanvasRenderer/TextRendererComponent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PerformanceActor class methods
////////////////////////////////////////////////////////////////////////////////////////////

PerformanceActor::PerformanceActor() {
	//* behaviourの設定
	(*this)->SetName("performance actor");

	auto transform = (*this)->AddComponent<SXAVENGER_ENGINE RectTransformComponent>();
	RectTransform& rect = transform->GetTransform();
	rect.translate = {};
	rect.pivot     = { 0, 0 };
	rect.scale     = { 128.0f, 33.0f };

	auto text = (*this)->AddComponent<SXAVENGER_ENGINE TextRendererComponent>();
	text->SetFont(SXAVENGER_ENGINE sContentStorage->Import<SXAVENGER_ENGINE ContentFont>("packages/font/MPLUSRounded1c-Regular.ttf")->GetId());
	text->SetSize(24.0f);
	text->SetText(L"---");
}

void PerformanceActor::Update() {

	auto text = (*this)->AddComponent<SXAVENGER_ENGINE TextRendererComponent>();

	TimePointd<TimeUnit::second> time = SXAVENGER_ENGINE System::GetDeltaTimed();
	double fps = 1.0 / time.time;

	text->SetText(std::format(L"FPS: {:.1f}", fps));

	float t = std::clamp(static_cast<float>(fps), 0.0f, 60.0f) / 60.0f;
	text->SetColor({ std::lerp(0.8f, 0.0f, t), std::lerp(0.0f, 0.8f, t), 0.0f, 1.0f });

}

void PerformanceActor::SetPosition(const Vector2f& position) {
	(*this)->GetComponent<SXAVENGER_ENGINE RectTransformComponent>()->GetTransform().translate = position;
}
