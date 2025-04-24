#include "SmokeParticle.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Content/InputGeometry/InputPrimitiveHelper.h>
#include <Engine/Asset/SxavengerAsset.h>

//* lib
#include <Lib/Adapter/Random/Random.h>
#include <Lib/Easing.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SmokeParticle class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SmokeParticle::Load() {
	textures_[0] = SxavengerAsset::TryImport<AssetTexture>("assets/textures/smoke1.png");
	textures_[1] = SxavengerAsset::TryImport<AssetTexture>("assets/textures/smoke2.png");
}

void SmokeParticle::Awake() {
	CreateParticle();
}

void SmokeParticle::Start() {
}

void SmokeParticle::Update() {
	for (size_t i = 0; i < 8; ++i) {
		Emit();
	}
}

void SmokeParticle::CreateParticle() {
	component_ = AddComponent<ParticleComponent>();
	component_->Init(1024, BlendMode::kBlendModeAdd);
	component_->SetPrimitive(InputPrimitiveHelper::CreatePlaneZForward({ 0.8f, 0.8f }));

	AddComponent<TransformComponent>();
	SetName("smoke particle");

	for (size_t i = 0; i < kSubComponentCount; ++i) {
		subParticles_[i] = std::make_unique<MonoBehaviour>();
		subParticles_[i]->SetParent(this);
		subComponents_[i] = subParticles_[i]->AddComponent<ParticleComponent>();
	}

	subComponents_[0]->Init(1024, BlendMode::kBlendModeNormal);
	subComponents_[0]->SetPrimitive(InputPrimitiveHelper::CreatePlaneZForward({ 0.4f, 0.4f }));

	//subComponents_[1]->Init(1024, BlendMode::kBlendModeAdd);
	//subComponents_[1]->SetPrimitive(InputPrimitiveHelper::CreatePlaneZForward({ 0.6f, 0.6f }));
}

void SmokeParticle::Emit() {

	Vector3f position = GetComponent<TransformComponent>()->GetPosition();

	{
		ParticleComponent::Element& element = component_->Emit(position);

		float scaler = Random::Generate(1.0f, 1.6f);
		element.transform.scale = { scaler, scaler, scaler };

		element.velocity.SetStart({ Random::Generate(0.02f, 0.2f), Random::Generate(0.05f, 0.1f), Random::Generate(0.02f, 0.2f) });
		element.velocity.SetEnd({ Random::Generate(0.01f, 0.02f), 0.0f, 0.0f });
		element.velocity.SetInterpolationFunction(EaseOutExpo);

		element.size.SetStart({ 0.0f, 0.0f, 0.0f });
		element.size.SetEnd({ 0.2f, 0.2f, 0.2f });

		element.albedo.SetStart({ 0.0f, 0.4f, 0.0f });
		element.albedo.SetEnd({ 0.0f, 0.2f, 0.0f });

		element.transparent.SetStart(1.0f);
		element.transparent.SetEnd(0.0f);
		element.transparent.SetInterpolationFunction(EaseOutCirc);

		element.transparentIndex = textures_[Random::Generate(0, 1)].WaitGet()->GetDescriptorSRV().GetIndex();

		element.lifeTime = { Random::Generate(3.0f, 4.0f) };

		element.isBillboard = true;
	}
	
	{
		ParticleComponent::Element& element = subComponents_[0]->Emit(position);

		float scaler = Random::Generate(1.0f, 1.2f);
		element.transform.scale = { scaler, scaler, scaler };

		element.velocity.SetStart({ Random::Generate(0.02f, 0.2f), Random::Generate(0.05f, 0.1f), Random::Generate(0.02f, 0.2f) });
		element.velocity.SetEnd({ Random::Generate(0.0f, 0.01f), 0.0f, 0.0f });
		element.velocity.SetInterpolationFunction(EaseOutExpo);

		element.albedo.SetEnd({ 0.1f, 0.1f, 0.1f });
		element.albedo.SetStart({ 1.0f, 1.0f, 1.0f });

		element.transparent.SetStart(1.0f);
		element.transparent.SetEnd(0.0f);
		element.transparent.SetInterpolationFunction(EaseOutCubic);

		element.transparentIndex = textures_[Random::Generate(0, 1)].WaitGet()->GetDescriptorSRV().GetIndex();

		element.lifeTime = { Random::Generate(2.0f, 4.0f) };

		element.isBillboard = true;
	}

}
