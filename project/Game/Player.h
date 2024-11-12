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
#include <Game/PlayerBullet.h>
#include <Game/Score.h>

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

	virtual void DrawAdaptive(_MAYBE_UNUSED const Camera3D* camera) override;

	PlayerBullet* GetBullet() const { return bullet_.get(); }

	DeltaTimePoint GetPoint() const { return loopTimer_; }

	bool IsEnd() const { return loopTimer_ >= loopTime_; }

	Rail* GetRail() const { return rail_.get(); }

	Vector3f CatmullRomPosition(DeltaTimePoint point) { return rail_->LoopCatmullRomPosition(point.time / loopTime_.time); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* externals *//

	CineCamera* camera_ = nullptr;

	//* member *//

	std::unique_ptr<Rail>         rail_;
	std::unique_ptr<Score>        score_;
	std::unique_ptr<PlayerBullet> bullet_;

	//* parameter *//

	DeltaTimePoint loopTime_ = { 40.0f }; //!< 1周の時間
	DeltaTimePoint loopTimer_;            //!< 計測用

	Vector3f cameraOffset_ = { 0.0f, 0.1f, -0.1f };

	Vector3f direction_;


	//=========================================================================================
	// private methods
	//=========================================================================================

	void Move();
	void Shot();

};