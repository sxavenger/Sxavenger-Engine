#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* sxl
#include "SxavengerLibrary.h"
#include "Exception.h"

//* c++
#include <concepts>
#include <optional>
#include <queue>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Library namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_LIBRARY_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// IndexAllocator class
////////////////////////////////////////////////////////////////////////////////////////////
template <std::integral T>
class IndexAllocator {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* allocator option *//

	//! @brief indexの最大数を設定.
	//! @param capacity indexの最大数.
	void Capacity(T capacity);

	//! @brief indexを取得.
	//! @throw capacityがnullのとき、capacityを超えるindexを返すとき.
	T Get();

	//! @brief indexを解放し、再利用可能なindexとしてキューに追加.
	//! @param index 解放するindex.
	void Release(T index);

	//* getter *//

	//! @brief indexの最大数を取得.
	T GetCapacity() const { return capacity_.value_or(0); }

	//! @brief 現在使用中のindexの数を取得.
	T GetUsedCount() const { return current_ - static_cast<T>(recycled_.size()); }

	//! @brief allocatorが利用可能かどうかを確認.
	bool CheckAvailable() const { return GetUsedCount() < GetCapacity(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::optional<T> capacity_ = std::nullopt;
	T current_ = 0;

	std::queue<T> recycled_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// IndexAllocator class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <std::integral T>
inline void IndexAllocator<T>::Capacity(T capacity) {
	capacity_ = std::max(capacity_.value_or(0), capacity);
	// 現在のcapacityを比較し大きい方を採用.
}

template <std::integral T>
inline T IndexAllocator<T>::Get() {
	if (!capacity_.has_value()) {
		Exception::RuntimeError("capacity is null.");
	}

	if (!recycled_.empty()) {
		//!< 再利用可能なindexがある場合はそれを返す.
		T index = recycled_.front();
		recycled_.pop();

		return index;
	}

	if (current_ >= capacity_.value()) {
		//!< capacityを超えるindexは返さない.
		Exception::RuntimeError("capacity over.");
	}

	//!< 現在のindexを返し、次のindexに進める.
	T index = current_++;

	return index;
}

template <std::integral T>
inline void IndexAllocator<T>::Release(T index) {
	if (!capacity_.has_value()) {
		Exception::RuntimeError("capacity is null.");
	}

	if (index >= capacity_.value()) {
		//!< capacityを超えるindexは返さない.
		Exception::RuntimeError("capacity over.");
	}

	//!< indexを再利用可能なindexとしてキューに追加.
	recycled_.emplace(index);
}

SXAVENGER_LIBRARY_NAMESPACE_END
