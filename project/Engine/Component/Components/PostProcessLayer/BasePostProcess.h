#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../Camera/CameraComponent.h"

//* engine
#include <Engine/System/DirectX/DirectXContext.h>

//* c++
#include <concepts>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class FRenderTargetTextures;

////////////////////////////////////////////////////////////////////////////////////////////
// BasePostProcess class
////////////////////////////////////////////////////////////////////////////////////////////
class BasePostProcess {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	virtual ~BasePostProcess() = default;

	virtual void Init() = 0;

	virtual void Process(const DirectXThreadContext* context, FRenderTargetTextures* textures, const CameraComponent* camera) = 0;

	bool IsEnabled() const { return isEnabled_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	bool isEnabled_ = true;

};

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcess concept
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
concept PostProcessConcept = std::derived_from<T, BasePostProcess>;
