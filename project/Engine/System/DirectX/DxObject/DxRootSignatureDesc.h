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

	void Reset();

	//* container setter *//

	void SetVirtual(uint32_t index, ShaderVisibility stage, D3D12_ROOT_PARAMETER_TYPE type, UINT shaderRegister, UINT registerSpace = 0);
	void SetHandle(uint32_t index, ShaderVisibility stage, D3D12_DESCRIPTOR_RANGE_TYPE type, UINT shaderRegister, UINT registerSpace = 0);
	void Set32bitConstants(uint32_t index, ShaderVisibility stage, UINT num32bit, UINT shaderRegister, UINT registerSpace = 0);

	void SetSamplerDesc(const D3D12_STATIC_SAMPLER_DESC& desc);
	void SetSamplerLinear(SamplerMode mode, ShaderVisibility stage, UINT shaderRegister, UINT registerSpace = 0);
	void SetSamplerAnisotropic(SamplerMode mode, ShaderVisibility stage, UINT shaderRegister, uint32_t anisotropic, UINT registerSpace = 0);
	void SetSamplerPoint(SamplerMode mode, ShaderVisibility stage, UINT shaderRegister, UINT registerSpace = 0);

	virtual ComPtr<ID3D12RootSignature> CreateRootSignature(ID3D12Device* device, D3D12_ROOT_SIGNATURE_FLAGS flags) const;

	//* operator *//

	BaseRootSignatureDesc& operator=(const BaseRootSignatureDesc&) = delete;
	BaseRootSignatureDesc(const BaseRootSignatureDesc&)            = delete;

	BaseRootSignatureDesc& operator=(BaseRootSignatureDesc&&) = default;
	BaseRootSignatureDesc(BaseRootSignatureDesc&&)            = default;

	//=========================================================================================
	// public variables
	//=========================================================================================

	std::vector<D3D12_ROOT_PARAMETER1>     params;
	std::vector<D3D12_DESCRIPTOR_RANGE1>   ranges_;

	std::vector<D3D12_STATIC_SAMPLER_DESC> samplers;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

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

	void SetVirtualCBV(uint32_t index, ShaderVisibility stage, UINT shaderRegister, UINT registerSpace = 0);
	void SetVirtualSRV(uint32_t index, ShaderVisibility stage, UINT shaderRegister, UINT registerSpace = 0);
	void SetVirtualUAV(uint32_t index, ShaderVisibility stage, UINT shaderRegister, UINT registerSpace = 0);

	//* handle *//

	void SetHandleSRV(uint32_t index, ShaderVisibility stage, UINT shaderRegister, UINT registerSpace = 0);
	void SetHandleUAV(uint32_t index, ShaderVisibility stage, UINT shaderRegister, UINT registerSpace = 0);

	ComPtr<ID3D12RootSignature> CreateGraphicsRootSignature(
		ID3D12Device* device,
		D3D12_ROOT_SIGNATURE_FLAGS flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED
	) const;

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

	void SetVirtualCBV(uint32_t index, UINT shaderRegister, UINT registerSpace = 0);
	void SetVirtualSRV(uint32_t index, UINT shaderRegister, UINT registerSpace = 0);
	void SetVirtualUAV(uint32_t index, UINT shaderRegister, UINT registerSpace = 0);

	//* handle *//

	void SetHandleSRV(uint32_t index, UINT shaderRegister, UINT registerSpace = 0);
	void SetHandleUAV(uint32_t index, UINT shaderRegister, UINT registerSpace = 0);

	ComPtr<ID3D12RootSignature> CreateComputeRootSignature(ID3D12Device* device) const;


};

_DXOBJECT_NAMESPACE_END
