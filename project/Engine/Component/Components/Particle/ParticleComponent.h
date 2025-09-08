#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/System/DirectX/DxObject/DxGraphicsPipelineState.h>
#include <Engine/System/Runtime/Performance/TimePoint.h>
#include <Engine/System/Runtime/Performance/DeltaTimePoint.h>
#include <Engine/Content/InputGeometry/InputPrimitive.h>
#include <Engine/Component/Components/Camera/CameraComponent.h>

//* lib
#include <Lib/Geometry/Vector3.h>
#include <Lib/Geometry/Color3.h>
#include <Lib/Transform/Transform.h>
#include <Lib/Motion/Motion.h>

//* c++
#include <memory>
#include <functional>
#include <list>

////////////////////////////////////////////////////////////////////////////////////////////
// ParticleComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class ParticleComponent final
	: public BaseComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Albedo structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Albedo {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// Type enum class
		////////////////////////////////////////////////////////////////////////////////////////////
		enum class Type : uint32_t {
			Value   = 0,
			Texture = 1
		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init();

		void SetValue(const Color3f& _albedo);

		void SetTexture(uint32_t _index, const Color3f& _albedo = kWhite3<float>);

		//=========================================================================================
		// public variables
		//=========================================================================================

		Type type;

		Color3f albedo;
		uint32_t index;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Transparent structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Transparent {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// Type enum class
		////////////////////////////////////////////////////////////////////////////////////////////
		enum class Type : uint32_t {
			Value   = 0,
			Texture = 1
		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init();

		void SetValue(float _transparent);

		void SetTexture(uint32_t _index, float _transparent = 1.0f);

		//=========================================================================================
		// public variables
		//=========================================================================================

		Type type;

		float    transparent;
		uint32_t index;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Particle structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Particle {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init();

		//=========================================================================================
		// public variables
		//=========================================================================================

		Albedo albedo;
		Transparent transparent;

		uint32_t isBillboard; //!< flag
		// todo: Y軸billboardを追加.

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Element structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Element {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init();

		void Update();

		//* getter *//

		bool IsDelete() const;

		Matrix4x4 GetMat() const;

		Particle GetParticle() const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		//* time
		DeltaTimePointf<TimeUnit::second> time;
		TimePointf<TimeUnit::second>      lifeTime;

		//* transform
		QuaternionTransform transform;
		MotionT<Vector3f> velocity;
		MotionT<Vector3f> size;

		//* color
		Color4f color;
		MotionT<Color3f> albedo;
		MotionT<float> transparent;

		//* textures
		std::optional<uint32_t> albedoIndex;
		std::optional<uint32_t> transparentIndex;

		//* parameter
		bool isBillboard = false;
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ParticleComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) {}
	~ParticleComponent() override = default;

	void Init(uint32_t count, BlendMode mode = BlendMode::kBlendModeNormal);

	void Update();


	void SetPrimitive(InputPrimitive&& primitive);

	Element& Emit(const Vector3f& position);

	void DrawParticle(const DirectXQueueContext* context, const CameraComponent* camera);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* emitter paraemter *//

	//std::function<Vector3f()> emitFunction_;

	//* particle parameter *//

	//* particle *//

	// primtive mesh
	std::optional<InputPrimitive> primitive_;

	// transforms
	std::unique_ptr<DxObject::DimensionBuffer<TransformationMatrix>> matrices_;

	// particle
	std::unique_ptr<DxObject::DimensionBuffer<Particle>> particles_;


	// intermediate
	uint32_t instance_ = 0;

	// element
	std::list<Element> elements_;

	//* HACK: pipeline *//

	std::unique_ptr<DxObject::ReflectionGraphicsPipelineState> pipeline_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void AssingBuffer();
};
