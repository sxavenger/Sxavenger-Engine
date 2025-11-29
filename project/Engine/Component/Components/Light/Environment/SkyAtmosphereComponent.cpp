#include "SkyAtmosphereComponent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../../Entity/MonoBehaviour.h"

//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/System/UI/SxImGui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void SkyAtmosphereComponent::Parameter::Init() {

	// rayleigh
	rayleigh_scattering        = Vector3f(0.005802f, 0.013558f, 0.033100f);
	rayleigh_density_exp_scale = -0.2f;

	// mie
	mie_scattering        = Vector3f(0.003996f, 0.003996f, 0.003996f);
	mie_extinction        = Vector3f(0.0044f, 0.0044f, 0.0044f);
	mie_density_exp_scale = -9.0f;

	// absorption
	absorption_extinction = Vector3f(0.000650f, 0.001881f, 0.000085f);

	absorption_density_0_layer_height  = 25.0f;
	absorption_density_0_linear_term   = 0.0f;
	absorption_density_0_constant_term = 1.0f;
	absorption_density_1_linear_term   = -0.008f;
	absorption_density_1_constant_term = 0.25f;

	// geometry
	bottom_radius = 6360.0f;
	top_radius    = 6460.0f;

	ground_albedo = Color3f(0.1f, 0.1f, 0.1f);
	multi_scattering_factor = 1.0f;

}

void SkyAtmosphereComponent::Parameter::Inspector() {
	// Rayleigh parameters
	if (ImGui::TreeNodeEx("Rayleigh", ImGuiTreeNodeFlags_Framed)) {
		ImGui::ColorEdit3("Scattering", &rayleigh_scattering.x);
		SxImGui::DragScalar("Density Exp Scale", &rayleigh_density_exp_scale, 0.1f);
		ImGui::TreePop();
	}

	// Mie parameters
	if (ImGui::TreeNodeEx("Mie", ImGuiTreeNodeFlags_Framed)) {
		ImGui::ColorEdit3("Scattering", &mie_scattering.x);
		ImGui::ColorEdit3("Extinction", &mie_extinction.x);
		SxImGui::DragScalar("Density Exp Scale", &mie_density_exp_scale, 0.1f);
		ImGui::TreePop();
	}

	// Absorption parameters
	if (ImGui::TreeNodeEx("Absorption", ImGuiTreeNodeFlags_Framed)) {
		ImGui::ColorEdit3("Extinction", &absorption_extinction.x);
		SxImGui::DragScalar("Density 0 Layer Height", &absorption_density_0_layer_height, 0.1f);
		SxImGui::DragScalar("Density 0 Linear Term", &absorption_density_0_linear_term, 0.01f);
		SxImGui::DragScalar("Density 0 Constant Term", &absorption_density_0_constant_term, 0.01f);
		SxImGui::DragScalar("Density 1 Linear Term", &absorption_density_1_linear_term, 0.01f);
		SxImGui::DragScalar("Density 1 Constant Term", &absorption_density_1_constant_term, 0.01f);
		ImGui::TreePop();
	}

	// Multi Scattering parameter
	if (ImGui::TreeNodeEx("Multi Scattering", ImGuiTreeNodeFlags_Framed)) {
		SxImGui::DragScalar("Factor", &multi_scattering_factor, 0.1f);
		ImGui::TreePop();
	}

	// Geometry parameters
	if (ImGui::TreeNodeEx("Geometry", ImGuiTreeNodeFlags_Framed)) {
		SxImGui::DragScalar("Bottom Radius", &bottom_radius, 1.0f);
		SxImGui::DragScalar("Top Radius", &top_radius, 1.0f);
		ImGui::ColorEdit3("Ground Albedo", &ground_albedo.r);
		ImGui::TreePop();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// SkyAtmosphereComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

SkyAtmosphereComponent::SkyAtmosphereComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) {
	parameter_ = std::make_unique<DxObject::ConstantBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice());
	parameter_->At().Init();
}

