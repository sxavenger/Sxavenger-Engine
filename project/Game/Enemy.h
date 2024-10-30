#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Game/Behavior/BaseBehavior.h>
#include <Engine/Game/Behavior/ModelBehavior.h>
#include <Engine/Game/Collider/Collider.h>

//* engine
#include <Engine/System/Performance.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Enemy class
////////////////////////////////////////////////////////////////////////////////////////////
class Enemy
	: public ModelBehavior, public Collider {
public:

	//=========================================================================================
	// public methods 
	//=========================================================================================

	Enemy()  = default;
	~Enemy() { Term(); }

	void Init(Model* model, const Vector3f& position, const Vector3f& velocity = {});

	void Term();

	void Update();

	void SetAttributeImGui() override;

	void OnCollisionEnter(_MAYBE_UNUSED Collider* const other) override;

	//* getter *//

	bool IsDelete() const { return isDelete_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* element parameter *//

	bool isDelete_ = false;

	Vector3f velocity_;
	// HACK: 敵1体にrailがありそれを使って動かす

};

////////////////////////////////////////////////////////////////////////////////////////////
// EnemyCollection class
////////////////////////////////////////////////////////////////////////////////////////////
class EnemyCollection
	: public BaseBehavior {
public:

	//=========================================================================================
	// public methods 
	//=========================================================================================

	EnemyCollection()  = default;
	~EnemyCollection() { Term(); }

	void Init();

	void Term();

	void Update();

	void SetAttributeImGui() override;

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// EnemyPopCommand structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct EnemyPopCommand {
		DeltaTimePoint popTime;
		Vector3f velocity;
		Vector3f position;
	};
	
	//=========================================================================================
	// private variables
	//=========================================================================================

	//* container *//

	std::list<std::unique_ptr<Enemy>> enemies_;

	//* command *//

	std::list<EnemyPopCommand> commands_;
	DeltaTimePoint popTimer_;

	//* element parameter *//

	Model* enemyModel_ = nullptr;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateEnemy(const Vector3f& position, const Vector3f& velocity = {});

	void CreateEnemyPopCommand(DeltaTimePoint popTime, const Vector3f& position, const Vector3f& velocity = {});

};