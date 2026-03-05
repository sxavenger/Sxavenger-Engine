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
	mie_phase_param = 0.2f; //!< 仮値.

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
		ImGui::DragFloat("Phase Param", &mie_phase_param, 0.01f);
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

void SkyAtmosphereComponent::Parameter::SetAerial(const DxObject::Descriptor& descriptorSRV) {
	flags |= Flag::Aerial;
	aerial = descriptorSRV.GetIndex();
}

////////////////////////////////////////////////////////////////////////////////////////////
// Texture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SkyAtmosphereComponent::Texture::Create(const Vector3ui& resolution, DXGI_FORMAT format, Dimension dimension) {

	auto device = System::GetDxDevice()->GetDevice();

	//!< sizeの設定
	size = {};

	switch (dimension) {
		case Dimension::Texture2D: //!< 2Dテクスチャの場合はzを1にする
			size = Vector3ui(resolution.x, resolution.y, 1);
			break;

		case Dimension::TextureCube: //!< キューブマップの場合はzを6にする
			size = Vector3ui(resolution.x, resolution.y, 6);
			break;

		case Dimension::Texture3D: //!< 3Dテクスチャの場合はそのまま
			size = resolution;
			break;
	}

	//!< resourceの作成
	resource = DxObject::Resource::CreateTexture(
		System::GetDxDevice(),
		dimension == Dimension::Texture3D ? D3D12_RESOURCE_DIMENSION_TEXTURE3D : D3D12_RESOURCE_DIMENSION_TEXTURE2D, //!< 3DテクスチャはTEXTURE3D, それ以外はTEXTURE2D
		size,
		1,
		format,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
		std::nullopt
	);

	{ //!< UAVの生成

		//!< handleの取得
		if (!descriptorSRV.HasHandle()) {
			descriptorUAV = System::GetDescriptor(DxObject::kDescriptor_UAV);
		}

		//!< descの設定
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format = format;

		switch (dimension) {
			case Dimension::Texture2D:
				desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
				break;

			case Dimension::TextureCube:
				desc.ViewDimension            = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
				desc.Texture2DArray.ArraySize = 6;
				break;

			case Dimension::Texture3D:
				desc.ViewDimension       = D3D12_UAV_DIMENSION_TEXTURE3D;
				desc.Texture3D.WSize     = size.z;
				break;
		}

		//!< UAVの生成
		device->CreateUnorderedAccessView(
			resource.Get(),
			nullptr,
			&desc,
			descriptorUAV.GetCPUHandle()
		);
	}

	{ //!< SRVの生成

		//!< handleの取得
		if (!descriptorSRV.HasHandle()) {
			descriptorSRV = System::GetDescriptor(DxObject::kDescriptor_SRV);
		}

		//!< descの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                  = format;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		switch (dimension) {
			case Dimension::Texture2D:
				desc.ViewDimension       = D3D12_SRV_DIMENSION_TEXTURE2D;
				desc.Texture2D.MipLevels = 1;
				break;

			case Dimension::TextureCube:
				desc.ViewDimension         = D3D12_SRV_DIMENSION_TEXTURECUBE;
				desc.TextureCube.MipLevels = 1;
				break;

			case Dimension::Texture3D:
				desc.ViewDimension       = D3D12_SRV_DIMENSION_TEXTURE3D;
				desc.Texture3D.MipLevels = 1;
				break;
		}

		//!< SRVの生成
		device->CreateShaderResourceView(
			resource.Get(),
			&desc,
			descriptorSRV.GetCPUHandle()
		);
	}
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
	CreateSkyCube();
	//CreateAerial(); FIXME
}

void SkyAtmosphereComponent::ShowComponentInspector() {

	SxGui::ImageLabel("transmittance", textures_[static_cast<uint8_t>(Type::Transmittance)].descriptorSRV.GetGPUHandle().ptr, ImVec2(256.0f, 64.0f));
	SxGui::ImageLabel("multiple scattering", textures_[static_cast<uint8_t>(Type::MultipleScattering)].descriptorSRV.GetGPUHandle().ptr, ImVec2(32, 32));
	//SxImGui::Image(skyView_.descriptorSRV.GetGPUHandle().ptr, ImVec2(200.0f, 100.0f));

	atmosphere_->At().Inspector();

}

void SkyAtmosphereComponent::Update(Mode mode, const DirectXQueueContext* context) {
	if (mode == Mode::Dynamic) {
		UpdateTransmittance(context);
		UpdateMultipleScattering(context);
	}

	UpdateSkyCube(context);
	//UpdateAerial(context);
}

void SkyAtmosphereComponent::CreateTransmittance() {

	//!< textureの生成
	textures_[static_cast<uint8_t>(Type::Transmittance)].Create({ 256, 64, 1 }, DXGI_FORMAT_R11G11B10_FLOAT, Texture::Dimension::Texture2D);
	textures_[static_cast<uint8_t>(Type::Transmittance)].resource.SetName(L"SkyAtmosphereComponent | Transmittance");

	//!< pipelineの生成
	pipelines_[static_cast<uint8_t>(Type::Transmittance)].CreateBlob(kPackagesDirectory / "shaders" / "Render" / "Atmosphere" / "Transmittance.cs.hlsl");
	pipelines_[static_cast<uint8_t>(Type::Transmittance)].ReflectionPipeline(System::GetDxDevice());
}

