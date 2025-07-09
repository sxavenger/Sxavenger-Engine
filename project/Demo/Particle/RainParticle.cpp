#include "RainParticle.h"

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
// RainParticle class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RainParticle::Load() {
	texture_ = SxavengerAsset::TryImport<AssetTexture>("assets/textures/particleDemo.png");
}

void RainParticle::Awake() {
	CreateParticle();
}

void RainParticle::Start() {
}

void RainParticle::Update() {
	for (size_t i = 0; i < 12; ++i) {
		Emit();
	}
}

void RainParticle::CreateParticle() {
	component_ = AddComponent<ParticleComponent>();
	component_->Init(2048, BlendMode::kBlendModeAdd);
	component_->SetPrimitive(InputPrimitiveHelper::CreatePlaneZForward({ 0.01f, 1.0f }));

	AddComponent<TransformComponent>();
	SetName("rain particle");
}

void RainParticle::Emit() {

	Vector3f position = GetComponent<TransformComponent>()->GetPosition();

	{

		position.x += Random::UniformDistribution(-12.0f, 12.0f);
		position.z += Random::UniformDistribution(-12.0f, 12.0f);

		ParticleComponent::Element& element = component_->Emit(position);
		element.velocity.SetStart({ 0.0f, -9.0f, 0.0f });
		element.velocity.SetEnd({ 0.0f, -9.0f, 0.0f });

		element.albedo.SetStart(kWhite3<float>);
		element.albedo.SetEnd(kWhite3<float>);

		element.transparent.SetStart(1.0f);
		element.transparent.SetEnd(1.0f);

		element.lifeTime = { 2.0f };

		element.transparentIndex = texture_.WaitGet()->GetDescriptorSRV().GetIndex();

		element.isBillboard = false;
	}

}
