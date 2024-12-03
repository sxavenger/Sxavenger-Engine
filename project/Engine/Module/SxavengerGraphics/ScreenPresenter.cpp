#include "ScreenPresenter.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Console/Console.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ScreenPresenter class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ScreenPresenter::Init() {
	ia_ = std::make_unique<VertexDimensionBuffer<ScreenPresentIA>>();
	ia_->Create(SxavengerSystem::GetDxDevice(), 3);
	(*ia_)[0].position = { -1.0f, 1.0f, 1.0f };
	(*ia_)[0].texcoord = { 0.0f, 0.0f };

	(*ia_)[1].position = { 3.0f, 1.0f, 1.0f };
	(*ia_)[1].texcoord = { 2.0f, 0.0f };

	(*ia_)[2].position = { -1.0f, -3.0f, 1.0f };
	(*ia_)[2].texcoord = { 0.0f, 2.0f };
}

void ScreenPresenter::Term() {
}

void ScreenPresenter::Present(GameWindow* window, const DirectXThreadContext* context, const D3D12_GPU_DESCRIPTOR_HANDLE& handle) {

	auto commandList = context->GetCommandList();

	sConsole->SetGraphicsPipeline(kPresentToScreen, context, window->GetSize());

	D3D12_VERTEX_BUFFER_VIEW vbv = ia_->GetVertexBufferView();

	commandList->IASetVertexBuffers(0, 1, &vbv);

	bind_.SetHandle("gTexture", handle);
	sConsole->BindGraphicsBuffer(kPresentToScreen, context, bind_);

	commandList->DrawInstanced(ia_->GetSize(), 1, 0, 0);

}