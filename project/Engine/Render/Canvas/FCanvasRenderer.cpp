#include "FCanvasRenderer.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../FRenderCore.h"

//* engine
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

	RenderSpriteComponents(context);
	RenderTextComponent(context);

	textures_->EndRenderTargetMainUI(context);
}

bool FCanvasRenderer::CheckRender() const {
	if (textures_ == nullptr) {
		return false;
	}

	return true;
}

void FCanvasRenderer::RenderSpriteComponents(const DirectXQueueContext* context) {

	FRenderCore::GetInstance()->GetLayer()->SetPipeline(
		FRenderCoreLayer::Type::Sprite, context, textures_->GetSize()
	);

	sComponentStorage->ForEach<SpriteRendererComponent>([&](SpriteRendererComponent* sprite) {
		if (!sprite->CheckTexture()) {
			return;
		}

		sprite->BindAIBuffer(context);

		DxObject::BindBufferDesc parameter = {};
		parameter.Set32bitConstants("Dimension", 2, &textures_->GetSize());
		parameter.SetAddress("gTransformation2d", sprite->GetTransform2dBuffer());
		parameter.SetAddress("gTransformationUV", sprite->GetTransformUVBuffer());
		parameter.SetHandle("gTexture",           sprite->GetTexture());

		FRenderCore::GetInstance()->GetLayer()->BindGraphicsBuffer(
			FRenderCoreLayer::Type::Sprite, context, parameter
		);

		sprite->DrawCall(context);
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
