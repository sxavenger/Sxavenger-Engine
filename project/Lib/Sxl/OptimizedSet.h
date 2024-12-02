#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* sxl
#include "SxavengerLibrary.h"

//* c++
#include <set>
#include <unordered_set>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxl
////////////////////////////////////////////////////////////////////////////////////////////
_SXL_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// OptimizerdSet class
////////////////////////////////////////////////////////////////////////////////////////////
template <class _Value>
class OptimizerdSet {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
	using Container = std::set<_Value>;
#else
	using Container = std::unordered_set<_Value>;
#endif

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	OptimizerdSet()  = default;
	~OptimizerdSet() = default;

	void Emplace(const _Value& value);

	bool Contains(const _Value& value) const;

	Container::iterator Erase(Container::iterator position);
	Container::iterator Erase(Container::const_iterator position);
	void Erase(const _Value& value);

	void Clear();

	size_t Size() const;

	Container::iterator Begin();
	Container::const_iterator Begin() const;

	Container::iterator End();
	Container::const_iterator End() const;

	const Container& GetSet() const { return set_; }
	Container& GetSet() { return set_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	Container set_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// OptimizerdSet class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template<class _Value>
inline void OptimizerdSet<_Value>::Emplace(const _Value& value) {
	set_.emplace(value);
}

template<class _Value>
inline bool OptimizerdSet<_Value>::Contains(const _Value& value) const {
	return set_.contains(value);
}

template<class _Value>
inline OptimizerdSet<_Value>::Container::iterator OptimizerdSet<_Value>::Erase(Container::iterator position) {
	return set_.erase(position);
}

template<class _Value>
inline OptimizerdSet<_Value>::Container::iterator OptimizerdSet<_Value>::Erase(Container::const_iterator position) {
	return set_.erase(position);
}

template<class _Value>
inline void OptimizerdSet<_Value>::Erase(const _Value& value) {
	set_.erase(value);
}

template<class _Value>
inline void OptimizerdSet<_Value>::Clear() {
	set_.clear();
}

template<class _Value>
inline size_t OptimizerdSet<_Value>::Size() const {
	return set_.size();
}

template<class _Value>
inline OptimizerdSet<_Value>::Container::iterator OptimizerdSet<_Value>::Begin() {
	return set_.begin();
}

template<class _Value>
inline OptimizerdSet<_Value>::Container::const_iterator OptimizerdSet<_Value>::Begin() const {
	return set_.begin();
}

template<class _Value>
inline OptimizerdSet<_Value>::Container::iterator OptimizerdSet<_Value>::End() {
	return set_.end();
}

template<class _Value>
inline OptimizerdSet<_Value>::Container::const_iterator OptimizerdSet<_Value>::End() const {
	return set_.end();
}

_SXL_NAMESPACE_END