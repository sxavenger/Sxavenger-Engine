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
	mat_ = uvTransform_.ToMatrix();
	Transfer();
}

void TransformComponent::SetImGuiCommand() {
	uvTransform_.SetImGuiCommand();
	UpdateMatrix();
}

json TransformComponent::OutputJson() {
	json root = json::object();
	root["scale"]     = IJsonSerializer::ToJson(uvTransform_.scale);
	root["rotate"]    = IJsonSerializer::ToJson(uvTransform_.rotate);
	root["translate"] = IJsonSerializer::ToJson(uvTransform_.translate);

	return root;
}

void TransformComponent::InputJson(const json& data) {
	uvTransform_.scale     = IJsonSerializer::JsonToVector3f(data.at("scale"));
	uvTransform_.rotate    = IJsonSerializer::JsonToQuaternion(data.at("rotate"));
	uvTransform_.translate = IJsonSerializer::JsonToVector3f(data.at("translate"));
	UpdateMatrix();
}
