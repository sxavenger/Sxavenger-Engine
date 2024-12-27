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
// OptimizedMap class
////////////////////////////////////////////////////////////////////////////////////////////
template <class _Key, class _Value>
class OptimizedMap {
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

	OptimizedMap()  = default;
	~OptimizedMap() = default;

	virtual void Emplace(const _Key& key, const _Value& value);
	virtual void TryEmplace(const _Key& key, const _Value& value);

	virtual bool Contains(const _Key& key) const;

	Container::iterator Erase(Container::iterator position);
	Container::iterator Erase(Container::const_iterator position);
	virtual void Erase(const _Key& key);

	virtual const _Value& At(const _Key& key) const;

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

	virtual _Value& operator[](const _Key& key);
	virtual const _Value& operator[](const _Key& key) const;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	Container map_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// OptimizedMap class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template<class _Key, class _Value>
inline void OptimizedMap<_Key, _Value>::Emplace(const _Key& key, const _Value& value) {
	map_.emplace(key, value);
}

template<class _Key, class _Value>
inline void OptimizedMap<_Key, _Value>::TryEmplace(const _Key& key, const _Value& value) {
	map_.try_emplace(key, value);
}

template<class _Key, class _Value>
inline bool OptimizedMap<_Key, _Value>::Contains(const _Key& key) const {
	return map_.contains(key);
}

template<class _Key, class _Value>
inline OptimizedMap<_Key, _Value>::Container::iterator OptimizedMap<_Key, _Value>::Erase(Container::iterator position) {
	return map_.erase(position);
}

template<class _Key, class _Value>
inline OptimizedMap<_Key, _Value>::Container::iterator OptimizedMap<_Key, _Value>::Erase(Container::const_iterator position) {
	return map_.erase(position);
}

template<class _Key, class _Value>
inline void OptimizedMap<_Key, _Value>::Erase(const _Key& key) {
	map_.erase(key);
}

template<class _Key, class _Value>
inline const _Value& OptimizedMap<_Key, _Value>::At(const _Key& key) const {
	return map_.at(key);
}

template<class _Key, class _Value>
inline void OptimizedMap<_Key, _Value>::Clear() {
	map_.clear();
}

template<class _Key, class _Value>
inline size_t OptimizedMap<_Key, _Value>::Size() const {
	return map_.size();
}

template<class _Key, class _Value>
inline OptimizedMap<_Key, _Value>::Container::iterator OptimizedMap<_Key, _Value>::Begin() {
	return map_.begin();
}

template<class _Key, class _Value>
inline OptimizedMap<_Key, _Value>::Container::const_iterator OptimizedMap<_Key, _Value>::Begin() const {
	return map_.begin();
}

template<class _Key, class _Value>
inline OptimizedMap<_Key, _Value>::Container::iterator OptimizedMap<_Key, _Value>::End() {
	return map_.end();
}

template<class _Key, class _Value>
inline OptimizedMap<_Key, _Value>::Container::const_iterator OptimizedMap<_Key, _Value>::End() const {
	return map_.end();
}

template<class _Key, class _Value>
inline _Value& OptimizedMap<_Key, _Value>::operator[](const _Key& key) {
	return map_[key];
}

template<class _Key, class _Value>
inline const _Value& OptimizedMap<_Key, _Value>::operator[](const _Key& key) const {
	return map_.at(key);
}

_SXL_NAMESPACE_END
