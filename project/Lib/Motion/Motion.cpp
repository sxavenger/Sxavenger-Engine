#include "Motion.h"

////////////////////////////////////////////////////////////////////////////////////////////
// MotionMode methods
////////////////////////////////////////////////////////////////////////////////////////////

float GetMotionT(MotionMode mode, float t) {
	switch (mode) {
		case MotionMode::Clamp:
			return std::clamp(t, 0.0f, 1.0f);

		case MotionMode::Wrap:
			return std::fmod(t, 1.0f);

		case MotionMode::Mirror:
			return 1.0f - std::abs(std::fmod(t, 2.0f) - 1.0f);

		default:
			return t;
	}
}
