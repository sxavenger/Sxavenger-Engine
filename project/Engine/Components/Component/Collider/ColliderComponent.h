#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"
#include "../Transform/TransformComponent.h"

//* collider
#include "CollisionDetection.h"

//* engine
#include <Engine/Foundation.h>

//* lib
#include <Lib/CXXAttribute.h>
#include <Lib/Sxl/Flag.h>

//* c++
#include <functional>
#include <bitset>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class CollisionCallbackCollection;

////////////////////////////////////////////////////////////////////////////////////////////
// ColliderComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class ColliderComponent final
	: public BaseComponent {
public:
	// hack: Boundingごとにクラスを分ける

	////////////////////////////////////////////////////////////////////////////////////////////
	// History enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class History : uint8_t { //!< 衝突履歴
		Current  = 1 << 0, //!< 現在frameの衝突履歴 (0b01)
		Previous = 1 << 1, //!< 1frame前の衝突履歴  (0b10)

		None = 0,                  //!< どちらの履歴もない状態 (0b00)
		All  = Current | Previous, //!< どちらの履歴もある状態 (0b11)
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// State structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct State {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Update();

		Sxl::Flag<History> GetBit() const;

		void Set(History history, const CollisionDetection::Detection& detection);

		//* operator [access] *//

		CollisionDetection::Detection& operator[](History history);
		const CollisionDetection::Detection& operator[](History history) const;

		CollisionDetection::Detection& At(History history);
		const CollisionDetection::Detection& At(History history) const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		std::array<CollisionDetection::Detection, static_cast<uint8_t>(History::Previous)> detections; //!< 衝突検出結果の履歴
		//!< detections[0]: 現在frameの衝突検出結果 (History::Current)
		//!< detections[1]: 1frame前の衝突検出結果 (History::Prev)

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ColliderComponent(EntityBehaviour* behaviour) : BaseComponent(behaviour) {}
	~ColliderComponent() override = default;

	void ShowComponentInspector() override;

	void Update();

	//* bounding option *//

	void SetBoundingSphere(const CollisionBoundings::Sphere& sphere = { .radius = 1.0f });

	void SetBoundingCapsule(const CollisionBoundings::Capsule& capsule = { .direction = { 0.0f, 1.0f, 0.0f }, .radius = { 1.0f }, .length = { 2.0f } });

	void SetBoundingAABB(const CollisionBoundings::AABB& aabb = { .min = { -0.5f, -0.5f, -0.5f }, .max = { 0.5f, 0.5f, 0.5f } });

	void SetBoundingOBB(const CollisionBoundings::OBB& obb = { .orientation = Quaternion::Identity(), .size = { 1.0f, 1.0f, 1.0f } });

	void SetBounding(const CollisionBoundings::Boundings& bounding) { bounding_ = bounding; }

	const CollisionBoundings::Boundings& GetBounding() const { return bounding_; }

	CollisionBoundings::BoundingType GetBoundingType() const { return static_cast<CollisionBoundings::BoundingType>(bounding_.index()); }

	//* collision state option *//

	void OnCollision(ColliderComponent* other, const CollisionDetection::Detection& detection);

	void CallbackOnCollision(const CollisionCallbackCollection* collection);

	//! @brief 衝突状態を設定
	//! @param other 対象のColliderComponent
	//! @param history 履歴の種類 (History::Current or History::Previous)
	//! @param detection 衝突検出結果
	void SetCollisionState(
		ColliderComponent* const other,
		History history, const CollisionDetection::Detection& detection
	);

	//* tag option *//

	void SetTag(const std::string& tag) { tag_ = tag; }

	const std::string& GetTag() const { return tag_; }

	//* active option *//

	void SetEnable(bool isEnable) { isEnable_ = isEnable; }

	bool IsEnable() const { return isEnable_; }

	//* other component option *//

	TransformComponent* RequireTransform() const;

	//* json option *//

	json ParseToJson() const override;

	void InputJson(const json& data) override;

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// BoundingPrimitiveLine structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct BoundingPrimitiveLine {
		//!< TODO: どこかのクラスに移す. ColliderComponentに置くのは微妙.
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void operator()(const CollisionBoundings::Sphere& sphere);
		void operator()(const CollisionBoundings::Capsule& capsule);
		void operator()(const CollisionBoundings::AABB& aabb);
		void operator()(const CollisionBoundings::OBB& obb);

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector3f position = {};
		Color4f color     = {};

	};

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	bool isEnable_ = true;

	//* tag *//

	std::string tag_ = "new collider";

	//* bounding *//

	CollisionBoundings::Boundings bounding_ = CollisionBoundings::Sphere{ 1.0f };

	//* states *//

	//! [unordered_map]
	//! key:   対象のcollider
	//! value: state
	std::unordered_map<ColliderComponent*, State> states_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* helper methods *//

	void PushBoundingLine() const;

};

SXAVENGER_ENGINE_NAMESPACE_END
