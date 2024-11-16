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
}

void DirectXCommon::Term() {
}


