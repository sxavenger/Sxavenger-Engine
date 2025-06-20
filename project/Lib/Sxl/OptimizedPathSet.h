#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* sxl
#include "SxavengerLibrary.h"

//* c++
#include <filesystem>
#include <set>
#include <unordered_set>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxl
////////////////////////////////////////////////////////////////////////////////////////////
_SXL_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// OptimizedPathSet class
////////////////////////////////////////////////////////////////////////////////////////////
class OptimizedPathSet {
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
	using Set = std::set<std::filesystem::path, FilepathLess>;
#else
	using Set = std::unordered_set<std::filesystem::path, FilepathHash, FilepathEqual>;
#endif

	using iterator       = Set::iterator;
	using const_iterator = Set::const_iterator;

	using value_type = Set::value_type;
	using key_type   = Set::key_type;
	using size_type  = Set::size_type;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constractor - desctructor

	//* iterators

	auto begin() noexcept { return set_.begin(); }
	auto end() noexcept { return set_.end(); }

	auto begin() const noexcept { return set_.begin(); }
	auto end() const noexcept { return set_.end(); }

	auto cbegin() const noexcept { return set_.cbegin(); }
	auto cend() const noexcept { return set_.cend(); }

	//* capacity

	bool Empty() const { return set_.empty(); }

	size_type Size() const { return set_.size(); }

	//* modifiers

	void Clear() noexcept { set_.clear(); }

	std::pair<iterator, bool> Insert(const value_type& x) {
		return set_.insert(x);
	}

	std::pair<iterator, bool> Intert(value_type&& x) {
		return set_.insert(std::move(x));
	}

	iterator Insert(const_iterator position, const value_type& x) {
		return set_.insert(position, x);
	}

	iterator Insert(const_iterator position, value_type&& x) {
		return set_.insert(position, std::move(x));
	}

	void Insert(std::initializer_list<value_type> init) {
		set_.insert(init);
	}

	template <typename... Args>
	std::pair<iterator, bool> Emplace(Args&&... args) {
		return set_.emplace(std::forward<Args>(args)...);
	}

	iterator Erase(iterator position) {
		return set_.erase(position);
	}

	iterator Erase(const_iterator position) {
		return set_.erase(position);
	}

	size_type Erase(const key_type& x) {
		return set_.erase(x);
	}

	//* element access

	iterator Find(const key_type& x) {
		return set_.find(x);
	}

	const_iterator Find(const key_type& x) const {
		return set_.find(x);
	}

	bool Contains(const key_type& x) const {
		return set_.contains(x);
	}

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Set set_;

};

_SXL_NAMESPACE_END
