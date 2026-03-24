#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FBaseRenderCore.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/Configuration/Configuration.h>
#include <Engine/Module/Pipeline/CustomComputePipeline.h>
#include <Engine/Module/Pipeline/CustomGraphicsPipeline.h>

//* c++
#include <array>

//* external
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreTransition class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderCoreTransition final
	: public FBaseRenderCore {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Transition enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Transition : uint8_t {
		MotionVectorTransition,

		LightingTransition,
		ReflectionTransition,
		MipmapTransition,
		TransparentTransition,
	};
	static inline const size_t kTransitionCount = magic_enum::enum_count<Transition>();

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init() override;

	//* transition option *//

	void SetPipeline(Transition transition, const DirectXQueueContext* context) const;

	void BindComputeBuffer(
		Transition transition, const DirectXQueueContext* context,
		const DxObject::BindBufferDesc& desc
	) const;

	void Dispatch(const DirectXQueueContext* context, const Vector2ui& resolution) const;

	//* present option *//

	void Present(const DirectXQueueContext* context, const Vector2ui& resolution, const D3D12_GPU_DESCRIPTOR_HANDLE& handle) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static inline const Vector2ui kNumthread             = { 16, 16 };
	static inline const std::filesystem::path kDirectory = kPackagesDirectory / "shaders" / "render" / "Transition";

	std::array<CustomReflectionComputePipeline, kTransitionCount> transitions_;
	DxObject::ReflectionGraphicsPipelineState presenter_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreatePresent();

	void CreateTransition(Transition transition, const std::filesystem::path& filepath);

};

SXAVENGER_ENGINE_NAMESPACE_END
