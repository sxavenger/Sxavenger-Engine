//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "RayTracingConfig.hlsli"
#include "Lighting.hlsli"

//=========================================================================================
// static variables
//=========================================================================================

static const float pi_v = 3.14159265358979323846;

////////////////////////////////////////////////////////////////////////////////////////////
// RaytracingAccelerationStructure
////////////////////////////////////////////////////////////////////////////////////////////
RaytracingAccelerationStructure gRtScene : register(t0); // global

////////////////////////////////////////////////////////////////////////////////////////////
// TraceRay methods
////////////////////////////////////////////////////////////////////////////////////////////

void TraceRay(in RayDesc desc, inout Payload payload) {
	TraceRay(
		gRtScene,
		flags,
		rayMask,
		0, // rayIndex
		1,
		0,
		desc,
		payload
	);
}

//=========================================================================================
// other global Buffers
//=========================================================================================
// camera
struct Camera {
	float4x4 world;
	float4x4 projInv;
};
ConstantBuffer<Camera> gCamera : register(b0); // global

// light
struct Light {
	float4 color;
	float4 position;
	float3 direction;
	float intensity;
	float range;
	float decay;
	
	int type; // lightType direction = 0, point = 1
};
ConstantBuffer<Light> gLight : register(b1);

namespace LIGHT {
	static const int DIRECTION = 0;
	static const int POINT     = 1;
}

//=========================================================================================
// local Buffers
//=========================================================================================
RWTexture2D<float4> gOutput : register(u0); // raygeneration

StructuredBuffer<uint> sIndexBuffer : register(t1);
StructuredBuffer<Vertex> sVertexBuffer : register(t2); // Hitgroups all use

Texture2D<float4> gTexture : register(t4); // Hitgroup(ground, cube)
Texture2D<float4> gNormalMap : register(t5); // Hitgroup(cube)
Texture2D<float4> gAmbientOcclusionMap : register(t6); // Hitgroup(cube)
SamplerState gSampler : register(s0);

struct SubobjectMaterial {
	float4 color;
	float4 shadowColor;
	float4 aoColor;
	int flags;
	int alphaRayType;
	float aoRange;
	float aoLimit;
};
ConstantBuffer<SubobjectMaterial> gSubobjectMaterial : register(b2); // subobject hitgroup

namespace SUBOBJECTFLAG {
	static const int LAMBERT      = 1 << 0,
	                 ALPHARAY     = 1 << 1,
	                 ALPHALAMBERT = 1 << 2,
	                 PHONG        = 1 << 3,
	                 SHADOW       = 1 << 4,
	                 AO           = 1 << 5;
}

namespace ALPHARAY {
	static const int PENETRATION = 0,
	                 REFLECTION  = 1;
}

////////////////////////////////////////////////////////////////////////////////////////////
// hitgroup methods
////////////////////////////////////////////////////////////////////////////////////////////

