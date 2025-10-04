#include "PerspectiveCamera.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Component/Components/PostProcessLayer/PostProcessLayerComponent.h>
#include <Engine/Component/ComponentHelper.h>
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/RenderSceneEditor.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PerspectiveCamera class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PerspectiveCamera::Load() {
}

void PerspectiveCamera::Awake() {

	keyboard_ = SxavengerSystem::GetInput()->GetKeyboardInput();
	mouse_    = SxavengerSystem::GetInput()->GetMouseInput();

	MonoBehaviour::SetName("perspective camera");
	transform_ = MonoBehaviour::AddComponent<TransformComponent>();
	camera_    = MonoBehaviour::AddComponent<CameraComponent>();

	camera_->SetTag(CameraComponent::Tag::GameCamera);

	auto process = MonoBehaviour::AddComponent<PostProcessLayerComponent>();
	process->SetTag(PostProcessLayerComponent::Tag::Local);

	{
		auto exposure = process->AddPostProcess<PostProcessAutoExposure>();
		exposure->GetParameter().minLogLuminance = -10.0f;
		exposure->GetParameter().maxLogLuminance = 20.0f;
		exposure->GetParameter().compensation    = -6.0f;
	}
	
	process->AddPostProcess<PostProcessLocalExposure>(false);
	process->AddPostProcess<PostProcessBloom>();

	{
		light_ = ComponentHelper::CreateSpotLightMonoBehaviour();
		MonoBehaviour::AddChild(light_.get());

		auto light = light_->GetComponent<SpotLightComponent>();
		light->GetParameter().color           = Color3f::Convert(0xF3EAD6);
		light->GetParameter().unit            = LightCommon::Units::Lumen;
		light->GetParameter().intensity       = 10.0f;
		light->GetParameter().radius          = 10000.0f;
		light->GetParameter().coneAngle       = { 0.0f, 0.356f };
		light->GetParameter().shadow.strength = 1.0f;
	}
	

}

void PerspectiveCamera::Start() {

	camera_->GetProjection().focal = 12.0f;

	perspective_ = Perspective::FirstPerson;
	UpdateFirstPersonView();

}

void PerspectiveCamera::Update() {

	switch (perspective_) {
		case Perspective::FirstPerson:
			InputFirstPerson();
			UpdateFirstPersonView();
			break;

		// TODO: ThirdPerson実装, Perspective切り替え時の遷移
	}

	UpdateView();
}

void PerspectiveCamera::Inspectable() {

	SerializeGuiFormatter<float>::DragScalar(sensitivity_, 0.001f, 0.0f, 1.0f);

	SerializeGuiFormatter<float>::DragScalar3(offset_, 0.1f);

}

Vector3f PerspectiveCamera::GetForward() const {
	return transform_->GetTransform().GetForward();
}

void PerspectiveCamera::InputFirstPerson() {

	bool isUpdate = false;

	if (mouse_->IsPress(MouseId::MOUSE_RIGHT)) {
		isUpdate = true;

		// HACK: Editorが表示されている例外の場合の処理はengine側で処理.
		// reference -> Unreal Engineなどの操作の主導権の遷移
		if (sEditorEngine->IsEditorDisplay()) {
			sEditorEngine->ExecuteEditorFunction<RenderSceneEditor>([&](RenderSceneEditor* editor) {
				isUpdate = editor->IsFocusGameWindow();
			});
		}
	}

	if (!isUpdate) {
		return; //!< 更新不要
	}

	// マウス移動量から回転量を計算
	Vector2f delta = mouse_->GetDeltaPosition();
	angle_ += delta * sensitivity_.Get();

	// マウスの制御
	mouse_->SetPosition(static_cast<Vector2i>(kMainWindowSize) / 2);
	mouse_->ShowCousor(false);

	// ホイールでfocus距離を変更
	float wheel = mouse_->GetDeltaWheel();

	if (wheel != 0.0f) {
		camera_->GetProjection().focal += wheel;
		camera_->GetProjection().focal = Clamp(camera_->GetProjection().focal, 12.0f, 40.0f);
	}
}

void PerspectiveCamera::UpdateFirstPersonView() {

	// 回転の制限
	angle_.y = std::clamp(angle_.y, -kPi / 2.5f, kPi / 2.5f);
	angle_.x = std::fmod(angle_.x, kTau);

	rotation_ = Quaternion::AxisAngle(kUnitY3<float>, angle_.x) * Quaternion::AxisAngle(kUnitX3<float>, angle_.y);
	distance_ = 0.0f;

	point_ = offset_;

	if (subject_) {
		point_ += subject_->GetPosition();
	}

}

void PerspectiveCamera::UpdateView() {

	Vector3f direction = Quaternion::RotateVector(kBackward3<float>, rotation_);
	transform_->GetTransform().translate = point_ + direction * distance_;
	transform_->GetTransform().rotate    = rotation_;

}
