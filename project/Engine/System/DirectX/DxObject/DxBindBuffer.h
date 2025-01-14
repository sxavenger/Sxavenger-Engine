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
_DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// BindBufferType enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class BindBufferType {
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
struct BindBufferDesc {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* container option *//

	void Clear();

	void SetBuffer(const std::string& name, const GPUBuffer& buffer);
	void SetAddress(const std::string& name, const D3D12_GPU_VIRTUAL_ADDRESS& address);
	void SetHandle(const std::string& name, const D3D12_GPU_DESCRIPTOR_HANDLE& handle);

	void Merge(const BindBufferDesc& desc);

	//* getter *//

	bool Contains(const std::string& name) const;

	const D3D12_GPU_VIRTUAL_ADDRESS& GetAddress(const std::string& name) const;

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetHandle(const std::string& name) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//! [unordered_map]
	//! key:   bufferName
	//! value: buffer
	std::unordered_map<std::string, GPUBuffer> container_;

};


////////////////////////////////////////////////////////////////////////////////////////////
// BindBufferTable class
////////////////////////////////////////////////////////////////////////////////////////////
class BindBufferTable {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BindBufferTable()  = default;
	~BindBufferTable() = default;

	void CreateTable(ID3D12ShaderReflection* reflection, ShaderVisibility visibility);

	GraphicsRootSignatureDesc CreateGraphicsRootSignatureDesc();
	ComPtr<ID3D12RootSignature> CreateGraphicsRootSignature(Device* device);

	ComputeRootSignatureDesc CreateComputeRootSignatureDesc();
	ComPtr<ID3D12RootSignature> CreateComputeRootSignature(Device* device);

	void Reset();

	void BindGraphicsBuffer(CommandContext* context, const BindBufferDesc& desc);
	void BindComputeBuffer(CommandContext* context, const BindBufferDesc& desc);

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// BindBufferInfo structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct BindBufferInfo {
		//* member *//
		std::optional<UINT>   rootParam;
		ShaderVisibility      visibility;
		UINT                  registerNum;
		D3D_SHADER_INPUT_TYPE type; //!< debug用
		BindBufferType        bindBufferType;

		//* methods *//
		void Create(const D3D12_SHADER_INPUT_BIND_DESC& _desc, ShaderVisibility _visibility) {
			rootParam      = std::nullopt;
			visibility     = _visibility;
			registerNum    = _desc.BindPoint;
			type           = _desc.Type;
			bindBufferType = ToBindBufferType(_desc.Type);
		}
	};

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

	void InsertBindBuffer(const D3D12_SHADER_INPUT_BIND_DESC& desc, ShaderVisibility visibility);

};

_DXOBJECT_NAMESPACE_END
