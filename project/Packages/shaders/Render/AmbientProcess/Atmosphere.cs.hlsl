//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "AmbientProcess.hlsli"
#include "../../Camera.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<float4> gMain : register(u0); //!< main texture

struct Parameter {
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
ConstantBuffer<Parameter> gParameter : register(b0);

ConstantBuffer<Camera> gCamera : register(b1);

//=========================================================================================
// static variables
//=========================================================================================

static const float pi_v = 3.1415926535897932384626433832795f;

static const float3 kInvWavelength = 1.0f / pow(gParameter.waveLength.rgb, 4.0f);

static const float kInnerRadiusPow2 = pow(gParameter.innerRadius, 2.0f);
static const float kOuterRadiusPow2 = pow(gParameter.outerRadius, 2.0f);

static const float kScale               = 1.0f / (gParameter.outerRadius - gParameter.innerRadius);
static const float kScaleOverScaleDepth = kScale / gParameter.scaleDepth;

static const float kKrESun = gParameter.kr * gParameter.eSun;
static const float kKmESun = gParameter.km * gParameter.eSun;
static const float kKr4pi  = gParameter.kr * pi_v * 4.0f;
static const float kKm4pi  = gParameter.km * pi_v * 4.0f;

static const float kG2 = gParameter.g * gParameter.g;

////////////////////////////////////////////////////////////////////////////////////////////
// structures
////////////////////////////////////////////////////////////////////////////////////////////

struct Ray {
	float3 origin;
	float3 direction;
};

struct Sphere {
	float3 center;
	float radius;
};


////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

Ray GenerateRay(float2 uv) {
	
	uv.y = -uv.y; //!< y軸反転
	
	// 逆射影変換
	float4 clip = float4(uv, 0.0f, 1.0f);
	float4 view = mul(clip, gCamera.projInv);
	view /= view.w;
	
	Ray ray = (Ray)0;
	
	// カメラ空間でのrayをワールド空間に変換
	ray.direction = normalize(mul(float4(view.xyz, 0.0f), gCamera.world).xyz);
	ray.origin    = gCamera.GetPosition();
	
	return ray;
}

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
	return gParameter.scaleDepth * exp(-0.00287 + x * (0.459 + x * (3.83 + x * (-6.80 + x * 5.25))));
}

////////////////////////////////////////////////////////////////////////////////////////////
// common method
////////////////////////////////////////////////////////////////////////////////////////////

bool CheckOverTexture(uint2 index) {
	return any(index >= gConfig.size);
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 index = dispatchThreadId.xy;
	
	if (CheckOverTexture(index)) {
		return; //!< texture size over
	}

	float4 main = gMain[index];

	if (main.a != 0.0f) {
		return; //!< already calculated
	}

	Ray rayD = GenerateRay((float2(index) + 0.5f) / gConfig.size);

	float3 skyPos = IntersectionPos(rayD.origin, rayD.direction, gParameter.outerRadius);
	if (skyPos.x == 0.0f) {
		// カメラが大気の外
		gMain[index] = float4(0.0f, 0.0f, 0.0f, 0.0f);
		return;
	}

	float3 ray = skyPos - rayD.origin;
	float far = length(skyPos - rayD.origin);
	ray /= far;

	
	float height      = length(rayD.origin);
	float depth       = exp(kScaleOverScaleDepth * (gParameter.innerRadius - height));
	float startAngle  = dot(ray, rayD.direction) / height;
	float startOffset = depth * IntegralApproximation(startAngle);

	const int kSamples = 5;

	float sampleLength = far / float(kSamples);
	float scaledLength = sampleLength * kScale;
	float3 sampleRay   = ray * sampleLength;
	float3 samplePoint = rayD.origin + sampleRay * 0.5f;

	float3 frontColor = (float3)0;

	for (int i = 0; i < kSamples; ++i) {
		float fHeight      = length(samplePoint);
		float fDepth       = exp(kScaleOverScaleDepth * (gParameter.innerRadius - fHeight));
		
		float fLightAngle = dot(gParameter.lightDir, samplePoint) / fHeight;
		float fCameraAngle = dot(ray, samplePoint) / fHeight;

		float fScatter = (startOffset + fDepth * (IntegralApproximation(fLightAngle) - IntegralApproximation(fCameraAngle)));

		float3 attenuate = exp(-fScatter * (kInvWavelength * kKr4pi + kKm4pi));
		frontColor      += attenuate * (fDepth * scaledLength);
		samplePoint     += sampleRay;
	}

	float4 primaryColor   = float4(frontColor * (kInvWavelength * kKrESun), 1.0f);
	float4 secondaryColor = float4(frontColor * kKmESun, 1.0f);
	
	float3 direction = normalize(rayD.origin - skyPos);

	float fCos = dot(gParameter.lightDir.xyz, direction) / length(direction);
	float rayPhase = 0.75f * (1.0f + fCos * fCos);

	float minPhase = 1.5f * ((1.0f - kG2) / (2.0f + kG2)) * (1.0f + fCos * fCos) / pow(abs(1.0f + kG2 - 2.0f * gParameter.g * fCos), 1.5f);

	float3 raycolor = (primaryColor * rayPhase).rgb;
	float3 miecolor = (secondaryColor * minPhase).rgb;

	float3 color = float3(1.0f, 1.0f, 1.0f) - exp(-gParameter.exposure * (raycolor + miecolor));

	gMain[index] = float4(color, 1.0f);

}
