#include "PostProcessGrayScale.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include <Engine/Render/FRenderTargetBuffer.h>
#include <Engine/Render/FRenderCore.h>

//* external
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessGrayScale::Parameter::Init() {
	type = Type::GrayScale;
}

void PostProcessGrayScale::Parameter::SetImGuiCommand() {
	if (ImGui::BeginCombo("Type", magic_enum::enum_name(type).data())) {

		for (const auto& [value, name] : magic_enum::enum_entries<Type>()) {
			if (ImGui::Selectable(name.data(), type == value)) {
				type = value;
			}
		}

		ImGui::EndCombo();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessGrayScale class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessGrayScale::Init() {
	parameter_ = std::make_unique<DxObject::DimensionBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	parameter_->At(0).Init();

	name_ = "Gray Scale";
}

void PostProcessGrayScale::Process(const DirectXQueueContext* context, FRenderTargetBuffer* buffer, const CameraComponent* camera) {

	camera;

	auto process = buffer->GetProcessTextures();
	process->NextProcess(context);

	auto core = FRenderCore::GetInstance()->GetProcess();

	core->SetPipeline(FRenderCoreProcess::ProcessType::GrayScale, context);

	BindBufferDesc desc = {};
	// common
	desc.Set32bitConstants("Dimension", 2, &buffer->GetSize());

	//* textures
	desc.SetHandle("gInput",  process->GetPrevTexture()->GetGPUHandleSRV());
	desc.SetHandle("gOutput", process->GetIndexTexture()->GetGPUHandleUAV());

	// parameter
	desc.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());

	core->BindComputeBuffer(FRenderCoreProcess::ProcessType::GrayScale, context, desc);
	core->Dispatch(context, buffer->GetSize());
}

void PostProcessGrayScale::ShowInspectorImGui() {
	parameter_->At(0).SetImGuiCommand();
}
