#include "DirectXCommon.h"
_DXOBJECT_USING

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

	compileBlobCollection_ = std::make_unique<CompileBlobCollection>();
	compileBlobCollection_->Init(shaderCompiler_.get());

	blendState_ = std::make_unique<BlendState>();
	blendState_->Init();

	GraphicsPipelineState::SetExternal(compileBlobCollection_.get(), blendState_.get());
}

void DirectXCommon::Term() {
}


