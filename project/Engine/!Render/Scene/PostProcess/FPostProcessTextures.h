#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../../FTexture.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/DirectXContext.h>

//* c++
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// FProcessTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class FProcessTexture {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FProcessTexture()  = default;
	~FProcessTexture() { Term(); }

	void Create(const FTexture* texture);

	void Term();

	//* process *//

	void CopyFromTexture(const DirectXThreadContext* context, const FTexture* texture);

	void CopyToTexture(const DirectXThreadContext* context, const FTexture* texture);

	//* getter *//

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleUAV() const { return descriptorUAV_.GetGPUHandle(); }

	const Vector2ui& GetSize() const { return size_; }

	const DXGI_FORMAT& GetFormat() const { return format_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* directx12 *//

	ComPtr<ID3D12Resource> resource_;

	DxObject::Descriptor descriptorUAV_;

	//* parameter *//

	Vector2ui   size_;
	DXGI_FORMAT format_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// FPostProcessTextures class
////////////////////////////////////////////////////////////////////////////////////////////
class FPostProcessTextures {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FPostProcessTextures()  = default;
	~FPostProcessTextures() = default;

	void Create(size_t size, const FTexture* texture);

	bool CheckMatchTexture(const FTexture* texture);

	//* process texture option *//

	void NextIndex();

	FProcessTexture* GetPrevTexture(size_t prev) const;

	void ResetIndex();

	void CopyFromTexture(const DirectXThreadContext* context, const FTexture* texture);

	void CopyToTexture(const DirectXThreadContext* context, const FTexture* texture);

	//* getter *//

	size_t GetSize() const;

private:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* textures *//

	std::vector<std::unique_ptr<FProcessTexture>> textures_;

	size_t index_ = 0;
	//* 現在のresultとなるtextureのindex

};
