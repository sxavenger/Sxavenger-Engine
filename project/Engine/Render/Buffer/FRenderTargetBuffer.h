#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FBaseBuffer.h"
#include "FProcessBuffer.h"
#include "../Common/FDepthStencilTexture.h"
#include "../Common/FPriorityTexture.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/Utility/StreamLogger.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* lib
#include <Lib/Sxl/SubclassRegistry.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderTargetBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderTargetBuffer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init(const Vector2ui& resolution);

	//* parameter option *//

	const Vector2ui& GetResolution() const { return resolution_; }

	//* buffer option *//

	template <class T> requires std::derived_from<T, FBaseBuffer>
	bool HasBuffer() const { return registry_.Contains<T>(); }

	template <class T> requires std::derived_from<T, FBaseBuffer>
	T* EnsureBuffer();

	template <class T> requires std::derived_from<T, FBaseBuffer>
	T* GetBuffer() const;

	//* depth stencil option *//

	FDepthStencilTexture* GetDepthStencil() const { return depthStencil_.get(); }

	FPriorityTexture* GetPriority() const { return priority_.get(); }

	//* process buffer option *//

	FProcessBuffer* GetProcess() const { return process_.get(); }

	//* render target option *//

	void ClearMainRenderTarget(const DirectXQueueContext* context);

	void BeginRenderTargetMainScene(const DirectXQueueContext* context);

	void EndRenderTargetMainScene(const DirectXQueueContext* context);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* parameter *//

	Vector2ui resolution_ = {}; //!< バッファの解像度

	//* buffer *//

	Sxl::SubclassRegistry<FBaseBuffer> registry_;
	//!< "FBaseBuffer" を継承したクラスを管理するレジストリ

	//* depth stencil *//

	std::unique_ptr<FDepthStencilTexture> depthStencil_;
	std::unique_ptr<FPriorityTexture> priority_;

	//* process buffer *//

	std::unique_ptr<FProcessBuffer> process_;

	//=========================================================================================
	// private methods
	//=========================================================================================

};

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderTargetBuffer class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <class T> requires std::derived_from<T, FBaseBuffer>
inline T* FRenderTargetBuffer::EnsureBuffer() {

	if (!FRenderTargetBuffer::HasBuffer<T>()) {
		registry_.Insert<T>(); //!< Bufferが存在しない場合は生成する.
	}

	T* buffer = registry_.Get<T>();
	buffer->Create(resolution_); //!< Bufferを生成する.

	return buffer;
}

template <class T> requires std::derived_from<T, FBaseBuffer>
inline T* FRenderTargetBuffer::GetBuffer() const {
	StreamLogger::AssertA(FRenderTargetBuffer::HasBuffer<T>(), "Render Target Buffer not found.");
	return registry_.Get<T>();
}

SXAVENGER_ENGINE_NAMESPACE_END


