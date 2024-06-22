#include "RayTracingObject.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyEngine.h>

////////////////////////////////////////////////////////////////////////////////////////////
// RayTracingObject base class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RayTracingObject::Init() {
	matrixBuffer_ = std::make_unique<DxObject::BufferPtrResource<Matrix4x4>>(MyEngine::GetDevicesObj(), 1);
	matrixBuffer_->SetPtr(0, &worldMatrix_);
}
