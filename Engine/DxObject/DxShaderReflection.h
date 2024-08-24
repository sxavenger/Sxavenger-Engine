#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DxObjectCommon
#include <DxObjectCommon.h>

//* DirectX12
#include <dxcapi.h>
#include <d3d12shader.h>

//* c++
#include <variant>
#include <unordered_map>
#include <vector>
#include <memory>

//* DxObject
#include <DxShaderBlob.h>
#include <DxRootSignatureDesc.h>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "dxcompiler.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// DxObject
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderReflectionTable class
////////////////////////////////////////////////////////////////////////////////////////////
class ShaderReflectionTable
	: public BaseShaderBlob {
public: //!< enviornment

	////////////////////////////////////////////////////////////////////////////////////////////
	// BufferInfo structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct BufferInfo {
		D3D_SHADER_INPUT_TYPE bufferType;
		UINT                  registerNum;
		ShaderVisibility      visibility;
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// BindBuffer using
	////////////////////////////////////////////////////////////////////////////////////////////
	using BindBuffer = std::variant<D3D12_GPU_VIRTUAL_ADDRESS, D3D12_GPU_DESCRIPTOR_HANDLE>;

	////////////////////////////////////////////////////////////////////////////////////////////
	// BufferTable structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct BufferTable {
		BufferInfo                info;
		std::optional<BindBuffer> buffer     = std::nullopt;
		std::optional<uint32_t>   paramIndex = std::nullopt;
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ShaderReflectionTable() = default;
	~ShaderReflectionTable() = default;

	//* info option *//

	void Create(IDxcBlob* blob, ShaderVisibility visibility);

	void Marge(ShaderReflectionTable* other);

	//* bind buffer *//

	void Bind(const std::string& bufferName, const BindBuffer& buffer);

	void BindGraphicsParameter(ID3D12GraphicsCommandList* commandList);

	//* getter *//

	GraphicsRootSignatureDesc CreateRootSignatureDesc();

	const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetElement() const { return elements_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================
	
	//* buffer table *//

	//! [unordered_map]
	//! key:   bindされてるbufferの名前
	//! value: buffer情報, buffer自体
	std::unordered_map<std::string, BufferTable> table_;

	uint32_t samplerCount_ = 0;

	//* input element *//

	std::vector<D3D12_INPUT_ELEMENT_DESC> elements_;
	std::list<std::string>                elementNameStr_; //!< stringの延命

	//=========================================================================================
	// private methods
	//=========================================================================================

	void MargeBufferInfo(const std::string& key, const BufferInfo& value);

	ComPtr<ID3D12ShaderReflection> CreateReflection(IDxcBlob* blob);

	void ReflectBuffer(ID3D12ShaderReflection* reflection, ShaderVisibility visibility);
	void ReflectInput(ID3D12ShaderReflection* reflection, ShaderVisibility visibility);

	DXGI_FORMAT GetFormat(D3D_REGISTER_COMPONENT_TYPE type, BYTE mask) const;
};

_DXOBJECT_NAMESPACE_END