#include "SkyAtmosphereComponent.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../../Entity/EntityBehaviour.h"

//* engine
#include <Engine/System/UI/SxImGui.h>
#include <Engine/System/System.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Atmosphere structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void SkyAtmosphereComponent::Atmosphere::Init() {

	// rayleigh
	rayleigh_scattering        = Vector3f(0.005802f, 0.013558f, 0.033100f);
	rayleigh_density_exp_scale = -0.2f;

	// mie
	mie_scattering        = Vector3f(0.003996f, 0.003996f, 0.003996f);
	mie_extinction        = Vector3f(0.0044f, 0.0044f, 0.0044f);
	mie_density_exp_scale = -9.0f;

	// absorption
	absorption_extinction = Vector3f(0.000650f, 0.001881f, 0.000085f);

	absorption_density_0_layer_height  = 0.0f;
	absorption_density_0_linear_term   = 0.0f;
	absorption_density_0_constant_term = 1.0f;
	absorption_density_1_linear_term   = -0.008f;
	absorption_density_1_constant_term = 0.25f;

	// geometry
	bottom_radius = 6360.0f;
	top_radius    = 6460.0f;

	ground_albedo = Color3f(0.1f, 0.1f, 0.1f);
	multi_scattering_factor = 1.0f;

	intensity = 80.0f;

}

void SkyAtmosphereComponent::Atmosphere::Inspector() {
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

	if (ImGui::TreeNodeEx("Sun", ImGuiTreeNodeFlags_Framed)) {
		SxImGui::DragScalar("Intensity", &intensity, 0.1f);
		ImGui::TreePop();
	}

}

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void SkyAtmosphereComponent::Parameter::Init() {
	flags       = Flag::None;
	environment = NULL;
}

void SkyAtmosphereComponent::Parameter::SetEnvironment(const DxObject::Descriptor& descriptorSRV) {
	flags |= Flag::Environment;
	environment = descriptorSRV.GetIndex();
}

////////////////////////////////////////////////////////////////////////////////////////////
// SkyAtmosphereComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

SkyAtmosphereComponent::SkyAtmosphereComponent(EntityBehaviour* behaviour) : BaseComponent(behaviour) {
	atmosphere_ = std::make_unique<DxObject::ConstantBuffer<Atmosphere>>();
	atmosphere_->Create(System::GetDxDevice());
	atmosphere_->At().Init();

	parameter_ = std::make_unique<DxObject::ConstantBuffer<Parameter>>();
	parameter_->Create(System::GetDxDevice());
	parameter_->At().Init();

	CreateTransmittance();
	CreateMultipleScattering();
	//CreateSkyView();
	CreateSkyCube();
}

void SkyAtmosphereComponent::ShowComponentInspector() {

	SxImGui::Image(textures_[static_cast<uint8_t>(Type::Transmittance)].descriptorSRV.GetGPUHandle().ptr, ImVec2(256.0f, 64.0f));
	SxImGui::Image(textures_[static_cast<uint8_t>(Type::MultipleScattering)].descriptorSRV.GetGPUHandle().ptr, ImVec2(32, 32));
	//SxImGui::Image(skyView_.descriptorSRV.GetGPUHandle().ptr, ImVec2(200.0f, 100.0f));

	atmosphere_->At().Inspector();

}

void SkyAtmosphereComponent::Update(Mode mode, const DirectXQueueContext* context) {
	if (mode == Mode::Dynamic) {
		UpdateTransmittance(context);
		UpdateMultipleScattering(context);
	}

	UpdateSkyCube(context);
	//UpdateSkyView(context);
}

void SkyAtmosphereComponent::UpdateTransmittance(const DirectXQueueContext* context) {

	context->GetDxCommand()->TransitionResourceState(
		textures_[static_cast<uint8_t>(Type::Transmittance)].resource.Get(),
		D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);

	pipeline1_.SetPipeline(context->GetDxCommand());

	DxObject::BindBufferDesc desc = {};
	desc.Set32bitConstants("Dimension", 2, std::array<UINT, 2>{ 256, 64 }.data());
	desc.SetHandle("gTransmittance", textures_[static_cast<uint8_t>(Type::Transmittance)].descriptorUAV.GetGPUHandle());
	desc.SetAddress("gAtmosphere",   atmosphere_->GetGPUVirtualAddress());

	pipeline1_.BindComputeBuffer(context->GetDxCommand(), desc);
	pipeline1_.Dispatch(context->GetDxCommand(), { DxObject::RoundUp(256, 16), DxObject::RoundUp(64, 16), 1 });

	context->GetDxCommand()->TransitionResourceState(
		textures_[static_cast<uint8_t>(Type::Transmittance)].resource.Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE
	);
}

