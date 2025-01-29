#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* state
#include "BaseEnemyState.h"

//* engine
#include <Engine/System/Runtime/Performance/TimePoint.h>

////////////////////////////////////////////////////////////////////////////////////////////
// EnemyStateReactionLight class
////////////////////////////////////////////////////////////////////////////////////////////
class EnemyStateReactionLight
	: public BaseEnemyState {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	EnemyStateReactionLight(Enemy* enemy) : BaseEnemyState(enemy) {};
	~EnemyStateReactionLight() = default;

	void Init() override;

	void Term() override;

	void Update() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* time *//

	TimePointf<TimeUnit::second> time_;
	TimePointf<TimeUnit::second> duration_;

};
