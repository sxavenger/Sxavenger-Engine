#include "FRenderTargetBuffer.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderTargetBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderTargetBuffer::Create(const Vector2ui& size) {

	// 引数の保存
	size_ = size;

	deferred_.Init(size_);
	lighting_.Init(size_);
	main_.Init(size_);

	depth_ = std::make_unique<FDepthTexture>();
	depth_->Create(size_);

	process_ = std::make_unique<FProcessTextures>();
	process_->Create(2, size_, FMainGBuffer::kColorFormat);
}

void FRenderTargetBuffer::BeginRenderTargetDeferred(const DirectXQueueContext* context) {
	// OMSetRenderTargetsの設定
	depth_->TransitionBeginRasterizer(context);
	deferred_.TransitionBeginRenderTarget(context, depth_->GetRasterizerCPUHandleDSV());

	// ClearRenderTargetの設定
	depth_->ClearRasterizerDepth(context);
	deferred_.ClearRenderTarget(context);
}

void FRenderTargetBuffer::EndRenderTargetDeferred(const DirectXQueueContext* context) {
	depth_->TransitionEndRasterizer(context);
	deferred_.TransitionEndRenderTarget(context);
}

void FRenderTargetBuffer::BeginRenderTargetLightingDirect(const DirectXQueueContext* context) {
	lighting_.TransitionBeginRenderTargetDirect(context);
	lighting_.ClearRenderTargetDirect(context);
}

void FRenderTargetBuffer::EndRenderTargetLightingDirect(const DirectXQueueContext* context) {
	lighting_.TransitionEndRenderTargetDirect(context);
}

void FRenderTargetBuffer::BeginUnorderedMainScene(const DirectXQueueContext* context) {
	main_.TransitionBeginUnorderedScene(context);
}

void FRenderTargetBuffer::EndUnorderedMainScene(const DirectXQueueContext* context) {
	main_.TransitionEndUnorderedScene(context);
}

void FRenderTargetBuffer::BeginRenderTargetMainTransparent(const DirectXQueueContext* context) {
	depth_->TransitionBeginRasterizer(context);
	main_.TransitionBeginRenderTargetScene(context, depth_->GetRasterizerCPUHandleDSV());
}

void FRenderTargetBuffer::EndRenderTargetMainTransparent(const DirectXQueueContext* context) {
	depth_->TransitionEndRasterizer(context);
	main_.TransitionEndRenderTargetScene(context);
}

void FRenderTargetBuffer::BeginRenderTargetMainUI(const DirectXQueueContext* context) {
	main_.TransitionBeginRenderTargetUI(context);
	main_.ClearRenderTargetUI(context);
}

void FRenderTargetBuffer::EndRenderTargetMainUI(const DirectXQueueContext* context) {
	main_.TransitionEndRenderTargetUI(context);
}

FBaseTexture* FRenderTargetBuffer::GetGBuffer(FDeferredGBuffer::Layout layout) {
	return deferred_.GetGBuffer(layout);
}

FBaseTexture* FRenderTargetBuffer::GetGBuffer(FLightingGBuffer::Layout layout) {
	return lighting_.GetGBuffer(layout);
}

FBaseTexture* FRenderTargetBuffer::GetGBuffer(FMainGBuffer::Layout layout) {
	return main_.GetGBuffer(layout);
}
