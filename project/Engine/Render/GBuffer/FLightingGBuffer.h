#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../Common/FBaseTexture.h"
#include "../Core/FRenderCoreRestir.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxUnorderedDimensionBuffer.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* external
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// FLightingGBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class FLightingGBuffer {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Layout enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Layout : uint8_t {
		Direct,
		Indirect_Reservoir,
		Indirect_Denoise,
		Indirect,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Reservoir enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Reservoir : uint8_t {
		Initialize,
		Temporal,
		Spatial,
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FLightingGBuffer()  = default;
	~FLightingGBuffer() = default;

	void Init(const Vector2ui& size);

	//* getter *//

	FBaseTexture* GetGBuffer(Layout layout) const;

	DxObject::UnorderedDimensionBuffer<FRenderCoreRestir::Reservoir<16>>* GetReservoir(Reservoir reservoir) const;

	DxObject::UnorderedDimensionBuffer<Vector2ui>* GetMoment() const { return moment_.get(); }

	static DXGI_FORMAT GetFormat(Layout layout);

	FRenderCoreRestir::Config& GetConfig() { return config_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static inline const size_t kLayoutCount_ = magic_enum::enum_count<Layout>();

	//* format *//

	static const std::array<DXGI_FORMAT, kLayoutCount_> kFormats_;

	//* buffer *//

	std::array<std::unique_ptr<FBaseTexture>, kLayoutCount_> buffers_ = {};


	std::array<std::unique_ptr<DxObject::UnorderedDimensionBuffer<FRenderCoreRestir::Reservoir<16>>>, magic_enum::enum_count<Reservoir>()> reservoirs_ = {};
	std::unique_ptr<DxObject::UnorderedDimensionBuffer<Vector2ui>> moment_ = nullptr;


	std::unique_ptr<DxObject::UnorderedDimensionBuffer<FRenderCoreRestir::Reservoir<7>>> environmentReservoir_ = nullptr;
	std::unique_ptr<DxObject::UnorderedDimensionBuffer<Vector2ui>> environmentMoment_ = nullptr;

	//* parameter *//

	FRenderCoreRestir::Config config_ = {};

};
