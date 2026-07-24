#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXROBJECT
#include "DxrObjectCommon.h"

//* DXOBJECT
#include <Engine/System/DirectX/DxObject/DxShaderCompiler.h>

//* DirectX12
#include <dxcapi.h>

//* c++
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// DXROBJECT namespace
////////////////////////////////////////////////////////////////////////////////////////////
DXROBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// RaytracingBlob class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief レイトレ用シェーダー(.hlsl)をコンパイルし, バイトコードとして保持するBlob
class RaytracingBlob {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	RaytracingBlob()  = default;
	~RaytracingBlob() = default;

	//! @brief 指定パスのシェーダーをコンパイルしてBlobを生成する
	//! @param[in] filepath シェーダーファイルのパス
	void Create(const std::filesystem::path& filepath);

	//* getter *//

	//! @brief シェーダーバイトコードを取得する
	//! @return D3D12_SHADER_BYTECODE
	D3D12_SHADER_BYTECODE GetBytecode() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* blob *//
	 
	ComPtr<IDxcBlob> blob_;

};

DXROBJECT_NAMESPACE_END