void SkyAtmosphereComponent::UpdateMultipleScattering(const DirectXQueueContext* context) {

	context->GetDxCommand()->TransitionResourceState(
		textures_[static_cast<uint8_t>(Type::MultipleScattering)].resource.Get(),
		D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);

	pipeline2_.SetPipeline(context->GetDxCommand());

	DxObject::BindBufferDesc desc = {};
	desc.Set32bitConstants("Dimension", 2, std::array<UINT, 2>{ 32, 32 }.data());
	desc.SetHandle("gTransmittance", textures_[static_cast<uint8_t>(Type::Transmittance)].descriptorSRV.GetGPUHandle());
	desc.SetHandle("gMultipleScattering", textures_[static_cast<uint8_t>(Type::MultipleScattering)].descriptorUAV.GetGPUHandle());
	desc.SetAddress("gAtmosphere", atmosphere_->GetGPUVirtualAddress());

	pipeline2_.BindComputeBuffer(context->GetDxCommand(), desc);
	pipeline2_.Dispatch(context->GetDxCommand(), { 32, 32, 1 });

	context->GetDxCommand()->TransitionResourceState(
		textures_[static_cast<uint8_t>(Type::MultipleScattering)].resource.Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE
	);

}

void SkyAtmosphereComponent::UpdateSkyView(const DirectXQueueContext* context) {

	context->GetDxCommand()->TransitionResourceState(
		textures_[static_cast<uint8_t>(Type::SkyView)].resource.Get(),
		D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);

	pipeline3_.SetPipeline(context->GetDxCommand());

	DxObject::BindBufferDesc desc = {};
	desc.Set32bitConstants("Dimension", 2, std::array<UINT, 2>{ 200, 100 }.data());
	desc.SetHandle("gSkyView",            textures_[static_cast<uint8_t>(Type::SkyView)].descriptorUAV.GetGPUHandle());
	desc.SetHandle("gTransmittance",      textures_[static_cast<uint8_t>(Type::Transmittance)].descriptorSRV.GetGPUHandle());
	desc.SetHandle("gMultipleScattering", textures_[static_cast<uint8_t>(Type::MultipleScattering)].descriptorSRV.GetGPUHandle());
	desc.SetAddress("gAtmosphere",        atmosphere_->GetGPUVirtualAddress());
	desc.SetAddress("gTransform",         RequireTransform()->GetGPUVirtualAddress());

	pipeline3_.BindComputeBuffer(context->GetDxCommand(), desc);
	pipeline3_.Dispatch(context->GetDxCommand(), { DxObject::RoundUp(200, 16), DxObject::RoundUp(100, 16), 1 });

	context->GetDxCommand()->TransitionResourceState(
		textures_[static_cast<uint8_t>(Type::SkyView)].resource.Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE
	);

}

void SkyAtmosphereComponent::UpdateSkyCube(const DirectXQueueContext* context) {

	context->GetDxCommand()->TransitionResourceState(
		textures_[static_cast<uint8_t>(Type::SkyCube)].resource.Get(),
		D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);

	pipeline4_.SetPipeline(context->GetDxCommand());

	DxObject::BindBufferDesc desc = {};
	desc.Set32bitConstants("Dimension", 2, std::array<UINT, 2>{ 128, 128 }.data());
	desc.SetHandle("gSkyCube",            textures_[static_cast<uint8_t>(Type::SkyCube)].descriptorUAV.GetGPUHandle());
	desc.SetHandle("gTransmittance",      textures_[static_cast<uint8_t>(Type::Transmittance)].descriptorSRV.GetGPUHandle());
	desc.SetHandle("gMultipleScattering", textures_[static_cast<uint8_t>(Type::MultipleScattering)].descriptorSRV.GetGPUHandle());
	desc.SetAddress("gAtmosphere",        atmosphere_->GetGPUVirtualAddress());
	desc.SetAddress("gTransform",         RequireTransform()->GetGPUVirtualAddress());

	pipeline4_.BindComputeBuffer(context->GetDxCommand(), desc);
	pipeline4_.Dispatch(context->GetDxCommand(), { DxObject::RoundUp(128, 16), DxObject::RoundUp(128, 16), 6 });

	context->GetDxCommand()->TransitionResourceState(
		textures_[static_cast<uint8_t>(Type::SkyCube)].resource.Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE
	);
	
}

