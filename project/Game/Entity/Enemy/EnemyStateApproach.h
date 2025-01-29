#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* state
#include "BaseEnemyState.h"

//* engine
#include <Engine/Module/Collider/Collider.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// EnemyStateApproach class
////////////////////////////////////////////////////////////////////////////////////////////
class EnemyStateApproach
	: public BaseEnemyState {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	EnemyStateApproach(Enemy* enemy) : BaseEnemyState(enemy) {};
	~EnemyStateApproach() = default;

	void Init() override;

	void Term() override;

	void Update() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	float speed_ = 0.2f;

	//* collider *//

	std::unique_ptr<Collider> approachCollider_;

};
