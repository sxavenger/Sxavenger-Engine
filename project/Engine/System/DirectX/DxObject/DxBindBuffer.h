#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxDevice.h"
#include "DxCommandContext.h"
#include "DxRootSignatureDesc.h"

//* DirectX12
#include <d3d12shader.h>

//* c++
#include <unordered_map>
#include <string>
#include <optional>
#include <variant>

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// BindBufferType enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class BindBufferType : uint8_t {
	None,
	k32bitConstants,
	kVirtual_CBV,
	kVirtual_SRV,
	kVirtual_UAV,
	kHandle_SRV,
	kHandle_UAV,
	kSampler
};

////////////////////////////////////////////////////////////////////////////////////////////
// BindBufferDesc structure
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Bind用Bufferの保持する構造体.
struct BindBufferDesc {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Constant32bit structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Constant32bits {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		UINT num32bit;
		const void* data;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
	using BindBuffer = std::variant<
		D3D12_GPU_VIRTUAL_ADDRESS,   //!< virtual address
		D3D12_GPU_DESCRIPTOR_HANDLE, //!< handle
		Constant32bits               //!< 32bit constant
	>;

	////////////////////////////////////////////////////////////////////////////////////////////
	// BufferType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class BufferType : uint8_t {
		VirtualAddress,
		Handle,
		Constant32bits
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* container option *//

	void Clear();

	void SetAddress(const std::string& name, const D3D12_GPU_VIRTUAL_ADDRESS& address);
	void SetHandle(const std::string& name, const D3D12_GPU_DESCRIPTOR_HANDLE& handle);
	void Set32bitConstants(const std::string& name, UINT num32bit, const void* data);

	void Merge(const BindBufferDesc& desc);

	//* getter *//

	bool Contains(const std::string& name) const;

	const Constant32bits& Get32bitConstants(const std::string& name) const;

	const D3D12_GPU_VIRTUAL_ADDRESS& GetAddress(const std::string& name) const;

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetHandle(const std::string& name) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//! [unordered_map]
	//! key:   bufferName
	//! value: buffer
	std::unordered_map<std::string, BindBuffer> container_;
	

};

////////////////////////////////////////////////////////////////////////////////////////////
// SamplerBindDesc structure
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Bind用Sampler情報を保持する構造体.
struct SamplerBindDesc {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SamplerBindDesc()  = default;
	~SamplerBindDesc() = default;

	void SetSamplerLinear(const std::string& name, SamplerMode mode);
	void SetSamplerAnisotropic(const std::string& name, SamplerMode mode, uint32_t anisotropic);
	void SetSamplerPoint(const std::string& name, SamplerMode mode);
	void SetSamplerDesc(const std::string& name, const D3D12_STATIC_SAMPLER_DESC& desc);

	bool Contains(const std::string& name) const;

	D3D12_STATIC_SAMPLER_DESC GetSampler(const std::string& name, ShaderVisibility stage, UINT shaderRegister, UINT registerSpace = 0) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unordered_map<std::string, D3D12_STATIC_SAMPLER_DESC> samplers_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// BindBufferTable class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Bind情報を管理するクラス.
class BindBufferTable {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// BindBufferInfo structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct BindBufferInfo {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================
		
		void Create(ID3D12ShaderReflection* reflection, const D3D12_SHADER_INPUT_BIND_DESC& _desc, ShaderVisibility _visibility);

		//=========================================================================================
		// public variables
		//=========================================================================================

		//* member *//
		std::optional<UINT>   rootParam;
		ShaderVisibility      visibility;
		UINT                  registerNum;
		UINT                  registerSpace;
		D3D_SHADER_INPUT_TYPE type; //!< debug用
		BindBufferType        bindBufferType;
		size_t                num32bit;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BindBufferTable()  = default;
	~BindBufferTable() = default;

	void CreateTable(ID3D12ShaderReflection* reflection, ShaderVisibility visibility);

	GraphicsRootSignatureDesc CreateGraphicsRootSignatureDesc();
	GraphicsRootSignatureDesc CreateGraphicsRootSignatureDesc(const SamplerBindDesc& samplerDesc);
	ComPtr<ID3D12RootSignature> CreateGraphicsRootSignature(Device* device);

	ComputeRootSignatureDesc CreateComputeRootSignatureDesc();
	ComputeRootSignatureDesc CreateComputeRootSignatureDesc(const SamplerBindDesc& samplerDesc);
	ComPtr<ID3D12RootSignature> CreateComputeRootSignature(Device* device);

	void Reset();

	void BindGraphicsBuffer(CommandContext* context, const BindBufferDesc& desc);
	void BindComputeBuffer(CommandContext* context, const BindBufferDesc& desc);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//! [unordered_map]
	//! key:   bufferName
	//! value: buffer情報
	std::unordered_map<std::string, BindBufferInfo> table_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	static BindBufferType ToBindBufferType(D3D_SHADER_INPUT_TYPE type);

	void InsertBindBuffer(ID3D12ShaderReflection* reflection, const D3D12_SHADER_INPUT_BIND_DESC& desc, ShaderVisibility visibility);

};

DXOBJECT_NAMESPACE_END
