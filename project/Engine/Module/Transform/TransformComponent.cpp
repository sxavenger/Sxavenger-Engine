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
		mat *= parent_->mat;
	}

	if (buffer_ != nullptr) {
		(*buffer_)[0].Transfer(mat);
	}
}

const D3D12_GPU_VIRTUAL_ADDRESS& BaseTransformComponent::GetGPUVirtualAddress() const {
	Assert(buffer_ != nullptr, "transform buffer is not craete.");
	return buffer_->GetGPUVirtualAddress();
}

const Vector3f BaseTransformComponent::GetPosition() const {
	Vector3f result = {};
	result.x = mat.m[3][0];
	result.y = mat.m[3][1];
	result.z = mat.m[3][2];
	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////
// TransformComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void TransformComponent::UpdateMatrix() {
	mat = transform.ToMatrix();
	Transfer();
}

void TransformComponent::SetImGuiCommand() {
	transform.SetImGuiCommand();
	UpdateMatrix();
}

json TransformComponent::OutputJson() {
	json root = json::object();
	root["scale"]     = IJsonSerializer::ToJson(transform.scale);
	root["rotate"]    = IJsonSerializer::ToJson(transform.rotate);
	root["translate"] = IJsonSerializer::ToJson(transform.translate);

	return root;
}

void TransformComponent::InputJson(const json& data) {
	transform.scale     = IJsonSerializer::ToVector3f(data.at("scale"));
	transform.rotate    = IJsonSerializer::ToQuaternion(data.at("rotate"));
	transform.translate = IJsonSerializer::ToVector3f(data.at("translate"));
	UpdateMatrix();
}
