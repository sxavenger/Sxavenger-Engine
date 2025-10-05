#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/Input/Input.h>
#include <Engine/Component/Components/Transform/TransformComponent.h>
#include <Engine/Module/GameObject/GameObject.h>
#include <Engine/Adapter/Parameter/SerializeParameter.h>
#include <Engine/Adapter/Parameter/SerializeGui.h>
#include <Engine/Preview/Content/UContentModel.h>
#include <Engine/Preview/Content/UContentObserver.h>

//* demo
#include <Demo/Object/PerspectiveCamera.h>

////////////////////////////////////////////////////////////////////////////////////////////
// EmissiveCubes class
////////////////////////////////////////////////////////////////////////////////////////////
class EmissiveCubes
	: public GameObject {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Load() override;

	void Awake() override;

	void Start() override;

	void Update() override;

	//* setter *//

	void SetCamera(PerspectiveCamera* camera) { camera_ = camera; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* input *//

	const KeyboardInput* keyboard_ = nullptr;

	//* asset *//

	UContentObserver<UContentModel> model_;

	//* external *//

	const PerspectiveCamera* camera_ = nullptr;

	//* children *//

	std::array<std::unique_ptr<MonoBehaviour>, 6> cubes_;

	//* parameter *//

	const std::array<Color3f, 6> colors_ = {
		Color3f{0.53f, 0.93f, 0.94f},
		Color3f{0.91f, 0.89f, 0.71f},
		Color3f{0.82f, 0.63f, 0.96f},
		Color3f{0.71f, 0.8f, 0.88f},
		Color3f{0.96f, 0.84f, 0.24f},
		Color3f{1.0f, 0.6f, 0.64f},
	};

};
