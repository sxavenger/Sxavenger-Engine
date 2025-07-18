#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>

//* lib
#include <Lib/Sxl/Flag.h>
#include <Lib/Geometry/Color4.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// LightCommon class
////////////////////////////////////////////////////////////////////////////////////////////
class LightCommon {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Units enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Units : uint32_t {
		Lumen   = 0, //!< lm
		Candela = 1, //!< cd
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// InlineShadow structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct InlineShadow {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init();

		//=========================================================================================
		// public variables
		//=========================================================================================

		float strength;
		Sxl::FlagMask<D3D12_RAY_FLAGS, uint32_t> flag;

		// todo: soft shadow
		// strength自体を削除したい.
	};
	
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	LightCommon()          = default;
	virtual ~LightCommon() = default;

	//* buffer option *//

	void CreateShadowBuffer();

	const D3D12_GPU_VIRTUAL_ADDRESS& GetShadowBufferAddress() const;

	const InlineShadow& GetShadowParameter() const;
	InlineShadow& GetShadowParameter();

	//* inspector *//

	void ShowCommonInspector();

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::unique_ptr<DxObject::DimensionBuffer<InlineShadow>> shadow_;

	static inline const Color4f kColor = Color4f::Convert(0xABCCC4FF);
	// b9cdc6

};
