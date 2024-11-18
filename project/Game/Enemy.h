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

//* game
#include <Game/Particle.h>

//* c++
#include <queue>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class Player;

////////////////////////////////////////////////////////////////////////////////////////////
// EnemyType enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum EnemyType {
	kEnemyType_Star,
	kEnemyType_Cube,
	kEnemyType_Rocket,
	kEnemyType_Ballun,

	kCountOfEnemyType
};

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

	void Init(Model* model, const Vector3f& position, const Vector3f& velocity = {}, ParticleCollection* particle = nullptr, int32_t point = 30);

	void Term();

	void Update();

	void SetAttributeImGui() override;

	void OnCollisionEnter(_MAYBE_UNUSED Collider* const other) override;

	//* getter *//

	bool IsDelete() const { return isDelete_; }

	int32_t GetPoint() const { return point_; }

	Model* GetModel() const { return model_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* element parameter *//

	bool isDelete_ = false;

	Vector3f velocity_;
	// HACK: 敵1体にrailがありそれを使って動かす

	int32_t point_;

	DeltaTimePoint deleteTime_;
	DeltaTimePoint deleteTimer_ = { 8.0f };

	ParticleCollection* particle_ = nullptr;
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

	void SetPlayer(Player* player) { player_ = player; }

	void SetParticleCollection(ParticleCollection* particle) { particle_ = particle; }

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// EnemyPopCommand structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct EnemyPopCommand {
		DeltaTimePoint point;
		EnemyType type;
		Vector3f velocity;
		Vector3f offset;
	};

	struct CompalePopCommand {
		bool operator()(const EnemyPopCommand& a, const EnemyPopCommand& b) {
			return a.point > b.point;  // t が小さい方が優先されるようにする
		}
	};
	
	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	Player* player_ = nullptr;
	ParticleCollection* particle_ = nullptr;

	//* container *//

	std::list<std::unique_ptr<Enemy>> enemies_;

	//* command *//

	std::priority_queue<EnemyPopCommand, std::vector<EnemyPopCommand>, CompalePopCommand> commands_;

	//* element parameter *//

	Model* enemyModel_[kCountOfEnemyType] = { nullptr };

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateEnemy(EnemyType type, const Vector3f& position, const Vector3f& velocity = {});

	void CreateEnemy(const EnemyPopCommand& command);

	void CreateEnemyPopCommand(DeltaTimePoint popTime, EnemyType type, const Vector3f& offset, const Vector3f& velocity = {});

	void PushCommand();

};