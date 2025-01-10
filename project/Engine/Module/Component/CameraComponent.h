#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "TransformComponent.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>

//* lib
#include <Lib/Geometry/Matrix4x4.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// CameraComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class CameraComponent
	: public TransformComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Camera structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Camera {

		//* member *//

		Matrix4x4 view;
		Matrix4x4 world;
		Matrix4x4 proj;
		Matrix4x4 projInv;
		//float near;
		//float far;

		//* method *//

		void Init();

		void TransferView(const Matrix4x4& _world);
		void TransferProj(const Matrix4x4& _proj);

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	CameraComponent()  = default;
	~CameraComponent() = default;

	void CreateBuffer();

	void UpdateViewMatrix();

	//* getter *//

	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddress() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::DimensionBuffer<Camera>> buffer_;
	
};
