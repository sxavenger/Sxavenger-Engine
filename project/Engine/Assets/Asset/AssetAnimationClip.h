#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "BaseAsset.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/Graphics/Animation/Animation.h>

//* lib
#include <Lib/Adapter/Uuid/Uuid.h>

//* external
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Asset namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Asset {

	////////////////////////////////////////////////////////////////////////////////////////////
	// AnimationClip class
	////////////////////////////////////////////////////////////////////////////////////////////
	class AnimationClip final
		: public BaseAsset {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* constructor / destructor *//

		AnimationClip(const Uuid& id) : BaseAsset(id) {}

		~AnimationClip() override = default;

		//* setup option *//

		void Setup(const aiAnimation* animation); //!< assimp用

		//* animation option *//

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		Animation animation_;

		//=========================================================================================
		// private methods
		//=========================================================================================

		//* [assimp] setup helper methods *//

		static TimePointd<TimeUnit::second> GetTime(double time, double ticksPerSeconds);

		static TransformAnimation LoadAnimation(const aiNodeAnim* aiNodeAnimation, double tickPerSeconds);

	};

}

SXAVENGER_ENGINE_NAMESPACE_END
