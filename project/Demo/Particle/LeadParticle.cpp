#include "LeadParticle.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Content/InputGeometry/InputPrimitiveHelper.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/System/UI/SxImGui.h>

//* lib
#include <Lib/Adapter/Random/Random.h>
#include <Lib/Easing.h>

////////////////////////////////////////////////////////////////////////////////////////////
// LeadParticle class methods
////////////////////////////////////////////////////////////////////////////////////////////

void LeadParticle::Load() {
	texture_ = SxavengerAsset::TryImport<AssetTexture>("assets/textures/particleDemo.png");
}

void LeadParticle::Awake() {
	component_ = GameObject::AddComponent<ParticleComponent>();
	component_->Init(2048, BlendMode::kBlendModeAdd);
	component_->SetPrimitive(InputPrimitiveHelper::CreatePlaneZForward({ 0.04f, 0.04f }));

	transform_ = GameObject::AddComponent<TransformComponent>();
	SetName("lead particle");

	collider_ = GameObject::AddComponent<ColliderComponent>();
	collider_->SetTag("lead particle");
	collider_->SetColliderBoundingSphere();
}

void LeadParticle::Start() {
	prePosition_ = transform_->GetPosition();
}

void LeadParticle::Update() {

	UpdateEmitter();
	UpdateEmit();
}

void LeadParticle::Inspectable() {
	//SxImGui::DragVector3("min", &min_.x, 0.01f);
	SxImGui::DragFloat("range", &range_, 0.01f, 0.0f);
}

void LeadParticle::SetTarget(const Vector3f& target) {
	targetPosition_ = target;
}

void LeadParticle::UpdateEmitter() {
	if (Vector3f::Distance(targetPosition_, transform_->translate) <= 0.5f) {
		collider_->SetActiveCollider(true);
		return;
	}

	collider_->SetActiveCollider(false);

	Vector3f direction = (targetPosition_ - transform_->GetPosition()).Normalize();
	transform_->translate += direction * speed_;
}

void LeadParticle::UpdateEmit() {
	const uint32_t emitCount = 12;
	for (uint32_t i = 0; i < emitCount; ++i) {
		Vector3f emitPosition = Vector3f::Lerp(prePosition_, transform_->GetPosition(), static_cast<float>(i) / (emitCount - 1));
		Emit(emitPosition);
	}

	prePosition_ = transform_->GetPosition();
}

void LeadParticle::Emit(const Vector3f& position) {

	/*Vector3f range = {
		Random::UniformDistribution(min_.x, max_.x),
		Random::UniformDistribution(min_.y, max_.y),
		Random::UniformDistribution(min_.z, max_.z),
	};*/

	Vector2f angle = {
		Random::UniformDistribution(-kPi, kPi),
		Random::UniformDistribution(-kPi, kPi)
	};

	float radius = Random::NormalDistribution() * range_;

	Vector3f emit = {
		std::sin(angle.x) * std::cos(angle.y),
		std::sin(angle.y),
		std::cos(angle.x) * std::cos(angle.y)
	};
	
	{
		ParticleComponent::Element& element = component_->Emit(position + emit * radius);

		float scaler = 1.0f;
		element.transform.scale = { scaler, scaler, scaler };

		element.velocity.SetStart({ Random::UniformDistribution(-0.004f, 0.004f), Random::UniformDistribution(-0.004f, 0.004f), Random::UniformDistribution(-0.004f, 0.004f) });
		element.velocity.SetEnd({ 0.0f, Random::UniformDistribution(0.005f, 0.01f), 0.0f });
		element.velocity.SetInterpolationFunction(EaseInOutElastic);

		//element.size.SetStart({ 0.0f, 0.0f, 0.0f });
		//element.size.SetEnd({ -0.2f, -0.2f, -0.2f });

		element.albedo.SetStart({ 1.0f, 3.0f, 0.6f });
		element.albedo.SetEnd({ 0.5f, 1.0f, 0.0f });

		element.transparent.SetStart(1.0f);
		element.transparent.SetEnd(0.0f);
		element.transparent.SetInterpolationFunction(EaseOutBounce);

		element.transparentIndex = texture_.WaitGet()->GetDescriptorSRV().GetIndex();
		

		element.lifeTime = { Random::UniformDistribution(4.0f, 10.0f) };

		element.isBillboard = true;
	}

}
