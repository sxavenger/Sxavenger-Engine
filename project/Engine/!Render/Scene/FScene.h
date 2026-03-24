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
class FScene {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// LightAddress structure
	////////////////////////////////////////////////////////////////////////////////////////////
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
	template <typename T>
	struct LightContainer {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init(DxObject::Device* device);

		void Resize(DxObject::Device* device, uint32_t _count);

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

	void Init();

	//* TLAS option *//

	void SetupTopLevelAS(const DirectXQueueContext* context);

	const DxrObject::TopLevelAS& GetTopLevelAS() const { return topLevelAS_; }

	//* state object option *//

	void SetupStateObject();

	//* light container option * //

	void SetupLightContainer();

	//* getter *//

	LightAddress GetDirectionalLightAddress() const { return directionalLightContainer_.GetAddress(); }

	LightAddress GetPointLightAddress() const { return pointLightContainer_.GetAddress(); }

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
