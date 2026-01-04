#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
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
class FRenderCoreTransition {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Transition enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Transition : uint8_t {
		LightingTransition,
		MipmapTransition,
		VelocityTransition,
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	//* transition option *//

	void SetPipeline(Transition transition, const DirectXQueueContext* context);

	void BindComputeBuffer(
		Transition transition, const DirectXQueueContext* context,
		const DxObject::BindBufferDesc& desc
	);

	void Dispatch(const DirectXQueueContext* context, const Vector2ui& size);

	//* present option *//

	void Present(const DirectXQueueContext* context, const Vector2ui& size, const D3D12_GPU_DESCRIPTOR_HANDLE& handle);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static inline const Vector2ui kNumThreadSize         = { 16, 16 };
	static inline const std::filesystem::path kDirectory = kPackagesDirectory / "shaders" / "render" / "Transition";

	static inline const size_t kTransitionCount = magic_enum::enum_count<Transition>();

	std::array<std::unique_ptr<CustomReflectionComputePipeline>, kTransitionCount> transitions_;
	std::unique_ptr<DxObject::ReflectionGraphicsPipelineState> presenter_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreatePresent();

	void CreateTransition(Transition transition, const std::filesystem::path& filepath);

};

SXAVENGER_ENGINE_NAMESPACE_END
