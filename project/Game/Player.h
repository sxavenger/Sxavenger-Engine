#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Game/Behavior/ModelBehavior.h>

//* engine
#include <Engine/System/Performance.h>
#include <Engine/Game/Camera/CineCamera.h>

//* Game
#include <Game/Rail.h>

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

	Player() = default;
	~Player() { Term(); }

	void Init();

	void Term();

	void Update();

	void SetAttributeImGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* externals *//

	CineCamera* camera_ = nullptr;

	//* member *//

	std::unique_ptr<Rail> rail_;

	//* parameter *//

	DeltaTimePoint loopTime_ = { 24.0f }; //!< 1周の時間
	DeltaTimePoint loopTimer_;             //!< 計測用

	//* bullet *//

	//std::unique_ptr<PlayerBullet> bullet_;


	//=========================================================================================
	// private methods
	//=========================================================================================

	void Move();

};