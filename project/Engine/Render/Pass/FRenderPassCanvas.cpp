#include "FRenderPassCanvas.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../FRenderCore.h"

//* engine
#include <Engine/Content/SxavengerContent.h>
#include <Engine/Component/Components/ComponentStorage.h>
#include <Engine/Component/Components/SpriteRenderer/SpriteRendererComponent.h>
#include <Engine/Component/Components/TextRenderer/TextRendererComponent.h>


////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassCanvas class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderPassCanvas::Render(const DirectXQueueContext* context, const Config& config) {

	context->BeginEvent(L"RenderPass - Canvas");

	config.buffer->BeginRenderTargetMainUI(context);

	PassSprite(context, config);

	PassText(context, config);

	config.buffer->EndRenderTargetMainUI(context);

	context->EndEvent();
}

void FRenderPassCanvas::PassSprite(const DirectXQueueContext* context, const Config& config) {

	FRenderCore::GetInstance()->GetLayer()->SetPipeline(
		FRenderCoreLayer::Type::Sprite, context, config.buffer->GetSize()
	);

	sComponentStorage->ForEachActive<SpriteRendererComponent>([&](SpriteRendererComponent* component) {
		if (!component->IsEnable()) {
			return;
		}

		component->BindAIBuffer(context);

		D3D12_GPU_DESCRIPTOR_HANDLE handle = component->GetTextureParameter().Empty()
			? SxavengerContent::GetGPUHandleSRV("white1x1")
			: component->GetTextureParameter().WaitRequire()->GetGPUHandleSRV();

		DxObject::BindBufferDesc parameter = {};
		parameter.Set32bitConstants("Dimension", 2, &config.buffer->GetSize());
		parameter.SetAddress("gTransform",        component->GetRectTransform()->GetGPUVirtualAddress());
		parameter.SetAddress("gTransformationUV", component->GetGPUVirtualAddressUV());
		parameter.SetHandle("gTexture",           handle);

		FRenderCore::GetInstance()->GetLayer()->BindGraphicsBuffer(
			FRenderCoreLayer::Type::Sprite, context, parameter
		);

		component->DrawCall(context);
	});

}

void FRenderPassCanvas::PassText(const DirectXQueueContext* context, const Config& config) {

	FRenderCore::GetInstance()->GetLayer()->SetPipeline(
		FRenderCoreLayer::Type::Text, context, config.buffer->GetSize()
	);

	sComponentStorage->ForEachActive<TextRendererComponent>([&](TextRendererComponent* component) {
		if (!component->IsEnable()) {
			return;
		}

		component->PerseText();
		// todo: engine側のupdateでやるようにする

		component->BindIABuffer(context);

		DxObject::BindBufferDesc desc = {};
		desc.Set32bitConstants("Dimension", 2, &config.buffer->GetSize());
		desc.SetAddress("gTransform", component->GetRectTransform()->GetGPUVirtualAddress());
		desc.SetHandle("gFont",       component->GetFont()->GetGPUHandleSRV());

		FRenderCore::GetInstance()->GetLayer()->BindGraphicsBuffer(
			FRenderCoreLayer::Type::Text, context, desc
		);

		component->DrawCall(context);
	});

}