void SkyAtmosphereComponent::CreateMultipleScattering() {

	//!< textureの生成
	textures_[static_cast<uint8_t>(Type::MultipleScattering)].Create({ 32, 32, 1 }, DXGI_FORMAT_R11G11B10_FLOAT, Texture::Dimension::Texture2D);
	textures_[static_cast<uint8_t>(Type::MultipleScattering)].resource.SetName(L"SkyAtmosphereComponent | MultipleScattering");

	//!< pipelineの生成

	pipelines_[static_cast<uint8_t>(Type::MultipleScattering)].CreateBlob(kPackagesDirectory / "shaders" / "Render" / "Atmosphere" / "MultipleScattering.cs.hlsl");

	DxObject::SamplerBindDesc desc = {};
	desc.SetSamplerLinear("gSampler", DxObject::SamplerMode::MODE_CLAMP);

	pipelines_[static_cast<uint8_t>(Type::MultipleScattering)].ReflectionPipeline(System::GetDxDevice(), desc);
}

void SkyAtmosphereComponent::CreateSkyCube() {

	//!< textureの生成
	textures_[static_cast<uint8_t>(Type::SkyCube)].Create({ 128, 128, 6 }, DXGI_FORMAT_R16G16B16A16_FLOAT, Texture::Dimension::TextureCube);
	textures_[static_cast<uint8_t>(Type::SkyCube)].resource.SetName(L"SkyAtmosphereComponent | SkyCube");

	//!< pipelineの生成

	pipelines_[static_cast<uint8_t>(Type::SkyCube)].CreateBlob(kPackagesDirectory / "shaders" / "render" / "Atmosphere" / "SkyCube.cs.hlsl");

	DxObject::SamplerBindDesc desc = {};
	desc.SetSamplerLinear("gSampler", DxObject::SamplerMode::MODE_CLAMP);

	pipelines_[static_cast<uint8_t>(Type::SkyCube)].ReflectionPipeline(System::GetDxDevice(), desc);

	//!< parameterの更新
	parameter_->At().SetEnvironment(textures_[static_cast<uint8_t>(Type::SkyCube)].descriptorSRV);

}

void SkyAtmosphereComponent::CreateAerial() {

	//!< textureの生成
	textures_[static_cast<uint8_t>(Type::Aerial)].Create({ 32, 32, 16 }, DXGI_FORMAT_R16G16B16A16_FLOAT, Texture::Dimension::Texture3D);
	textures_[static_cast<uint8_t>(Type::Aerial)].resource.SetName(L"SkyAtmosphereComponent | Aerial");

	//!< pipelineの生成
	
	pipelines_[static_cast<uint8_t>(Type::Aerial)].CreateBlob(kPackagesDirectory / "shaders" / "render" / "Atmosphere" / "Aerial.cs.hlsl");

	DxObject::SamplerBindDesc desc = {};
	desc.SetSamplerLinear("gSampler", DxObject::SamplerMode::MODE_CLAMP);

	pipelines_[static_cast<uint8_t>(Type::Aerial)].ReflectionPipeline(System::GetDxDevice(), desc);

	//!< parameterの更新
	parameter_->At().SetAerial(textures_[static_cast<uint8_t>(Type::Aerial)].descriptorSRV);
}

