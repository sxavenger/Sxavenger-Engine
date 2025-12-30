#include "BehaviourHelper.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* entity
#include "MonoBehaviourStorage.h"

//* component
#include "../Component/Transform/TransformComponent.h"
#include "../Component/Camera/CameraComponent.h"
#include "../Component/Light/Punctual/DirectionalLightComponent.h"
#include "../Component/Light/Punctual/PointLightComponent.h"
#include "../Component/Light/Punctual/SpotLightComponent.h"
#include "../Component/Armature/ArmatureComponent.h"
#include "../Component/MeshRenderer/MeshRendererComponent.h"
#include "../Component/MeshRenderer/SkinnedMeshRendererComponent.h"

////////////////////////////////////////////////////////////////////////////////////////////
// BehaviourHelper class methods
////////////////////////////////////////////////////////////////////////////////////////////

BehaviourAddress BehaviourHelper::Create() {
	return sMonoBehaviourStorage->RegisterBehaviour();
}

BehaviourAddress BehaviourHelper::Create(const std::string& name) {
	BehaviourAddress address = sMonoBehaviourStorage->RegisterBehaviour();
	address->SetName(name);

	return address;
}

void BehaviourHelper::Destroy(BehaviourAddress& address) {
	sMonoBehaviourStorage->PushUnregisterQueue(address);
}

void BehaviourHelper::CreateTransformBehaviour(const BehaviourAddress& address) {
	address->AddComponent<TransformComponent>();
}

BehaviourAddress BehaviourHelper::CreateTransformBehaviour() {
	BehaviourAddress address = BehaviourHelper::Create("behaviour");
	BehaviourHelper::CreateTransformBehaviour(address);
	return address;
}

void BehaviourHelper::CreateCameraBehaviour(const BehaviourAddress& address) {
	address->AddComponent<TransformComponent>();
	address->AddComponent<CameraComponent>();
}

BehaviourAddress BehaviourHelper::CreateCameraBehaviour() {
	BehaviourAddress address = BehaviourHelper::Create("camera");
	BehaviourHelper::CreateCameraBehaviour(address);
	return address;
}

void BehaviourHelper::CreateDirectionalLightBehaviour(const BehaviourAddress& address) {
	address->AddComponent<TransformComponent>();
	address->AddComponent<DirectionalLightComponent>();
}

BehaviourAddress BehaviourHelper::CreateDirectionalLightBehaviour() {
	BehaviourAddress address = BehaviourHelper::Create("directional light");
	BehaviourHelper::CreateDirectionalLightBehaviour(address);

	// 初期値の設定
	auto transform = address->GetComponent<TransformComponent>();
	transform->rotate = Quaternion::AxisAngle(Vector3f{ 1.0f, 0.1f, 0.0f }.Normalize(), kPi / 2.2f);

	return address;
}

void BehaviourHelper::CreatePointLightBehaviour(const BehaviourAddress& address) {
	address->AddComponent<TransformComponent>();
	address->AddComponent<PointLightComponent>();
}

BehaviourAddress BehaviourHelper::CreatePointLightBehaviour() {
	BehaviourAddress address = BehaviourHelper::Create("point light");
	BehaviourHelper::CreatePointLightBehaviour(address);
	return address;
}

void BehaviourHelper::CreateSpotLightBehaviour(const BehaviourAddress& address) {
	address->AddComponent<TransformComponent>();
	address->AddComponent<SpotLightComponent>();
}

BehaviourAddress BehaviourHelper::CreateSpotLightBehaviour() {
	BehaviourAddress address = BehaviourHelper::Create("spot light");
	BehaviourHelper::CreateSpotLightBehaviour(address);
	return address;
}

void BehaviourHelper::CreateStaticMeshBehaviour(const BehaviourAddress& address, const std::shared_ptr<ContentModel>& model) {
	model->WaitComplete();
	// TODO: Waitせず作成する.

	// modelのnodeから階層構造を作成
	CreateStaticMeshBehaviourNode(address, model->GetRoot(), model);
}

BehaviourAddress BehaviourHelper::CreateStaticMeshBehaviour(const std::shared_ptr<ContentModel>& model) {
	BehaviourAddress address = BehaviourHelper::Create();

	BehaviourHelper::CreateStaticMeshBehaviour(address, model);

	return address;
}

