//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../Lighting.hlsli"

//=========================================================================================
// Material structure
//=========================================================================================
struct Material {
	float4 color;
	float4x4 uvTransform;
	uint isDisplayMeshlet;
	uint lambertType;
	uint phongType;
	float specPow;
};

namespace LAMBERT {
	static const uint LAMBERT     = 0,
	                  HALFLAMBERT = 1,
	                  NONE        = 2;
}

namespace PHONG {
	static const uint PHONG      = 0,
	                  BLINNPHONG = 1,
	                  NONE       = 2;
}

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float CalculateLamertInfluence(uint lamertType, float3 normal, float3 direction, float intencity, float factor = 1.0f) {
	
	float influence = 1.0f;
	
	switch (lamertType) {
		case LAMBERT::LAMBERT:
			influence = LambertReflection(normal, direction) * intencity * factor;
			break;
		
		case LAMBERT::HALFLAMBERT:
			influence = HalfLambertReflection(normal, direction) * intencity * factor;
			break;
		
		case LAMBERT::NONE:
			break;
	}
	
	return influence;

}

float CalculatePhongInfluence(uint phongType, float3 normal, float3 position, float3 toLightDirection, float3 toCameraDirection, float specPow, float intencity, float factor = 1.0f) {
	
	float influence = 0.0f;
	
	switch (phongType) {
		case PHONG::PHONG:
			influence = PhongReflection(position, normal, toLightDirection, -toCameraDirection, specPow) * intencity * factor;
			break;
		
		case PHONG::BLINNPHONG:
			influence = BlinnPhongReflection(position, normal, toLightDirection, toCameraDirection, specPow) * intencity * factor;
			break;
		
		case PHONG::NONE:
			break;
	}
	
	return influence;
	
}

float3 ComputeLightColor(float4 albedo, float3 normal, float3 position, Material material, Light light, float3 cameraPosition) {
	
	float3 result = albedo.rgb;
	
	switch (light.lightType) {
		case LIGHT::DIRECTIONAL:
			{
				result *= CalculateLamertInfluence(material.lambertType, normal, -light.direction, light.intensity) * light.color.rgb;
			
				float3 toLight  = -light.direction;
				float3 toCamera = normalize(cameraPosition - position);
				
				result
					+= CalculatePhongInfluence(material.phongType, normal, position, toLight, toCamera, material.specPow, light.intensity) * float3(1.0f, 1.0f, 1.0f); //!< specColor
			}
			break;
		
		case LIGHT::POINT:
			{
				float3 direction = normalize(light.position.xyz - position.xyz);
				float distance   = length(position.xyz - light.position.xyz);
				float factor     = pow(saturate(-distance / light.range + 1.0f), light.decay);
		
				result *= CalculateLamertInfluence(material.lambertType, normal, direction, light.intensity, factor) * light.color.rgb;
				
				float3 toLight  = direction;
				float3 toCamera = normalize(cameraPosition - position);
				
				result
					+= CalculatePhongInfluence(material.phongType, normal, position, toLight, toCamera, material.specPow, light.intensity, factor) * float3(1.0f, 1.0f, 1.0f); //!< specColor
			}
			break;
		
		case LIGHT::SPOT:
			{
				// angle
				float3 direction = normalize(position.xyz - light.position.xyz);
				float angle = dot(direction, light.direction);
				float falloffFactor = saturate((angle - light.angle) / (1.0f - light.angle));
		
				// length
				float distance = length(position.xyz - light.position.xyz);
				float attenuationFactor = pow(saturate(-distance / light.range + 1.0f), light.decay);
		
				float factor = attenuationFactor * falloffFactor;
		
				result *= CalculateLamertInfluence(material.lambertType, normal, direction, light.intensity, factor) * light.color.rgb;
				
				float3 toLight  = direction;
				float3 toCamera = normalize(cameraPosition - position);
				
				result
					+= CalculatePhongInfluence(material.phongType, normal, position, toLight, toCamera, material.specPow, light.intensity, factor) * float3(1.0f, 1.0f, 1.0f); //!< specColor

			}
			break;
	}
	
	return result;
	
}

