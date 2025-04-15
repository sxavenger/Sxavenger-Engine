//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* library
#include "../../Library/Math.hlsli"

//* component
#include "../../Component/CameraComponent.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// define
////////////////////////////////////////////////////////////////////////////////////////////

#define _NUM_THREADS_X 16
#define _NUM_THREADS_Y 16

//=========================================================================================
// buffers
//=========================================================================================

//!< output atmosphere cube map texture.
RWTexture2DArray<float4> gAtmosphere : register(u0);

////////////////////////////////////////////////////////////////////////////////////////////
// static const parameter variables
////////////////////////////////////////////////////////////////////////////////////////////

static const uint kNumSamples = 16; //!< サンプリング数
static const uint kNumScatter = 8; //!< 散乱回数

static const float kEarthRadius      = 6360e3; //!< 地球の半径
static const float kAtmosphereRadius = 6420e3; //!< 大気の半径

static const float kG = 0.76f; //!< Henyey-Greensteinのg係数

static const float kHr = 7994.0f; //!< Rayleighのスケールハイト
static const float kHm = 1200.0f; //!< Mieのスケールハイト

static const float3 kBetaR = float3(5.8e-6, 13.5e-6, 33.1e-6); //!< Rayleighの散乱係数
static const float3 kBetaM = float3(21e-6, 21e-6, 21e-6);      //!< Mieの散乱係数

// todo: parameter.
static const float3 kSunDir      = normalize(float3(0.0f, 4.0f, -10.0f)); //!< 太陽の方向
static const float kSunIntensity = 20.0f; //!< 太陽の強さ

// hack: 文献から引っ張ってくる.

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

float3 GetDirection(float2 uv, uint face) {
	switch (face) {
		case 0:
			return normalize(float3(1.0f, -uv.y, -uv.x)); //!< +x

		case 1:
			return normalize(float3(-1.0f, -uv.y, uv.x)); //!< -x

		case 2:
			return normalize(float3(uv.x, 1.0f, uv.y)); //!< +y

		case 3:
			return normalize(float3(uv.x, -1.0f, -uv.y)); //!< -y

		case 4:
			return normalize(float3(uv.x, -uv.y, 1.0f)); //!< +z

		case 5:
			return normalize(float3(-uv.x, -uv.y, -1.0f)); //!< -z
	}

	return float3(0.0f, 0.0f, 0.0f); //!< error.
}

float RayleighPhaseFunction(float mu) {
	return 3.0f / (16.0f * kPi) * (1.0f + mu * mu);
}

float HenyeyGreensteinPhaseFunction(float mu) {
	return 1.0f / (4.0f * kPi) * (1.0f - kG * kG) / pow(1.0f + kG * kG - 2.0f * kG * mu, 1.5f);
}

bool IntersectSphere(Ray ray, Sphere sphere, inout float distanceNear, inout float distanceFar) {
	float3 rc     = sphere.center - ray.origin;
	float radius2 = sphere.radius * sphere.radius;
	float tca     = dot(rc, ray.direction);
	float d2      = dot(rc, rc) - tca * tca;
	
	if (d2 > radius2) {
		return false;
	}
	
	float thc = sqrt(radius2 - d2);
	
	distanceNear = tca - thc;
	distanceFar  = tca + thc;
	return true;
}

bool GetSunLight(Ray ray, Sphere sphere, inout float opticalDepthR, inout float opticalDepthM) {
	
	float distanceNear = 0.0f;
	float distanceFar  = 0.0f;
	
	if (!IntersectSphere(ray, sphere, distanceNear, distanceFar)) {
		return false;
	}
	
	float marchStep = distanceFar / kNumSamples;
	
	opticalDepthR = 0.0f;
	opticalDepthM = 0.0f;
	
	for (uint i = 0; i < kNumSamples; ++i) {
		
		float3 samplePoint = ray.origin + ray.direction * ((marchStep * i) + 0.5f * marchStep);
		float height = length(samplePoint) - kEarthRadius;
		
		if (height < 0.0f) {
			return false; //!< 地表に到達
		}
		
		opticalDepthR += exp(-height / kHr) * marchStep;
		opticalDepthM += exp(-height / kHm) * marchStep;
	}
	
	return true;
}

float3 GetIncidentLight(Ray ray, Sphere atmosphere) {
	
	float distanceNear = 0.0f;
	float distanceFar  = 0.0f;

	if (!IntersectSphere(ray, atmosphere, distanceNear, distanceFar)) {
		return float3(1.0f, 0.0f, 0.0f);
	}
	
	//float marchStep = (distanceFar - distanceNear) / kNumSamples;
	float marchStep = distanceFar / kNumSamples;
	
	float mu = dot(ray.direction, kSunDir);
	
	float phaseR = RayleighPhaseFunction(mu);
	float phaseM = HenyeyGreensteinPhaseFunction(mu);
	
	float opticalDepthR = 0.0f;
	float opticalDepthM = 0.0f;
	
	float3 sumR = float3(0.0f, 0.0f, 0.0f);
	float3 sumM = float3(0.0f, 0.0f, 0.0f);
	
	for (uint i = 0; i < kNumSamples; ++i) {
		
		float3 samplePoint = ray.origin + ray.direction * ((marchStep * i) + 0.5f * marchStep);
		float height       = length(samplePoint) - kEarthRadius;
		
		float hr = exp(-height / kHr) * marchStep;
		float hm = exp(-height / kHm) * marchStep;
		
		opticalDepthR += hr;
		opticalDepthM += hm;
		
		Ray lightRay = (Ray)0;
		lightRay.origin    = samplePoint;
		lightRay.direction = kSunDir;
		
		float opticalDepthLightR = 0.0f;
		float opticalDepthLightM = 0.0f;
		
		bool isOverground = GetSunLight(lightRay, atmosphere, opticalDepthLightR, opticalDepthLightM);
		
		if (isOverground) {
			float3 tan
				= kBetaR * (opticalDepthR + opticalDepthLightR) + kBetaM * (opticalDepthM + opticalDepthLightM) * 1.1f;

			float3 attebuation = exp(-tan);
			
			sumR += attebuation * hr;
			sumM += attebuation * hm;
		}
	}
	
	return kSunIntensity * (phaseR * sumR * kBetaR + phaseM * sumM * kBetaM);
}


////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	// cube mapのsize
	uint3 size;
	gAtmosphere.GetDimensions(size.x, size.y, size.z);

	// textureサイズのチェック
	if (any(dispatchThreadId >= size)) {
		return;
	}

	uint3 index = dispatchThreadId;
	float2 uv   = (float2(dispatchThreadId.xy + 0.5f) / size.xy) * 2.0f - 1.0f; //!< [-1, 1]に変換

	float3 direction = GetDirection(uv, index.z);

	// atmosphere calculation
	
	Sphere atmosphere = (Sphere)0;
	atmosphere.center = float3(0.0f, 0.0f, 0.0f); //!< 地球の中心
	atmosphere.radius = kAtmosphereRadius;

	Ray ray = (Ray)0;
	ray.direction = direction;
	ray.origin    = float3(0.0f, kEarthRadius, 0.0f); //!< カメラ位置

	float3 color = GetIncidentLight(ray, atmosphere);

	gAtmosphere[index] = float4(color, 1.0f); //!< 書き込み
	
}
