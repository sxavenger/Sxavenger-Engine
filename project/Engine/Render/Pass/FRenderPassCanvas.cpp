#include "FRenderPassCanvas.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../Buffer/FMainBuffer.h"
#include "../Core/FRenderCore.h"
#include "../Core/FRenderCoreCanvas.h"

//* engine
#include <Engine/System/Utility/RuntimeLogger.h>
#include <Engine/Graphics/Graphics.h>
#include <Engine/Components/Component/CanvasRenderer/SpriteRendererComponent.h>
#include <Engine/Components/Component/CanvasRenderer/TextRendererComponent.h>
#include <Engine/Components/Component/ComponentStorage.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassCanvas class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderPassCanvas::Render(const DirectXQueueContext* context, const FRenderConfig& config) {

	if (!config.buffer->HasBuffer<FMainBuffer>()) {
		RuntimeLogger::LogError("[FRenderPass - Canvas]", "FMainBuffer is required."); //!< FMainBufferが必要
		return; //!< Bufferが不適格
	}

	FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreCanvas>(); //!< RenderCoreの確保

	FBaseRenderPass::BeginRenderPass(context, "Canvas", config);

	{ //!< Render Canvas Pass

		BeginRenderCanvasPass(context, config.buffer);

		RenderSprite(context, config);

		RenderText(context, config);

		EndRenderCanvasPass(context, config.buffer);
	}

	FBaseRenderPass::EndRenderPass(context);
}

void FRenderPassCanvas::BeginRenderCanvasPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	auto commandList = context->GetCommandList();

	FMainBuffer* main          = buffer->GetBuffer<FMainBuffer>();
	FPriorityTexture* priority = buffer->GetPriority();

	static const size_t kBufferCount = 1;
	std::array<FRenderTexture*, kBufferCount> buffers = {
		&main->GetBuffer(FMainBuffer::Layout::Canvas),
	};

	{ //!< barrierの設定

		std::vector<D3D12_RESOURCE_BARRIER> barriers;

		//!< Main Bufferのbarrier設定
		for (size_t i = 0; i < kBufferCount; ++i) {
			buffers[i]->SetTransitionRenderTarget(barriers);
		}
		
		//!< barrierの発行
		context->GetDxCommand()->ResourceBarrier(barriers);
	}

	{ //!< Render Targetの設定

		std::array<D3D12_CPU_DESCRIPTOR_HANDLE, kBufferCount> handles = {};

		for (size_t i = 0; i < kBufferCount; ++i) {
			handles[i] = buffers[i]->GetCPUHandleRTV();
		}

		commandList->OMSetRenderTargets(
			static_cast<UINT>(handles.size()), handles.data(), false,
			&priority->GetCPUHandleDSV()
		);
	}
}

void FRenderPassCanvas::EndRenderCanvasPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	FMainBuffer* main = buffer->GetBuffer<FMainBuffer>();

	static const size_t kBufferCount = 1;
	std::array<FRenderTexture*, kBufferCount> buffers = {
		&main->GetBuffer(FMainBuffer::Layout::Canvas),
	};

	{ //!< barrierの設定
		std::vector<D3D12_RESOURCE_BARRIER> barriers;

		//!< Main Bufferのbarrier設定
		for (size_t i = 0; i < kBufferCount; ++i) {
			buffers[i]->SetTransitionDefaultState(barriers);
		}

		//!< barrierの発行
		context->GetDxCommand()->ResourceBarrier(barriers);
	}
}

void FRenderPassCanvas::RenderSprite(const DirectXQueueContext* context, const FRenderConfig& config) {

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreCanvas>();
	core->SetPipeline(FRenderCoreCanvas::Pipeline::Sprite, context, config.buffer->GetResolution());

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};

	//!< 共通parameterの設定
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetResolution());

	sComponentStorage->ForEachActive<SpriteRendererComponent>([&](SpriteRendererComponent* component) {

		if (!component->IsEnable()) {
			return; //!< 不適格component.
		}

		component->BindInputAssembler(context);

		D3D12_GPU_DESCRIPTOR_HANDLE handle = component->GetTextureParameter().Empty()
			? Graphics::GetGPUHandleSRV("white1x1")
			: component->GetTextureParameter().WaitRequire()->GetGPUHandleSRV();

		//!< componentのparameterの設定
		desc.SetAddress("gTransform",        component->GetRectTransform()->GetGPUVirtualAddress());
		desc.SetAddress("gTransformationUV", component->GetGPUVirtualAddressUV());
		desc.SetHandle("gTexture",           handle);

		core->BindGraphicsBuffer(FRenderCoreCanvas::Pipeline::Sprite, context, desc);
		component->DrawCall(context);
	});

}

void FRenderPassCanvas::RenderText(const DirectXQueueContext* context, const FRenderConfig& config) {

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreCanvas>();
	core->SetPipeline(FRenderCoreCanvas::Pipeline::Text, context, config.buffer->GetResolution());

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};

	//!< 共通parameterの設定
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetResolution());

	sComponentStorage->ForEachActive<TextRendererComponent>([&](TextRendererComponent* component) {

		if (!component->IsEnable()) {
			return; //!< 不適格component.
		}

		component->ParseText();
		// todo: engine側のupdateでやるようにする

		component->BindInputAssembler(context);

		//!< componentのparameterの設定
		desc.SetAddress("gTransform", component->GetRectTransform()->GetGPUVirtualAddress());
		desc.SetHandle("gFont",       component->GetFont()->GetGPUHandleSRV());

		core->BindGraphicsBuffer(FRenderCoreCanvas::Pipeline::Text, context, desc);
		component->DrawCall(context);
	});
	
}
