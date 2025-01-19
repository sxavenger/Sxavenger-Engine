//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "ReflectionRaytracing.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// static const variables
////////////////////////////////////////////////////////////////////////////////////////////

static const float pi = 3.1415926535897932384626433832795f;

////////////////////////////////////////////////////////////////////////////////////////////
// static const parameter variables
////////////////////////////////////////////////////////////////////////////////////////////

static const float3 kSunDir      = normalize(float3(0.0f, 1.0f, 10.0f)); //!< ‘¾—z‚Ì•ûŒü
static const float kSunIntensity = 20.0f;                                //!< ‘¾—z‚Ì‹­‚³

static const uint kNumSamples = 16; //!< ƒTƒ“ƒvƒŠƒ“ƒO”
static const uint kNumScatter = 8; //!< ŽU—‰ñ”

static const float kEarthRadius      = 6360e3; //!< ’n‹…‚Ì”¼Œa
static const float kAtmosphereRadius = 6420e3; //!< ‘å‹C‚Ì”¼Œa

static const float kG = 0.76f; //!< Henyey-Greenstein‚ÌgŒW”

static const float kHr = 7994.0f; //!< Rayleigh‚Ìscale height
static const float kHm = 1200.0f; //!< Mie‚Ìscale height

static const float3 kBetaR = float3(5.8e-6, 13.5e-6, 33.1e-6); //!< Rayleigh‚ÌŽU—ŒW”
static const float3 kBetaM = float3(21e-6, 21e-6, 21e-6);      //!< Mie‚ÌŽU—ŒW”

////////////////////////////////////////////////////////////////////////////////////////////
// structures
////////////////////////////////////////////////////////////////////////////////////////////

struct Sphere {
	float3 center;
	float  radius;
};

struct Ray {
	float3 origin;
	float3 direcion;
};

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[shader("miss")] //!< attmosphere miss shader
void mainMiss(inout Payload payload) {

	Sphere sphere = (Sphere)0;
	sphere.center = float3(0.0f, 0.0f, 0.0f);
	sphere.radius = 1.0f;

	Ray ray = (Ray)0;
	ray.origin   = WorldRayOrigin() + kEarthRadius;
	ray.direcion = WorldRayDirection();
	
	// todo: attmosphere scattering
	
	payload.color = float3(1.0f, 1.0f, 0.0f);
	
}
