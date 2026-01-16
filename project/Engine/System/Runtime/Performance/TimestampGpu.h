#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxObjectCommon.h>
#include <Engine/System/DirectX/DxObject/DxDevice.h>
#include <Engine/System/DirectX/DxObject/DxReadbackDimensionBuffer.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* lib
#include <Lib/Adapter/Time/TimePoint.h>
#include <Lib/Adapter/Time/RunTimeTracker.h>

//* c++
#include <list>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// TimestampGpu class
////////////////////////////////////////////////////////////////////////////////////////////
class TimestampGpu {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Stamp structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Stamp {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void SetTime(uint64_t beginTick, uint64_t endTick, uint64_t frequency);

		//=========================================================================================
		// public variables
		//=========================================================================================

		std::string name;

		TimePointd<TimeUnit::millisecond> section; //!< 区間時間

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Timestamp structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Timestamp {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void MarkStamp(const std::string& name);

		void Resolve(const DxObject::ReadbackDimensionBuffer<uint64_t>& buffer, uint64_t frequency);

		void Clear() { stamps.clear(); }

		uint32_t GetStampCount() const { return static_cast<uint32_t>(stamps.size()); }

		//=========================================================================================
		// public variables
		//=========================================================================================

		std::list<Stamp> stamps;

		static const uint32_t kStampCount = 1 << 6;
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init(DxObject::Device* device);

	void Begin();

	void ReadTimestamp(const DirectXQueueContext* context);

	void End(const DirectXQueueContext* context);

	void BeginRecord(const DirectXQueueContext* context, const std::string& name);

	void EndRecord(const DirectXQueueContext* context);

	const Timestamp& GetTimestamp() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* record buffer *//

	ComPtr<ID3D12QueryHeap> query_;
	DxObject::ReadbackDimensionBuffer<uint64_t> readback_;

	//* record parameter *//

	RunTimeTracker runtime_;

	TimePointd<TimeUnit::second> recordInterval_ = 1.0f;
	TimePointd<TimeUnit::second> recordedTimer_  = {};
	bool isRecord_ = true;

	//* timestamp parameter *//

	static const uint8_t kTimestampCount = 2;
	std::array<Timestamp, kTimestampCount> timestamps_;

	uint8_t currentIndex_ = 0;

};

SXAVENGER_ENGINE_NAMESPACE_END
