#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../Camera/CameraComponent.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* c++
#include <concepts>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class FRenderTargetBuffer;

////////////////////////////////////////////////////////////////////////////////////////////
// BasePostProcess class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief ポストプロセスレイヤー(各エフェクト)の基底クラス
class BasePostProcess {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// ProcessInfo structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief ポストプロセス実行に必要な情報(対象バッファ/カメラ/適用ウェイト)
	struct ProcessInfo {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================


		//=========================================================================================
		// public variables
		//=========================================================================================

		FRenderTargetBuffer* buffer   = nullptr;
		const CameraComponent* camera = nullptr;

		float weight = 1.0f;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	virtual ~BasePostProcess() = default;

	//! @brief エフェクトを初期化する (派生クラスで実装)
	virtual void Init() = 0;

	//! @brief エフェクトを適用する (派生クラスで実装)
	//! @param[in] context DirectXのキューコンテキスト
	//! @param[in] info    処理情報
	virtual void Process(const DirectXQueueContext* context, const ProcessInfo& info) = 0;

	//! @brief インスペクタにパラメータ編集UIを表示する (派生クラスで実装)
	virtual void ShowInspectorImGui() = 0;

	//! @brief エフェクトが有効かを返す
	bool IsEnabled() const { return isEnabled_; }
	//! @brief 有効フラグへの参照を取得する
	bool& IsEnabled() { return isEnabled_; }

	//! @brief エフェクトの有効/無効を設定する
	void SetEnabled(bool isEnabled) { isEnabled_ = isEnabled; }

	//! @brief エフェクト名を取得する
	const std::string& GetName() { return name_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	bool isEnabled_ = true;

	std::string name_ = "base post process";

};

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcess concept
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief BasePostProcessを継承した型のみを受け付けるconcept
template <class T>
concept PostProcessConcept = std::derived_from<T, BasePostProcess>;

SXAVENGER_ENGINE_NAMESPACE_END
