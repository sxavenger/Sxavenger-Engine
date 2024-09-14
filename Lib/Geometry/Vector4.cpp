#include "Vector4.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Color4 cast methods
////////////////////////////////////////////////////////////////////////////////////////////

constexpr Color4i ToColor4i(const Color4f& color) {
	return { static_cast<int>(color.r * 255.0f), static_cast<int>(color.g * 255.0f), static_cast<int>(color.b * 255.0f), static_cast<int>(color.a * 255.0f) };
}

constexpr Color4f ToColor4f(const Color4i& color) {
	return { color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f };
}

constexpr Color4f ToColor4f(uint32_t colorCode) {
	Color4f result = {};

	result.r = ((colorCode >> (8 * 3)) & 0xFF) / 255.0f;
	result.g = ((colorCode >> (8 * 2)) & 0xFF) / 255.0f;
	result.b = ((colorCode >> (8 * 1)) & 0xFF) / 255.0f;
	result.a = ((colorCode >> (8 * 0)) & 0xFF) / 255.0f;

	return result;
}
