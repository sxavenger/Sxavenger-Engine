#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// collider base
#include <Collider.h>

// c++
#include <memory>

// IAs
#include <DrawMethod.h>

// engine
#include <Transform.h>

// Attribute
#include <Attribute.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DebugObject base class
////////////////////////////////////////////////////////////////////////////////////////////
class DebugObject
	: public Attribute {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DebugObject()          = default;
	virtual ~DebugObject() = default;

	virtual void Init() = 0;

	virtual void Update() = 0;

	virtual void Draw() = 0;

	bool IsDelete() const { return isDelete_; }

protected:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Material structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Material {
		Color4f color;

		void Init() {
			color = { 0.0f, 1.0f, 1.0f, 0.5f };
		}
	};

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* member *//

	bool isDelete_ = false;

	//* IA
	DrawData ia_;

	//* Buffer
	EulerTransformBuffer transform_;
	std::unique_ptr<DxObject::BufferResource<Material>> material_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// DebugObjects namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace DebugObjects {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Sphere(collider: sphere) class
	////////////////////////////////////////////////////////////////////////////////////////////
	class Sphere
		: public DebugObject
		, public Collider {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		Sphere() = default;
		~Sphere() { Term(); }

		void Init() override;

		void Term();

		void Update() override;

		void Draw() override;

		void SetAttributeImGui() override;

		const Vector3f& GetColliderPosition() const override { return transform_.transform.translate; }

		void OnCollisionEnter(_MAYBE_UNUSED Collider* const other) override;

		void OnCollisionExit(_MAYBE_UNUSED Collider* const other) override;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Box(collider: AABB) class
	////////////////////////////////////////////////////////////////////////////////////////////
	class Box
		: public DebugObject
		, public Collider {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init() override;

		void Term();

		void Update() override;

		void Draw() override;

		void SetAttributeImGui() override;

		const Vector3f& GetColliderPosition() const override { return transform_.transform.translate; }

		void OnCollisionEnter(_MAYBE_UNUSED Collider* const other) override;

		void OnCollisionExit(_MAYBE_UNUSED Collider* const other) override;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		QuaternionTransformBuffer qt_;
		Vector3f v = { 1.0f, 0.0f, 0.0 };
		float angle = 3.141592f / 2.0f;

	};


}

