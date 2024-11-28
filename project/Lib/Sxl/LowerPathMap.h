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
// LowerPathMap class
////////////////////////////////////////////////////////////////////////////////////////////
template <class _Value>
class LowerPathMap
	: public OptimizerdMap<std::filesystem::path, _Value> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	LowerPathMap()  = default;
	~LowerPathMap() = default;

	void Emplace(const std::filesystem::path& key, const _Value& value);
	void TryEmplace(const std::filesystem::path& key, const _Value& value);

	bool Contains(const std::filesystem::path& key) const;

	void Erase(const std::filesystem::path& key);

	const _Value& At(const std::filesystem::path& key) const;

	//=========================================================================================
	// operator
	//=========================================================================================

	_Value& operator[](const std::filesystem::path& key);
	const _Value& operator[](const std::filesystem::path& key) const;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	static std::filesystem::path ToLower(const std::filesystem::path& path);

};

////////////////////////////////////////////////////////////////////////////////////////////
// LowerPathMap class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template<class _Value>
inline void LowerPathMap<_Value>::Emplace(const std::filesystem::path& key, const _Value& value) {
	this->map_.emplace(ToLower(key), value);
}

template<class _Value>
inline void LowerPathMap<_Value>::TryEmplace(const std::filesystem::path& key, const _Value& value) {
	this->map_.try_emplace(ToLower(key), value);
}

template<class _Value>
inline bool LowerPathMap<_Value>::Contains(const std::filesystem::path& key) const {
	return this->map_.contains(ToLower(key));
}

template<class _Value>
inline void LowerPathMap<_Value>::Erase(const std::filesystem::path& key) {
	this->map_.erase(ToLower(key));
}

template<class _Value>
inline const _Value& LowerPathMap<_Value>::At(const std::filesystem::path& key) const {
	return this->map_.at(ToLower(key));
}

template<class _Value>
inline _Value& LowerPathMap<_Value>::operator[](const std::filesystem::path& key) {
	return this->map_[ToLower(key)];
}

template<class _Value>
inline const _Value& LowerPathMap<_Value>::operator[](const std::filesystem::path& key) const {
	return this->map_.at(ToLower(key));
}

template<class _Value>
inline std::filesystem::path LowerPathMap<_Value>::ToLower(const std::filesystem::path& path) {

	std::string str = path.string();

	std::transform(
		str.begin(), str.end(),
		str.begin(),
		[](char c) { return static_cast<char>(std::tolower(c)); }
	);

	return std::filesystem::path(str);
}

_SXL_NAMESPACE_END
