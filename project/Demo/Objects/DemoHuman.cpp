#include "DemoHuman.h"

////////////////////////////////////////////////////////////////////////////////////////////
// DemoHuman class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DemoHuman::Init() {
	AnimationBehavior::SetName("demo human behavior");

	auto observer1 = SxavengerAsset::TryImport<ModelAnimator>("asset/model/human/walk.gltf");
	observer1.Lock()->Create<Model>();

	model_ = observer1.Lock()->Get<Model>();
	SxavengerAsset::PushTask(model_.Lock());
	model_.Lock()->WaitComplete();

	observer1.Lock()->Create<Animator>();
	animator1_ = observer1.Lock()->Get<Animator>();
	SxavengerAsset::PushTask(animator1_.Lock());
	animator1_.Lock()->WaitComplete();

	auto observer2 = SxavengerAsset::TryImport<ModelAnimator>("asset/model/human/sneakWalk.gltf");
	observer2.Lock()->Create<Animator>();
	animator2_ = observer2.Lock()->Get<Animator>();
	SxavengerAsset::PushTask(animator2_.Lock());
	animator2_.Lock()->WaitComplete();

	skeleton_ = std::make_unique<SkeletonMesh>();
	skeleton_->Create(model_.Lock());

	AnimationBehavior::SetRenderingFlag(BehaviorRenderingType::kSystematic);
	AnimationBehavior::SetRenderingFlag(BehaviorRenderingType::kLateAdaptive);
	AnimationBehavior::skeletonMesh_ = skeleton_.get();
	AnimationBehavior::model_ = model_;

	sprite_.Init();
	sprite_.SetHandle(model_.Lock()->GetTextureHandle(0));
	sprite_.SetSize({ 100.0f, 100.0f });
}

void DemoHuman::Update() {

	time_ += SxavengerSystem::GetDeltaTime();

	if (!model_.Lock()->IsCompleted()) {
		return;
	}

	const float point = 4.0f;
	float time = std::fmod(time_.time, point);

	float t = std::sin((time / point) * pi_v);

	skeleton_->UpdateTransitionAnimation(
		animator1_.Lock()->GetAnimation(0), time_, true,
		animator2_.Lock()->GetAnimation(0), time_, true,
		t
	);
	
}

void DemoHuman::DrawLateAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame) {
	sprite_.Draw();
}
