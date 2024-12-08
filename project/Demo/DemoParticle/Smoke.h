#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Demo/Particle/BaseParticle.h>

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/System/DirectX/DxObject/DxGraphicsPipelineState.h>
#include <Engine/Module/Camera/Camera3d.h>
#include <Engine/Asset/Asset.h>
#include <Engine/System/Runtime/Performance/DeltaTimePoint.h>

//* c++
#include <list>

////////////////////////////////////////////////////////////////////////////////////////////
// Illmaination class
////////////////////////////////////////////////////////////////////////////////////////////
class Smoke
	: public BaseParticle {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Smoke()  = default;
	~Smoke() { Term(); }

	void Init();

	void Term();

	void Update();

	void Emit(const Vector3f& position) override;

	void SetAttributeImGui() override;

	virtual void DrawAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame);

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Element structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Element {
		QuaternionTransform transform;
		Color4f             color;

		Vector3f addRotate;

		DeltaTimePoint<TimeUnit::s> lifeTime;
		DeltaTimePoint<TimeUnit::s> currentTime;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	const Camera3d* camera_ = nullptr;

	//* member *//

	std::list<Element> elements_;

	std::shared_ptr<AssetTexture> texture_;
	std::shared_ptr<AssetModel> model_;

	//* parameter *//

	static const uint32_t kInstanceCount_ = 128;
	uint32_t instanceIndex_ = 0;

	//* buffer *//

	std::unique_ptr<DxObject::DimensionBuffer<Color4f>> colorBuffer_;

	std::unique_ptr<DxObject::ReflectionGraphicsPipelineState> state_;

};