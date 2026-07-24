#include "FRenderTargetBuffer.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FMainBuffer.h"
#include "FDepthStencilBuffer.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderTargetBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderTargetBuffer::Init(const Vector2ui& resolution) {

	//!< 解像度の更新
	resolution_ = resolution;

	//!< Bufferの生成
	FRenderTargetBuffer::EnsureBuffer<FMainBuffer>();
	FRenderTargetBuffer::EnsureBuffer<FDepthStencilBuffer>();

	//!< ProcessBufferの生成
	process_ = std::make_unique<FProcessBuffer>();
	process_->Create(4, { .resolution = resolution_, .format = FBaseBuffer::kColorFormat });
	// TODO: ProcessBuffer側も解像度変更に対応させる.

}

void FRenderTargetBuffer::ResetBuffer() {
	registry_.Clear(); //!< Bufferを全て破棄する.
	FRenderTargetBuffer::EnsureBuffer<FMainBuffer>(); //!< Bufferを全て破棄した後に、MainBufferだけは生成しておく.
}

void FRenderTargetBuffer::ClearMainRenderTarget(const DirectXQueueContext* context) {
	//!< MainBufferのRenderTargetをクリアする
	FMainBuffer* main = FRenderTargetBuffer::EnsureBuffer<FMainBuffer>();
	main->GetBuffer(FMainBuffer::Layout::Scene).ClearRenderTarget(context);
	main->GetBuffer(FMainBuffer::Layout::Canvas).ClearRenderTarget(context);

	//!< DepthStencilをクリアする
	FDepthStencilBuffer* depthStencil = FRenderTargetBuffer::EnsureBuffer<FDepthStencilBuffer>();
	depthStencil->GetBuffer(FDepthStencilBuffer::Layout::Scene).ClearDepthStencil(context);
	depthStencil->GetBuffer(FDepthStencilBuffer::Layout::Canvas).ClearDepthStencil(context);
}

void FRenderTargetBuffer::BeginRenderTargetMainScene(const DirectXQueueContext* context) {

	FMainBuffer* main                 = FRenderTargetBuffer::EnsureBuffer<FMainBuffer>();
	FDepthStencilBuffer* depthStencil = FRenderTargetBuffer::EnsureBuffer<FDepthStencilBuffer>();

	main->GetBuffer(FMainBuffer::Layout::Scene).TransitionRenderTarget(context);
	depthStencil->GetBuffer(FDepthStencilBuffer::Layout::Scene).TransitionDepthWrite(context);

	context->GetCommandList()->OMSetRenderTargets(
		1, &main->GetBuffer(FMainBuffer::Layout::Scene).GetCPUHandleRTV(), false,
		&depthStencil->GetBuffer(FDepthStencilBuffer::Layout::Scene).GetCPUHandleDSV()
	);

}

void FRenderTargetBuffer::EndRenderTargetMainScene(const DirectXQueueContext* context) {

	FMainBuffer* main                 = FRenderTargetBuffer::EnsureBuffer<FMainBuffer>();
	FDepthStencilBuffer* depthStencil = FRenderTargetBuffer::EnsureBuffer<FDepthStencilBuffer>();

	main->GetBuffer(FMainBuffer::Layout::Scene).TransitionDefaultState(context);
	depthStencil->GetBuffer(FDepthStencilBuffer::Layout::Scene).TransitionDefaultState(context);
}
