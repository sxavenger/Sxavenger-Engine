#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/ComPtr.h>
#include <Engine/System/DirectX/DxObject/DxObjectCommon.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/DirectXContext.h>

//* lib
#include <Lib/Geometry/Vector2.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FSceneDepth class
////////////////////////////////////////////////////////////////////////////////////////////
class FSceneDepth { //!< 深度バッファ管理クラス
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FSceneDepth()  = default;
	~FSceneDepth() { Term(); }

	void Create(const Vector2ui& size);

	void Term();

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// RasterizerDepth structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct RasterizerDepth {

		//* member *//

		ComPtr<ID3D12Resource> resource_;
		DxObject::Descriptor descriptorDSV_;
		DxObject::Descriptor descriptorSRV_;

		//* method *//

		void Create(const Vector2ui& size); //!< Format is default depth format only.

		void Delete();

		void BeginWrite(const DirectXThreadContext* context) const; //!< Transition to depth write state.

		void EndWrite(const DirectXThreadContext* context) const; //!< Transition to pixel shader resource state.

		void ClearDepth(const DirectXThreadContext* context) const; //!< Clear depth value.

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// RaytracingDepth structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct RaytracingDepth {

		//* member *//

		ComPtr<ID3D12Resource> resource_;
		DxObject::Descriptor descriptorUAV_;

		//* method *//

		void Create(const Vector2ui& size); //!< Format is default depth compatible format only.

		void Delete();

		void BeginWrite(const DirectXThreadContext* context) const; //!< Transition to unordered access state.

		void EndWrite(const DirectXThreadContext* context) const; //!< Transition to pixel shader resource state.

	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	RasterizerDepth rasterizer_;
	RaytracingDepth raytracing_;

	// todo: 深度プリパスの追加
};
