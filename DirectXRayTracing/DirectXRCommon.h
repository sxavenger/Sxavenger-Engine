#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DirectXCommon
#include <DirectXCommon.h>

// dxrObject
#include <DxrShaderManager.h>
#include <DxrShaderBlob.h>
#include <DxrRootSignature.h>
#include <DxrStateObject.h>
#include <DxrResultBuffer.h>
#include <DxrShaderTable.h>

#include "DrawMethod.h"
#include "DxrAccelerationStructure.h"
#include "RayTracingCamera.h"
#include "RayTracingLight.h"
#include "RayTracingAO.h"

#include "DxBufferResource.h"
#include "Vector4.h"
#include "Model.h"

////////////////////////////////////////////////////////////////////////////////////////////
// DirectXRCommon class
////////////////////////////////////////////////////////////////////////////////////////////
class DirectXRCommon
	: public DirectXCommon {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void InitRayTracing(int32_t clientWidth, int32_t clientHeight);

	void TermRayTracing();

	void DrawRayTracing();

	void DrawRasterlize();

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetResultBufferTexture() const {
		return resultBuffer_->GetSRVHandleGPU();
	}

	static DirectXRCommon* GetInstance();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================
	
	std::unique_ptr<DxrObject::ShaderManager>  shaderManager_;
	std::unique_ptr<DxrObject::ResultBuffer>   resultBuffer_; //!< のちにDxrSourceにしてもいいかも...?

	// stateObjects //
	std::unique_ptr<DxrObject::RootSignature> globalRootSignature_;
	std::unique_ptr<DxrObject::StateObject>   stateObject_;
	std::unique_ptr<DxrObject::ShaderTable>   shaderTable_;

	// world //

	std::unique_ptr<Model> teapot_;
	std::unique_ptr<DxrObject::BottomLevelAS> teapotBlas_;

	std::unique_ptr<Model> room_;
	std::unique_ptr<DxrObject::BottomLevelAS> roomBlas_;

	std::unique_ptr<DxrObject::TopLevelAS>    tlas_;

	// primitive constantBuffer
	std::unique_ptr<DxObject::BufferResource<Vector4f>> material_;

	// constantBuffer //
	std::unique_ptr<RayTracingCamera> camera_;
	std::unique_ptr<RayTracingLight> light_;
	std::unique_ptr<RayTracingAO> ambientOcclusion_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateStateObject(int32_t clientWidth, int32_t clientHeight);

	void CreateAccelerationStructure();

};