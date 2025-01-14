#include "DirectXCommon.h"
_DXOBJECT_USING
_DXROBJECT_USING

//=========================================================================================
// static variables
//=========================================================================================

const DxObject::Device::DxLeakChecker DirectXCommon::leakChecher_ = {};

////////////////////////////////////////////////////////////////////////////////////////////
// DirectXContext class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DirectXCommon::Init() {
	//* DXOBJECTの初期化
	device_ = std::make_unique<Device>();
	device_->Init();

	descriptorHeaps_ = std::make_unique<DescriptorHeaps>();
	descriptorHeaps_->Init(device_.get());

	shaderCompiler_ = std::make_unique<ShaderCompiler>();
	shaderCompiler_->Init();

	blendState_ = std::make_unique<BlendState>();
	blendState_->Init();

	ShaderBlob::SetExternal(shaderCompiler_.get());
	GraphicsPipelineState::SetExternal(blendState_.get());

	RaytracingBlob::SetExternal(shaderCompiler_.get());
}

void DirectXCommon::Term() {
}


