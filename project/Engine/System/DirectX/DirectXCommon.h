#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObject/DxDevice.h"
#include "DxObject/DxDescriptorHeaps.h"
#include "DxObject/DxShaderCompiler.h"
#include "DxObject/DxCompileBlobCollection.h"
#include "DxObject/DxBlendState.h"

//* DXOBJECT utility
#include "DxObject/DxDescriptor.h"
#include "DxObject/DxCommandContext.h"
#include "DxObject/DxSwapChain.h"
#include "DxObject/DxGraphicsPipelineState.h"
#include "DxObject/DxComputePipelineState.h"

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

	DxObject::CompileBlobCollection* GetCompileBlobCollection() const { return compileBlobCollection_.get(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DXOBJECT *//

	DxObject::Device::DxLeakChecker leakChecher_;

	std::unique_ptr<DxObject::Device>                device_;
	std::unique_ptr<DxObject::DescriptorHeaps>       descriptorHeaps_;
	std::unique_ptr<DxObject::ShaderCompiler>        shaderCompiler_;
	std::unique_ptr<DxObject::CompileBlobCollection> compileBlobCollection_;
	std::unique_ptr<DxObject::BlendState>            blendState_;

};