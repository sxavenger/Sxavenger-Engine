#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT origin
#include "DxObject/DxDevice.h"
#include "DxObject/DxDescriptorHeaps.h"
#include "DxObject/DxShaderCompiler.h"
#include "DxObject/DxBlendState.h"

//* DXOBJECT utility
#include "DxObject/DxDescriptor.h"
#include "DxObject/DxCommandContext.h"
#include "DxObject/DxSwapChain.h"
#include "DxObject/DxShaderBlob.h"
#include "DxObject/DxGraphicsPipelineState.h"
#include "DxObject/DxComputePipelineState.h"

//* DXROBJECT utility
#include "DxrObject/DxrRaytracingBlob.h"

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// DirectXCommon class
////////////////////////////////////////////////////////////////////////////////////////////
class DirectXCommon {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DirectXCommon()  = default;
	~DirectXCommon() { Term(); }

	void Init();

	void Term();

	//* getter *//

	DxObject::Device* GetDevice() const { return device_.get(); }

	DxObject::DescriptorHeaps* GetDesriptorHeaps() const { return descriptorHeaps_.get(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DXOBJECT *//

	static const DxObject::Device::DxLeakChecker leakChecher_;

	std::unique_ptr<DxObject::Device>          device_;
	std::unique_ptr<DxObject::DescriptorHeaps> descriptorHeaps_;
	std::unique_ptr<DxObject::ShaderCompiler>  shaderCompiler_;
	std::unique_ptr<DxObject::BlendState>      blendState_;

};
