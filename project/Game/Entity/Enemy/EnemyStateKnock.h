#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* state
#include "BaseEnemyState.h"

//* engine
#include <Engine/System/Runtime/Performance/TimePoint.h>

////////////////////////////////////////////////////////////////////////////////////////////
// EnemyStateKnock class
////////////////////////////////////////////////////////////////////////////////////////////
class EnemyStateKnock
	: public BaseEnemyState {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	EnemyStateKnock(Enemy* enemy) : BaseEnemyState(enemy) {};
	~EnemyStateKnock() = default;

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
