#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// c++
#include <memory>

// dxObject
#include <DxBufferResource.h>

// object
#include <ObjectStructure.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FullScreen class
////////////////////////////////////////////////////////////////////////////////////////////
class FullScreen { // FIXME: class name
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FullScreen() { Init(); };

	~FullScreen() { Term(); }

	void Init();

	void Term();

	void DrawTexture(const D3D12_GPU_DESCRIPTOR_HANDLE& texture);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	// IA
	std::unique_ptr<DxObject::BufferResource<VertexData>> vertex_;
	std::unique_ptr<DxObject::IndexBufferResource>        index_;

	// constantBuffer
	std::unique_ptr<DxObject::BufferResource<Matrix4x4>>     matrix_;

};
