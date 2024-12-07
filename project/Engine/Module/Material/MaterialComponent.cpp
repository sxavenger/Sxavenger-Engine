#include "MaterialComponent.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// MaterialComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void MaterialComponent::CreateBuffer() {
	transformBuffer_ = std::make_unique<DimensionBuffer<Matrix4x4>>();
	transformBuffer_->Create(SxavengerSystem::GetDxDevice(), 1);
	(*transformBuffer_)[0] = Matrix4x4::Identity();

	colorBuffer_ = std::make_unique<DimensionBuffer<Color4f>>();
	colorBuffer_->Create(SxavengerSystem::GetDxDevice(), 1);
	(*colorBuffer_)[0] = kWhite4;
}

void MaterialComponent::Transfer() {
	if (transformBuffer_ != nullptr) {
		(*transformBuffer_)[0] = uvTransform_.ToMatrix();
	}

	if (colorBuffer_ != nullptr) {
		(*colorBuffer_)[0] = color_;
	}
}

void MaterialComponent::SetImGuiCommand() {
	uvTransform_.SetImGuiCommand();
	ImGui::ColorEdit4("color", &color_.r);
	Transfer();
}

json MaterialComponent::OutputJson() {
	json root = json::object();
	root["scale"]     = IJsonSerializer::ToJson(uvTransform_.scale);
	root["rotate"]    = uvTransform_.rotate;
	root["translate"] = IJsonSerializer::ToJson(uvTransform_.translate);
	root["color"]     = IJsonSerializer::ToJson(color_);

	return root;
}

void MaterialComponent::InputJson(const json& data) {
	uvTransform_.scale     = IJsonSerializer::ToVector2f(data.at("scale"));
	uvTransform_.rotate    = data.at("rotate");
	uvTransform_.translate = IJsonSerializer::ToVector2f(data.at("translate"));

	color_ = IJsonSerializer::ToColor4f(data.at("color"));

	Transfer();
}

const D3D12_GPU_VIRTUAL_ADDRESS& MaterialComponent::GetTransformGPUVirtualAddress() const {
	Assert(transformBuffer_ != nullptr, "material buffer is not craete.");
	return transformBuffer_->GetGPUVirtualAddress();
}

const D3D12_GPU_VIRTUAL_ADDRESS& MaterialComponent::GetColorGPUVirtualAddress() const {
	Assert(colorBuffer_ != nullptr, "material buffer is not craete.");
	return colorBuffer_->GetGPUVirtualAddress();
}
