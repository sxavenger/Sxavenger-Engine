#include "DirectXCommon.h"
DXOBJECT_USING
_DXROBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// DirectXCommon class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DirectXCommon::Init() {

	//* DXOBJECTの初期化
	device_ = std::make_unique<Device>();
	device_->Init();

	descriptorHeaps_ = std::make_unique<DescriptorHeaps>();
	descriptorHeaps_->Init(device_.get());

	ShaderCompiler::GetInstance()->Init();
}

void DirectXCommon::Term() {
}

