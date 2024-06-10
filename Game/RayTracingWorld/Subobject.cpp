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

	blas_->SetAddress(2, materialBuffer_.get());

}

void Subobject::SetImGuiCommand() {

	// transformの更新
	ImGui::Text("transform");
	transform_.SetImGuiCommand();
	worldMatrix_ = transform_.SetMatrix();

	// materialの更新
	ImGui::Text("material");
	ImGui::ColorEdit4("color", &material_.color.r);


	// delete
	if (ImGui::Button("Delete")) {
		isDelete_ = true;
	}

}
