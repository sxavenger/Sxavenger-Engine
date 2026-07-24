#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>

//* lib
#include <Lib/Adapter/Time/TimePoint.h>
#include <Lib/Adapter/Time/RunTimeTracker.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Performance class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief フレームの経過時間を計測し, フレームレート制御(待機)を行うクラス
class Performance {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief フレーム計測を開始する
	void Begin();

	//! @brief フレーム計測を終了し, 必要ならフレームレート制御の待機を行う
	void End();

	//! @brief 前フレームからのデルタ時間を取得する (double)
	//! @tparam T 時間単位
	//! @return デルタ時間
	template <TimeUnit T = TimeUnit::second>
	TimePointd<T> GetDeltaTimed() const { return runtime_.GetDeltaTime<T>(); }

	//! @brief 前フレームからのデルタ時間を取得する (float)
	//! @tparam T 時間単位
	//! @return デルタ時間
	template <TimeUnit T = TimeUnit::second>
	TimePointf<T> GetDeltaTimef() const { return runtime_.GetDeltaTime<T>(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* run time tracker *//

	RunTimeTracker runtime_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void WaitFrame() const;

};

SXAVENGER_ENGINE_NAMESPACE_END
