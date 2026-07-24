#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxrObject/DxrAccelerationStructure.h>
#include <Engine/System/DirectX/DxrObject/DxrStateObjectContext.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>
#include <Engine/System/DirectX/DirectXAlignment.h>
#include <Engine/Components/Component/ComponentStorage.h>
#include <Engine/Components/Component/MeshRenderer/MeshRendererComponent.h>
#include <Engine/Components/Component/MeshRenderer/SkinnedMeshRendererComponent.h>
#include <Engine/Components/Component/Light/Punctual/DirectionalLightComponent.h>
#include <Engine/Components/Component/Light/Punctual/PointLightComponent.h>
#include <Engine/Components/Component/Light/Punctual/SpotLightComponent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FScene class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 描画対象シーンのTLAS(加速構造)とライト情報をGPU向けに構築・保持するクラス
class FScene {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// LightAddress structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief シェーダーへ渡すライト情報(個数と変換/パラメータのGPUアドレス)
	struct LightAddress {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		uint32_t count;
		D3D12_GPU_VIRTUAL_ADDRESS transforms;
		D3D12_GPU_VIRTUAL_ADDRESS parameters;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// LightContainer structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 同種ライトの変換行列とパラメータをまとめてGPUバッファ化するコンテナ
	//! @tparam T ライト固有のパラメータ型
	template <typename T>
	struct LightContainer {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @brief バッファを初期化する
		//! @param[in] device DirectXデバイス
		void Init(DxObject::Device* device);

		//! @brief ライト数に合わせてバッファサイズを確保する
		//! @param[in] device DirectXデバイス
		//! @param[in] _count 収容するライト数
		void Resize(DxObject::Device* device, uint32_t _count);

		//! @brief シェーダー向けのアドレス情報を取得する
		//! @return ライトのアドレス情報
		LightAddress GetAddress() const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		uint32_t count;
		DxObject::DimensionBuffer<TransformationMatrix> transforms;
		DxObject::DimensionBuffer<T> parameters;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief シーンを初期化する
	void Init();

	//* TLAS option *//

	//! @brief シーン内のメッシュからトップレベル加速構造(TLAS)を構築する
	//! @param[in] context DirectXのキューコンテキスト
	void SetupTopLevelAS(const DirectXQueueContext* context);

	//! @brief 構築済みのTLASを取得する
	//! @return TLASへの参照
	const DxrObject::TopLevelAS& GetTopLevelAS() const { return topLevelAS_; }

	//* state object option *//

	//! @brief レイトレ用のステートオブジェクト(シェーダーテーブル等)を構築する
	void SetupStateObject();

	//* light container option * //

	//! @brief シーン内の全ライトを収集し, 種別ごとのGPUバッファへ格納する
	void SetupLightContainer();

	//* getter *//

	//! @brief 平行光源のアドレス情報を取得する
	//! @return 平行光源のライトアドレス
	LightAddress GetDirectionalLightAddress() const { return directionalLightContainer_.GetAddress(); }

	//! @brief 点光源のアドレス情報を取得する
	//! @return 点光源のライトアドレス
	LightAddress GetPointLightAddress() const { return pointLightContainer_.GetAddress(); }

	//! @brief スポットライトのアドレス情報を取得する
	//! @return スポットライトのライトアドレス
	LightAddress GetSpotLightAddress() const { return spotLightContainer_.GetAddress(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* raytracing parameter *//

	DxrObject::TopLevelAS topLevelAS_;

	//* light container *//

	LightContainer<DirectionalLightComponent::Parameter> directionalLightContainer_;
	LightContainer<PointLightComponent::Parameter>       pointLightContainer_;
	LightContainer<SpotLightComponent::Parameter>        spotLightContainer_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void SetupDirectionalLight();

	void SetupPointLight();

	void SetupSpotLight();

};

////////////////////////////////////////////////////////////////////////////////////////////
// LightContainer structure template methods
////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
inline void FScene::LightContainer<T>::Init(DxObject::Device* device) {
	count = 0;
	transforms.Create(device, 1);
	parameters.Create(device, 1);
}

template<typename T>
inline void FScene::LightContainer<T>::Resize(DxObject::Device* device, uint32_t _count) {
	//!< 引数の保存
	count = _count;

	if (transforms.GetSize() < count) {
		transforms.Create(device, count);
	}

	if (parameters.GetSize() < count) {
		parameters.Create(device, count);
	}
}

template <typename T>
inline FScene::LightAddress FScene::LightContainer<T>::GetAddress() const {
	LightAddress address = {};
	address.count      = count;
	address.transforms = transforms.GetGPUVirtualAddress();
	address.parameters = parameters.GetGPUVirtualAddress();

	return address;
}

SXAVENGER_ENGINE_NAMESPACE_END
