#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <string>
#include <unordered_map>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Library namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Sxl {

	////////////////////////////////////////////////////////////////////////////////////////////
	// LowerUnorderedMapA class
	////////////////////////////////////////////////////////////////////////////////////////////
	template <class _Value>
	class LowerUnorderedMapA {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// using
		////////////////////////////////////////////////////////////////////////////////////////////
		using UnorderdMapA = std::unordered_map<std::string, _Value>;

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		LowerUnorderedMapA()  = default;
		~LowerUnorderedMapA() = default;

		void Emplace(const std::string& key, const _Value& value);
		void TryEmplace(const std::string& key, const _Value& value);

		bool Contains(const std::string& key) const;

		void Erase(const std::string& key);

		void Clear();

		size_t Size() const;

		const UnorderdMapA& GetMap() const { return map_; }
		UnorderdMapA& GetMap() { return map_; }

		//=========================================================================================
		// operator
		//=========================================================================================

		_Value& operator[](const std::string& key);
		const _Value& operator[](const std::string& key) const;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		UnorderdMapA map_;

		//=========================================================================================
		// private variables
		//=========================================================================================

		static std::string ToLower(const std::string& str);

	};

	template<class _Value>
	inline void LowerUnorderedMapA<_Value>::Emplace(const std::string& key, const _Value& value) {
		map_.emplace(ToLower(key), value);
	}

	template<class _Value>
	inline void LowerUnorderedMapA<_Value>::TryEmplace(const std::string& key, const _Value& value) {
		map_.try_emplace(ToLower(key), value);
	}

	template<class _Value>
	inline bool LowerUnorderedMapA<_Value>::Contains(const std::string& key) const {
		return map_.contains(ToLower(key));
	}

	template<class _Value>
	inline void LowerUnorderedMapA<_Value>::Erase(const std::string& key) {
		map_.erase(ToLower(key));
	}

	template<class _Value>
	inline void LowerUnorderedMapA<_Value>::Clear() {
		map_.clear();
	}

	template<class _Value>
	inline size_t LowerUnorderedMapA<_Value>::Size() const {
		return map_.size();
	}

	template<class _Value>
	inline _Value& LowerUnorderedMapA<_Value>::operator[](const std::string& key) {
		return map_[ToLower(key)];
	}

	template<class _Value>
	inline const _Value& LowerUnorderedMapA<_Value>::operator[](const std::string& key) const {
		return map_.at(ToLower(key));
	}

	template<class _Value>
	inline std::string LowerUnorderedMapA<_Value>::ToLower(const std::string& str) {
		std::string lower = str;

		std::transform(
			str.begin(), str.end(),
			lower.begin(),
			[](char c) { return static_cast<char>(std::tolower(c)); }
		);

		return lower;
	}

	////////////////////////////////////////////////////////////////////////////////////////////
	// LowerUnorderedMapW class
	////////////////////////////////////////////////////////////////////////////////////////////
	template <class _Value>
	class LowerUnorderedMapW {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// using
		////////////////////////////////////////////////////////////////////////////////////////////
		using UnorderdMapW = std::unordered_map<std::wstring, _Value>;
		
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		LowerUnorderedMapW()  = default;
		~LowerUnorderedMapW() = default;

		void Emplace(const std::wstring& key, const _Value& value);
		void TryEmplace(const std::wstring& key, const _Value& value);

		bool Contains(const std::wstring& key) const;

		void Erase(const std::wstring& key);

		const _Value& At(const std::wstring& key) const;

		void Clear();

		size_t Size() const;

		const UnorderdMapW& GetMap() const { return map_; }
		UnorderdMapW& GetMap() { return map_; }

		//=========================================================================================
		// operator
		//=========================================================================================

		_Value& operator[](const std::wstring& key);
		const _Value& operator[](const std::wstring& key) const;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		UnorderdMapW map_;

		//=========================================================================================
		// private variables
		//=========================================================================================

		static std::wstring ToLower(const std::wstring& str);

	};

	template<class _Value>
	inline std::wstring LowerUnorderedMapW<_Value>::ToLower(const std::wstring& str) {
		std::wstring lower = str;

		std::transform(
			str.begin(), str.end(),
			lower.begin(),
			[](wchar_t c) { return static_cast<wchar_t>(std::tolower(c)); }
		);

		return lower;
	}

	template<class _Value>
	inline void LowerUnorderedMapW<_Value>::Emplace(const std::wstring& key, const _Value& value) {
		map_.emplace(ToLower(key), value);
	}

	template<class _Value>
	inline void LowerUnorderedMapW<_Value>::TryEmplace(const std::wstring& key, const _Value& value) {
		map_.try_emplace(ToLower(key), value);
	}

	template<class _Value>
	inline bool LowerUnorderedMapW<_Value>::Contains(const std::wstring& key) const {
		return map_.contains(ToLower(key));
	}

	template<class _Value>
	inline void LowerUnorderedMapW<_Value>::Erase(const std::wstring& key) {
		map_.erase(ToLower(key));
	}

	template<class _Value>
	inline const _Value& LowerUnorderedMapW<_Value>::At(const std::wstring& key) const {
		return map_.at(ToLower(key));
	}

	template<class _Value>
	inline void LowerUnorderedMapW<_Value>::Clear() {
		map_.clear();
	}

	template<class _Value>
	inline size_t LowerUnorderedMapW<_Value>::Size() const {
		return map_.size();
	}

	template<class _Value>
	inline _Value& LowerUnorderedMapW<_Value>::operator[](const std::wstring& key) {
		return map_[ToLower(key)];
	}

	template<class _Value>
	inline const _Value& LowerUnorderedMapW<_Value>::operator[](const std::wstring& key) const {
		return map_.at(ToLower(key));
	}

} // namespace Sxl