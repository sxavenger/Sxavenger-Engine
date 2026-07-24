#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FBaseBuffer.h"
#include "FProcessBuffer.h"
#include "../Common/FDepthStencilTexture.h"

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
//! @brief 1つの描画対象が持つ各種バッファ(GBuffer等)とプロセスバッファを型ごとに束ねて管理するコンテナ
class FRenderTargetBuffer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief 指定解像度で初期化する
	//! @param[in] resolution 描画対象の解像度
	void Init(const Vector2ui& resolution);

	//* parameter option *//

	//! @brief 描画対象の解像度を取得する
	//! @return 解像度
	const Vector2ui& GetResolution() const { return resolution_; }

	//* buffer option *//

	//! @brief 指定型のバッファを保持しているかを返す
	//! @tparam T FBaseBuffer派生のバッファ型
	//! @retval true  保持している
	//! @retval false 保持していない
	template <class T> requires std::derived_from<T, FBaseBuffer>
	bool HasBuffer() const { return registry_.Contains<T>(); }

	//! @brief 指定型のバッファを取得する. 無ければ生成し, 解像度に合わせてリサイズする
	//! @tparam T FBaseBuffer派生のバッファ型
	//! @return バッファへのポインタ
	template <class T> requires std::derived_from<T, FBaseBuffer>
	T* EnsureBuffer();

	//! @brief 指定型のバッファを取得する (存在しない場合はアサート)
	//! @tparam T FBaseBuffer派生のバッファ型
	//! @return バッファへのポインタ
	template <class T> requires std::derived_from<T, FBaseBuffer>
	T* GetBuffer() const;

	//! @brief 保持している全バッファを破棄する
	void ResetBuffer();

	//* process buffer option *//

	//! @brief ポストプロセス用のプロセスバッファを取得する
	//! @return プロセスバッファへのポインタ
	FProcessBuffer* GetProcess() const { return process_.get(); }

	//* render target option *//

	//! @brief メインレンダーターゲットをクリアする
	//! @param[in] context DirectXのキューコンテキスト
	void ClearMainRenderTarget(const DirectXQueueContext* context);

	//! @brief メインシーンへの描画を開始する (レンダーターゲット設定)
	//! @param[in] context DirectXのキューコンテキスト
	void BeginRenderTargetMainScene(const DirectXQueueContext* context);

	//! @brief メインシーンへの描画を終了する (バリア遷移)
	//! @param[in] context DirectXのキューコンテキスト
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
	buffer->Resize(resolution_); //!< Bufferを生成する.

	return buffer;
}

template <class T> requires std::derived_from<T, FBaseBuffer>
inline T* FRenderTargetBuffer::GetBuffer() const {
	StreamLogger::AssertA(FRenderTargetBuffer::HasBuffer<T>(), "Render Target Buffer not found.");
	return registry_.Get<T>();
}

SXAVENGER_ENGINE_NAMESPACE_END


