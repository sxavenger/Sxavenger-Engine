#include "Subobject.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyEngine.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Subobject base class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Subobject::SetOnTLAS(DxrObject::TopLevelAS* tlas) {
	tlas->SetBLAS(blas_.get(), worldMatrix_, 0);
}

void Subobject::InitMaterial() {

	materialBuffer_ = std::make_unique<DxObject::BufferPtrResource<SubobjectMaterial>>(MyEngine::GetDevicesObj(), 1);
	materialBuffer_->SetPtr(0, &material_);

	blas_->SetBuffer(2, materialBuffer_->GetGPUVirtualAddress());

}
