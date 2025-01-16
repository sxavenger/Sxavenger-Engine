#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxGraphicsPipelineState.h>
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/System/DirectX/DirectXContext.h>

//* lib
#include <Lib/Geometry/Matrix4x4.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SpriteCommon class
////////////////////////////////////////////////////////////////////////////////////////////
class SpriteCommon {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SpriteCommon()  = default;
	~SpriteCommon() = default;

	void Init();

	void SetPipeline(const DirectXThreadContext* context) const;

	void BindBuffer(const DirectXThreadContext* context, const DxObject::BindBufferDesc& desc) const;
	
private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::ReflectionGraphicsPipelineState> pipeline_;

	std::unique_ptr<DxObject::DimensionBuffer<Matrix4x4>> buffer_;

};
