////////////////////////////////////////////////////////////////////////////////////////////
// config structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Payload {
	uint reflectionCount;
	float4 color;
	int isCollisionRay;
	int isCollision; //!< use Lighting 
};

struct MyAttribute {
	float2 barys;
};

////////////////////////////////////////////////////////////////////////////////////////////
// RaytracingAccelerationStructure
////////////////////////////////////////////////////////////////////////////////////////////
RaytracingAccelerationStructure gRtScene : register(t0); // global

//=========================================================================================
// other global Buffers
//=========================================================================================
struct Camera {
	float4x4 world;
	float4x4 projInv;
};
ConstantBuffer<Camera> gCamera : register(b0); // global

struct GlassMaterial {
	float4 color;
};
StructuredBuffer<GlassMaterial> gGlassMaterial : register(t3);

struct DirectionalLigth {
	float4 color;
	float4 position;
	float3 direction;
	float intensity;
	float range;
	float decay;
	
	int isLightingEnable; // flag
	int isShadowEnable; // flag
};
ConstantBuffer<DirectionalLigth> gLight : register(b1);

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

static const uint kMaxReflectionCount = 10;

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
// Blend methods
////////////////////////////////////////////////////////////////////////////////////////////

float4 AlphaBlend(float4 colorA, float4 colorB) {
	float4 result;
	
	// alpha
	result.a = colorA.a + colorB.a * (1.0f - colorA.a);
	
	// color
	result.rgb = (colorA.rgb * colorA.a + colorB.rgb * colorB.a * (1.0f - colorA.a)) / result.a;
	
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
	
	rayDesc.TMin = 0;
	rayDesc.TMax = 100000;
	
	Payload payload;
	payload.reflectionCount = 0;
	payload.color           = float4(0, 0, 0, 0);
	payload.isCollisionRay  = false;
	payload.isCollision     = false;
	
	RAY_FLAG flags = RAY_FLAG_NONE;
	uint rayMask = 0xFF;
	
	TraceRay(
		gRtScene,
		flags,
		rayMask,
		0, // rayIndex
		1,
		0,
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
	payload.color = float4(0.0f, 0.15f, 0.4f, 1.0f);
	payload.isCollision = false;
}

////////////////////////////////////////////////////////////////////////////////////////////
// closesthit shader
////////////////////////////////////////////////////////////////////////////////////////////

[shader("closesthit")] // hitgruop: room
void mainRoomCHS(inout Payload payload, in MyAttribute attrib) {
	
	if (payload.isCollisionRay) {
		payload.isCollision = true;
		return;
	}
	
	if (payload.reflectionCount > kMaxReflectionCount) {
		return;
	}
	
	payload.reflectionCount++;
	
	float4 defaultColor = float4(0.2f, 0.2f, 0.2f, 1.0f);
	
	// hit vertex
	Vertex vtx = GetHitVertex(attrib);
	float3 worldPos = mul(vtx.position.xyz, (float3x3)ObjectToWorld4x3());
	
	
	
	payload.color = defaultColor;
	return;

}

[shader("closesthit")] // hitgruop: glass
void mainGlassCHS(inout Payload payload, in MyAttribute attrib) {
	
	if (payload.isCollisionRay) {
		payload.isCollision = true;
	}
	
	if (payload.reflectionCount > kMaxReflectionCount) {
		return;
	}
	
	payload.reflectionCount++;
	
	payload.color = gGlassMaterial[InstanceID()].color;
	
	if (payload.color.a < 1.0f) { // alpha
		
		// hit vertex
		Vertex vtx = GetHitVertex(attrib);
		float3 worldPos = mul(vtx.position.xyz, (float3x3)ObjectToWorld4x3());
		
		RayDesc rayDesc;
		rayDesc.Origin    = worldPos;
		rayDesc.Direction = WorldRayDirection();
		rayDesc.TMin      = 0.0001f;
		rayDesc.TMax      = 10000;
		
		Payload glassPayload;
		glassPayload.color           = (float4)0;
		glassPayload.isCollision     = false;
		glassPayload.isCollisionRay  = false;
		glassPayload.reflectionCount = payload.reflectionCount;

		RAY_FLAG flags = RAY_FLAG_NONE;
		uint rayMask = 0xFF;
	
		TraceRay(
			gRtScene,
			flags,
			rayMask,
			0, // rayIndex
			1,
			0,
			rayDesc,
			glassPayload
		);
		
		payload.color = AlphaBlend(payload.color, glassPayload.color);

	}
	
	return;

}