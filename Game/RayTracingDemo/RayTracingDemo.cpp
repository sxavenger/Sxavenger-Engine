#include "RayTracingDemo.h"
_DXROBJECT_USING

#include <DrawMethod.h>

////////////////////////////////////////////////////////////////////////////////////////////
// RayTracingDemo class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RayTracingDemo::Init() {

	Attribute::SetThisAttribute("raytracing 3.0");

	manager_ = std::make_unique<ShaderBlobManager>();
	blob_    = std::make_unique<ShaderBlob>();
	blob_->Create(L"RayTracingTest.hlsl");

	{
		raygen_ = std::make_unique<ExportGroup>();
		raygen_->Create(L"mainRaygen", DxrObject::kExport_Raygeneration);

		LocalRootSignatureDesc descRaygen;
		descRaygen.SetUAV(0, 0);

		raygen_->CreateRootSignature(descRaygen);

		output_ = std::make_unique<DummyTexture>();
		output_->Create(Sxavenger::GetDxCommon(), { kWindowWidth, kWindowHeight });

		raygenRecord_ = std::make_unique<BufferRecord>(raygen_.get());
		raygenRecord_->BindHandle(0, output_->GetGPUHandleUAV());
	}
	
	{
		miss_ = std::make_unique<ExportGroup>();
		miss_->Create(L"mainMiss", DxrObject::kExport_Miss);

		/*LocalRootSignatureDesc descMiss;
		descMiss.Resize(1, 0);
		descMiss.SetCBV(0, 0);

		miss_->CreateRootSignature(descMiss);*/

		missColor_ = std::make_unique<DxObject::BufferResource<Color4f>>(Sxavenger::GetDevicesObj(), 1);
		(*missColor_)[0] = { 1.0f, 1.0f, 1.0f, 1.0f };

		missRecord_ = std::make_unique<BufferRecord>(miss_.get());
		//missRecord_->BindAddress(0, missColor_->GetGPUVirtualAddress());
	}

	table_ = std::make_unique<ShaderTable>();

	tlas_ = std::make_unique<TopLevelAS>();

	{
		hitgroup_ = std::make_unique<ExportGroup>();
		hitgroup_->Create(L"hitgroup", kExport_Hitgroup);
		hitgroup_->SetEntryPointHitgroup(L"mainCHS");

		LocalRootSignatureDesc desc;

		hitgroup_->CreateRootSignature(desc);

		model_ = std::make_unique<Model>("resources/model", "teapot.obj");

		blas_ = std::make_unique<BottomLevelAS>();
		blas_->Create(model_->GetMesh(0), hitgroup_.get());

		//tlas_->BeginSetupInstance();
		//tlas_->SetInstance(blas_.get(), transform_.ToMatrix(), 0);
		//tlas_->EndSetupInstance();

	}

	blob_->SetEntryPointExport(raygen_.get());
	blob_->SetEntryPointExport(miss_.get());
	blob_->SetEntryPointExport(hitgroup_.get());

	stateObject_ = std::make_unique<StateObject>();

	GlobalRootSignatureDesc desc;
	desc.SetSRV(0, 0);
	desc.SetCBV(1, 1);

	stateObject_->CreateRootSignature(desc);
	stateObject_->CreateStateObject(blob_.get());

	camera_ = std::make_unique<RayTracingCamera>();
}

void RayTracingDemo::Term() {
}

void RayTracingDemo::Update() {

	tlas_->BeginSetupInstance();

	for (int i = 0; i < instanceNum_; ++i) {
		tlas_->SetInstance(blas_.get(), transform_.ToMatrix() * Matrix::MakeTranslate({static_cast<float>(i), 0.0f, 0.0f}), 0);
	}

	tlas_->EndSetupInstance();


}

void RayTracingDemo::Draw() {
	
	table_->Create(stateObject_.get(), tlas_.get(), raygenRecord_.get(), missRecord_.get());
	Sxavenger::GetDxCommon()->BeginUnorderedAccess(output_.get());

	auto commandList = Sxavenger::GetCommandList();

	stateObject_->SetPipeline(commandList);

	commandList->SetComputeRootDescriptorTable(0, tlas_->GetGPUHandle());
	commandList->SetComputeRootConstantBufferView(1, camera_->GetGPUVirtualAddress());

	commandList->DispatchRays(table_->GetDispatchRayDesc());

	Sxavenger::GetDxCommon()->EndUnorderedAccess(output_.get());

	console->DisplayTextureForConsole("raytracing 3.0", output_->GetGPUHandleSRV());
	
}

void RayTracingDemo::SetAttributeImGui() {
	ImGui::ColorEdit4("miss color", &(*missColor_)[0].r);
	transform_.SetImGuiCommand();

	ImGui::InputInt("instance", &instanceNum_);
}
