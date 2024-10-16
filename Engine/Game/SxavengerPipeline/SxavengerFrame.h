#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// SystematicFrame class
////////////////////////////////////////////////////////////////////////////////////////////
class SystematicFrame {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// GBuffer enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum GBuffer {
		GBUFFER_ALBEDO,
		GBUFFER_NORMAL,
		GBUFFER_POSITION,
		GBUFFER_PBR_MATERIAL,
		kCountOfGBuffer
	};
	static_assert(kCountOfGBuffer < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT, "[Deferred GBuffer]: The number of GBuffer exceeds <D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT>.");

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

};

class XaltFrame {

};

class AdaptiveFrame {

};

class VisualFrame {

};