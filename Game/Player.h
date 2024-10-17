#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Beta/ModelBehavior.h>

//* engine
#include <Engine/System/Performance.h>
#include <Engine/Beta/CineCamera.h>

//* Game
#include <Game/Rail.h>
#include <Game/PlayerBullet.h>

//* c++
#include <memory>

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
	~Player() { Term(); }

	void Init();

	void Term();

	void Update();

	void SetAttributeImGui() override;

	//* setter *//

	void SetRail(Rail* rail) { rail_ = rail; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* externals *//

	CineCamera* camera_ = nullptr;
	Rail* rail_         = nullptr;

	//* parameter *//
	
	DeltaTimePoint loopTime_  = { 24.0f }; //!< 1周の時間
	DeltaTimePoint loopTimer_;             //!< 計測用

	//* bullet *//

	std::unique_ptr<PlayerBullet> bullet_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Move();

};