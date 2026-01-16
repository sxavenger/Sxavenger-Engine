#include "FRenderPassCanvas.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../FRenderCore.h"

//* engine
#include <Engine/Graphics/Graphics.h>
#include <Engine/Components/Component/CanvasRenderer/SpriteRendererComponent.h>
#include <Engine/Components/Component/CanvasRenderer/TextRendererComponent.h>
#include <Engine/Components/Component/ComponentStorage.h>


////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassCanvas class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderPassCanvas::Render(const DirectXQueueContext* context, const Config& config) {

	context->BeginEvent(L"RenderPass - Canvas");
	System::BeginRecordGpu(std::format("[{}] RenderPass - Canvas", magic_enum::enum_name(config.tag)));

	config.buffer->TransitionBeginRenderTargetMainCanvas(context);

	PassSprite(context, config);

	PassText(context, config);

	config.buffer->TransitionEndRenderTargetMainCanvas(context);

	System::EndRecordGpu();
	context->EndEvent();
}

void FRenderPassCanvas::PassSprite(const DirectXQueueContext* context, const Config& config) {

	auto core = FRenderCore::GetInstance()->GetCanvas();

	core->SetPipeline(FRenderCoreCanvas::Type::Sprite, context, config.buffer->GetSize());

	sComponentStorage->ForEachActive<SpriteRendererComponent>([&](SpriteRendererComponent* component) {
		if (!component->IsEnable()) {
			return;
		}

		component->BindAIBuffer(context);

		D3D12_GPU_DESCRIPTOR_HANDLE handle = component->GetTextureParameter().Empty()
			? Graphics::GetGPUHandleSRV("white1x1")
			: component->GetTextureParameter().WaitRequire()->GetGPUHandleSRV();

		DxObject::BindBufferDesc parameter = {};
		parameter.Set32bitConstants("Dimension", 2, &config.buffer->GetSize());
		parameter.SetAddress("gTransform",        component->GetRectTransform()->GetGPUVirtualAddress());
		parameter.SetAddress("gTransformationUV", component->GetGPUVirtualAddressUV());
		parameter.SetHandle("gTexture",           handle);

		core->BindGraphicsBuffer(FRenderCoreCanvas::Type::Sprite, context, parameter);

		component->DrawCall(context);
	});

}

void FRenderPassCanvas::PassText(const DirectXQueueContext* context, const Config& config) {

	auto core = FRenderCore::GetInstance()->GetCanvas();

	core->SetPipeline(FRenderCoreCanvas::Type::Text, context, config.buffer->GetSize());

	sComponentStorage->ForEachActive<TextRendererComponent>([&](TextRendererComponent* component) {
		if (!component->IsEnable()) {
			return;
		}

		component->ParseText();
		// todo: engine側のupdateでやるようにする

		component->BindIABuffer(context);

		DxObject::BindBufferDesc desc = {};
		desc.Set32bitConstants("Dimension", 2, &config.buffer->GetSize());
		desc.SetAddress("gTransform", component->GetRectTransform()->GetGPUVirtualAddress());
		desc.SetHandle("gFont",       component->GetFont()->GetGPUHandleSRV());

		core->BindGraphicsBuffer(FRenderCoreCanvas::Type::Text, context, desc);

		component->DrawCall(context);
	});

}
