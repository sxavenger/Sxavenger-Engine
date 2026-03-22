#include "FRenderTargetBuffer.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FMainBuffer.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderTargetBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderTargetBuffer::Init(const Vector2ui& resolution) {

	//!< 解像度の更新
	resolution_ = resolution;

	//!< DepthStencilの生成
	depthStencil_ = std::make_unique<FDepthStencilTexture>(resolution_);
	priority_     = std::make_unique<FPriorityTexture>(resolution_);
	// TODO: DepthStencil側も解像度変更に対応させる.

	//!< Bufferの生成
	FRenderTargetBuffer::EnsureBuffer<FMainBuffer>();

	//!< ProcessBufferの生成
	process_ = std::make_unique<FProcessBuffer>();
	process_->Create(4, { .resolution = resolution_, .format = FBaseBuffer::kColorFormat });
	// TODO: ProcessBuffer側も解像度変更に対応させる.

}

void FRenderTargetBuffer::ClearMainRenderTarget(const DirectXQueueContext* context) {
	//!< MainBufferのRenderTargetをクリアする
	FMainBuffer* main = FRenderTargetBuffer::EnsureBuffer<FMainBuffer>();
	main->GetBuffer(FMainBuffer::Layout::Scene).ClearRenderTarget(context);
	main->GetBuffer(FMainBuffer::Layout::Canvas).ClearRenderTarget(context);

	//!< DepthStencilをクリアする
	depthStencil_->ClearDepthStencil(context);
	priority_->ClearDepthStencil(context);
}

void FRenderTargetBuffer::BeginRenderTargetMainScene(const DirectXQueueContext* context) {

	FMainBuffer* main                  = FRenderTargetBuffer::EnsureBuffer<FMainBuffer>();
	FDepthStencilTexture* depthStencil = GetDepthStencil();

	main->GetBuffer(FMainBuffer::Layout::Scene).TransitionRenderTarget(context);
	depthStencil->TransitionDepthWrite(context);

	context->GetCommandList()->OMSetRenderTargets(
		1, &main->GetBuffer(FMainBuffer::Layout::Scene).GetCPUHandleRTV(), false,
		&depthStencil->GetCPUHandleDSV()
	);

}

void FRenderTargetBuffer::EndRenderTargetMainScene(const DirectXQueueContext* context) {

	FMainBuffer* main                  = FRenderTargetBuffer::EnsureBuffer<FMainBuffer>();
	FDepthStencilTexture* depthStencil = GetDepthStencil();

	main->GetBuffer(FMainBuffer::Layout::Scene).TransitionDefaultState(context);
	depthStencil->TransitionDefaultState(context);
}
