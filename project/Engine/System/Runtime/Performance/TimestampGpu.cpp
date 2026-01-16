#include "TimestampGpu.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Stamp structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void TimestampGpu::Stamp::SetTime(uint64_t beginTick, uint64_t endTick, uint64_t frequency) {
	section = TimePointd<TimeUnit::second>{ static_cast<double>(endTick - beginTick) / static_cast<double>(frequency) };
}

////////////////////////////////////////////////////////////////////////////////////////////
// Timestamp structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void TimestampGpu::Timestamp::MarkStamp(const std::string& name) {
	Stamp stamp = {};
	stamp.name = name;
	stamps.emplace_back(std::move(stamp));

	StreamLogger::AssertA(stamps.size() <= kStampCount, "GPU Timestamp stamp count exceeds limit.");
}

void TimestampGpu::Timestamp::Resolve(const DxObject::ReadbackDimensionBuffer<uint64_t>& buffer, uint64_t frequency) {
	for (size_t index = 0; Stamp& stamp : stamps) {

		uint64_t beginTick = buffer.At(index * 2 + 0);
		uint64_t endTick   = buffer.At(index * 2 + 1);

		stamp.SetTime(beginTick, endTick, frequency);
		++index;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// TimestampGpu class methods
////////////////////////////////////////////////////////////////////////////////////////////

void TimestampGpu::Init(DxObject::Device* device) {

	D3D12_QUERY_HEAP_DESC desc = {};
	desc.Type  = D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
	desc.Count = Timestamp::kStampCount * 2;

	device->GetDevice()->CreateQueryHeap(
		&desc,
		IID_PPV_ARGS(query_.GetAddressOf())
	);

	readback_.Capacity(device, Timestamp::kStampCount * 2);
	readback_.Resize(Timestamp::kStampCount * 2);

}

void TimestampGpu::Begin() {
	// timestampの更新かどうかの判定
	recordedTimer_ -= runtime_.GetDeltaTime<TimeUnit::second>();
	isRecord_      = (recordedTimer_ <= 0.0f);

	if (isRecord_) { //!< timestampの更新開始
		timestamps_[currentIndex_].Clear();
	}

	runtime_.Begin();
}

void TimestampGpu::ReadTimestamp(const DirectXQueueContext* context) {
	if (!isRecord_) {
		return;
	}

	// timestampの記録を取得
	context->GetCommandList()->ResolveQueryData(
		query_.Get(),
		D3D12_QUERY_TYPE_TIMESTAMP,
		0,
		timestamps_[currentIndex_].GetStampCount() * 2,
		readback_.GetResource(),
		0
	);
}

void TimestampGpu::End(const DirectXQueueContext* context) {
	runtime_.End();

	if (!isRecord_) {
		return;
	}

	//!< timestampの更新終了
	timestamps_[currentIndex_].Resolve(readback_, context->GetTimestampFrequency());
	
	++currentIndex_ %= kTimestampCount; //!< 現在のtimestampIndexを更新
	recordedTimer_  = recordInterval_;  //!< 次のtimestamp更新までの時間をリセット
	isRecord_       = false;
}

void TimestampGpu::BeginRecord(const DirectXQueueContext* context, const std::string& name) {
	if (!isRecord_) {
		return;
	}

	context->GetCommandList()->EndQuery(
		query_.Get(),
		D3D12_QUERY_TYPE_TIMESTAMP,
		timestamps_[currentIndex_].GetStampCount() * 2 + 0
	);

	timestamps_[currentIndex_].MarkStamp(name);
}

void TimestampGpu::EndRecord(const DirectXQueueContext* context) {
	if (!isRecord_) {
		return;
	}

	context->GetCommandList()->EndQuery(
		query_.Get(),
		D3D12_QUERY_TYPE_TIMESTAMP,
		(timestamps_[currentIndex_].GetStampCount() - 1) * 2 + 1
	);
}

const TimestampGpu::Timestamp& TimestampGpu::GetTimestamp() const {
	//!< 計測済みのtimestampを返却
	return timestamps_[(currentIndex_ + kTimestampCount - 1) % kTimestampCount];
}
