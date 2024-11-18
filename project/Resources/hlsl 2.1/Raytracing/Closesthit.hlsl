//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "ClosesthitCommon.hlsli"

//=========================================================================================
// LocalBuffer
//=========================================================================================

Texture2D<float4> gAlbedo : register(t2);
SamplerState     gSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float4 AlphaBlend(float4 base, float4 add) {
	float4 result;
	
	// alpha
	result.a = base.a + add.a * (1.0f - base.a);
	
	// color
	result.rgb = (base.rgb * base.a + add.rgb * add.a * (1.0f - base.a)) / result.a;
	
	return result;
	
}

////////////////////////////////////////////////////////////////////////////////////////////
// entry point closesthit
////////////////////////////////////////////////////////////////////////////////////////////
[shader("closesthit")]
void mainClosesthit(inout Payload payload, Attribute attribute) {

	if (payload.Update()) {
		return;
	}

	Vertex vertex = GetHitWorldVertex(attribute);

	float4 color = gAlbedo.SampleLevel(gSampler, vertex.texcoord, 0.0f);

	RayDesc desc;
	desc.Origin    = vertex.position.xyz;
	desc.Direction = reflect(WorldRayDirection(), vertex.normal);
	desc.TMin      = kTmin;
	desc.TMax      = kTmax;

	TraceRay(
		payload, desc
	);

	color = AlphaBlend(float4(payload.color.rgb, 0.4f), color);
	
	payload.color = color;

}