#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// c++
#include <chrono>
#include <memory>

// DxObject
#include <Engine/System/DxObject/DxBufferResource.h>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class DeltaTimePoint;

////////////////////////////////////////////////////////////////////////////////////////////
// SecondsUnit enum
////////////////////////////////////////////////////////////////////////////////////////////
enum SecondsUnit {
	us, //!< マイクロ秒
	ms, //!< ミリ秒
	s,  //!< 秒

	kCountOfSecondsUnit //!< 単位の数
};

////////////////////////////////////////////////////////////////////////////////////////////
// Performance class
////////////////////////////////////////////////////////////////////////////////////////////
class Performance {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief フレーム計測開始
	static void BeginFrame();

	//! @breif フレーム計測終了
	static void EndFrame();

	//! @brief 1フレームの時間を返却
	//! @brief ...前1フレームの計測した数値を返却
	//! 
	//! @param[in] uint 秒単位
	static DeltaTimePoint GetDeltaTime(SecondsUnit unit);

	//* gpu buffer *//

	//! @brief bufferの生成
	static void CreateBuffer();

	//! @brief bufferの終了処理
	static void TermBuffer();

	//! @brief BufferのGPUAddressの取得
	static const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress();

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Perframe class
	////////////////////////////////////////////////////////////////////////////////////////////
	struct PerFrame {
		float time;
		float deltaTime;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* chrono *//

	static std::chrono::steady_clock::time_point reference_;

	//* delta *//

	static float deltaTime_;

	//* buffer *//

	static std::unique_ptr<DxObject::BufferResource<PerFrame>> perFrameBuffer_;

	static float secondsConversions_[SecondsUnit::kCountOfSecondsUnit]; //!< 秒数単位変換表 micro * this[...]

	//=========================================================================================
	// private variables
	//=========================================================================================

	static void WaitForFPS(float fps);

};

////////////////////////////////////////////////////////////////////////////////////////////
// DeltaTimePoint class
////////////////////////////////////////////////////////////////////////////////////////////
class DeltaTimePoint { //!< frameとdeltaTimeを区別するために作成
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void AddDeltaTime() {
		*this += Performance::GetDeltaTime(s);
	}

	void SubtractDeltaTime() {
		*this -= Performance::GetDeltaTime(s);
	}

	//=========================================================================================
	// public operator
	//=========================================================================================

	void operator=(const DeltaTimePoint& other) {
		time = other.time;
	}

	DeltaTimePoint& operator+=(DeltaTimePoint other) {
		time += other.time;
		return *this;
	}

	DeltaTimePoint& operator-=(DeltaTimePoint other) {
		time -= other.time;
		return *this;
	}

	bool operator==(DeltaTimePoint other) const {
		return time == other.time;
	}

	bool operator<(DeltaTimePoint other) const {
		return time < other.time;
	}

	bool operator<=(DeltaTimePoint other) const {
		return time <= other.time;
	}

	bool operator>(DeltaTimePoint other) const {
		return time > other.time;
	}

	bool operator>=(DeltaTimePoint other) const {
		return time >= other.time;
	}

	//=========================================================================================
	// public member
	//=========================================================================================

	float time;

private:
};