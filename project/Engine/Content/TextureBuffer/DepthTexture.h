#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* texture
#include "BaseOffscreenTexture.h"

//* engine
#include <Engine/System/DirectX/DirectXContext.h>

//* lib
#include <Lib/Geometry/Vector4.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DepthTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class DepthTexture
	: public BaseOffscreenTexture {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DepthTexture()  = default;
	~DepthTexture() { Term(); }

	void Create(const Vector2ui& size); //!< default depth format only.
	//!< format独自設定したい場合は, DepthFormatの変換を用意する.

	void Term();

	//* depth option *//

	void TransitionBeginDepthWrite(const DirectXThreadContext* context);

	void TransitionEndDepthWrite(const DirectXThreadContext* context);

	void ClearDepth(const DirectXThreadContext* context);

	//* getter *//

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandleDSV() const { return descriptorDSV_.GetCPUHandle(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* descriptor *//

	DxObject::Descriptor descriptorDSV_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateResource();
	void CreateDSV();
	void CreateSRV();

};
