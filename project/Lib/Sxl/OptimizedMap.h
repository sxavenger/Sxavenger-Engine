#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* sxl
#include "SxavengerLibrary.h"

//* c++
#include <map>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxl
////////////////////////////////////////////////////////////////////////////////////////////
_SXL_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// OptimizerdMap class
////////////////////////////////////////////////////////////////////////////////////////////
template <class _Key, class _Value>
class OptimizerdMap {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
	using Container = std::map<_Key, _Value>;
#else
	using Container = std::unordered_map<_Key, _Value>;
#endif

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	OptimizerdMap()  = default;
	~OptimizerdMap() = default;

	void Emplace(const _Key& key, const _Value& value);
	void TryEmplace(const _Key& key, const _Value& value);

	bool Contains(const _Key& key) const;

	Container::iterator Erase(Container::iterator position);
	Container::iterator Erase(Container::const_iterator position);
	void Erase(const _Key& key);

	const _Value& At(const _Key& key) const;

	void Clear();

	size_t Size() const;

	Container::iterator Begin();
	Container::const_iterator Begin() const;

	Container::iterator End();
	Container::const_iterator End() const;

	const Container& GetMap() const { return map_; }
	Container& GetMap() { return map_; }

	//=========================================================================================
	// operator
	//=========================================================================================

	_Value& operator[](const _Key& key);
	const _Value& operator[](const _Key& key) const;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	Container map_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// OptimizerdMap class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template<class _Key, class _Value>
inline void OptimizerdMap<_Key, _Value>::Emplace(const _Key& key, const _Value& value) {
	map_.emplace(key, value);
}

template<class _Key, class _Value>
inline void OptimizerdMap<_Key, _Value>::TryEmplace(const _Key& key, const _Value& value) {
	map_.try_emplace(key, value);
}

template<class _Key, class _Value>
inline bool OptimizerdMap<_Key, _Value>::Contains(const _Key& key) const {
	return map_.contains(key);
}

template<class _Key, class _Value>
inline OptimizerdMap<_Key, _Value>::Container::iterator OptimizerdMap<_Key, _Value>::Erase(Container::iterator position) {
	return map_.erase(position);
}

template<class _Key, class _Value>
inline OptimizerdMap<_Key, _Value>::Container::iterator OptimizerdMap<_Key, _Value>::Erase(Container::const_iterator position) {
	return map_.erase(position);
}

template<class _Key, class _Value>
inline void OptimizerdMap<_Key, _Value>::Erase(const _Key& key) {
	map_.erase(key);
}

template<class _Key, class _Value>
inline const _Value& OptimizerdMap<_Key, _Value>::At(const _Key& key) const {
	return map_.at(key);
}

template<class _Key, class _Value>
inline void OptimizerdMap<_Key, _Value>::Clear() {
	map_.clear();
}

template<class _Key, class _Value>
inline size_t OptimizerdMap<_Key, _Value>::Size() const {
	return map_.size();
}

template<class _Key, class _Value>
inline OptimizerdMap<_Key, _Value>::Container::iterator OptimizerdMap<_Key, _Value>::Begin() {
	return map_.begin();
}

template<class _Key, class _Value>
inline OptimizerdMap<_Key, _Value>::Container::const_iterator OptimizerdMap<_Key, _Value>::Begin() const {
	return map_.begin();
}

template<class _Key, class _Value>
inline OptimizerdMap<_Key, _Value>::Container::iterator OptimizerdMap<_Key, _Value>::End() {
	return map_.end();
}

template<class _Key, class _Value>
inline OptimizerdMap<_Key, _Value>::Container::const_iterator OptimizerdMap<_Key, _Value>::End() const {
	return map_.end();
}

template<class _Key, class _Value>
inline _Value& OptimizerdMap<_Key, _Value>::operator[](const _Key& key) {
	return map_[key];
}

template<class _Key, class _Value>
inline const _Value& OptimizerdMap<_Key, _Value>::operator[](const _Key& key) const {
	return map_.at(key);
}

_SXL_NAMESPACE_END
