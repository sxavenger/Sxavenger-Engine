////////////////////////////////////////////////////////////////////////////////////////////
// Vertex structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Vertex {
	float4 position;
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
	float4 position;
	float3 direction;
	float intensity;
	float range;
	float decay;
	
	int isLightingEnable; // flag
	int isShadowEnable; // flag
};
ConstantBuffer<DirectionalLigth> gLight : register(b1);

struct AmbientOcclusionParam {
	int isAOEnable; // flag
	
	float aoRadius;
	uint subdivision;
};
ConstantBuffer<AmbientOcclusionParam> gAO : register(b2);

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
	int isCollisionRay;
	int isCollision; //!< use Lighting 
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
// methods
////////////////////////////////////////////////////////////////////////////////////////////



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
	payload.color          = float3(0, 0, 0);
	payload.isCollisionRay = false;
	payload.isCollision    = false;
	
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
	payload.isCollision = false;
}

////////////////////////////////////////////////////////////////////////////////////////////
// closestHit shader
////////////////////////////////////////////////////////////////////////////////////////////
[shader("closesthit")]
void mainCHS(inout Payload payload, in MyAttribute attrib) {
	
	if (payload.isCollisionRay) {
		payload.isCollision = true;
		return;
	}
	
	Vertex vtx = GetHitVertex(attrib);
	float3 worldPos = mul(vtx.position.xyz, (float3x3)ObjectToWorld4x3());
	
	float3 resultColor = float3(1.0f, 1.0f, 1.0f);
	
	// point
	float3 direction = normalize(worldPos - gLight.position.xyz);
	float distance   = length(worldPos - gLight.position.xyz);
	
	if (gLight.isLightingEnable) {
		// direction
		//float NdotL = dot(mul(vtx.normal, (float3x3)ObjectToWorld4x3()), -gLight.direction);
		//float cos = pow(NdotL * 0.5f + 0.5f, 2);
		//resultColor *= cos * gLight.color.rgb;
		
		// catoffRay setting
		RayDesc rayDesc;
		rayDesc.Origin    = worldPos + vtx.normal * 0.0001f;
		rayDesc.Direction = -direction;
		rayDesc.TMin      = 0;
		rayDesc.TMax      = distance;
		
		payload.isCollisionRay = true;
		payload.isCollision    = false;
		
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
		
		if (payload.isCollision) {
			resultColor = float3(0.1f, 0.1f, 0.1f);
			
		} else {
			float factor = pow(saturate(-distance / gLight.range + 1.0f), gLight.decay);
			resultColor *= factor * gLight.color.rgb;
		}
	}
	
	if (gLight.isShadowEnable) { //!< ShadowRay
	
		// TraceRay Setting
		RayDesc rayDesc;
		rayDesc.Origin    = worldPos - WorldRayDirection() * 0.0001f;
		rayDesc.Direction = -direction;
		rayDesc.TMin      = 0;
		rayDesc.TMax      = distance;
		
		payload.isCollisionRay = true;
		payload.isCollision    = false;
		
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
		
		
		if (payload.isCollision) {
			resultColor -= float3(0.4f, 0.4f, 0.4f); // shadowColor
			resultColor = saturate(resultColor);
		}
	}
	
	// ambient occlusion
	
	if (gAO.isAOEnable) {
		
		float3 u;
		
		if (all(vtx.normal.xy == 0.0f)) {
			u = float3(1.0f, 0.0f, 0.0f);
			
		} else {
			u = normalize(float3(-vtx.normal.y, vtx.normal.x, 0.0f));
		}
		
		float3 v = normalize(cross(vtx.normal, u));
		
		const float pi = 3.14159265358979323846;
		
		uint hitCount = 0;
		
		// ao ray
		for (uint lat = 0; lat < gAO.subdivision; ++lat) {
			float theta = (lat / float(gAO.subdivision)) * (pi / 2.0f);
			
			for (uint lon = 0; lon < gAO.subdivision; ++lon) {
				float phi = (lon / float(gAO.subdivision)) * (2 * pi);

				float x = sin(theta) * cos(phi);
				float y = sin(theta) * sin(phi);
				float z = cos(theta);
				
				float3 direction = u * x + v * y + vtx.normal * z;
				
				// TraceRay Setting
				RayDesc rayDesc;
				rayDesc.Origin    = worldPos - WorldRayDirection() * 0.0001f;
				rayDesc.Direction = direction;
				rayDesc.TMin      = 0;
				rayDesc.TMax      = gAO.aoRadius;
		
				payload.isCollisionRay = true;
				payload.isCollision    = false;
		
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
				
				if (payload.isCollision) {
					hitCount++;
				}
				
			}
		}
		
		float3 aoColor = (hitCount / float(gAO.subdivision * gAO.subdivision)) * float3(1.0f, 1.0f, 1.0f);
		
		resultColor -= aoColor;
		resultColor = saturate(resultColor);
	
	}
	
	// result
	payload.color = resultColor;
}