#pragma once

//-----------------------------------------------------------------------------------------
// define / c++ attribute
//-----------------------------------------------------------------------------------------

//! https://cpprefjp.github.io/lang/cpp17/maybe_unused.html
#define MAYBE_UNUSED [[maybe_unused]]

//! https://cpprefjp.github.io/lang/cpp17/nodiscard.html
#define NODISCARD [[nodiscard]]

//! https://cpprefjp.github.io/lang/cpp20/likely_and_unlikely_attributes.html
#define LIKELY   [[likely]]
#define UNLIKELY [[unlikely]]

//! https://cpprefjp.github.io/lang/cpp11/attributes.html
#define NORETURN [[noreturn]]

//! https://cpprefjp.github.io/lang/cpp14/deprecated_attr.html
#define DEPRECATED(msg) [[deprecated(msg)]]
