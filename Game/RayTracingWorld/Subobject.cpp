#include "Subobject.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyEngine.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Subobject base class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Subobject::Init (
	DxObject::BufferResource<VertexData>* vertices, DxObject::IndexBufferResource* indices,
	DxObject::StructuredBuffer* verticesStructuredBuffer, DxObject::StructuredBuffer* indicesStructuredBuffer) {

	blas_ = std::make_unique<DxrObject::BottomLevelAS>();
	blas_->Create(
		vertices, indices,
		verticesStructuredBuffer, indicesStructuredBuffer,
		L"subobject"
	);

	transform_.translate = { 0.0f, 2.0f, 0.0f };
	worldMatrix_ = transform_.SetMatrix();

	InitMaterial();
}

void Subobject::SetOnTLAS(DxrObject::TopLevelAS* tlas) {
	tlas->SetBLAS(blas_.get(), worldMatrix_, 0);
}

void Subobject::SetAttributeImGui() {
	SetImGuiCommand();
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

	ImGui::Spacing();

	// materialの更新
	ImGui::Text("material");
	ImGui::ColorEdit4("color", &material_.color.r);

	ImGui::Spacing();
	
	//!< flag
	ImGui::Text("flag");

	{
		ImVec2 regionMax = ImGui::GetWindowContentRegionMax();
		ImVec2 regionMin = ImGui::GetWindowContentRegionMin();
		ImVec2 childSize = { regionMax.x - regionMax.x, 300.0f };
		ImGui::BeginChild(ImGui::GetID((void*)0), childSize, ImGuiChildFlags_Border, ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar);

		ImGui::Checkbox("halfLambert", &flags_[FLAG_LAMBERT]);
		ImGui::Checkbox("alphaRay", &flags_[FLAG_ALPHARAY]);

		if (flags_[FLAG_ALPHARAY]) {
			ImGui::RadioButton("penetration", &material_.alphaRayType, ALPHARAY_PENETRATION);
			ImGui::SameLine();
			ImGui::RadioButton("reflection", &material_.alphaRayType, ALPHARAY_REFLECTION);
		}

		ImGui::Checkbox("alphaRay after lambert", &flags_[FLAG_ALPHALAMBERT]);
		ImGui::Checkbox("blinnPhong", &flags_[FLAG_PHONG]);
		ImGui::Checkbox("shadow", &flags_[FLAG_SHADOW]);

		if (flags_[FLAG_SHADOW]) {
			ImGui::ColorEdit4("shadowColor", &material_.shadowColor.r);
		}

		ImGui::Checkbox("ambientOcclusion", &flags_[FLAG_AO]);

		if (flags_[FLAG_AO]) {
			ImGui::ColorEdit4("ambientOcclusionColor", &material_.aoColor.r);
			ImGui::DragFloat("ambientOcclusionRange", &material_.aoRange, 0.01f, 0.0f, 10.0f);
			ImGui::SliderFloat("aombientOcclusionLimit", &material_.aoLimit, 0.0f, 8.0f * 8.0f);
		}

		// 一度初期化
		material_.flags = 0;

		for (int i = 0; i < kCountOfSubobjectFlag; ++i) {
			// bitFlagにして変換
			material_.flags |= flags_[i] << i;
		}

		ImGui::EndChild();
	}

	ImGui::Spacing();

	// delete
	if (ImGui::Button("Delete")) {
		isDelete_ = true;
	}

}
