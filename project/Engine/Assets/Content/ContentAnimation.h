#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* content
#include "BaseContent.h"

//* engine
#include <Engine/Foundation.h>

//* external
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//* c++
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ContentAnimation class
////////////////////////////////////////////////////////////////////////////////////////////
class ContentAnimation final
	: public BaseContent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	ContentAnimation() : BaseContent(Async::Execution::Cpu) {}

	~ContentAnimation() override = default;

	//* content option *//

	void Attach(const std::filesystem::path& filepath, const std::any& parameter) override;

	void Load(MAYBE_UNUSED const DirectXQueueContext* context) override;

	//* id option *//

	const Uuid& GetAnimation(size_t index) const { return animations_[index]; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::vector<Uuid> animations_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void AttachUuid(const std::filesystem::path& filepath);

	//* load helper methods *//

	static std::shared_ptr<Assimp::Importer> LoadImporter(const std::filesystem::path& filepath, uint32_t option = 0);

};

SXAVENGER_ENGINE_NAMESPACE_END
