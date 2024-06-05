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

	void CreateStateObject(
		int32_t clientWidth, int32_t clientHeight,
		DxrObject::TopLevelAS* tlas
	);

	void BeginRayTracing();

	void EndRayTracing();

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

};