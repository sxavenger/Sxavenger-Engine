#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* lib
#include <Lib/Math/Vector2.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FPresenter class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 最終テクスチャをスワップチェーンへ描画表示する処理を提供する
class FPresenter {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief 指定テクスチャを画面へ表示(present)する
	//! @param[in] context    DirectXのキューコンテキスト
	//! @param[in] resolution 出力解像度
	//! @param[in] handle     表示するテクスチャのSRV GPUハンドル
	static void Present(const DirectXQueueContext* context, const Vector2ui& resolution, const D3D12_GPU_DESCRIPTOR_HANDLE& handle);

};

SXAVENGER_ENGINE_NAMESPACE_END
