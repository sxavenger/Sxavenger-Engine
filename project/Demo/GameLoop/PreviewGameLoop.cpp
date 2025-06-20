#include "PreviewGameLoop.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Component/Components/Camera/CameraComponent.h>
#include <Engine/Component/Components/Light/Punctual/PointLightComponent.h>
#include <Engine/Component/ComponentHelper.h>
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/DevelopEditor.h>

#include <Engine/Content/Exporter/TextureExporter.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PreviewGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PreviewGameLoop::Init(GameLoop::Context* context) {
	context->SetProcess(GameLoop::Process::Init, std::nullopt, [this]() { InitGame(); });
	context->SetProcess(GameLoop::Process::Term, std::nullopt, [this]() { TermGame(); });
	context->SetProcess(GameLoop::Process::Update, std::nullopt, [this]() {
		if (auto develop = sEditorEngine->GetEditor<DevelopEditor>()) {
			if (!develop->IsProcessRequired()) {
				return;
			}
		}

		UpdateGame();
	});
	context->SetProcess(GameLoop::Process::Render, std::nullopt, [this]() { DrawGame(); });

	context->SetCondition(
		[this]() { return !SxavengerSystem::ProcessMessage(); }
	);
}

void PreviewGameLoop::Term() {
}

void PreviewGameLoop::InitGame() {

	main_ = SxavengerSystem::CreateMainWindow(kMainWindowSize, L"sxavenger engine preview window", { 0.14f, 0.2f, 0.24f, 1.f }).lock();
	main_->SetIcon("packages/icon/SxavengerEngineIcon.ico", { 32, 32 });

	auto device = SxavengerSystem::GetDxDevice()->GetDevice();
	
	{ //!< resourceの生成

		// propの設定
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		// descの設定
		D3D12_RESOURCE_DESC desc = {};
		desc.Width            = size_.x;
		desc.Height           = size_.y;
		desc.DepthOrArraySize = static_cast<uint16_t>(size_.z);
		desc.MipLevels        = 1;
		desc.Format           = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		auto hr = device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			nullptr,
			IID_PPV_ARGS(&resource_)
		);
		Exception::Assert(SUCCEEDED(hr));
	}

	{ //!< UAVの生成

		// descriptorの取得
		descriptorUAV_ = SxavengerSystem::GetDescriptor(kDescriptor_UAV);

		// descの設定
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format                   = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.ViewDimension            = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.ArraySize = static_cast<uint32_t>(size_.z);

		device->CreateUnorderedAccessView(
			resource_.Get(),
			nullptr,
			&desc,
			descriptorUAV_.GetCPUHandle()
		);
	}

	pipeline_.CreateBlob("assets/shaders/texcube.cs.hlsl");
	pipeline_.ReflectionPipeline(SxavengerSystem::GetDxDevice());

	// execute pipeline
	pipeline_.SetPipeline(SxavengerSystem::GetMainThreadContext()->GetDxCommand());

	DxObject::BindBufferDesc desc = {};
	desc.SetHandle("gOutput", descriptorUAV_.GetGPUHandle());

	pipeline_.BindComputeBuffer(SxavengerSystem::GetMainThreadContext()->GetDxCommand(), desc);
	pipeline_.Dispatch(
		SxavengerSystem::GetMainThreadContext()->GetDxCommand(),
		{ DxObject::RoundUp(size_.x, 16), DxObject::RoundUp(size_.y, 16), DxObject::RoundUp(size_.z, 1) }
	);

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   = resource_.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_COPY_SOURCE;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	SxavengerSystem::GetMainThreadContext()->GetCommandList()->ResourceBarrier(1, &barrier);

	// save
	TextureExporter::Export(
		SxavengerSystem::GetMainThreadContext(),
		TextureExporter::TextureDimension::TextureCube,
		resource_.Get(),
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		"preview_texture.dds"
	);
}

void PreviewGameLoop::TermGame() {
}

void PreviewGameLoop::UpdateGame() {

	
}

void PreviewGameLoop::DrawGame() {

	main_->BeginRendering();
	main_->ClearWindow();

	SxavengerSystem::RenderImGui();

	main_->EndRendering();
}

