////////////////////////////////////////////////////////////////////////////////////////////
// Vertex structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Vertex {
	float3 position;
	float2 texcoord;
	float3 normal;
};

////////////////////////////////////////////////////////////////////////////////////////////
// RaytracingAccelerationStructure
////////////////////////////////////////////////////////////////////////////////////////////
RaytracingAccelerationStructure gRtScene : register(t0); // global

//=========================================================================================
// Camera constant buffer
//=========================================================================================
struct Camera {
	float4x4 world;
	float4x4 projInv;
};
ConstantBuffer<Camera> gCamera : register(b0); // global

struct DirectionalLigth {
	float4 color;
	float3 direction;
	float intensity;
};
ConstantBuffer<DirectionalLigth> gLight : register(b1);

//=========================================================================================
// Buffers
//=========================================================================================
RWTexture2D<float4> gOutput : register(u0); // local (for RayGen)

StructuredBuffer<uint>   sIndexBuffer : register(t1);
StructuredBuffer<Vertex> sVertexBuffer : register(t2); // local (for HitGroup)

////////////////////////////////////////////////////////////////////////////////////////////
// structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Payload {
	float3 color;
};

struct MyAttribute {
	float2 barys;
};

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
	
	float4 target     = mul(gCamera.projInv, float4(d.x, d.y, 1, 1));
	rayDesc.Direction = mul(gCamera.world, float4(target.xyz, 0)).xyz;
	
	rayDesc.TMin = 0;
	rayDesc.TMax = 100000;
	
	Payload payload;
	payload.color = float3(0, 0, 0);
	
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
	
	float3 col = payload.color;
	
	gOutput[launchIndex.xy] = float4(col, 1.0f);
	
}

////////////////////////////////////////////////////////////////////////////////////////////
// miss shader
////////////////////////////////////////////////////////////////////////////////////////////
[shader("miss")]
void mainMS(inout Payload payload) {
	//payload.color = float3(0.1f, 0.25f, 0.5f); // default color
	payload.color = float3(0.0f, 0.15f, 0.4f);   // test color
}

////////////////////////////////////////////////////////////////////////////////////////////
// closestHit shader
////////////////////////////////////////////////////////////////////////////////////////////
[shader("closesthit")]
void mainCHS(inout Payload payload, in MyAttribute attrib) {
	Vertex vtx = GetHitVertex(attrib);
	
	float3 defaultColor = float3(1.0f, 1.0f, 1.0f);
	
	float NdotL = dot(mul(vtx.normal, (float3x3)ObjectToWorld4x3()), -gLight.direction);
	float cos = pow(NdotL * 0.5f + 0.5f, 2);
	
	payload.color = defaultColor * cos;
}