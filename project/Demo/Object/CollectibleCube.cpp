#include "CollectibleCube.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Content/InputGeometry/InputPrimitiveHelper.h>
#include <Engine/Component/Components/Transform/TransformComponent.h>
#include <Engine/Component/Components/Collider/ColliderComponent.h>
#include <Engine/Component/Components/Particle/ParticleComponent.h>
#include <Engine/Component/Components/Particle/EmitterComponent.h>
#include <Engine/Component/ComponentHelper.h>

//* lib
#include <Lib/Easing.h>
#include <Lib/Adapter/Random/Random.h>

////////////////////////////////////////////////////////////////////////////////////////////
// CollectibleCube class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CollectibleCube::Init() {

	model_ = sUContentStorage->Import<UContentModel>("assets/models/primitive/cube_skeleton.obj");

	MonoBehaviour::SetName("collectible cube");

	// rootの設定
	{
		MonoBehaviour::AddComponent<TransformComponent>();

		auto collider = MonoBehaviour::AddComponent<ColliderComponent>();
		collider->SetColliderBoundingSphere();
		collider->SetTag("item");
	}
	

	// cube1(child)の設定
	{
		std::unique_ptr<MonoBehaviour> cube = std::make_unique<MonoBehaviour>();
		cube_[0] = cube.get();
		MonoBehaviour::AddChild(std::move(cube));

		ComponentHelper::CreateStaticMeshBehaviour(cube_[0], model_.Get());
		ComponentHelper::DetachBehaviourMaterial(cube_[0]);
		ComponentHelper::ModifyBehaviourMaterial(cube_[0], [this](UAssetMaterial* material) {
			material->SetMode(UAssetMaterial::Mode::Emissive);
			material->GetBuffer().albedo.SetValue({ 12.0f, 0.0f, 0.0f });
		});

		auto transform = cube_[0]->AddComponent<TransformComponent>();
		transform->GetTransform().scale  = { 0.2f, 0.2f, 0.2f };
		transform->GetTransform().rotate = Quaternion::ToQuaternion({ Random::UniformDistribution(0.0f, kPi), Random::UniformDistribution(0.0f, kPi), Random::UniformDistribution(0.0f, kPi) });
	}

	// cube2(child)の設定
	{
		std::unique_ptr<MonoBehaviour> cube = std::make_unique<MonoBehaviour>();
		cube_[1] = cube.get();
		MonoBehaviour::AddChild(std::move(cube));

		ComponentHelper::CreateStaticMeshBehaviour(cube_[1], model_.Get());
		ComponentHelper::DetachBehaviourMaterial(cube_[1]);
		ComponentHelper::ModifyBehaviourMaterial(cube_[1], [this](UAssetMaterial* material) {
			material->SetMode(UAssetMaterial::Mode::Emissive);
			material->GetBuffer().albedo.SetValue({ 6.0f, 0.0f, 0.0f });
		});

		auto transform = cube_[1]->AddComponent<TransformComponent>();
		transform->GetTransform().scale = { 0.24f, 0.24f, 0.24f };
		transform->GetTransform().rotate = Quaternion::ToQuaternion({ 0.7f, 0.7f, 0.7f });
	}

	for (size_t i = 0; i < 2; ++i) {
		std::unique_ptr<MonoBehaviour> behaviour = std::make_unique<MonoBehaviour>();
		behaviour->AddComponent<TransformComponent>()->GetTransform().translate = { 0.0f, 0.5f, i == 0 ? 1.0f : -1.0f };

		auto particle = behaviour->AddComponent<ParticleComponent>();
		particle->Init(128, BlendMode::kBlendModeAdd);
		particle->SetPrimitive(InputPrimitiveHelper::CreatePlaneZForward({ 0.08f, 0.08f }));

		particle->SetFunction([]() {
			ParticleComponent::Parameter parameter = {};
			parameter.time        = Random::UniformDistribution(1.0f, 1.5f);
			parameter.transparent = MotionT<float>::CreateInterpolation(0.0f, 1.0f, EaseOutExpo, MotionMode::MirrorOne);
			parameter.velocity    = MotionT<float>::CreateInterpolation(0.0f, 0.2f, EaseInOutExpo, MotionMode::MirrorOne);
			parameter.color       = MotionT<Color3f>::CreateInterpolation({1.0f, 0.0f, 0.0f}, {2.0f, 1.0f, 1.0f}, EaseInOutCubic, MotionMode::MirrorOne);
			return parameter;
		});

		auto emitter = behaviour->AddComponent<EmitterComponent>();
		emitter->SetEmitCount(8);

		cube_[0]->AddChild(std::move(behaviour));
	}

	for (size_t i = 0; i < 2; ++i) {
		std::unique_ptr<MonoBehaviour> behaviour = std::make_unique<MonoBehaviour>();
		behaviour->AddComponent<TransformComponent>()->GetTransform().translate = { 0.0f, 0.5f, i == 0 ? 2.0f : -2.0f };

		auto particle = behaviour->AddComponent<ParticleComponent>();
		particle->Init(128, BlendMode::kBlendModeAdd);
		particle->SetPrimitive(InputPrimitiveHelper::CreatePlaneZForward({ 0.1f, 0.1f }));

		particle->SetFunction([]() {
			ParticleComponent::Parameter parameter = {};
			parameter.time        = Random::UniformDistribution(1.0f, 1.5f);
			parameter.transparent = MotionT<float>::CreateInterpolation(0.0f, 1.0f, EaseOutExpo, MotionMode::MirrorOne);
			parameter.velocity    = MotionT<float>::CreateInterpolation(0.0f, 0.2f, EaseInOutExpo, MotionMode::MirrorOne);
			parameter.color       = MotionT<Color3f>::CreateInterpolation({1.0f, 0.0f, 0.0f}, {12.0f, 0.0f, 0.0f}, EaseInOutCubic, MotionMode::MirrorOne);
			return parameter;
		});

		behaviour->AddComponent<EmitterComponent>();

		cube_[1]->AddChild(std::move(behaviour));
	}

	cube_[0]->GetComponent<TransformComponent>()->GetTransform().rotate *= Quaternion::AxisAngle({ 0.0f, 1.0f, 0.0f }, Random::UniformDistribution(0.0f, 0.1f));
	cube_[1]->GetComponent<TransformComponent>()->GetTransform().rotate *= Quaternion::AxisAngle({ 1.0f, 0.0f, 0.0f }, Random::UniformDistribution(0.0f, 0.1f));
}

void CollectibleCube::Update() {

	cube_[0]->GetComponent<TransformComponent>()->GetTransform().rotate *= Quaternion::AxisAngle({0.0f, 1.0f, 0.0f}, 0.01f);
	cube_[1]->GetComponent<TransformComponent>()->GetTransform().rotate *= Quaternion::AxisAngle({1.0f, 0.0f, 0.0f}, 0.02f);

}