void BehaviourHelper::CreateSkinnedMeshBehaviour(const BehaviourAddress& address, const std::shared_ptr<ContentModel>& model) {
	model->WaitComplete();
	// TODO: Waitせず作成する.

	{ //!< ArmatureComponent
		BehaviourAddress armature = BehaviourHelper::Create(ArmatureComponent::kArmatureName);
		armature->SetRenamable(false);
		armature->AddComponent<ArmatureComponent>()->SetSkeleton(model->GetSkeletonId());
		address->AddChild(std::move(armature));
	}

	// meshの登録
	for (size_t i = 0; i < model->GetMeshCount(); ++i) {

		auto mesh     = sAssetStorage->GetAsset<AssetMesh>(model->GetMeshId(i));
		auto material = sAssetStorage->GetAsset<AssetMaterial>(model->GetMeshToMaterialId(i));

		BehaviourAddress child = BehaviourHelper::Create(mesh->GetName());

		MonoBehaviour* ptr = child.Get();
		address->AddChild(std::move(child));

		//!< transform component の追加
		ptr->AddComponent<TransformComponent>();

		//!< skinned mesh renderer component の追加
		auto renderer = ptr->AddComponent<SkinnedMeshRendererComponent>();
		renderer->CreateMesh(mesh->GetId());
		renderer->SetMaterial(material->GetId());
	}

}

BehaviourAddress BehaviourHelper::CreateSkinnedMeshBehaviour(const std::shared_ptr<ContentModel>& model) {
	BehaviourAddress address = BehaviourHelper::Create();

	BehaviourHelper::CreateSkinnedMeshBehaviour(address, model);

	return address;
}

void BehaviourHelper::ApplyAnimation(const BehaviourAddress& address, const Animation& animation, TimePointd<TimeUnit::second> time, bool isLoop) {
	auto child = address->FindChild(ArmatureComponent::kArmatureName);
	child->GetComponent<ArmatureComponent>()->UpdateAnimation(animation, time, isLoop);
}

void BehaviourHelper::ApplyAnimationTransition(
	const BehaviourAddress& address,
	const Animation& prevAnimation, TimePointd<TimeUnit::second> prevTime, bool prevIsLoop,
	const Animation& currAnimation, TimePointd<TimeUnit::second> currTime, bool currIsLoop,
	float transitionT) {
	auto child = address->FindChild(ArmatureComponent::kArmatureName);
	child->GetComponent<ArmatureComponent>()->TransitionAnimation(
		prevAnimation, prevTime, prevIsLoop, currAnimation, currTime, currIsLoop, transitionT
	);
}

void BehaviourHelper::CreateStaticMeshBehaviourNode(const BehaviourAddress& parent, const BornNode& node, const std::shared_ptr<ContentModel>& model) {

	BehaviourAddress child = BehaviourHelper::Create(node.name);

	// transform component の追加
	auto transform = child->AddComponent<TransformComponent>();
	transform->GetTransform() = node.transform;

	if (node.meshIndices.size() == 1) {
		// componentが1つの場合, そのままMeshRendererComponentを追加
		const uint32_t meshIndex = node.meshIndices.front();

		auto mesh     = sAssetStorage->GetAsset<AssetMesh>(model->GetMeshId(meshIndex));
		auto material = sAssetStorage->GetAsset<AssetMaterial>(model->GetMeshToMaterialId(meshIndex));

		auto renderer = child->AddComponent<MeshRendererComponent>();
		renderer->SetMesh(mesh->GetId());
		renderer->SetMaterial(material->GetId());

	} else {
		// componentが一つしか付けられないので苦肉の策
		for (auto& meshIndex : node.meshIndices) {

			auto mesh     = sAssetStorage->GetAsset<AssetMesh>(model->GetMeshId(meshIndex));
			auto material = sAssetStorage->GetAsset<AssetMaterial>(model->GetMeshToMaterialId(meshIndex));

			auto behaviour = BehaviourHelper::Create(mesh->GetName());

			//!< transform component の追加
			behaviour->AddComponent<TransformComponent>();

			//!< mesh renderer component の追加
			auto renderer = behaviour->AddComponent<MeshRendererComponent>();
			renderer->SetMesh(mesh->GetId());
			renderer->SetMaterial(material->GetId());

			child->AddChild(std::move(behaviour));
		}
	}

	// 再帰的に登録
	for (const auto& childNode : node.children) {
		CreateStaticMeshBehaviourNode(child, childNode, model);
	}

	parent->AddChild(std::move(child));
}
