#include "FBaseBuffer.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Math/VectorComparison.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FBaseBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FBaseBuffer::Resize(const Vector2ui& resolution) {
	if (Comparison::All(resolution_ == resolution)) {
		return; //!< 解像度が同じ場合はリサイズしない.
	}

	Create(resolution); //!< 解像度が異なる場合はバッファを再生成する.
	resolution_ = resolution; //!< 解像度の更新
}
