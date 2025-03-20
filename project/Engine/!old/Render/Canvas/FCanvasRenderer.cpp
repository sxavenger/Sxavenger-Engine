#include "FCanvasRenderer.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../FRenderCore.h"

//* engine
#include <Engine/Component/ComponentStorage.h>
#include <Engine/Component/Components/SpriteRenderer/SpriteRendererComponent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FCanvasRenderer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FCanvasRenderer::Render(const DirectXThreadContext* context) {
	if (!CheckRender()) {
		return;
	}

	textures_->BeginCanvasPass(context);

	RenderSpriteComponents(context);

	textures_->EndCanvasPass(context);
}

bool FCanvasRenderer::CheckRender() const {
	if (textures_ == nullptr) {
		return false;
	}

	return true;
}

void FCanvasRenderer::RenderSpriteComponents(const DirectXThreadContext* context) {

	FRenderCore::GetInstance()->GetLayer()->SetPipeline(
		FRenderCoreLayer::PipelineType::Sprite, context, textures_->GetSize()
	);

	sComponentStorage->ForEach<SpriteRendererComponent>([&](SpriteRendererComponent* sprite) {
		if (!sprite->CheckTexture()) {
			return;
		}

		sprite->BindAIBuffer(context);

		DxObject::BindBufferDesc parameter = {};
		parameter.SetAddress("gParameter",        textures_->GetParameter());
		parameter.SetAddress("gTransformation2d", sprite->GetTransform2dBuffer());
		parameter.SetAddress("gTransformationUV", sprite->GetTransformUVBuffer());
		parameter.SetHandle("gTexture",           sprite->GetTexture());

		FRenderCore::GetInstance()->GetLayer()->BindGraphicsBuffer(
			FRenderCoreLayer::PipelineType::Sprite, context, parameter
		);

		sprite->DrawCall(context);
	});

}