void SkyAtmosphereComponent::ShowComponentInspector() {

	SxImGui::Image(transmittance_.descriptorSRV.GetGPUHandle().ptr, ImVec2(256.0f, 64.0f));
	SxImGui::Image(multipleScattering_.descriptorSRV.GetGPUHandle().ptr, ImVec2(32, 32));
	//SxImGui::Image(skyView_.descriptorSRV.GetGPUHandle().ptr, ImVec2(200.0f, 100.0f));

	parameter_->At().Inspector();

}

void SkyAtmosphereComponent::CreateTransmittance() {

	auto device = SxavengerSystem::GetDxDevice()->GetDevice();

	{ //!< resourceの生成

		// propの設定
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		// descの設定
		D3D12_RESOURCE_DESC desc = {};
		desc.Width            = 256;
		desc.Height           = 64;
		desc.MipLevels        = 1;
		desc.DepthOrArraySize = 1;
		desc.Format           = DXGI_FORMAT_R11G11B10_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		auto hr = device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&transmittance_.resource)
		);
		DxObject::Assert(hr, L"texture create failed.");
	}

	{ //!< descriptorUAVの生成
		// handleの取得
		transmittance_.descriptorUAV = SxavengerSystem::GetDescriptor(DxObject::kDescriptor_UAV);

		// descの設定
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format        = DXGI_FORMAT_R11G11B10_FLOAT;
		desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

		// UAVの生成
		device->CreateUnorderedAccessView(
			transmittance_.resource.Get(),
			nullptr,
			&desc,
			transmittance_.descriptorUAV.GetCPUHandle()
		);
	}

	{ //!< descriptorSRVの生成
		// handleの取得
		transmittance_.descriptorSRV = SxavengerSystem::GetDescriptor(DxObject::kDescriptor_SRV);

		// descの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                    = DXGI_FORMAT_R11G11B10_FLOAT;
		desc.ViewDimension             = D3D12_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels       = 1;
		desc.Shader4ComponentMapping   = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		// SRVの生成
		device->CreateShaderResourceView(
			transmittance_.resource.Get(),
			&desc,
			transmittance_.descriptorSRV.GetCPUHandle()
		);
	}

	pipeline1_.CreateBlob(kPackagesShaderDirectory / "Render" / "Atmosphere" / "Transmittance.cs.hlsl");
	pipeline1_.ReflectionPipeline(SxavengerSystem::GetDxDevice());
}

void SkyAtmosphereComponent::UpdateTransmittance(const DirectXQueueContext* context) {

	pipeline1_.SetPipeline(context->GetDxCommand());

	DxObject::BindBufferDesc desc = {};
	desc.Set32bitConstants("Dimension", 2, std::array<UINT, 2>{ 256, 64 }.data());
	desc.SetHandle("gTransmittance", transmittance_.descriptorUAV.GetGPUHandle());
	desc.SetAddress("gAtmosphere", parameter_->GetGPUVirtualAddress());

	pipeline1_.BindComputeBuffer(context->GetDxCommand(), desc);
	pipeline1_.Dispatch(context->GetDxCommand(), { DxObject::RoundUp(256, 16), DxObject::RoundUp(64, 16), 1 });
}

