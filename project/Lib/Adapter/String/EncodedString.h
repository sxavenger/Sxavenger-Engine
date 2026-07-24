#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <string>
#include <variant>

////////////////////////////////////////////////////////////////////////////////////////////
// EncodedString class
////////////////////////////////////////////////////////////////////////////////////////////
class EncodedString {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Type class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Type : size_t {
		None    = 0,
		String  = 1,
		WString = 2,
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	EncodedString() = default;

	//* operator [assignment] *//

	//!< encoded string [copy]
	EncodedString(const EncodedString& other) noexcept : string_(other.string_) {}
	EncodedString& operator=(const EncodedString& other) noexcept { string_ = other.string_; return *this; }

	//!< encoded string [move]
	EncodedString(EncodedString&& other) noexcept : string_(std::move(other.string_)) {}
	EncodedString& operator=(EncodedString&& other) noexcept { string_ = std::move(other.string_); return *this; }

	//!< string [copy]
	EncodedString(const std::string& str) noexcept : string_(str) {}
	EncodedString& operator=(const std::string& str) noexcept { string_ = str; return *this; }

	//!< string [move]
	EncodedString(std::string&& str) noexcept : string_(std::move(str)) {}
	EncodedString& operator=(std::string&& str) noexcept { string_ = std::move(str); return *this; }

	//!< wstring [copy]
	EncodedString(const std::wstring& wstr) noexcept : string_(wstr) {}
	EncodedString& operator=(const std::wstring& wstr) noexcept { string_ = wstr; return *this; }

	//!< wstring [move]
	EncodedString(std::wstring&& wstr) noexcept : string_(std::move(wstr)) {}
	EncodedString& operator=(std::wstring&& wstr) { string_ = std::move(wstr); return *this; }

	//!< empty
	EncodedString(std::monostate) noexcept : string_(std::monostate{}) {}
	EncodedString& operator=(std::monostate) noexcept { string_ = std::monostate{}; return *this; }

	//!< template [forwarding] (string)
	template <class T> requires std::constructible_from<std::string, T>
	EncodedString(T&& str) : string_(std::forward<T>(str)) {}

	template <class T> requires std::assignable_from<std::string&, T>
	EncodedString& operator=(T&& str) { string_ = std::forward<T>(str); return *this; }

	//!< template [forwarding] (wstring)
	template <class T> requires std::constructible_from<std::wstring, T>
	EncodedString(T&& wstr) : string_(std::forward<T>(wstr)) {}

	template <class T> requires std::assignable_from<std::wstring&, T>
	EncodedString& operator=(T&& wstr) { string_ = std::forward<T>(wstr); return *this; }

	//* encoded string options *//

	Type GetEncodedType() const;

	std::string GetString() const;

	std::wstring GetWString() const;

	const std::string& AsString() const;

	const std::wstring& AsWString() const;

	//* convert option *//

	static std::string Convert(const std::wstring_view& wstr);

	template <class T> requires std::constructible_from<std::wstring_view, T> //!< std::wstring_viewに変換可能な型であれば、変換してからConvertする
	static std::string Convert(const T& wstr) { return EncodedString::Convert(std::wstring_view(wstr)); }
	
	static std::wstring Convert(const std::string_view& str);

	template <class T> requires std::constructible_from<std::string_view, T> //!< std::string_viewに変換可能な型であれば、変換してからConvertする
	static std::wstring Convert(const T& str) { return EncodedString::Convert(std::string_view(str)); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::variant<std::monostate, std::string, std::wstring> string_ = std::monostate{};

};
