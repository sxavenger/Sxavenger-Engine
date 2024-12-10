#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Module/Behavior/ModelBehavior.h>

//* engine
#include <Engine/Asset/Asset.h>

//* game
#include <Game/Object/Weapon.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Player class
////////////////////////////////////////////////////////////////////////////////////////////
class Player
	: public ModelBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Player()  = default;
	~Player() = default;

	void Init();

	void Term();

	void Update();

	void SetAttributeImGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::shared_ptr<AssetModel> model_;

	float speed_ = 1.0f;

	Vector3f velocity_ = {};

	//* weapon *//

	std::unique_ptr<Weapon> weapon_;

	DeltaTimePoint<TimeUnit::s> attackTime_ = {};

	//=========================================================================================
	// private methods
	//=========================================================================================

	void MoveWithKeyboard();
	void AttackWithKeyboard();

};