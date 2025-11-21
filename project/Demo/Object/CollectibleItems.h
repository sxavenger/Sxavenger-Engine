#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Component/Components/Transform/TransformComponent.h>
#include <Engine/Module/GameObject/GameObject.h>
#include <Engine/Adapter/Parameter/SerializeParameter.h>
#include <Engine/Adapter/Parameter/SerializeGui.h>

//* demo
#include "CollectibleCube.h"

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

	void Inspectable() override;

	//* getter *//

	bool IsCollected() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static const size_t kItemCount = 6;

	//* children *//

	std::array<CollectibleCube*, kItemCount> cubes_;

	SerializeParameter<std::vector<Vector3f>> positions_{
		"CollectibleItems::positions",
		{
			{ 5.0f, 0.4f, 0.0f },
			{ -5.0f, 0.4f, 0.0f },
			{ 0.0f, 0.4f, 5.0f },
			{ 0.0f, 0.4f, -5.0f },
			{ 3.5f, 0.4f, 3.5f },
			{ -3.5f, 0.4f, -3.5f }
		},
	};

};
