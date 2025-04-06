#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/Asset/Observer/AssetObserver.h>
#include <Engine/Asset/Assets/Texture/AssetTexture.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// SkyLightComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class SkyLightComponent
	: public BaseComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Parameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Parameter {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// Type enum class
		////////////////////////////////////////////////////////////////////////////////////////////
		enum class Type : uint32_t {
			None,
			TextureCube,
			Texture2d
		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init();

		void SetTextureCube(uint32_t _index);

		void SetTexture2d(uint32_t _index);

		void SetTexture(const AssetObserver<AssetTexture>& texture);

		//=========================================================================================
		// public variables
		//=========================================================================================

		Type type      = Type::None;
		uint32_t index = NULL;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SkyLightComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) { Init(); }
	virtual ~SkyLightComponent() = default;

	virtual void ShowComponentInspector() override;

	void Init();

	//* getter *//

	const D3D12_GPU_VIRTUAL_ADDRESS& GetParameterBufferAddress() const;

	const Parameter& GetParameter() const;
	Parameter& GetParameter();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::DimensionBuffer<Parameter>> parameter_;

};
