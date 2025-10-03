#include "PerformanceActor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Preview/Content/UContentStorage.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PerformanceActor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PerformanceActor::Init(const Vector2f& position) {
	transform_ = MonoBehaviour::AddComponent<RectTransformComponent>();
	text_      = MonoBehaviour::AddComponent<TextRendererComponent>();

	MonoBehaviour::SetName("Performance Actor");
	RectTransform& transform = transform_->GetTransform();
	transform.translate = position;
	transform.pivot     = { 0, 0 };
	transform.scale     = { 128.0f, 33.0f };

	text_->SetFont(sUContentStorage->Import<UContentFont>("assets/font/MPLUSRounded1c-Regular.ttf")->GetId());
	text_->SetSize(32.0f);
	text_->SetText(L"---");
}

void PerformanceActor::Update() {
	
	TimePointd<TimeUnit::second> time = SxavengerSystem::GetDeltaTime();

	std::wstring text;
	text += std::format(L"FPS: {:.1f}", 1.0 / time.time);

	text_->SetText(text);

	double fps = 1.0 / time.time;
	float t = std::clamp(static_cast<float>(fps), 0.0f, 60.0f) / 60.0f;
	text_->SetColor({ std::lerp(0.8f, 0.0f, t), std::lerp(0.0f, 0.8f, t), 0.0f, 1.0f});
}
