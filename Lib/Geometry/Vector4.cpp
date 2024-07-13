#include "Vector4.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Color4 cast methods
////////////////////////////////////////////////////////////////////////////////////////////

Color4i ToColor4(const Color4f& color) {
	return { static_cast<int>(color.r * 255.0f), static_cast<int>(color.g * 255.0f), static_cast<int>(color.b * 255.0f), static_cast<int>(color.a * 255.0f) };
}

Color4f ToColor4(const Color4i& color) {
	return { color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f };
}
