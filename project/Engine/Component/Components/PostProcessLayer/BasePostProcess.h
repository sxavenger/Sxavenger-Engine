#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../Camera/CameraComponent.h"

//* engine
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* c++
#include <concepts>
#include <string>

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

	virtual void Process(const DirectXQueueContext* context, FRenderTargetTextures* textures, const CameraComponent* camera) = 0;

	virtual void ShowInspectorImGui() = 0;

	bool IsEnabled() const { return isEnabled_; }
	bool& IsEnabled() { return isEnabled_; }

	const std::string& GetName() { return name_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	bool isEnabled_ = true;

	std::string name_ = "base post process";

};

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcess concept
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
concept PostProcessConcept = std::derived_from<T, BasePostProcess>;
