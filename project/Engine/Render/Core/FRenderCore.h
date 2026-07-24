#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FBaseRenderCore.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/Assets/Asset/AssetTexture.h>
#include <Engine/Assets/Asset/AssetParameter.h>

//* lib
#include <Lib/Sxl/SubclassRegistry.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCore class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 各種RenderCoreと共通ルックアップテクスチャ(BRDF LUT/SMAA)を束ねて管理するハブ (シングルトン)
class FRenderCore final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief 共通リソースを初期化する
	void Init();

	//! @brief 保持しているRenderCoreを破棄する
	void Term();

	//! @brief 指定型のRenderCoreを取得する. 無ければ生成・初期化する
	//! @tparam T FBaseRenderCore派生の型
	//! @return RenderCoreへのポインタ
	template <typename T> requires std::derived_from<T, FBaseRenderCore>
	T* EnsureRenderCore();

	//! @brief 指定型のRenderCoreを保持しているかを返す
	//! @tparam T FBaseRenderCore派生の型
	//! @retval true  保持している
	//! @retval false 保持していない
	template <typename T> requires std::derived_from<T, FBaseRenderCore>
	bool HasRenderCore() const { return registry_.Contains<T>(); }

	//* singleton *//

	//! @brief シングルトンインスタンスを取得する
	//! @return インスタンスへのポインタ
	static FRenderCore* GetInstance();

	//* getter *//

	//! @brief 事前計算済みBRDF LUTのSRV GPUハンドルを取得する
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetBRDFLut() const { return brdfLut_.WaitGet()->GetGPUHandleSRV(); }

	//! @brief SMAAのエリアテクスチャのSRV GPUハンドルを取得する
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetSMAAAreaTexture() const { return smaaArea_.WaitGet()->GetGPUHandleSRV(); }

	//! @brief SMAAのサーチテクスチャのSRV GPUハンドルを取得する
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetSMAASearchTexture() const { return smaaSearch_.WaitGet()->GetGPUHandleSRV(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* render core *//

	Sxl::SubclassRegistry<FBaseRenderCore> registry_;

	//* texture *//

	AssetParameter<AssetTexture> brdfLut_;
	AssetParameter<AssetTexture> smaaArea_;
	AssetParameter<AssetTexture> smaaSearch_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCore class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T> requires std::derived_from<T, FBaseRenderCore>
inline T* FRenderCore::EnsureRenderCore() {

	if (!registry_.Contains<T>()) {
		//!< 存在しない場合は生成して初期化する.
		registry_.Insert<T>(); 
		registry_.Get<T>()->Init();
	}

	return registry_.Get<T>();
}

SXAVENGER_ENGINE_NAMESPACE_END


