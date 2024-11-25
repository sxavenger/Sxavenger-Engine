#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"

//* c++
#include <vector>
#include <list>

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// BaseRootSignatureDesc base structure
////////////////////////////////////////////////////////////////////////////////////////////
struct BaseRootSignatureDesc {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BaseRootSignatureDesc()  { Reserve(); }
	~BaseRootSignatureDesc() = default;

	//* container option *//

	void Reserve(uint32_t paramsCapacity = kDefaultParamCapacity_, uint32_t samplersCapacity = kDefaultSamplerCapacity_);

	void Clear();

	void ShrinkToFit();

	//* container setter *//

	void SetVirtual(uint32_t index, ShaderVisibility stage, D3D12_ROOT_PARAMETER_TYPE type, uint32_t shaderRegister);
	void SetHandle(uint32_t index, ShaderVisibility stage, D3D12_DESCRIPTOR_RANGE_TYPE type, uint32_t shaderRegister);

	void SetSamplerLinear(SamplerMode mode, ShaderVisibility stage, UINT shaderRegister);
	void SetSamplerAnisotropic(SamplerMode mode, ShaderVisibility stage, UINT shaderRegister, uint32_t anisotropic);
	void SetSamplerPoint(SamplerMode mode, ShaderVisibility stage, UINT shaderRegister);

	virtual ComPtr<ID3D12RootSignature> CreateRootSignature(ID3D12Device* device, D3D12_ROOT_SIGNATURE_FLAGS flags) const;

	//=========================================================================================
	// public variables
	//=========================================================================================

	std::vector<D3D12_ROOT_PARAMETER>      params;
	std::vector<D3D12_STATIC_SAMPLER_DESC> samplers;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::vector<D3D12_DESCRIPTOR_RANGE> ranges_;

	//* parameter *//

	static const uint32_t kDefaultParamCapacity_   = 32;
	static const uint32_t kDefaultSamplerCapacity_ = 32;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	void AutoResizeParam(uint32_t index);
	void AutoResizeSampler(uint32_t index);

};

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsRootSignatureDesc structure
////////////////////////////////////////////////////////////////////////////////////////////
struct GraphicsRootSignatureDesc
	: public BaseRootSignatureDesc {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* virtual address *//

	void SetVirtualCBV(uint32_t index, ShaderVisibility stage, uint32_t shaderRegister);
	void SetVirtualSRV(uint32_t index, ShaderVisibility stage, uint32_t shaderRegister);
	void SetVirtualUAV(uint32_t index, ShaderVisibility stage, uint32_t shaderRegister);

	//* handle *//

	void SetHandleSRV(uint32_t index, ShaderVisibility stage, uint32_t shaderRegister);
	void SetHandleUAV(uint32_t index, ShaderVisibility stage, uint32_t shaderRegister);

	ComPtr<ID3D12RootSignature> CreateGraphicsRootSignature(ID3D12Device* device) const;
};

////////////////////////////////////////////////////////////////////////////////////////////
// ComputeRootSignatureDesc structure
////////////////////////////////////////////////////////////////////////////////////////////
struct ComputeRootSignatureDesc
	: public BaseRootSignatureDesc {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* virtual address *//

	void SetVirtualCBV(uint32_t index, uint32_t shaderRegister);
	void SetVirtualSRV(uint32_t index, uint32_t shaderRegister);
	void SetVirtualUAV(uint32_t index, uint32_t shaderRegister);

	//* handle *//

	void SetHandleSRV(uint32_t index, uint32_t shaderRegister);
	void SetHandleUAV(uint32_t index, uint32_t shaderRegister);

	ComPtr<ID3D12RootSignature> CreateComputeRootSignature(ID3D12Device* device) const;
};

_DXOBJECT_NAMESPACE_END