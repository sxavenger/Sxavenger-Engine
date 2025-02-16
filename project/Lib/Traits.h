#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// defines
////////////////////////////////////////////////////////////////////////////////////////////

// vvv copy and move defines vvv

#define _DEFAULT_COPY(_class) \
	_class(const _class&)            noexcept = default; \
	_class& operator=(const _class&) noexcept = default;

#define _DEFAULT_MOVE(_class) \
	_class(_class&&)            noexcept = default; \
	_class& operator=(_class&&) noexcept = default;

#define _DELETE_COPY(_class) \
	_class(const _class&)            = delete; \
	_class& operator=(const _class&) = delete;

#define _DELETE_MOVE(_class) \
	_class(_class&&)            = delete; \
	_class& operator=(_class&&) = delete;
