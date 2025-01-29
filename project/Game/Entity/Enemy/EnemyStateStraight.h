#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* state
#include "BaseEnemyState.h"

//* engine
#include <Engine/System/Runtime/Performance/TimePoint.h>

////////////////////////////////////////////////////////////////////////////////////////////
// EnemyStateStraight class
////////////////////////////////////////////////////////////////////////////////////////////
class EnemyStateStraight
	: public BaseEnemyState {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	EnemyStateStraight(Enemy* enemy) : BaseEnemyState(enemy) {};
	~EnemyStateStraight() = default;

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
