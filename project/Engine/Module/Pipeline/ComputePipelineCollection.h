#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxComputePipelineState.h>
#include <Engine/System/DirectX/DirectXContext.h>

//* lib
#include <Lib/Geometry/Vector2.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ComputePipelineType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum ComputePipelineType {
	//* transition
	kTransition_SampleLighting,

	////* xclipse
	kXclipse_Atmosphere,
	kXclipse_SSAO,

	////* visual
	//kVisual_Glayscale,
	//kVisual_LUT,
	kVisual_DoF,
	//kVisual_Toon,

	kCountOfComputePipelineType
};

////////////////////////////////////////////////////////////////////////////////////////////
// ComputePipelineCollection class
////////////////////////////////////////////////////////////////////////////////////////////
class ComputePipelineCollection {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
	template <class T>
	using ComputeArray = std::array<T, ComputePipelineType::kCountOfComputePipelineType>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ComputePipelineCollection()  = default;
	~ComputePipelineCollection() = default;

	void Init();

	void Term();

	//* pipeline option *//

	void SetPipeline(ComputePipelineType type, const DirectXThreadContext* context);

	void BindComputeBuffer(ComputePipelineType type, const DirectXThreadContext* context, const DxObject::BindBufferDesc& desc);

	void Dispatch(const DirectXThreadContext* context, const Vector2ui& size);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	ComputeArray<std::unique_ptr<DxObject::ReflectionComputePipelineState>> pipelines_;

	//* config *//

	const Vector2ui numthreads_ = { 32, 32 };

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateTransition();

	void CreateVisual();

	void CreateXclipse();

};
