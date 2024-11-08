#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Game/Behavior/ModelBehavior.h>
#include <Engine/Game/Collider/Collider.h>

//* Game
#include <Game/Score.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerBullet class
////////////////////////////////////////////////////////////////////////////////////////////
class PlayerBullet //!< razerが近い
	: public ModelBehavior, public Collider {
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

	void SetAttributeImGui() override;

	void DrawAdaptive(_MAYBE_UNUSED const Camera3D* camera) override;
	void DrawSystematic(_MAYBE_UNUSED const Camera3D* camera) override;

	virtual void OnCollisionEnter(_MAYBE_UNUSED Collider* const other);

	//* setter *//

	void SetScore(Score* score) { score_ = score; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	Score* score_ = nullptr;
	
	//* member *//

	bool     isShot_ = false;
	Vector3f direction_;
	Vector3f position_;

	Quaternion rotate_ = Quaternion::Identity();

	//* parameter *//

	float maxEnergy_      = 4.0f;      //!< 最大エネルギー量
	float energy_         = maxEnergy_;
	float energyDecay_    = 1.0f; //!< 発射時の減る量        * deltaTime
	float energyRecovery_ = 1.0f; //!< 発射してない時の回復量 * deltaTime
	
};