void SkyAtmosphereComponent::CreateTransmittance() {

	auto device = System::GetDxDevice()->GetDevice();

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
			D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(&textures_[static_cast<uint8_t>(Type::Transmittance)].resource)
		);
		DxObject::Assert(hr, L"texture create failed.");
	}

	{ //!< descriptorUAVの生成
		// handleの取得
		textures_[static_cast<uint8_t>(Type::Transmittance)].descriptorUAV = System::GetDescriptor(DxObject::kDescriptor_UAV);

		// descの設定
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format        = DXGI_FORMAT_R11G11B10_FLOAT;
		desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

		// UAVの生成
		device->CreateUnorderedAccessView(
			textures_[static_cast<uint8_t>(Type::Transmittance)].resource.Get(),
			nullptr,
			&desc,
			textures_[static_cast<uint8_t>(Type::Transmittance)].descriptorUAV.GetCPUHandle()
		);
	}

	{ //!< descriptorSRVの生成
		// handleの取得
		textures_[static_cast<uint8_t>(Type::Transmittance)].descriptorSRV = System::GetDescriptor(DxObject::kDescriptor_SRV);

		// descの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                    = DXGI_FORMAT_R11G11B10_FLOAT;
		desc.ViewDimension             = D3D12_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels       = 1;
		desc.Shader4ComponentMapping   = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		// SRVの生成
		device->CreateShaderResourceView(
			textures_[static_cast<uint8_t>(Type::Transmittance)].resource.Get(),
			&desc,
			textures_[static_cast<uint8_t>(Type::Transmittance)].descriptorSRV.GetCPUHandle()
		);
	}

	pipeline1_.CreateBlob(kPackagesDirectory / "shaders" / "Render" / "Atmosphere" / "Transmittance.cs.hlsl");
	pipeline1_.ReflectionPipeline(System::GetDxDevice());
}

void SkyAtmosphereComponent::CreateMultipleScattering() {

	auto device = System::GetDxDevice()->GetDevice();

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
			D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(&textures_[static_cast<uint8_t>(Type::MultipleScattering)].resource)
		);
		DxObject::Assert(hr, L"texture create failed.");
	}

	{ //!< descriptorUAVの生成
		// handleの取得
		textures_[static_cast<uint8_t>(Type::MultipleScattering)].descriptorUAV = System::GetDescriptor(DxObject::kDescriptor_UAV);

		// descの設定
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format        = DXGI_FORMAT_R11G11B10_FLOAT;
		desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

		// UAVの生成
		device->CreateUnorderedAccessView(
			textures_[static_cast<uint8_t>(Type::MultipleScattering)].resource.Get(),
			nullptr,
			&desc,
			textures_[static_cast<uint8_t>(Type::MultipleScattering)].descriptorUAV.GetCPUHandle()
		);
	}

	{ //!< descriptorSRVの生成
		// handleの取得
		textures_[static_cast<uint8_t>(Type::MultipleScattering)].descriptorSRV = System::GetDescriptor(DxObject::kDescriptor_SRV);

		// descの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                    = DXGI_FORMAT_R11G11B10_FLOAT;
		desc.ViewDimension             = D3D12_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels       = 1;
		desc.Shader4ComponentMapping   = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		// SRVの生成
		device->CreateShaderResourceView(
			textures_[static_cast<uint8_t>(Type::MultipleScattering)].resource.Get(),
			&desc,
			textures_[static_cast<uint8_t>(Type::MultipleScattering)].descriptorSRV.GetCPUHandle()
		);
	}

	pipeline2_.CreateBlob(kPackagesDirectory / "shaders" / "Render" / "Atmosphere" / "MultipleScattering.cs.hlsl");

	DxObject::SamplerBindDesc desc = {};
	desc.SetSamplerLinear("gSampler", DxObject::SamplerMode::MODE_CLAMP);

	pipeline2_.ReflectionPipeline(System::GetDxDevice(), desc);
}

