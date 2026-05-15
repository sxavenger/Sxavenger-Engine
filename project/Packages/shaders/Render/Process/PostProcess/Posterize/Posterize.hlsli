#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../PostProcess.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// PosterizeMode namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace PosterizeMode {
	static const uint Ceil  = 0, //!< 切り上げ [c++ std::ceil](https://cpprefjp.github.io/reference/cmath/ceil.html)
	                  Floor = 1, //!< 切り捨て [c++ std::floor](https://cpprefjp.github.io/reference/cmath/floor.html)
	                  Round = 2; //!< 四捨五入 [c++ std::round](https://cpprefjp.github.io/reference/cmath/round.html)
}

////////////////////////////////////////////////////////////////////////////////////////////
// Posterize structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Posterize {
	
	//=========================================================================================
	// public variables
	//=========================================================================================

	//* coordinate posterize *//

	float coordinateLevel;
	uint coordinateMode;

	//* color posterize *//

	float colorLevel;
	uint colorMode;
	
};
