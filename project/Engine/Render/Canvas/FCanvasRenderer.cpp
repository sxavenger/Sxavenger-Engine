#include "FCanvasRenderer.h"

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
// FCanvasRenderer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FCanvasRenderer::Render(const DirectXQueueContext* context) {
	if (!CheckRender()) {
		return;
	}

	textures_->BeginRenderTargetMainUI(context);

	RenderSpriteComponent(context);
	RenderTextComponent(context);

	textures_->EndRenderTargetMainUI(context);
}

bool FCanvasRenderer::CheckRender() const {
	if (textures_ == nullptr) {
		return false;
	}

	return true;
}

void FCanvasRenderer::RenderSpriteComponent(const DirectXQueueContext* context) {

	FRenderCore::GetInstance()->GetLayer()->SetPipeline(
		FRenderCoreLayer::Type::Sprite, context, textures_->GetSize()
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
		parameter.Set32bitConstants("Dimension", 2, &textures_->GetSize());
		parameter.SetAddress("gTransform",        component->GetRectTransform()->GetGPUVirtualAddress());
		parameter.SetAddress("gTransformationUV", component->GetGPUVirtualAddressUV());
		parameter.SetHandle("gTexture",           handle);

		FRenderCore::GetInstance()->GetLayer()->BindGraphicsBuffer(
			FRenderCoreLayer::Type::Sprite, context, parameter
		);

		component->DrawCall(context);
	});

}

void FCanvasRenderer::RenderTextComponent(const DirectXQueueContext* context) {

	FRenderCore::GetInstance()->GetLayer()->SetPipeline(
		FRenderCoreLayer::Type::Text, context, textures_->GetSize()
	);

	sComponentStorage->ForEachActive<TextRendererComponent>([&](TextRendererComponent* component) {
		if (!component->IsEnable()) {
			return;
		}

		component->PerseText();
		// todo: engine側のupdateでやるようにする

		component->BindIABuffer(context);

		DxObject::BindBufferDesc desc = {};
		desc.Set32bitConstants("Dimension", 2, &textures_->GetSize());
		desc.SetAddress("gTransform", component->GetRectTransform()->GetGPUVirtualAddress());
		desc.SetHandle("gFont",       component->GetFont()->GetGPUHandleSRV());

		FRenderCore::GetInstance()->GetLayer()->BindGraphicsBuffer(
			FRenderCoreLayer::Type::Text, context, desc
		);

		component->DrawCall(context);
	});

}