void SkyAtmosphereComponent::CreateSkyView() {

	auto device = System::GetDxDevice()->GetDevice();

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
			D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(&textures_[static_cast<uint8_t>(Type::SkyView)].resource)
		);
		DxObject::Assert(hr, L"texture create failed.");
	}

	{ //!< descriptorUAVの生成
		// handleの取得
		textures_[static_cast<uint8_t>(Type::SkyView)].descriptorUAV = System::GetDescriptor(DxObject::kDescriptor_UAV);

		// descの設定
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format        = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

		// UAVの生成
		device->CreateUnorderedAccessView(
			textures_[static_cast<uint8_t>(Type::SkyView)].resource.Get(),
			nullptr,
			&desc,
			textures_[static_cast<uint8_t>(Type::SkyView)].descriptorUAV.GetCPUHandle()
		);
	}

	{ //!< descriptorSRVの生成
		// handleの取得
		textures_[static_cast<uint8_t>(Type::SkyView)].descriptorSRV = System::GetDescriptor(DxObject::kDescriptor_SRV);

		// descの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                    = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.ViewDimension             = D3D12_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels       = 1;
		desc.Shader4ComponentMapping   = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		// SRVの生成
		device->CreateShaderResourceView(
			textures_[static_cast<uint8_t>(Type::SkyView)].resource.Get(),
			&desc,
			textures_[static_cast<uint8_t>(Type::SkyView)].descriptorSRV.GetCPUHandle()
		);
	}

	pipeline3_.CreateBlob(kPackagesDirectory / "shaders" / "render" / "Atmosphere" / "SkyView.cs.hlsl");

	DxObject::SamplerBindDesc desc = {};
	desc.SetSamplerLinear("gSampler", DxObject::SamplerMode::MODE_CLAMP);

	pipeline3_.ReflectionPipeline(System::GetDxDevice(), desc);

}

void SkyAtmosphereComponent::CreateSkyCube() {

	auto device = System::GetDxDevice()->GetDevice();

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
			D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(&textures_[static_cast<uint8_t>(Type::SkyCube)].resource)
		);
		DxObject::Assert(hr, L"texture create failed.");
	}

	{ //!< descriptorUAVの生成
		// handleの取得
		textures_[static_cast<uint8_t>(Type::SkyCube)].descriptorUAV = System::GetDescriptor(DxObject::kDescriptor_UAV);

		// descの設定
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format                   = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.ViewDimension            = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.ArraySize = 6;

		// UAVの生成
		device->CreateUnorderedAccessView(
			textures_[static_cast<uint8_t>(Type::SkyCube)].resource.Get(),
			nullptr,
			&desc,
			textures_[static_cast<uint8_t>(Type::SkyCube)].descriptorUAV.GetCPUHandle()
		);
	}

	{ //!< descriptorSRVの生成
		// handleの取得
		textures_[static_cast<uint8_t>(Type::SkyCube)].descriptorSRV = System::GetDescriptor(DxObject::kDescriptor_SRV);

		// descの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                  = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURECUBE;
		desc.TextureCube.MipLevels   = 1;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		// SRVの生成
		device->CreateShaderResourceView(
			textures_[static_cast<uint8_t>(Type::SkyCube)].resource.Get(),
			&desc,
			textures_[static_cast<uint8_t>(Type::SkyCube)].descriptorSRV.GetCPUHandle()
		);
	}

	pipeline4_.CreateBlob(kPackagesDirectory / "shaders" / "render" / "Atmosphere" / "SkyCube.cs.hlsl");

	DxObject::SamplerBindDesc desc = {};
	desc.SetSamplerLinear("gSampler", DxObject::SamplerMode::MODE_CLAMP);

	pipeline4_.ReflectionPipeline(System::GetDxDevice(), desc);

	parameter_->At().SetEnvironment(textures_[static_cast<uint8_t>(Type::SkyCube)].descriptorSRV);

}

void SkyAtmosphereComponent::SetIntensity(float intensity) {
	atmosphere_->At().intensity = intensity;
}

const D3D12_GPU_VIRTUAL_ADDRESS& SkyAtmosphereComponent::GetGPUVirtualAddress() const {
	return parameter_->GetGPUVirtualAddress();
}

const TransformComponent* SkyAtmosphereComponent::RequireTransform() const {
	return BaseComponent::GetBehaviour()->RequireComponent<TransformComponent>();
}
