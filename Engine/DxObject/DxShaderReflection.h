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
//#include <variant>
#include <unordered_map>
#include <vector>
#include <memory>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "dxcompiler.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// DxObject
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class ShaderBlobManager; //!< manager
class GraphicsBlob; //!*
class CSBlob;       //!*< pipeline用

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderReflection class
////////////////////////////////////////////////////////////////////////////////////////////
class ShaderReflection {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ShaderReflection() = delete;
	ShaderReflection(IDxcBlob* blob) { Init(blob); }

	~ShaderReflection() { Term(); }

	void Init(IDxcBlob* blob);

	void Term();

	//* getter *//

	const std::unordered_map<LPCSTR, D3D_SHADER_INPUT_TYPE>& GetBindBuffer() const { return bindBuffers_; }

	//* external setter *//

	static void SetShaderBlobManager(ShaderBlobManager* manager) { manager_ = manager; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	static ShaderBlobManager* manager_;

	//* ID3D12 *//

	ComPtr<ID3D12ShaderReflection> reflection_;

	//* buffer members *//

	//! [unordered_map]
	//! key:   shaderで使うbuffer名
	//! value: Bufferの種類 ex.ConstantBuffer, StructuredBuffer
	std::unordered_map<LPCSTR, D3D_SHADER_INPUT_TYPE> bindBuffers_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateReflection(IDxcBlob* blob);
	
	void Reflection();

};

////////////////////////////////////////////////////////////////////////////////////////////
// ReflectionPipeline class
////////////////////////////////////////////////////////////////////////////////////////////
class ReflectionPipeline {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void CreatePipeline(GraphicsBlob* graphicsBlob);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::vector<std::unique_ptr<ShaderReflection>> reflections_;

};

_DXOBJECT_NAMESPACE_END