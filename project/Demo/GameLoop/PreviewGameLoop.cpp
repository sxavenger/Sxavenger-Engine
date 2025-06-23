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
		desc.DepthOrArraySize = 1;
		desc.MipLevels        = 1;
		desc.Format           = DXGI_FORMAT_R8G8B8A8_UNORM;
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
		desc.Format                   = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.ViewDimension            = D3D12_UAV_DIMENSION_TEXTURE2D;

		device->CreateUnorderedAccessView(
			resource_.Get(),
			nullptr,
			&desc,
			descriptorUAV_.GetCPUHandle()
		);
	}

	pipeline_.CreateBlob("assets/shaders/test.cs.hlsl");
	pipeline_.ReflectionPipeline(SxavengerSystem::GetDxDevice());

	// execute pipeline
	pipeline_.SetPipeline(SxavengerSystem::GetMainThreadContext()->GetDxCommand());

	DxObject::BindBufferDesc desc = {};
	desc.SetHandle("gOutput", descriptorUAV_.GetGPUHandle());

	pipeline_.BindComputeBuffer(SxavengerSystem::GetMainThreadContext()->GetDxCommand(), desc);
	pipeline_.Dispatch(
		SxavengerSystem::GetMainThreadContext()->GetDxCommand(),
		{ DxObject::RoundUp(size_.x, 16), DxObject::RoundUp(size_.y, 16), 1 }
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

