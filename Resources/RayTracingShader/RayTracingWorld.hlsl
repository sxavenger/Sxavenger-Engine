////////////////////////////////////////////////////////////////////////////////////////////
// config structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Payload {
	int rayType;
	uint reflectionCount;
	float4 color;
	int isCollision;
	float length; // origin to hit position
};

namespace RAYTYPE {
	static int VIEW       = 0;
	static int REFLECTION = 1;
	static int COLLISION  = 2;
}

struct MyAttribute {
	float2 barys;
};

// raysettings //
static const RAY_FLAG flags = RAY_FLAG_CULL_BACK_FACING_TRIANGLES; // culling back
static const uint rayMask = 0xFF;

static const float kTMin = 0.0001f;

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

// glassMaterial
struct GlassMaterial {
	float4 color;
};
StructuredBuffer<GlassMaterial> sMaterials : register(t3);

// groundTexture
Texture2D<float4> gGroundTexture : register(t4);
SamplerState gSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// Vertex structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Vertex {
	float4 position;
	float2 texcoord;
	float3 normal;
};

//=========================================================================================
// local Buffers
//=========================================================================================
RWTexture2D<float4> gOutput : register(u0); // raygeneration

StructuredBuffer<uint> sIndexBuffer : register(t1);
StructuredBuffer<Vertex> sVertexBuffer : register(t2); // Hitgroups

//=========================================================================================
// static const variables
//=========================================================================================

static const uint kLimitReflectionCount = 10;

////////////////////////////////////////////////////////////////////////////////////////////
// hitgroup methods
////////////////////////////////////////////////////////////////////////////////////////////

inline float3 CalcBarycentrics(float2 barys) {
	return float3(
		1.0f - barys.x - barys.y,
		barys.x,
		barys.y
	);
}

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
		result.normal   += sVertexBuffer[index].normal * w;
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
bool CheckReflection(inout Payload payload, in float4 collisionColor = float4(0, 0, 0, 1)) {
	
	payload.isCollision = true;
	
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

float HalfLambertReflection(float3 normal, float3 lightDirection, float exponent = 2.0f) {

	float NdotL = dot(normal, -lightDirection);
	float result = pow(NdotL * 0.5f + 0.5f, exponent);
	
	return result;

}

float3 BlinnPhong(float3 worldPos, float3 normal, float3 lightDirection, float specPow, float4 specColor = float4(1, 1, 1, 1)) {
	
	float3 halfVector = normalize(-lightDirection - WorldRayDirection());
	float NdotH = max(0, dot(normal, halfVector));
	float spec = pow(NdotH, specPow);
	
	return specColor.rgb * spec;
	
}

float4 CalculateLightColor(float4 defaultColor, float3 worldPos, float3 worldNormal, bool isUseBlinnPhong = false) {
	
	float4 result = defaultColor;
	
	if (gLight.type == LIGHT::DIRECTION) {
		
		result.rgb *= HalfLambertReflection(worldNormal, gLight.direction) * gLight.color.rgb * gLight.intensity;
		
		if (isUseBlinnPhong) {
			result.rgb += BlinnPhong(worldPos, worldNormal, gLight.direction, 50.0f);
		}
		
	} else if (gLight.type == LIGHT::POINT) {
		
		float3 direction = normalize(worldPos.xyz - gLight.position.xyz);
		float distance = length(worldPos.xyz - gLight.position.xyz);
		float factor = pow(saturate(-distance / gLight.range + 1.0f), gLight.decay);
		
		result.rgb *= HalfLambertReflection(worldNormal, direction) * gLight.color.rgb * gLight.intensity * factor;
		
		if (isUseBlinnPhong) {
			result.rgb += BlinnPhong(worldPos, worldNormal, direction, 50.0f) * factor;
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
	
	if (payload.rayType != RAYTYPE::VIEW) {
		payload.color       = float4(0.0f, 0.0f, 0.0f, 0.0f);
		payload.length      = 0.0f;
		payload.isCollision = false;
	}
	
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
	
	float4 resultColor = gGroundTexture.SampleLevel(gSampler, vtx.texcoord * 40.0f, 0.0f);
	
	// lighting
	resultColor = CalculateLightColor(resultColor, worldPosition, worldNormal);
	
	if (payload.rayType == RAYTYPE::COLLISION) {
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

	payload.color  = resultColor;
	payload.length = RayTCurrent();
}

//-----------------------------------------------------------------------------------------
// glass
//-----------------------------------------------------------------------------------------
[shader("closesthit")]
void mainGlassCHS(inout Payload payload, in MyAttribute attrib) {
	
	float4 resultColor = sMaterials[InstanceID()].color;
	
	if (CheckReflection(payload, resultColor)) {
		return;
	}
	
	// hit vertex
	Vertex vtx = GetHitVertex(attrib);
	float3 worldPosition = mul(vtx.position, ObjectToWorld4x3());
	float3 worldNormal = normalize(mul(vtx.normal, (float3x3)ObjectToWorld4x3()));
	
	if (payload.rayType == RAYTYPE::REFLECTION) {
		payload.color  = resultColor;
		payload.length = RayTCurrent();
		return;
	}

	if (payload.color.a != 1.0f) {
		
		// payload init
		Payload alphaPayload;
		alphaPayload.rayType         = RAYTYPE::REFLECTION;
		alphaPayload.color           = float4(0, 0, 0, 0);
		alphaPayload.reflectionCount = payload.reflectionCount;
		
		// desc
		RayDesc desc;
		desc.Origin    = worldPosition;
		desc.Direction = WorldRayDirection();
		desc.TMin      = kTMin;
		desc.TMax      = 10000;
	
		TraceRay(
			desc,
			alphaPayload
		);
		
		resultColor = AlphaBlend(resultColor, alphaPayload.color);
	}
	
	payload.color = resultColor;
}

//-----------------------------------------------------------------------------------------
// bunny
//-----------------------------------------------------------------------------------------
[shader("closesthit")]
void mainBunnyCHS(inout Payload payload, in MyAttribute attrib) {
	
	if (CheckReflection(payload)) {
		return;
	}
	
	Vertex vtx = GetHitVertex(attrib);
	float3 worldPosition = mul(vtx.position, ObjectToWorld4x3());
	float3 worldNormal = normalize(mul(vtx.normal, (float3x3)ObjectToWorld4x3()));
	
	float4 resultColor = float4(1.0f, 1.0f, 1.0f, 0.1f);
	
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
		
		resultColor = AlphaBlend(resultColor, reflectionRay.color);
	}
	
	
	// lighting
	resultColor = CalculateLightColor(resultColor, worldPosition, worldNormal, true);
	
	payload.color  = saturate(resultColor);
	payload.length = RayTCurrent();
	return;
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
	payload.length = RayTCurrent();
	return;
}