void SkyAtmosphereComponent::CreateMultipleScattering() {

	auto device = SxavengerSystem::GetDxDevice()->GetDevice();

	{ //!< resourceの生成

		// propの設定
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		// descの設定
		D3D12_RESOURCE_DESC desc = {};
		desc.Width            = 32;
		desc.Height           = 32;
		desc.MipLevels        = 1;
		desc.DepthOrArraySize = 1;
		desc.Format           = DXGI_FORMAT_R11G11B10_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		auto hr = device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&multipleScattering_.resource)
		);
		DxObject::Assert(hr, L"texture create failed.");
	}

	{ //!< descriptorUAVの生成
		// handleの取得
		multipleScattering_.descriptorUAV = SxavengerSystem::GetDescriptor(DxObject::kDescriptor_UAV);

		// descの設定
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format        = DXGI_FORMAT_R11G11B10_FLOAT;
		desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

		// UAVの生成
		device->CreateUnorderedAccessView(
			multipleScattering_.resource.Get(),
			nullptr,
			&desc,
			multipleScattering_.descriptorUAV.GetCPUHandle()
		);
	}

	{ //!< descriptorSRVの生成
		// handleの取得
		multipleScattering_.descriptorSRV = SxavengerSystem::GetDescriptor(DxObject::kDescriptor_SRV);

		// descの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                    = DXGI_FORMAT_R11G11B10_FLOAT;
		desc.ViewDimension             = D3D12_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels       = 1;
		desc.Shader4ComponentMapping   = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		// SRVの生成
		device->CreateShaderResourceView(
			multipleScattering_.resource.Get(),
			&desc,
			multipleScattering_.descriptorSRV.GetCPUHandle()
		);
	}

	pipeline2_.CreateBlob(kPackagesShaderDirectory / "Render" / "Atmosphere" / "MultipleScattering.cs.hlsl");

	DxObject::SamplerBindDesc desc = {};
	desc.SetSamplerLinear("gSampler", DxObject::SamplerMode::MODE_CLAMP);

	pipeline2_.ReflectionPipeline(SxavengerSystem::GetDxDevice(), desc);
}

void SkyAtmosphereComponent::UpdateMultipleScattering(const DirectXQueueContext* context) {

	pipeline2_.SetPipeline(context->GetDxCommand());

	DxObject::BindBufferDesc desc = {};
	desc.Set32bitConstants("Dimension", 2, std::array<UINT, 2>{ 32, 32 }.data());
	desc.SetHandle("gTransmittance", transmittance_.descriptorSRV.GetGPUHandle());
	desc.SetHandle("gMultipleScattering", multipleScattering_.descriptorUAV.GetGPUHandle());
	desc.SetAddress("gAtmosphere", parameter_->GetGPUVirtualAddress());

	pipeline2_.BindComputeBuffer(context->GetDxCommand(), desc);
	pipeline2_.Dispatch(context->GetDxCommand(), { 32, 32, 1 });

}

void SkyAtmosphereComponent::CreateSkyView() {

	auto device = SxavengerSystem::GetDxDevice()->GetDevice();

	{ //!< resourceの生成

		// propの設定
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		// descの設定
		D3D12_RESOURCE_DESC desc = {};
		desc.Width            = 200;
		desc.Height           = 100;
		desc.DepthOrArraySize = 1;
		desc.MipLevels        = 1;
		desc.Format           = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		auto hr = device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&skyView_.resource)
		);
		DxObject::Assert(hr, L"texture create failed.");
	}

	{ //!< descriptorUAVの生成
		// handleの取得
		skyView_.descriptorUAV = SxavengerSystem::GetDescriptor(DxObject::kDescriptor_UAV);

		// descの設定
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format        = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

		// UAVの生成
		device->CreateUnorderedAccessView(
			skyView_.resource.Get(),
			nullptr,
			&desc,
			skyView_.descriptorUAV.GetCPUHandle()
		);
	}

	{ //!< descriptorSRVの生成
		// handleの取得
		skyView_.descriptorSRV = SxavengerSystem::GetDescriptor(DxObject::kDescriptor_SRV);

		// descの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                    = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.ViewDimension             = D3D12_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels       = 1;
		desc.Shader4ComponentMapping   = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		// SRVの生成
		device->CreateShaderResourceView(
			skyView_.resource.Get(),
			&desc,
			skyView_.descriptorSRV.GetCPUHandle()
		);
	}

	pipeline3_.CreateBlob(kPackagesShaderDirectory / "Render" / "Atmosphere" / "SkyView.cs.hlsl");

	DxObject::SamplerBindDesc desc = {};
	desc.SetSamplerLinear("gSampler", DxObject::SamplerMode::MODE_CLAMP);

	pipeline3_.ReflectionPipeline(SxavengerSystem::GetDxDevice(), desc);

}