void SkyAtmosphereComponent::UpdateTransmittance(const DirectXQueueContext* context) {

	textures_[static_cast<uint8_t>(Type::Transmittance)].resource.Transition(
		context->GetDxCommand(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);

	pipelines_[static_cast<uint8_t>(Type::Transmittance)].SetPipeline(context->GetDxCommand());

	const Vector3ui& resolution = textures_[static_cast<uint8_t>(Type::Transmittance)].size;

	DxObject::BindBufferDesc desc = {};
	desc.Set32bitConstants("Dimension", 3, &resolution);
	desc.SetHandle("gTransmittance",       textures_[static_cast<uint8_t>(Type::Transmittance)].descriptorUAV.GetGPUHandle());
	desc.SetAddress("gAtmosphere",         atmosphere_->GetGPUVirtualAddress());

	pipelines_[static_cast<uint8_t>(Type::Transmittance)].BindComputeBuffer(context->GetDxCommand(), desc);
	pipelines_[static_cast<uint8_t>(Type::Transmittance)].Dispatch(context->GetDxCommand(), { DxObject::RoundUp(resolution.x, 16), DxObject::RoundUp(resolution.y, 16), 1 });

	textures_[static_cast<uint8_t>(Type::Transmittance)].resource.Transition(
		context->GetDxCommand(),
		D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE
	);
}

void SkyAtmosphereComponent::UpdateMultipleScattering(const DirectXQueueContext* context) {

	textures_[static_cast<uint8_t>(Type::MultipleScattering)].resource.Transition(
		context->GetDxCommand(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);

	pipelines_[static_cast<uint8_t>(Type::MultipleScattering)].SetPipeline(context->GetDxCommand());

	const Vector3ui& resolution = textures_[static_cast<uint8_t>(Type::MultipleScattering)].size;

	DxObject::BindBufferDesc desc = {};
	desc.Set32bitConstants("Dimension", 3, &resolution);
	desc.SetHandle("gTransmittance",       textures_[static_cast<uint8_t>(Type::Transmittance)].descriptorSRV.GetGPUHandle());
	desc.SetHandle("gMultipleScattering",  textures_[static_cast<uint8_t>(Type::MultipleScattering)].descriptorUAV.GetGPUHandle());
	desc.SetAddress("gAtmosphere",         atmosphere_->GetGPUVirtualAddress());

	pipelines_[static_cast<uint8_t>(Type::MultipleScattering)].BindComputeBuffer(context->GetDxCommand(), desc);
	pipelines_[static_cast<uint8_t>(Type::MultipleScattering)].Dispatch(context->GetDxCommand(), { resolution.x, resolution.y, 1 });

	textures_[static_cast<uint8_t>(Type::MultipleScattering)].resource.Transition(
		context->GetDxCommand(),
		D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE
	);

}

void SkyAtmosphereComponent::UpdateSkyCube(const DirectXQueueContext* context) {

	textures_[static_cast<uint8_t>(Type::SkyCube)].resource.Transition(
		context->GetDxCommand(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);

	pipelines_[static_cast<uint8_t>(Type::SkyCube)].SetPipeline(context->GetDxCommand());

	const Vector3ui& resolution = textures_[static_cast<uint8_t>(Type::SkyCube)].size;

	DxObject::BindBufferDesc desc = {};
	desc.Set32bitConstants("Dimension", 3, &resolution);
	desc.SetHandle("gSkyCube",             textures_[static_cast<uint8_t>(Type::SkyCube)].descriptorUAV.GetGPUHandle());
	desc.SetHandle("gTransmittance",       textures_[static_cast<uint8_t>(Type::Transmittance)].descriptorSRV.GetGPUHandle());
	desc.SetHandle("gMultipleScattering",  textures_[static_cast<uint8_t>(Type::MultipleScattering)].descriptorSRV.GetGPUHandle());
	desc.SetAddress("gAtmosphere",         atmosphere_->GetGPUVirtualAddress());
	desc.SetAddress("gTransform",          RequireTransform()->GetGPUVirtualAddress());

	pipelines_[static_cast<uint8_t>(Type::SkyCube)].BindComputeBuffer(context->GetDxCommand(), desc);
	pipelines_[static_cast<uint8_t>(Type::SkyCube)].Dispatch(context->GetDxCommand(), { DxObject::RoundUp(resolution.x, 16), DxObject::RoundUp(resolution.y, 16), resolution.z });

	textures_[static_cast<uint8_t>(Type::SkyCube)].resource.Transition(
		context->GetDxCommand(),
		D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE
	);
	
}

void SkyAtmosphereComponent::UpdateAerial(const DirectXQueueContext* context) {

	textures_[static_cast<uint8_t>(Type::Aerial)].resource.Transition(
		context->GetDxCommand(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);

	pipelines_[static_cast<uint8_t>(Type::Aerial)].SetPipeline(context->GetDxCommand());

	const Vector3ui& resolution = textures_[static_cast<uint8_t>(Type::Aerial)].size;

	DxObject::BindBufferDesc desc = {};
	desc.Set32bitConstants("Dimension", 3, &resolution);
	desc.SetHandle("gAerial",              textures_[static_cast<uint8_t>(Type::Aerial)].descriptorUAV.GetGPUHandle());
	desc.SetHandle("gTransmittance",       textures_[static_cast<uint8_t>(Type::Transmittance)].descriptorSRV.GetGPUHandle());
	desc.SetHandle("gMultipleScattering",  textures_[static_cast<uint8_t>(Type::MultipleScattering)].descriptorSRV.GetGPUHandle());
	desc.SetAddress("gAtmosphere",         atmosphere_->GetGPUVirtualAddress());
	desc.SetAddress("gTransform",          RequireTransform()->GetGPUVirtualAddress());

	pipelines_[static_cast<uint8_t>(Type::Aerial)].BindComputeBuffer(context->GetDxCommand(), desc);
	pipelines_[static_cast<uint8_t>(Type::Aerial)].Dispatch(context->GetDxCommand(), { DxObject::RoundUp(resolution.x, 16), DxObject::RoundUp(resolution.y, 16), resolution.z });

	textures_[static_cast<uint8_t>(Type::Aerial)].resource.Transition(
		context->GetDxCommand(),
		D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE
	);
}

void SkyAtmosphereComponent::SetIntensity(float intensity) {
	atmosphere_->At().intensity = intensity;
}

const D3D12_GPU_VIRTUAL_ADDRESS SkyAtmosphereComponent::GetGPUVirtualAddress() const {
	return parameter_->GetGPUVirtualAddress();
}

const TransformComponent* SkyAtmosphereComponent::RequireTransform() const {
	return BaseComponent::GetBehaviour()->RequireComponent<TransformComponent>();
}
