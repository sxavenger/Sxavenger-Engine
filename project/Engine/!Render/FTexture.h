#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/DirectXContext.h>\


//* lib
#include <Lib/Geometry/Vector2.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class FTexture {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FTexture()  = default;
	~FTexture() { Term(); }

	void Create(const Vector2ui& size, DXGI_FORMAT format = DxObject::kOffscreenFormat);

	void Term();

	//* transition methods *//

	void TransitionBeginRenderTarget(const DirectXThreadContext* context) const;

	void TransitionEndRenderTarget(const DirectXThreadContext* context) const;

	void ClearRenderTarget(const DirectXThreadContext* context) const;

	void TransitionBeginUnordered(const DirectXThreadContext* context) const;

	void TransitionEndUnordered(const DirectXThreadContext* context) const;

	//* getter *//

	ID3D12Resource* GetResource() const { return resource_.Get(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* directx12 *//

	ComPtr<ID3D12Resource> resource_;

	DxObject::Descriptor descriptorUAV_;
	DxObject::Descriptor descriptorRTV_;

	//* parameter *//

	Vector2ui   size_;
	DXGI_FORMAT format_;

};
