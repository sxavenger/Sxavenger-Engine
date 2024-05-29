#pragma once
/*
 レイトレでの ambient occlusion の制御バッファのクラス
*/

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
#include <DxBufferResource.h>

// Geometry

// c++
#include <memory>

// attribute
#include <Attribute.h>


////////////////////////////////////////////////////////////////////////////////////////////
// RayTracingAO class
////////////////////////////////////////////////////////////////////////////////////////////
class RayTracingAO
	: public Attribute {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	RayTracingAO() { Init(); }

	~RayTracingAO() { Term(); }

	void Init();

	void Term();

	void SetAttributeImGui() override;

	const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() {
		return resource_->GetGPUVirtualAddress();
	}

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// AOParameter Structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct AOParameter {
		int isAmbientOcclusionEnable = false;

		float aoRadius = 0.2f;
		uint32_t subdivision = 10;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::BufferPtrResource<AOParameter>> resource_;
	AOParameter parameter_;

};