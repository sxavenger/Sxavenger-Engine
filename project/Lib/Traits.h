#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// defines
////////////////////////////////////////////////////////////////////////////////////////////

// vvv copy and move defines vvv

#define DEFAULT_COPY(_class) \
	_class(const _class&)            noexcept = default; \
	_class& operator=(const _class&) noexcept = default;

#define DEFAULT_MOVE(_class) \
	_class(_class&&)            noexcept = default; \
	_class& operator=(_class&&) noexcept = default;

#define DELETE_COPY(_class) \
	_class(const _class&)            = delete; \
	_class& operator=(const _class&) = delete;

#define DELETE_MOVE(_class) \
	_class(_class&&)            = delete; \
	_class& operator=(_class&&) = delete;
