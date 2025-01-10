#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* sxl
#include "SxavengerLibrary.h"
#include "OptimizedMap.h"

//* c++
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxl
////////////////////////////////////////////////////////////////////////////////////////////
_SXL_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// OptimizedLowerPathMap class
////////////////////////////////////////////////////////////////////////////////////////////
template <class _Value>
class OptimizedLowerPathMap
	: public OptimizedMap<std::filesystem::path, _Value> {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
	using _Key = std::filesystem::path;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	OptimizedLowerPathMap()  = default;
	~OptimizedLowerPathMap() = default;

	void Emplace(const _Key& key, const _Value& value) override;
	void TryEmplace(const _Key& key, const _Value& value) override;

	bool Contains(const _Key& key) const override;

	void Erase(const _Key& key) override;

	const _Value& At(const _Key& key) const override;
	_Value& At(const _Key& key) override;

	//=========================================================================================
	// operator
	//=========================================================================================

	_Value& operator[](const _Key& key) override;
	const _Value& operator[](const _Key& key) const override;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	static _Key ToLower(const _Key& key);

};

template <class _Value>
inline void OptimizedLowerPathMap<_Value>::Emplace(const _Key& key, const _Value& value) {
	OptimizedMap<std::filesystem::path, _Value>::Emplace(ToLower(key), value);
}

template <class _Value>
inline void OptimizedLowerPathMap<_Value>::TryEmplace(const _Key& key, const _Value& value) {
	OptimizedMap<std::filesystem::path, _Value>::TryEmplace(ToLower(key), value);
}

template <class _Value>
inline bool OptimizedLowerPathMap<_Value>::Contains(const _Key& key) const {
	return OptimizedMap<std::filesystem::path, _Value>::Contains(ToLower(key));
}

template <class _Value>
inline void OptimizedLowerPathMap<_Value>::Erase(const _Key& key) {
	OptimizedMap<std::filesystem::path, _Value>::Erase(ToLower(key));
}

template <class _Value>
inline const _Value& OptimizedLowerPathMap<_Value>::At(const _Key& key) const {
	return OptimizedMap<std::filesystem::path, _Value>::At(ToLower(key));
}

template<class _Value>
inline _Value& OptimizedLowerPathMap<_Value>::At(const _Key& key) {
	return OptimizedMap<std::filesystem::path, _Value>::At(ToLower(key));
}

template <class _Value>
inline _Value& OptimizedLowerPathMap<_Value>::operator[](const _Key& key) {
	return OptimizedMap<std::filesystem::path, _Value>::operator[](ToLower(key));
}

template <class _Value>
inline const _Value& OptimizedLowerPathMap<_Value>::operator[](const _Key& key) const {
	return OptimizedMap<std::filesystem::path, _Value>::operator[](ToLower(key));
}

template <class _Value>
inline OptimizedLowerPathMap<_Value>::_Key OptimizedLowerPathMap<_Value>::ToLower(const _Key& key) {
	_Key lowerpath;

	for (const auto& part : key) {
		std::string lower = part.string();

		std::transform(
			lower.begin(), lower.end(),
			lower.begin(),
			[](unsigned char c) {
				return static_cast<char>(std::tolower(c));
			}
		);

		lowerpath /= lower;
	}

	return lowerpath;
}

_SXL_NAMESPACE_END
