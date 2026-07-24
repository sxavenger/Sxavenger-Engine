#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FRenderConfig.h"
#include "FBaseRenderPass.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassContext class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 登録された描画パス群を順に実行し, 1フレームの描画を統括するコンテキスト
class FRenderPassContext {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* render option *//

	//! @brief 登録済みの全パスを順番に実行する
	//! @param[in] context DirectXのキューコンテキスト
	//! @param[in] config  描画設定
	void Render(const DirectXQueueContext* context, const FRenderConfig& config);

	//* render pass option *//

	//! @brief 描画パスを実行リストの末尾へ追加する
	//! @param[in] pass 追加するパス
	void Insert(std::unique_ptr<FBaseRenderPass>&& pass);

	//! @brief 指定した型の描画パスを生成して追加する
	//! @tparam T FBaseRenderPass派生の具象パス型
	template <FRenderPass T>
	void Insert() { Insert(std::make_unique<T>()); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::list<std::unique_ptr<FBaseRenderPass>> passes_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	static FRenderConfig ResolveConfig(const FRenderConfig& _config);

};

SXAVENGER_ENGINE_NAMESPACE_END
