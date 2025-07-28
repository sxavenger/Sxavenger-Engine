//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "PathtracingCommon.hlsli"

//=========================================================================================
// local buffers
//=========================================================================================

////////////////////////////////////////////////////////////////////////////////////////////
// raygeneration main entry point
////////////////////////////////////////////////////////////////////////////////////////////
_RAYGENERATION void mainRaygeneration() {

	uint2 index     = DispatchRaysIndex().xy;
	uint2 dimension = DispatchRaysDimensions().xy;

	if (IsBeginFrame()) {
		gIndirect[index] = float4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	if (!CheckNeedSample()) {
		return; //!< これ以上のsampleは不必要
	}

	DeferredSurface surface;
	if (!surface.GetSurface(index)) {
		gIndirect[index] = float4(0.0f, 0.0f, 0.0f, 0.0f);
		return; // surfaceが存在しない
	}

	// primary trace.

	float4 diffuse_indirect = float4(0.0f, 0.0f, 0.0f, 0.0f);
	for (uint i = 0; i < sampleStep; ++i) {

		uint currentSampleIndex = sampleStep * currentFrame + i;
		uint2 xi = Hammersley(currentSampleIndex, sampleCount);

		{ //!< Diffuseサンプリング

			RayDesc desc;
			desc.Origin    = surface.position;
			desc.Direction = ImportanceSampleLambert(xi, surface.normal);
			desc.TMin      = kTMin;
			desc.TMax      = kTMax;

			Payload payload = TracePrimaryRay(desc);

			diffuse_indirect += payload.indirect;
			// FIXME: ただの加算ではないはず...
		}
	}

	diffuse_indirect /= sampleCount; //!< 平均化

	float4 indirect = diffuse_indirect;
	
	gIndirect[index] += indirect;
	
}
