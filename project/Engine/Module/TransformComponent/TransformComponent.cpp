#include "TransformComponent.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseTransformComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseTransformComponent::CreateBuffer() {
	buffer_ = std::make_unique<DimensionBuffer<TransformationMatrix>>();
	buffer_->Create(SxavengerSystem::GetDxDevice(), 1);
	(*buffer_)[0].Init();
}

void BaseTransformComponent::Transfer() {
	if (parent_ != nullptr) {
		mat_ *= parent_->mat_;
	}

	if (buffer_ != nullptr) {
		(*buffer_)[0].Transfer(mat_);
	}
}

const D3D12_GPU_VIRTUAL_ADDRESS& BaseTransformComponent::GetGPUVirtualAddress() const {
	Assert(buffer_ != nullptr, "transform buffer is not craete.");
	return buffer_->GetGPUVirtualAddress();
}

const Vector3f BaseTransformComponent::GetPosition() const {
	Vector3f result = {};
	result.x = mat_.m[3][0];
	result.y = mat_.m[3][1];
	result.z = mat_.m[3][2];
	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////
// TransformComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void TransformComponent::UpdateMatrix() {
	mat_ = transform_.ToMatrix();
	Transfer();
}

void TransformComponent::SetImGuiCommand() {
	transform_.SetImGuiCommand();
	UpdateMatrix();
}

json TransformComponent::OutputJson() {
	json root = json::object();
	root["scale"]     = GeometryJsonSerializer::ToJson(transform_.scale);
	root["rotate"]    = GeometryJsonSerializer::ToJson(transform_.rotate);
	root["translate"] = GeometryJsonSerializer::ToJson(transform_.translate);

	return root;
}

void TransformComponent::InputJson(const json& data) {
	transform_.scale     = GeometryJsonSerializer::JsonToVector3f(data.at("scale"));
	transform_.rotate    = GeometryJsonSerializer::JsonToQuaternion(data.at("rotate"));
	transform_.translate = GeometryJsonSerializer::JsonToVector3f(data.at("translate"));
	UpdateMatrix();
}
