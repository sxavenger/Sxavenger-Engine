#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxResource.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>
#include <Engine/Assets/Asset/AssetTexture.h>

//* lib
#include <Lib/Math/Vector2.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FLUTTexture class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 2Dの色変換LUT画像をタイル分割し, 3Dテクスチャとして展開して保持するLUTテクスチャ
class FLUTTexture {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Parameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief LUTの3Dサイズと元2D画像のタイル配置情報
	struct Parameter {
		Vector2ui size;
		Vector2ui tile;
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief 2D LUTテクスチャとタイル数から3D LUT用リソースを生成する
	//! @param[in] texture 元となる2D LUTテクスチャ
	//! @param[in] tile    2D画像のタイル分割数
	void Create(const std::shared_ptr<AssetTexture>& texture, const Vector2ui& tile);

	//! @brief 2D LUTを3Dテクスチャへ展開するコンピュートシェーダーを実行する
	//! @param[in] context DirectXのキューコンテキスト
	void Dispatch(const DirectXQueueContext* context);

	//* getter *//

	//! @brief 3D LUTのSRV GPUハンドルを取得する
	//! @return SRVのGPUディスクリプタハンドルへの参照
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const { return descriptorSRV_.GetGPUHandle(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* 2d texture *//

	Parameter parameter_;
	std::shared_ptr<AssetTexture> texture_;

	//* 3d texture *//

	DxObject::Resource resource_;
	DxObject::Descriptor descriptorSRV_;
	DxObject::Descriptor descriptorUAV_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateResource(const Vector2ui& size, const Vector2ui& tile);
	void CreateDescriptor(const Vector2ui& size, const Vector2ui& tile);

};

SXAVENGER_ENGINE_NAMESPACE_END