Vertex GetHitVertex(MyAttribute attrib) {
	
	Vertex result = (Vertex)0;
	float3 barycentrics = CalcBarycentrics(attrib.barys);
	uint vertexId = PrimitiveIndex() * 3;
	
	float weights[3] = {
		barycentrics.x, barycentrics.y, barycentrics.z
	};
	
	for (int i = 0; i < 3; ++i) {
		uint index = sIndexBuffer[vertexId + i];
		float w = weights[i];
		
		result.position += sVertexBuffer[index].position * w;
		result.texcoord += sVertexBuffer[index].texcoord * w;
		result.normal += sVertexBuffer[index].normal * w;
	}
	
	result.normal = normalize(result.normal);
	
	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////
// ReflectionMethods methods
////////////////////////////////////////////////////////////////////////////////////////////

//! @brief reflection check
//!
//! @retval true  faild: collisionRay || reflection count limit count over
//! @retval false ok
bool CheckReflection(inout Payload payload, float4 collisionColor = float4(0, 0, 0, 1)) {
	
	payload.isCollision = true;
	payload.length      = RayTCurrent();
	
	if (payload.rayType == RAYTYPE::COLLISION) {
		payload.color = collisionColor;
		return true;
	}
	
	if (payload.reflectionCount > kLimitReflectionCount) {
		
		// initialize payload
		payload.color       = float4(0, 0, 0, 0);
		payload.isCollision = false;
		payload.length      = 0.0f;
		
		return true;
	}
	
	payload.reflectionCount++;

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Blend methods
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
// Lighting methods
////////////////////////////////////////////////////////////////////////////////////////////

float4 CalculateLightColor(float4 defaultColor, float3 worldPos, float3 worldNormal, bool isUseBlinnPhong = false) {
	
	float4 result = defaultColor;
	
	if (gLight.type == LIGHT::DIRECTION) {
		
		result.rgb *= HalfLambertReflection(worldNormal, -gLight.direction) * gLight.color.rgb * gLight.intensity;
		
		if (isUseBlinnPhong) {
			result.rgb += BlinnPhong(worldPos, worldNormal, -gLight.direction, 50.0f);
		}
		
	} else if (gLight.type == LIGHT::POINT) {
		
		float3 direction = normalize(worldPos.xyz - gLight.position.xyz);
		float distance = length(worldPos.xyz - gLight.position.xyz);
		float factor = pow(saturate(-distance / gLight.range + 1.0f), gLight.decay);
		
		result.rgb *= HalfLambertReflection(worldNormal, -direction) * gLight.color.rgb * gLight.intensity * factor;
		
		if (isUseBlinnPhong) {
			result.rgb += BlinnPhong(worldPos, worldNormal, -direction, 50.0f) * factor;
		}
	}
	
	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Ray methods
////////////////////////////////////////////////////////////////////////////////////////////

float4 TraceShadowRay(float4 currentColor, float3 worldPos, uint reflectionCount) {
	
	float4 result = currentColor;
	
	// world to light direction
	float3 direction = -gLight.direction;
	float tMax = 10000;
	
	if (gLight.type == LIGHT::POINT) {
		direction = normalize(gLight.position.xyz - worldPos);
		tMax      = length(gLight.position.xyz - worldPos);
	}
	
	// shadow
	Payload shadowRay;
	shadowRay.rayType         = RAYTYPE::COLLISION;
	shadowRay.reflectionCount = reflectionCount;
	shadowRay.color           = float4(0, 0, 0, 0);
	shadowRay.isCollision     = false;
	shadowRay.length          = 0.0f;
	
	//!< this direction light
	RayDesc desc;
	desc.Origin    = worldPos;
	desc.Direction = direction;
	desc.TMin      = kTMin;
	desc.TMax      = tMax;
	
	TraceRay(
		desc,
		shadowRay
	);
	
	if (shadowRay.isCollision) {
		if (gLight.type == LIGHT::POINT) {
			float factor = pow(saturate(-tMax / gLight.range + 1.0f), gLight.decay);
			shadowRay.color *= factor;
		}
		
		result = AlphaBlend(float4(shadowRay.color.rgb, saturate(shadowRay.color.a - 0.5f)), currentColor);
	}
	
	return result;
}

float4 TraceCutoffRay(float4 currentColor, float3 worldPos, float3 worldNormal, uint reflectionCount) {
	
	// world to light direction
	// direction
	float3 direction = -gLight.direction;
	float tMax = 10000;
	
	if (gLight.type == LIGHT::POINT) {
		// point
		direction = normalize(gLight.position.xyz - worldPos);
		tMax = length(gLight.position.xyz - worldPos);
	}
	
	Payload cutoffRay;
	cutoffRay.rayType         = RAYTYPE::COLLISION;
	cutoffRay.color           = float4(0, 0, 0, 0);
	cutoffRay.isCollision     = false;
	cutoffRay.length          = 0;
	cutoffRay.reflectionCount = reflectionCount;
	
	RayDesc desc;
	desc.Origin    = worldPos - WorldRayDirection() * kTMin;
	desc.Direction = direction;
	desc.TMin      = kTMin;
	desc.TMax      = tMax;
	
	TraceRay(
		desc, cutoffRay
	);
	
	float4 resultColor;
	
	if (cutoffRay.isCollision) {
		resultColor = float4(0, 0, 0, 1);

	} else {
		// not hit other object -> path light
		resultColor = CalculateLightColor(currentColor, worldPos, worldNormal);
	}
	
	return resultColor;
}

float4 TraceAmbientOcclusionRay(float4 currentCulor, float3 worldPos, float3 worldNormal, uint subdivision, float range, uint reflectionCount) {
	
	if (all(currentCulor.rgba == 0.0f)) {
		return currentCulor;
	}
	
	float3 u;
	
	if (all(worldNormal.xy == 0.0f)) {
		u = float3(1.0f, 0.0f, 0.0f);
			
	} else {
		u = normalize(float3(-worldNormal.y, worldNormal.x, 0.0f));
	}
	
	float3 v = normalize(cross(worldNormal, u));
	
	uint hitCount = 0;
	
	// ambient occlusion ray
	for (uint lat = 0; lat < subdivision; ++lat) {
		float theta = (lat / float(subdivision)) * (pi_v / 2.0f);
			
		for (uint lon = 0; lon < subdivision; ++lon) {
			float phi = (lon / float(subdivision)) * (2 * pi_v);
			
			float x = sin(theta) * cos(phi);
			float y = sin(theta) * sin(phi);
			float z = cos(theta);
			
			float3 direction = u * x + v * y + worldNormal * z;
			
			// raysetting
			Payload aoRay;
			aoRay.rayType         = RAYTYPE::COLLISION;
			aoRay.color           = float4(0, 0, 0, 0);
			aoRay.isCollision     = false;
			aoRay.length          = 0;
			aoRay.reflectionCount = reflectionCount;
			
			RayDesc desc;
			desc.Origin    = worldPos + worldNormal * kTMin;
			desc.Direction = direction;
			desc.TMin      = kTMin;
			desc.TMax      = range;
			
			TraceRay(
				desc, aoRay
			);
			
			if (aoRay.isCollision) {
				hitCount++;
			}

		}
	}
	
	// setting color
	float4 ambientColor = float4(0, 0, 0, 0);
	ambientColor.a = saturate(float(hitCount) / (float(subdivision * subdivision) / 1.2f)); // HACK: user select ambient power
	
	float4 result = AlphaBlend(ambientColor, currentCulor);
	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Texture methods
////////////////////////////////////////////////////////////////////////////////////////////

float3 GetNormalFormTexture(float2 texcoord) {
	
	float3 normalSample = gNormalMap.SampleLevel(gSampler, texcoord, 0.0f).rgb;
	return normalize(normalSample * 2.0f - 1.0f);
	
}

////////////////////////////////////////////////////////////////////////////////////////////
// raygeneration shader
////////////////////////////////////////////////////////////////////////////////////////////

[shader("raygeneration")]
void mainRayGen() {
	
	uint2 launchIndex = DispatchRaysIndex().xy;
	float2 dims       = float2(DispatchRaysDimensions().xy);
	
	float2 d      = (launchIndex.xy + 0.5f) / dims.xy * 2.0f - 1.0f;
	float aspect  = dims.x / dims.y;
	
	RayDesc rayDesc;
	rayDesc.Origin = mul(gCamera.world, float4(0, 0, 0, 1)).xyz;
	
	float4 target     = mul(gCamera.projInv, float4(d.x, -d.y, 1, 1));
	rayDesc.Direction = mul(gCamera.world, float4(target.xyz, 0)).xyz;
	
	rayDesc.TMin = kTMin;
	rayDesc.TMax = 100000;
	
	Payload payload;
	payload.rayType         = RAYTYPE::VIEW;
	payload.reflectionCount = 0;
	payload.color           = float4(0, 0, 0, 0);
	payload.isCollision     = false;
	payload.length          = 0.0f;
	
	TraceRay(
		rayDesc,
		payload
	);
	
	gOutput[launchIndex.xy] = payload.color;
	
}

////////////////////////////////////////////////////////////////////////////////////////////
// miss shader
////////////////////////////////////////////////////////////////////////////////////////////

[shader("miss")]
void mainMS(inout Payload payload) {
	
	payload.length      = 0.0f;
	payload.isCollision = false;
	
	//if (payload.rayType != RAYTYPE::VIEW) {
	//	payload.color = float4(0, 0, 0, 0);
	//	return;
	//}
	
	// clearValue
	payload.color = float4(0.0f, 0.15f, 0.4f, 1.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////
// closesthit shader
////////////////////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------------
// ground
//-----------------------------------------------------------------------------------------
[shader("closesthit")]
void mainGroundCHS(inout Payload payload, in MyAttribute attrib) {
	
	if (CheckReflection(payload)) {
		return;
	}
	
	// hit vertex
	Vertex vtx = GetHitVertex(attrib);
	float3 worldPosition = mul(vtx.position, ObjectToWorld4x3());
	float3 worldNormal = normalize(mul(vtx.normal, (float3x3)ObjectToWorld4x3()));
	
	float4 resultColor = gTexture.SampleLevel(gSampler, vtx.texcoord * 40.0f, 0.0f);
	
	// lighting
	resultColor = CalculateLightColor(resultColor, worldPosition, worldNormal);
	
	if (payload.rayType == RAYTYPE::REFLECTION) {
		payload.color  = resultColor;
		payload.length = RayTCurrent();
		return;
	}
	
	// reflection
	{
		Payload reflectionRay;
		reflectionRay.rayType         = RAYTYPE::REFLECTION;
		reflectionRay.color           = float4(0, 0, 0, 0);
		reflectionRay.isCollision     = false;
		reflectionRay.reflectionCount = payload.reflectionCount;
		
		const float tMax = 5.0f;
		
		RayDesc desc;
		desc.Origin    = worldPosition;
		desc.Direction = reflect(WorldRayDirection(), worldNormal);
		desc.TMin      = kTMin;
		desc.TMax      = tMax;
		
		TraceRay(
			desc,
			reflectionRay
		);
		
		if (reflectionRay.isCollision) {
			
			reflectionRay.color.a
				= pow((1.0f - (reflectionRay.length / tMax)), 2.0f) * reflectionRay.color.a * 0.5f;
			
			resultColor = AlphaBlend(reflectionRay.color, resultColor);
		}
	}
	
	resultColor = TraceShadowRay(resultColor, worldPosition, payload.reflectionCount);
	
	payload.color = resultColor;
	payload.length = RayTCurrent();
}

//-----------------------------------------------------------------------------------------
// teapot
//-----------------------------------------------------------------------------------------
[shader("closesthit")]
void mainTeapotCHS(inout Payload payload, in MyAttribute attrib) {
	
	if (CheckReflection(payload)) {
		return;
	}
	
	// hit vertex
	Vertex vtx = GetHitVertex(attrib);
	float3 worldPosition = mul(vtx.position, ObjectToWorld4x3());
	float3 worldNormal = normalize(mul(vtx.normal, (float3x3)ObjectToWorld4x3()));
	
	const float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float4 resultColor = color;
	
	// lighting
	resultColor = CalculateLightColor(resultColor, worldPosition, worldNormal);

	payload.color = resultColor;
}

//-----------------------------------------------------------------------------------------
// player
//-----------------------------------------------------------------------------------------
[shader("closesthit")]
void mainPlayerCHS(inout Payload payload, in MyAttribute attrib) {
	
	if (CheckReflection(payload)) {
		return;
	}
	
	float4 resultColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
	
	Vertex vtx = GetHitVertex(attrib);
	float3 worldPosition = mul(vtx.position, ObjectToWorld4x3());
	float3 worldNormal = normalize(mul(vtx.normal, (float3x3)ObjectToWorld4x3()));
	
	// lighting
	resultColor = CalculateLightColor(resultColor, worldPosition, worldNormal);
	
	payload.color = resultColor;
	return;
}

//-----------------------------------------------------------------------------------------
// cube
//-----------------------------------------------------------------------------------------
[shader("closesthit")]
void mainCubeCHS(inout Payload payload, in MyAttribute attrib) {
	
	if (CheckReflection(payload)) {
		return;
	}
	
	// hit vertex
	Vertex vtx = GetHitVertex(attrib);
	float3 worldPosition = mul(vtx.position, ObjectToWorld4x3());
	float3 worldNormal = normalize(mul(vtx.normal, (float3x3)ObjectToWorld4x3()));
	
	float4 resultColor = float4(1, 1, 1, 1);
	
	// textures sampling
	resultColor = gTexture.SampleLevel(gSampler, vtx.texcoord, 0.0f);
	float3 texNormal = normalize(gNormalMap.SampleLevel(gSampler, vtx.texcoord, 0.0f).rgb * 2.0f - 1.0f);
	float ambientOcclusion = gAmbientOcclusionMap.SampleLevel(gSampler, vtx.texcoord, 0.0f).r;
		
	// directional light parameter
	float3 toLightDirection = -gLight.direction;
	float factor            = 1.0f;
	
	if (gLight.type == LIGHT::POINT) { //!< fixed lighting parameter
		
		// direction
		toLightDirection = normalize(gLight.position.xyz - worldPosition.xyz);
		
		// factor
		float distance = length(worldPosition.xyz - gLight.position.xyz);
		factor = pow(saturate(-distance / gLight.range + 1.0f), gLight.decay);
	}
	
	resultColor.rgb *= HalfLambertReflection(texNormal, toLightDirection) * gLight.color.rgb * gLight.intensity * factor;
	resultColor.rgb *= ambientOcclusion;
	
	payload.color = resultColor;
}

//-----------------------------------------------------------------------------------------
// subobject
//-----------------------------------------------------------------------------------------
[shader("closesthit")]
void mainSubobjectCHS(inout Payload payload, in MyAttribute attrib) {
	
	if (CheckReflection(payload)) {
		return;
	}
	
	float4 resultColor = gSubobjectMaterial.color;
	
	// coordinate: barycentrics -> local -> world
	Vertex vtx           = GetHitVertex(attrib);
	float3 worldPosition = mul(vtx.position, ObjectToWorld4x3());
	float3 worldNormal   = normalize(mul(vtx.normal, (float3x3)ObjectToWorld4x3()));
	
	// lambert
	if (gSubobjectMaterial.flags & SUBOBJECTFLAG::LAMBERT) {
		
		// directional light parameter
		float3 toLightDirection = -gLight.direction;
		float factor            = 1.0f;
		
		if (gLight.type == LIGHT::POINT) { //!< fixed lighting parameter
			
			// direction
			toLightDirection = normalize(gLight.position.xyz - worldPosition.xyz);
			
			// factor
			float distance = length(worldPosition.xyz - gLight.position.xyz);
			factor = pow(saturate(-distance / gLight.range + 1.0f), gLight.decay);
		}
		
		resultColor.rgb *= HalfLambertReflection(worldNormal, toLightDirection) * gLight.color.rgb * gLight.intensity * factor;
	}
	
	// alphaRay
	// FIXME: anyhit shader (.a == 0.0f)
	if ((gSubobjectMaterial.flags & SUBOBJECTFLAG::ALPHARAY) && (resultColor.a != 1.0f)) {
		
		// penetration ray direction
		float3 direction = WorldRayDirection();
		
		if (gSubobjectMaterial.alphaRayType == ALPHARAY::REFLECTION) {
			// reflection ray direction
			direction = reflect(WorldRayDirection(), worldNormal);
		}
		
		Payload reflectionRay;
		reflectionRay.rayType         = RAYTYPE::REFLECTION;
		reflectionRay.color           = float4(0, 0, 0, 0);
		reflectionRay.reflectionCount = payload.reflectionCount;
		reflectionRay.isCollision     = false;
		reflectionRay.length          = 0;
		
		RayDesc desc;
		desc.Origin    = worldPosition;
		desc.Direction = direction;
		desc.TMin      = kTMin;
		desc.TMax      = 10000;
		
		TraceRay(
			desc, reflectionRay
		);
		
		resultColor = AlphaBlend(resultColor, reflectionRay.color);
		
	}
	
	// alpha ray after lambert
	if (gSubobjectMaterial.flags & SUBOBJECTFLAG::ALPHALAMBERT) {
		
		// directional light parameter
		float3 toLightDirection = -gLight.direction;
		float factor            = 1.0f;
		
		if (gLight.type == LIGHT::POINT) { //!< fixed lighting parameter
			
			// direction
			toLightDirection = normalize(gLight.position.xyz - worldPosition.xyz);
			
			// factor
			float distance = length(worldPosition.xyz - gLight.position.xyz);
			factor = pow(saturate(-distance / gLight.range + 1.0f), gLight.decay);
		}
		
		resultColor.rgb *= HalfLambertReflection(worldNormal, toLightDirection) * gLight.color.rgb * gLight.intensity * factor;
	}
	
	// phong
	if (gSubobjectMaterial.flags & SUBOBJECTFLAG::PHONG) {
		
		// directional light parameter
		float3 toLightDirection = -gLight.direction;
		float factor = 1.0f;
		
		if (gLight.type == LIGHT::POINT) { //!< fixed lighting parameter
			
			// direction
			toLightDirection = normalize(gLight.position.xyz - worldPosition.xyz);
			
			// factor
			float distance = length(worldPosition.xyz - gLight.position.xyz);
			factor = pow(saturate(-distance / gLight.range + 1.0f), gLight.decay);
		}
		
		resultColor.rgb += BlinnPhong(worldPosition, worldNormal, toLightDirection, 50.0f) * factor;
		resultColor.rgb = saturate(resultColor.rgb); //!< clamp 0.0f ~ 1.0f
	}
	
	// shadow
	if (gSubobjectMaterial.flags & SUBOBJECTFLAG::SHADOW) {
		
		// setting parameter //
		// directionalLight
		float3 toLightDirection = -gLight.direction;
		float  toLightLength     = 10000;
		
		if (gLight.type == LIGHT::POINT) { //!< fix point light parameter
			toLightDirection = normalize(gLight.position.xyz - worldPosition.xyz);
			toLightLength    = length(worldPosition.xyz - gLight.position.xyz);
		}
		
		Payload collisionRay;
		collisionRay.rayType         = RAYTYPE::COLLISION;
		collisionRay.color           = float4(0, 0, 0, 0);
		collisionRay.length          = 0;
		collisionRay.isCollision     = false;
		collisionRay.reflectionCount = payload.reflectionCount;
		
		RayDesc desc;
		desc.Origin    = worldPosition;
		desc.Direction = toLightDirection;
		desc.TMin      = kTMin;
		desc.TMax      = toLightLength;
		
		TraceRay(
			desc, collisionRay
		);
		
		if (collisionRay.isCollision) {
			resultColor = AlphaBlend(gSubobjectMaterial.shadowColor, resultColor);
		}
	}
	
	// return to payload in reflection ray
	if (payload.rayType == RAYTYPE::REFLECTION) {
		payload.color = resultColor;
		return;
	}
	
	// ambientOcclusion
	if (gSubobjectMaterial.flags & SUBOBJECTFLAG::AO) {
		
		// normal base vector
		float3 u;
		float3 v;
	
		if (all(worldNormal.xy == 0.0f)) {
			u = float3(1.0f, 0.0f, 0.0f);
			
		} else {
			u = normalize(float3(-worldNormal.y, worldNormal.x, 0.0f));
		}
	
		v = normalize(cross(worldNormal, u));
		
		// parameter //
		const uint kSubdivision = 8;
		const float range       = gSubobjectMaterial.aoRange;
		
		uint hitRayCount = 0;
	
		// ambient occlusion ray
		for (uint lat = 0; lat < kSubdivision; ++lat) {
			float theta = (lat / float(kSubdivision)) * (pi_v / 2.0f);
				
			for (uint lon = 0; lon < kSubdivision; ++lon) {
				float phi = (lon / float(kSubdivision)) * (2 * pi_v);
				
				float x = sin(theta) * cos(phi);
				float y = sin(theta) * sin(phi);
				float z = cos(theta);
				
				float3 direction = u * x + v * y + worldNormal * z;
				
				// raysetting
				Payload aoRay;
				aoRay.rayType         = RAYTYPE::COLLISION;
				aoRay.color           = float4(0, 0, 0, 0);
				aoRay.isCollision     = false;
				aoRay.length          = 0;
				aoRay.reflectionCount = payload.reflectionCount;
				
				RayDesc desc;
				desc.Origin    = worldPosition + worldNormal * kTMin;
				desc.Direction = direction;
				desc.TMin      = kTMin;
				desc.TMax      = range;
				
				TraceRay(
					desc, aoRay
				);
				
				if (aoRay.isCollision) {
					hitRayCount++;
				}
			}
		}
		
		float4 aoColor = gSubobjectMaterial.aoColor;
		aoColor.a = saturate(float(hitRayCount) / gSubobjectMaterial.aoLimit) * aoColor.a;
		
		resultColor = AlphaBlend(aoColor, resultColor);
	}
	
	payload.color = resultColor;
	return;
}