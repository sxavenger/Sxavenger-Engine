#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../Camera/CameraComponent.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* c++
#include <concepts>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class FRenderTargetBuffer;

////////////////////////////////////////////////////////////////////////////////////////////
// BasePostProcess class
////////////////////////////////////////////////////////////////////////////////////////////
class BasePostProcess {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// ProcessInfo structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct ProcessInfo {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================


		//=========================================================================================
		// public variables
		//=========================================================================================

		FRenderTargetBuffer* buffer   = nullptr;
		const CameraComponent* camera = nullptr;

		float weight = 1.0f;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	virtual ~BasePostProcess() = default;

	virtual void Init() = 0;

	virtual void Process(const DirectXQueueContext* context, const ProcessInfo& info) = 0;

	virtual void ShowInspectorImGui() = 0;

	bool IsEnabled() const { return isEnabled_; }
	bool& IsEnabled() { return isEnabled_; }

	void SetEnabled(bool isEnabled) { isEnabled_ = isEnabled; }

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

SXAVENGER_ENGINE_NAMESPACE_END
