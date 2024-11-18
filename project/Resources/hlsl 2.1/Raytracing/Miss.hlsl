//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "RaytracingCommon.hlsli"
#include "../MathLib.hlsli"
//* test atmosphric scattering

//=========================================================================================
// LocalBuffer
//=========================================================================================

struct AtmosphericScattering {
	float4 waveLength;
	float3 lightDir;
	float innerRadius;
	float outerRadius;
	float kr;
	float km;
	float eSun;
	float g;
	float scaleDepth;
	float exposure;
};
ConstantBuffer<AtmosphericScattering> gAtmosphetic : register(b0);

//=========================================================================================
// static variables
//=========================================================================================

static const float3 kInvWavelength = 1.0f / pow(gAtmosphetic.waveLength.rgb, 4.0f);

static const float kInnerRadiusPow2 = pow(gAtmosphetic.innerRadius, 2.0f);
static const float kOuterRadiusPow2 = pow(gAtmosphetic.outerRadius, 2.0f);

static const float kScale               = 1.0f / (gAtmosphetic.outerRadius - gAtmosphetic.innerRadius);
static const float kScaleOverScaleDepth = kScale / gAtmosphetic.scaleDepth;

static const float kKrESun = gAtmosphetic.kr * gAtmosphetic.eSun;
static const float kKmESun = gAtmosphetic.km * gAtmosphetic.eSun;
static const float kKr4pi  = gAtmosphetic.kr * pi_v * 4.0f;
static const float kKm4pi  = gAtmosphetic.km * pi_v * 4.0f;

static const float kG2 = gAtmosphetic.g * gAtmosphetic.g;

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float3 IntersectionPos(float3 rayPos, float3 rayDir, float sphereRadius) {
	const float A = dot(rayDir, rayDir);
	const float B = 2.0 * dot(rayPos, rayDir);
	const float C = dot(rayPos, rayPos) - sphereRadius * sphereRadius;

	float discriminant = B * B - 4.0 * A * C;
	if (discriminant < 0.0f)
		return float3(0, 0, 0); // 交差なし

	float t = (-B - sqrt(discriminant)) / (2.0 * A);
	return rayPos + rayDir * t;
}

float IntegralApproximation(float fCos) {
	float x = 1.0 - fCos;
	return gAtmosphetic.scaleDepth * exp(-0.00287 + x * (0.459 + x * (3.83 + x * (-6.80 + x * 5.25))));
}

////////////////////////////////////////////////////////////////////////////////////////////
// entry point miss
////////////////////////////////////////////////////////////////////////////////////////////
[shader("miss")]
void mainMiss(inout Payload payload) {
	//payload.color = float4(0.227f, 0.313f, 0.294f, 1.0f);

	payload.SetIntersection(false);

	if (payload.rayType == RayType::kRayType_Intersection) {
		payload.isIntersection = false;
		return;
	}

	float3 rayOrign = WorldRayOrigin();
	float3 rayDirection = WorldRayDirection();

	float3 skyPos = IntersectionPos(rayOrign, rayDirection, gAtmosphetic.outerRadius);
	if (skyPos.x == 0.0f) {
		// カメラが大気の外
		payload.color = (float4)0;
		return;
	}

	float3 ray = skyPos - rayOrign;
	float far = length(skyPos - rayOrign);
	ray /= far;

	
	float height      = length(rayOrign);
	float depth       = exp(kScaleOverScaleDepth * (gAtmosphetic.innerRadius - height));
	float startAngle  = dot(ray, rayOrign) / height;
	float startOffset = depth * IntegralApproximation(startAngle);

	const int kSamples = 5;

	float sampleLength = far / float(kSamples);
	float scaledLength = sampleLength * kScale;
	float3 sampleRay   = ray * sampleLength;
	float3 samplePoint = rayOrign + sampleRay * 0.5f;

	float3 frontColor = (float3)0;

	for (int i = 0; i < kSamples; ++i) {
		float fHeight      = length(samplePoint);
		float fDepth       = exp(kScaleOverScaleDepth * (gAtmosphetic.innerRadius - fHeight));
		
		float fLightAngle = dot(gAtmosphetic.lightDir, samplePoint) / fHeight;
		float fCameraAngle = dot(ray, samplePoint) / fHeight;

		float fScatter = (startOffset + fDepth * (IntegralApproximation(fLightAngle) - IntegralApproximation(fCameraAngle)));

		float3 attenuate = exp(-fScatter * (kInvWavelength * kKr4pi + kKm4pi));
		frontColor      += attenuate * (fDepth * scaledLength);
		samplePoint     += sampleRay;
	}

	float4 primaryColor   = float4(frontColor * (kInvWavelength * kKrESun), 1.0f);
	float4 secondaryColor = float4(frontColor * kKmESun, 1.0f);
	
	float3 direction = normalize(rayOrign - skyPos);

	float fCos = dot(gAtmosphetic.lightDir.xyz, direction) / length(direction);
	float rayPhase = 0.75f * (1.0f + fCos * fCos);

	float minPhase = 1.5f * ((1.0f - kG2) / (2.0f + kG2)) * (1.0f + fCos * fCos) / pow(abs(1.0f + kG2 - 2.0f * gAtmosphetic.g * fCos), 1.5f);

	float3 raycolor = (primaryColor * rayPhase).rgb;
	float3 miecolor = (secondaryColor * minPhase).rgb;

	float3 color = float3(1.0f, 1.0f, 1.0f) - exp(-gAtmosphetic.exposure * (raycolor + miecolor));
	payload.color = float4(saturate(color), 1.0f);

}

