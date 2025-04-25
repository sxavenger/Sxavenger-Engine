#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* sxl
#include "SxavengerLibrary.h"

//* c++
#include <filesystem>
#include <map>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxl
////////////////////////////////////////////////////////////////////////////////////////////
_SXL_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// OptimizedPathMap class
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
class OptimizedPathMap {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// compare structure
	////////////////////////////////////////////////////////////////////////////////////////////

	struct FilepathHash {
		std::size_t operator()(const std::filesystem::path& path) const {
			std::string str = path.lexically_normal().string();
			std::transform(
				str.begin(), str.end(), str.begin(),
				[](char c) { return static_cast<char>(std::tolower(static_cast<unsigned char>(c))); }
			);
			return std::hash<std::string>()(str);
		}
	};

	struct FilepathEqual {
		bool operator()(const std::filesystem::path& lhs, const std::filesystem::path& rhs) const {
			std::string strA = lhs.lexically_normal().string();
			std::string strB = rhs.lexically_normal().string();

			std::transform(
				strA.begin(), strA.end(), strA.begin(),
				[](char c) { return static_cast<char>(std::tolower(static_cast<unsigned char>(c))); }
			);
			std::transform(
				strB.begin(), strB.end(), strB.begin(),
				[](char c) { return static_cast<char>(std::tolower(static_cast<unsigned char>(c))); }
			);

			return strA == strB;
		}
	};

	struct FilepathLess {
		bool operator()(const std::filesystem::path& lhs, const std::filesystem::path& rhs) const {
			std::string strA = lhs.lexically_normal().string();
			std::string strB = rhs.lexically_normal().string();

			std::transform(
				strA.begin(), strA.end(), strA.begin(),
				[](char c) { return static_cast<char>(std::tolower(static_cast<unsigned char>(c))); }
			);
			std::transform(
				strB.begin(), strB.end(), strB.begin(),
				[](char c) { return static_cast<char>(std::tolower(static_cast<unsigned char>(c))); }
			);

			return strA < strB;
		}
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _DEVELOPMENT
	using Map = std::map<std::filesystem::path, T, FilepathLess>;
#else
	using Map = std::unordered_map<std::filesystem::path, T, FilepathHash, FilepathEqual>;
#endif

	using iterator       = Map::iterator;
	using const_iterator = Map::const_iterator;

	using value_type = Map::value_type;
	using key_type   = Map::key_type;
	using size_type  = Map::size_type;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constractor - desctructor

	//* iterators

	iterator begin() noexcept { return map_.begin(); }
	iterator end() noexcept { return map_.end(); }

	const_iterator cbegin() const noexcept { return map_.cbegin(); }
	const_iterator cend() const noexcept { return map_.cend(); }

	//* capacity

	bool Empty() const { return map_.empty(); }

	size_type Size() const { return map_.size(); }

	//* modifiers

	void Clear() noexcept { map_.clear(); }

	std::pair<iterator, bool> Insert(const value_type& x) {
		return map_.insert(x);
	}

	std::pair<iterator, bool> Intert(value_type&& x) {
		return map_.insert(std::move(x));
	}

	iterator Insert(const_iterator position, const value_type& x) {
		return map_.insert(position, x);
	}

	iterator Insert(const_iterator position, value_type&& x) {
		return map_.insert(position, std::move(x));
	}

	void Insert(std::initializer_list<value_type> init) {
		map_.insert(init);
	}

	template <typename... Args>
	std::pair<iterator, bool> Emplace(Args&&... args) {
		return map_.emplace(std::forward<Args>(args)...);
	}

	iterator Erase(iterator position) {
		return map_.erase(position);
	}

	iterator Erase(const_iterator position) {
		return map_.erase(position);
	}

	size_type Erase(const key_type& x) {
		return map_.erase(x);
	}

	//* element access

	T& operator[](const key_type& x) {
		return map_[x];
	}

	T& operator[](key_type&& x) {
		return map_[std::move(x)];
	}

	T& At(const key_type& x) {
		return map_.at(x);
	}

	const T& At(const key_type& x) const {
		return map_.at(x);
	}

	iterator Find(const key_type& x) {
		return map_.find(x);
	}

	const_iterator Find(const key_type& x) const {
		return map_.find(x);
	}

	bool Contains(const key_type& x) const {
		return map_.contains(x);
	}

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Map map_;

};

_SXL_NAMESPACE_END
