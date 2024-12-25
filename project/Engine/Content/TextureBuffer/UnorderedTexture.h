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
// UnorderedTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class UnorderedTexture
	: public BaseOffscreenTexture {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	UnorderedTexture() = default;
	~UnorderedTexture() { Term(); }

	void Create(const Vector2ui& size, DXGI_FORMAT format = DxObject::kOffscreenFormat);

	void Term();

	//* unordered option *//

	void TransitionBeginUnordered(const DirectXThreadContext* context);

	void TransitionEndUnordered(const DirectXThreadContext* context);

	//* getter *//

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleUAV() const { return descriptorUAV_.GetGPUHandle(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* descriptor *//

	DxObject::Descriptor descriptorUAV_;

};
