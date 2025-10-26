#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Component/Components/Transform/TransformComponent.h>
#include <Engine/Module/GameObject/GameObject.h>
#include <Engine/Adapter/Parameter/SerializeParameter.h>
#include <Engine/Adapter/Parameter/SerializeGui.h>
#include <Engine/Preview/Content/UContentModel.h>
#include <Engine/Preview/Content/UContentObserver.h>

////////////////////////////////////////////////////////////////////////////////////////////
// CollectibleItems class
////////////////////////////////////////////////////////////////////////////////////////////
class CollectibleItems
	: public GameObject {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Load() override;

	void Awake() override;

	void Start() override;

	void Update() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static const size_t kItemCount = 6;

	//* asset *//

	UContentObserver<UContentModel> model_;

	//* children *//

	std::array<std::unique_ptr<MonoBehaviour>, kItemCount> cubes_;

	//* parameter *//

	const std::array<Color3f, kItemCount> colors_ = {
		Color3f{0.53f, 0.93f, 0.94f},
		Color3f{0.91f, 0.89f, 0.71f},
		Color3f{0.82f, 0.63f, 0.96f},
		Color3f{0.71f, 0.8f, 0.88f},
		Color3f{0.96f, 0.84f, 0.24f},
		Color3f{1.0f, 0.6f, 0.64f},
	};

};
