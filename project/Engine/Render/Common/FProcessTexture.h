#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* texture
#include "FBaseTexture.h"
#include "FTexture.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/DirectXContext.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FProcessTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class FProcessTexture
	: public FBaseTexture {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FProcessTexture() = default;

	void Create(const Vector2ui& size, DXGI_FORMAT format = DxObject::kOffscreenFormat);

private:
};

////////////////////////////////////////////////////////////////////////////////////////////
// FProcessTextures class
////////////////////////////////////////////////////////////////////////////////////////////
class FProcessTextures {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Create(uint32_t count, const Vector2ui& size, DXGI_FORMAT format = DxObject::kOffscreenFormat);

	//* texture option *//

	void BeginProcess(const DirectXThreadContext* context, FTexture* texture);

	void EndProcess(const DirectXThreadContext* context, FTexture* texture);

	//* container option *//

	void NextProcess(const DirectXThreadContext* context);

	FProcessTexture* GetPrevTexture(uint32_t prev = 1) const;

	FProcessTexture* GetIndexTexture() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::vector<std::unique_ptr<FProcessTexture>> textures_;

	uint32_t index_ = 0;
	//* 現在のresultとなるtextureのindex

};
