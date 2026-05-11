#include "TransformComponent.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../Entity/EntityBehaviour.h"

//* engine
#include <Engine/System/Utility/StreamLogger.h>
#include <Engine/System/System.h>

////////////////////////////////////////////////////////////////////////////////////////////
// TransformComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

TransformComponent::TransformComponent(EntityBehaviour* behaviour) : BaseComponent(behaviour) {
	buffer_ = std::make_unique<DxObject::ConstantBuffer<TransformationMatrix>>();
	buffer_->Create(System::GetDxDevice());
	buffer_->At() = TransformationMatrix::Identity();
}

void TransformComponent::ShowComponentInspector() {

	
	{ //!< translateの表示
		SxGui::DragVectorN<float, 3>("transform", &transform_.translate.x, 0.01f);
	}

	{ //!< rotateの表示

		//!< eulerの表示
		Vector3f prev = Quaternion::ToEuler(transform_.rotate);
		Vector3f edit = prev;

		if (SxGui::DragVectorN<float, 3>("rotate", &edit.x, 0.01f)) {

			ImGuiIO& io = ImGui::GetIO();

			if (io.WantTextInput) {
				//!< TextInputの場合, Euler角の直接代入.
				transform_.rotate = Quaternion::ToQuaternion(edit);

			} else {
				//!< Dragの場合, Euler角の差分から回転を計算.
				Vector3f delta = edit - prev;

				Vector3<Quaternion> q = {
					Quaternion::AxisAngle(kUnitX3<float>, delta.x),
					Quaternion::AxisAngle(kUnitY3<float>, delta.y),
					Quaternion::AxisAngle(kUnitZ3<float>, delta.z)
				};

				transform_.rotate *= q.x * q.y * q.z;
			}
		}

	}

	
	{ //!< scaleの表示
		SxGui::DragVectorN<float, 3>("scale", &transform_.scale.x, 0.01f);
	}

	UpdateMatrix();
}

const D3D12_GPU_VIRTUAL_ADDRESS TransformComponent::GetGPUVirtualAddress() const {
	StreamLogger::AssertA(buffer_ != nullptr, "transform buffer is not create.");
	return buffer_->GetGPUVirtualAddress();
}

const TransformationMatrix& TransformComponent::GetTransformationMatrix() const {
	StreamLogger::AssertA(buffer_ != nullptr, "transform buffer is not create.");
	return buffer_->At();
}

const Matrix4x4& TransformComponent::GetMatrix() const {
	StreamLogger::AssertA(buffer_ != nullptr, "transform buffer is not create.");
	return buffer_->At().mat;
}

const Vector3f TransformComponent::GetPosition() const {
	return transformation_.translate;
}

const Vector3f TransformComponent::GetDirection() const {
	return transform_.GetForward(); //!< TODO: transformaitonからforwardを計算するように変更.
}

void TransformComponent::UpdateMatrix() {
	transformation_ = transform_.ToTransformation();

	if (const BehaviourAddress& parent = BaseComponent::GetBehaviour()->GetParent()) {
		if (auto component = parent->GetComponent<TransformComponent>()) {
			transformation_ *= component->transformation_;
		}
	}

	TransferGPU();
}

bool TransformComponent::HasParent() const {
	if (const BehaviourAddress& parent = BaseComponent::GetBehaviour()->GetParent()) {
		if (auto component = parent->GetComponent<TransformComponent>()) {
			return true;
		}
	}

	return false;
}

void TransformComponent::TransferGPU() {
	if (buffer_ != nullptr) {
		buffer_->At().Transfer(transformation_);
	}
}

json TransformComponent::ParseToJson() const {
	json component = json::object();
	component["transform"] = transform_.Serialize();

	return component;
}

void TransformComponent::InputJson(const json& data) {
	transform_ = TransformQuaternion::Deserialize(data.at("transform"));
}
