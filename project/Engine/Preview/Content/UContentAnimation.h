#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* ucontent
#include "UBaseContent.h"

//* external
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//* c++
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// UContentAnimation class
////////////////////////////////////////////////////////////////////////////////////////////
class UContentAnimation final
	: public UBaseContent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	using Uuids = std::vector<Uuid>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	UContentAnimation()           = default;
	~UContentAnimation() override = default;

	void AsyncLoad(_MAYBE_UNUSED const DirectXQueueContext* context) override;

	AsyncExecution GetAsyncExecution() const { return AsyncExecution::None; }

	void AttachUuid() override;

	//* content option *//

	void Load(const std::filesystem::path& filepath, uint32_t assimpOption);

	//* getter *//

	const Uuid& GetAnimation(size_t index) const { return animations_[index]; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Uuids animations_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* helper method *//

	void GetUuid();

	void LoadAnimations(const aiScene* aiScene);

};