void SkyAtmosphereComponent::UpdateSkyView(const DirectXQueueContext* context) {

	pipeline3_.SetPipeline(context->GetDxCommand());

	DxObject::BindBufferDesc desc = {};
	desc.Set32bitConstants("Dimension", 2, std::array<UINT, 2>{ 200, 100 }.data());
	desc.SetHandle("gSkyView",            skyView_.descriptorUAV.GetGPUHandle());
	desc.SetHandle("gTransmittance",      transmittance_.descriptorSRV.GetGPUHandle());
	desc.SetHandle("gMultipleScattering", multipleScattering_.descriptorSRV.GetGPUHandle());
	desc.SetAddress("gAtmosphere",        parameter_->GetGPUVirtualAddress());
	desc.SetAddress("gTransform",         RequireTransform()->GetGPUVirtualAddress());

	pipeline3_.BindComputeBuffer(context->GetDxCommand(), desc);
	pipeline3_.Dispatch(context->GetDxCommand(), { DxObject::RoundUp(200, 16), DxObject::RoundUp(100, 16), 1 });

}

void SkyAtmosphereComponent::CreateSkyCube() {

	auto device = SxavengerSystem::GetDxDevice()->GetDevice();

	{ //!< resourceの生成

		// propの設定
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		// descの設定
		D3D12_RESOURCE_DESC desc = {};
		desc.Width            = 128;
		desc.Height           = 128;
		desc.DepthOrArraySize = 6;
		desc.MipLevels        = 1;
		desc.Format           = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		auto hr = device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&skyCube_.resource)
		);
		DxObject::Assert(hr, L"texture create failed.");
	}

	{ //!< descriptorUAVの生成
		// handleの取得
		skyCube_.descriptorUAV = SxavengerSystem::GetDescriptor(DxObject::kDescriptor_UAV);

		// descの設定
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format                   = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.ViewDimension            = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.ArraySize = 6;

		// UAVの生成
		device->CreateUnorderedAccessView(
			skyCube_.resource.Get(),
			nullptr,
			&desc,
			skyCube_.descriptorUAV.GetCPUHandle()
		);
	}

	{ //!< descriptorSRVの生成
		// handleの取得
		skyCube_.descriptorSRV = SxavengerSystem::GetDescriptor(DxObject::kDescriptor_SRV);

		// descの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                  = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURECUBE;
		desc.TextureCube.MipLevels   = 1;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		// SRVの生成
		device->CreateShaderResourceView(
			skyCube_.resource.Get(),
			&desc,
			skyCube_.descriptorSRV.GetCPUHandle()
		);
	}

	pipeline4_.CreateBlob(kPackagesShaderDirectory / "Render" / "Atmosphere" / "SkyCube.cs.hlsl");

	DxObject::SamplerBindDesc desc = {};
	desc.SetSamplerLinear("gSampler", DxObject::SamplerMode::MODE_CLAMP);

	pipeline4_.ReflectionPipeline(SxavengerSystem::GetDxDevice(), desc);

}

void SkyAtmosphereComponent::UpdateSkyCube(const DirectXQueueContext* context) {

	pipeline4_.SetPipeline(context->GetDxCommand());

	DxObject::BindBufferDesc desc = {};
	desc.Set32bitConstants("Dimension", 2, std::array<UINT, 2>{ 128, 128 }.data());
	desc.SetHandle("gSkyCube",            skyCube_.descriptorUAV.GetGPUHandle());
	desc.SetHandle("gTransmittance",      transmittance_.descriptorSRV.GetGPUHandle());
	desc.SetHandle("gMultipleScattering", multipleScattering_.descriptorSRV.GetGPUHandle());
	desc.SetAddress("gAtmosphere",        parameter_->GetGPUVirtualAddress());
	desc.SetAddress("gTransform",         RequireTransform()->GetGPUVirtualAddress());

	pipeline4_.BindComputeBuffer(context->GetDxCommand(), desc);
	pipeline4_.Dispatch(context->GetDxCommand(), { DxObject::RoundUp(128, 16), DxObject::RoundUp(128, 16), 6 });
	
}

const TransformComponent* SkyAtmosphereComponent::RequireTransform() const {
	return BaseComponent::GetBehaviour()->RequireComponent<TransformComponent>();
}
