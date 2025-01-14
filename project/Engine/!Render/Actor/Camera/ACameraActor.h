#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* actor
#include "../AActor.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>


////////////////////////////////////////////////////////////////////////////////////////////
// ACameraActor class
////////////////////////////////////////////////////////////////////////////////////////////
class ACameraActor
	: public AActor {
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

	ACameraActor()          = default;
	virtual ~ACameraActor() = default;

	void Init();

	void UpdateView();

	//* getter *//

	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddress() const;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::unique_ptr<DxObject::DimensionBuffer<Camera>> buffer_;

};
