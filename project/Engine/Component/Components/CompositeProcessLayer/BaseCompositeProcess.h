#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>
#include <Engine/Render/FRenderTargetTextures.h>

//* c++
#include <concepts>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseCompositeProcess class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseCompositeProcess {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	virtual ~BaseCompositeProcess() = default;

	virtual void Init() = 0;

	virtual void Process(const DirectXQueueContext* context, FRenderTargetTextures* textures) = 0;

	virtual void ShowInspectorImGui() = 0;

	bool IsEnabled() const { return isEnabled_; }
	bool& IsEnabled() { return isEnabled_; }

	const std::string& GetName() { return name_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	bool isEnabled_ = true;

	std::string name_ = "base composite process";

};

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcess concept
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
concept CompositeProcessConcept = std::derived_from<T, BaseCompositeProcess>;
