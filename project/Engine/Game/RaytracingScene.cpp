#include "RaytracingScene.h"
_DXROBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// RaytracingScene class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RaytracingScene::Term() {
}

void RaytracingScene::CreateTLAS() {
	tlas_ = std::make_unique<TopLevelAS>();
	tlas_->Create();
}

void RaytracingScene::BeginSetupTLAS() {
	tlas_->BeginSetupInstance();
}

void RaytracingScene::EndSetupTLAS() {
	tlas_->EndSetupInstance();
}

void RaytracingScene::SetBLASInstance(const DxrObject::BottomLevelAS* blas, const Matrix4x4& worldMatrix, const DxrObject::BufferRecoreder* recorder, UINT instanceId) {
	tlas_->SetInstance(blas, worldMatrix, recorder, instanceId);
}

void RaytracingScene::CreateBlobGroup() {
	blobGroup_ = std::make_unique<RaytracingShaderBlobGroup>();
	blobGroup_->Create();
}

void RaytracingScene::SetBlob(const DxrObject::RaytracingShaderBlob* blob) {
	blobGroup_->SetRaytracingShaderBlob(blob);
}

void RaytracingScene::CreateStateObject(const DxrObject::GlobalRootSignatureDesc& desc) {
	stateObject_ = std::make_unique<StateObject>();
	stateObject_->CreateRootSignature(desc);
	stateObject_->CreateStateObject(blobGroup_.get());
}

void RaytracingScene::SetStateObject() {
	stateObject_->SetStateObject();
}

void RaytracingScene::CreateTable() {
	table_ = std::make_unique<ShaderTable>();
}

void RaytracingScene::SetupShaderTable(const DxrObject::BufferRecoreder* raygeneration, const DxrObject::BufferRecoreder* miss) {
	table_->Create(stateObject_.get(), tlas_.get(), raygeneration, miss);
}

void RaytracingScene::DispatchRays() {
	table_->DispatchRays();
}
