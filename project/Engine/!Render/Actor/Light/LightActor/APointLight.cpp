#include "APointLight.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/System/Config/SxavengerDirectory.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ALightActor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void APointLight::Init() {
	cb_ = std::make_unique<DxObject::DimensionBuffer<Parameter>>();
	cb_->Create(SxavengerSystem::GetDxDevice(), 1);
	cb_->At(0).Init();

	TestInit();
}

void APointLight::Render() {
	TestRender();
}

void APointLight::TestInit() {
	ia_.Create(8, 6 * 6);

	ia_.GetVertex()->At(0) = { -0.5f, -0.5f,  0.5f };
	ia_.GetVertex()->At(1) = { 0.5f, -0.5f,  0.5f };
	ia_.GetVertex()->At(2) = { 0.5f,  0.5f,  0.5f };
	ia_.GetVertex()->At(3) = { -0.5f,  0.5f,  0.5f };
	ia_.GetVertex()->At(4) = { -0.5f, -0.5f, -0.5f };
	ia_.GetVertex()->At(5) = { 0.5f, -0.5f, -0.5f };
	ia_.GetVertex()->At(6) = { 0.5f,  0.5f, -0.5f };
	ia_.GetVertex()->At(7) = { -0.5f,  0.5f, -0.5f };

	ia_.GetIndex()->At(0) = 0; ia_.GetIndex()->At(1) = 1; ia_.GetIndex()->At(2) = 2;
	ia_.GetIndex()->At(3) = 0; ia_.GetIndex()->At(4) = 2; ia_.GetIndex()->At(5) = 3;
	ia_.GetIndex()->At(6) = 1; ia_.GetIndex()->At(7) = 5; ia_.GetIndex()->At(8) = 6;
	ia_.GetIndex()->At(9) = 1; ia_.GetIndex()->At(10) = 6; ia_.GetIndex()->At(11) = 2;
	ia_.GetIndex()->At(12) = 5; ia_.GetIndex()->At(13) = 4; ia_.GetIndex()->At(14) = 7;
	ia_.GetIndex()->At(15) = 5; ia_.GetIndex()->At(16) = 7; ia_.GetIndex()->At(17) = 6;
	ia_.GetIndex()->At(18) = 4; ia_.GetIndex()->At(19) = 0; ia_.GetIndex()->At(20) = 3;
	ia_.GetIndex()->At(21) = 4; ia_.GetIndex()->At(22) = 3; ia_.GetIndex()->At(23) = 7;
	ia_.GetIndex()->At(24) = 3; ia_.GetIndex()->At(25) = 2; ia_.GetIndex()->At(26) = 6;
	ia_.GetIndex()->At(27) = 3; ia_.GetIndex()->At(28) = 6; ia_.GetIndex()->At(29) = 7;
	ia_.GetIndex()->At(30) = 4; ia_.GetIndex()->At(31) = 5; ia_.GetIndex()->At(32) = 1;
	ia_.GetIndex()->At(33) = 4; ia_.GetIndex()->At(34) = 1; ia_.GetIndex()->At(35) = 0;

	pipeline_ = std::make_unique<DxObject::ReflectionGraphicsPipelineState>();
	pipeline_->CreateBlob(kPackagesShaderDirectory / "render/light/baseLightRender.vs.hlsl", DxObject::GraphicsShaderType::vs);
	pipeline_->CreateBlob(kPackagesShaderDirectory / "render/light/pointLight.ps.hlsl", DxObject::GraphicsShaderType::ps);
	pipeline_->ReflectionRootSignature(SxavengerSystem::GetDxDevice());

	DxObject::GraphicsPipelineDesc desc = {};
	desc.elements.clear();
	desc.SetElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);

	pipeline_->CreatePipeline(SxavengerSystem::GetDxDevice(), desc);

}

void APointLight::TestRender() {
}
