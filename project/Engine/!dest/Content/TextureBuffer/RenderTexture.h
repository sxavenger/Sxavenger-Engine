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
// RenderTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class RenderTexture
	: public BaseOffscreenTexture {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	RenderTexture()  = default;
	~RenderTexture() { Term(); }

	void Create(const Vector2ui& size, const Color4f& clearColor = kDefaultClearColor, DXGI_FORMAT format = DxObject::kOffscreenFormat);

	void Term();

	//* render option *//

	void TransitionBeginRenderTarget(const DirectXThreadContext* context);

	void TransitionEndRenderTarget(const DirectXThreadContext* context);

	void ClearRenderTarget(const DirectXThreadContext* context);

	//* getter *//

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandleRTV() const { return descriptorRTV_.GetCPUHandle(); }

	//=========================================================================================
	// public variables
	//=========================================================================================

	static const Color4f kDefaultClearColor;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* descriptor *//

	DxObject::Descriptor descriptorRTV_;

	//* parameter *//

	Color4f clearColor_;

};
