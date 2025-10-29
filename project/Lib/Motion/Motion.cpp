#include "Motion.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////////////
// MotionT class methods
////////////////////////////////////////////////////////////////////////////////////////////
float GetMotionT(MotionMode mode, float t) {
	switch (mode) {
		case MotionMode::Default:
			return t;

		case MotionMode::Clamp:
			return std::clamp(t, 0.0f, 1.0f);
			
		case MotionMode::Wrap:
			return t - std::floor(t);

		case MotionMode::Mirror:
			{
				float ft = std::floor(t);
				float p  = t - ft;

				if (static_cast<int32_t>(ft) % 2 == 1) {
					p = 1.0f - p;
				}

				return p;
			}

		case MotionMode::WrapOne:
			return GetMotionT(MotionMode::Wrap, t * 2.0f);

		case MotionMode::MirrorOne:
			return GetMotionT(MotionMode::Mirror, t * 2.0f);
	}

	return 0.0f;
}
