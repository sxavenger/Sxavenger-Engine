#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DxObjectCommon
#include <DxObjectCommon.h>

//* c++
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// DxObject
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderVisibility enum
////////////////////////////////////////////////////////////////////////////////////////////
enum ShaderVisibility {
	VISIBILITY_ALL           = D3D12_SHADER_VISIBILITY_ALL,
	VISIBILITY_VERTEX        = D3D12_SHADER_VISIBILITY_VERTEX,
	VISIBILITY_HULL          = D3D12_SHADER_VISIBILITY_HULL,
	VISIBILITY_DOMAIN        = D3D12_SHADER_VISIBILITY_DOMAIN,
	VISIBILITY_GEOMETRY      = D3D12_SHADER_VISIBILITY_GEOMETRY,
	VISIBILITY_PIXEL         = D3D12_SHADER_VISIBILITY_PIXEL,
	VISIBILITY_AMPLIFICATION = D3D12_SHADER_VISIBILITY_AMPLIFICATION,
	VISIBILITY_MESH          = D3D12_SHADER_VISIBILITY_MESH
};

////////////////////////////////////////////////////////////////////////////////////////////
// SamplerMode enum
////////////////////////////////////////////////////////////////////////////////////////////
enum SamplerMode {
	MODE_WRAP        = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
	MODE_MIRROR      = D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
	MODE_CLAMP       = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
	MODE_BORDER      = D3D12_TEXTURE_ADDRESS_MODE_BORDER,
	MODE_MIRROR_ONCE = D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE
};

////////////////////////////////////////////////////////////////////////////////////////////
// BaseRootSignatureDesc base structure
////////////////////////////////////////////////////////////////////////////////////////////
struct BaseRootSignatureDesc {
public:

	//=========================================================================================
	// methods
	//=========================================================================================

	virtual ~BaseRootSignatureDesc() { Clear(); }

	//* parameter optional *//

	void Resize(uint32_t paramSize, uint32_t samplerSize);

	void Clear();

	//* creates *//

	ComPtr<ID3D12RootSignature> CreateRootSignature(ID3D12Device* device) const;

	//=========================================================================================
	// members
	//=========================================================================================

	std::vector<D3D12_ROOT_PARAMETER>      params;
	std::vector<D3D12_STATIC_SAMPLER_DESC> samplers;

protected:

	std::vector<D3D12_DESCRIPTOR_RANGE> ranges_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsRootSignatureDesc structure
////////////////////////////////////////////////////////////////////////////////////////////
struct GraphicsRootSignatureDesc
	: public BaseRootSignatureDesc {
public:

	//=========================================================================================
	// methods
	//=========================================================================================

	//* virtualAddress *//

	void SetCBV(uint32_t index, ShaderVisibility stage, uint32_t shaderRegister);

	void SetVirtualSRV(uint32_t index, ShaderVisibility stage, uint32_t shaderRegister);

	void SetVirtualUAV(uint32_t index, ShaderVisibility stage, uint32_t shaderRegister);

	//* handle *//

	void SetSRV(uint32_t index, ShaderVisibility stage, uint32_t shaderRegister);

	void SetUAV(uint32_t index, ShaderVisibility stage, uint32_t shaderRegister);

	//* sampler *//

	void SetSampler(uint32_t sampleIndex, SamplerMode mode, ShaderVisibility stage, UINT shaderRegister);

	void SetSamplerAnisotropic(uint32_t sampleIndex, SamplerMode mode, ShaderVisibility stage, UINT shaderRegister, uint32_t anisotropic);

};

////////////////////////////////////////////////////////////////////////////////////////////
// CSRootSignatureDesc structure
////////////////////////////////////////////////////////////////////////////////////////////
struct CSRootSignatureDesc
	: public GraphicsRootSignatureDesc {
public:

	//=========================================================================================
	// methods
	//=========================================================================================

	//* virtualAddress *//

	void SetCBV(uint32_t index, uint32_t shaderRegister);

	void SetVirtualSRV(uint32_t index, uint32_t shaderRegister);

	void SetVirtualUAV(uint32_t index, uint32_t shaderRegister);

	//* handle *//

	void SetSRV(uint32_t index, uint32_t shaderRegister);

	void SetUAV(uint32_t index, uint32_t shaderRegister);

	//* sampler *//

	void SetSampler(uint32_t sampleIndex, SamplerMode mode, UINT shaderRegister);

	void SetSamplerAnisotropic(uint32_t sampleIndex, SamplerMode mode, UINT shaderRegister, uint32_t anisotropic);

};

_DXOBJECT_NAMESPACE_END