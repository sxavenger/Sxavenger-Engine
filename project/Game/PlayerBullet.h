#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Game/Behavior/BaseBehavior.h>
#include <Engine/Game/Behavior/ModelBehavior.h>

//* engine
#include <Engine/System/Performance.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerBullet class
////////////////////////////////////////////////////////////////////////////////////////////
class PlayerBullet //!< razerが近い
	: public BaseBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	PlayerBullet()  = default;
	~PlayerBullet() { Term(); }

	void Init();

	void Term();

	void Update();

	void Shoot(bool isShot, const Vector3f& position, const Vector3f& direction);

	void CheckCollision();

	void SetAttributeImGui() override;

	void DrawAdaptive(_MAYBE_UNUSED const Camera3D* camera) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* member *//

	bool     isShot_ = false;
	Vector3f direction_;
	Vector3f position_;

	//* parameter *//

	float maxEnergy_      = 4.0f;      //!< 最大エネルギー量
	float energy_         = maxEnergy_;
	float energyDecay_    = 1.0f; //!< 発射時の減る量        * deltaTime
	float energyRecovery_ = 1.0f; //!< 発射してない時の回復量 * deltaTime
	
};