//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "RayTracingCommon.hlsli"

//=========================================================================================
// local buffers
//=========================================================================================

////////////////////////////////////////////////////////////////////////////////////////////
// raygeneration main entry point
////////////////////////////////////////////////////////////////////////////////////////////
RAYGENERATION void mainRaygeneration() {

	uint2 index     = DispatchRaysIndex().xy;
	uint2 dimension = DispatchRaysDimensions().xy;

	Lux::Setting setting = gSetting;

	GBuffer::Surface surface;
	if (!surface.FetchSurface(GBuffer::FetchArgument::Create(Lux::GetSurfacePixel(index, setting), resolution, gCamera.projInv, gCamera.world))) {
		//!< surfaceが存在しない場合
		gRadiance[index] = float4(0.0f, 0.0f, 0.0f, 0.0f);
		return;
	}

	float2 uv = float2(index % setting.atlasDimension) / setting.atlasDimension; //!< atlas内のuv座標
	float3 direction = Octahedral::DecodePyramid(uv);

	Payload payload = Payload::TracePrimaryRay(surface.position, Lux::ConvertDirection(direction, surface.normal));

	gRadiance[index] = float4(payload.lo, 1.0f);
}

