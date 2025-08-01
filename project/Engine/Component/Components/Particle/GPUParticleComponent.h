#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"

//* particle
#include "ParticleConfig.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxUnorderedDimensionBuffer.h>
#include <Engine/System/DirectX/DxObject/DxAppendConsumeBuffer.h>
#include <Engine/Content/InputGeometry/InputPrimitive.h>

//* lib
#include <Lib/Geometry/Vector3.h>
#include <Lib/Geometry/Color3.h>

//* c++
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// GPUParticleComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class GPUParticleComponent final
	: public BaseComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Element structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Element {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		Color3f color;
		float transparent;

		Vector3f position;
		float padding; //!< FIXME!
		Vector3f scale;

		float lifetime;
		float timer;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	GPUParticleComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) {}
	~GPUParticleComponent() override = default;

	//* particle option *//

	void Create(uint32_t count);

	void Update(const DirectXQueueContext* context);

	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddress() const;

	//* primitive option *//

	void SetPrimitive(InputPrimitive&& primitive);

	void BindIABuffer(const DirectXQueueContext* context) const;

	void DrawCall(const DirectXQueueContext* context) const;

	bool HasPritimive() const;

	//* config option *//

	ParticleConfig& GetConfig() { return config_; }
	const ParticleConfig& GetConfig() const { return config_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::optional<InputPrimitive> primitive_;

	std::unique_ptr<DxObject::UnorderedDimensionBuffer<Element>> buffer_;
	std::unique_ptr<DxObject::AppendConsumeBuffer<uint32_t>> freeList_;

	ParticleConfig config_ = {};

	std::unique_ptr<DxObject::ReflectionComputePipelineState> pipeline_; //!< HACK;

	//=========================================================================================
	// friend
	//=========================================================================================
	friend class EmitterComponent;

};
