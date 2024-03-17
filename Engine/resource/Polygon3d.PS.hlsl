#include "Object3d.hlsli"

struct Material {
	float4 color;
	float4x4 uvTransform;
	int lambertType;
	int enableBlinnPhong;
	float specPow;
};
ConstantBuffer<Material> gMaterial : register(b0);

struct Light {
	float4 color;
	float4 position;
	float3 direction;
	float range;
	float intensity;
	
	int lightType;
};
ConstantBuffer<Light> gLight : register(b1);

struct Camera3D {
	float4 position;
};
ConstantBuffer<Camera3D> gCamera3D : register(b2);

struct PSOutput {
	float4 color : SV_TARGET0;
};

PSOutput main(VSOutput input) {
	
	PSOutput output;
    
	float4 defaultColor = gMaterial.color;
	output.color = defaultColor;

	float intensity;
	float3 direction;
	
	if (gLight.lightType == 0) { //!< directionLight
		intensity = gLight.intensity;
		
	} else { //!< PointLight
		
		float distance;
		
		if (any(input.worldPos - gLight.position != float4(0.0f, 0.0f, 0.0f, 0.0f))) {
			distance = length(input.worldPos - gLight.position);
			
		} else {
			distance = 0.0f;
		}
		
		intensity = 1.0f - (1.0f / gLight.range) * distance;
		intensity = clamp(intensity, 0.0f, 1.0f);

		intensity = pow(intensity, 2.0f); // 指数関数敵に変化
		
		
		
		direction = normalize(input.worldPos - gLight.position).xyz;
	}
    
    // lighting
	if (gMaterial.lambertType == 0) { //!< lambert
		
		float NdotL = dot(normalize(input.normal), -direction);
		float cos = saturate(NdotL);
        
		float4 lambert = defaultColor * gLight.color * cos * intensity;
		output.color = lambert;
		
	} else if (gMaterial.lambertType == 1) { //!< halfLambert
		
		float NdotL = dot(normalize(input.normal), -direction);
		float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        
		float4 lambert = defaultColor * gLight.color * cos * intensity;
		output.color = lambert;
	}
    
	if (gMaterial.enableBlinnPhong != 0) { //!< enableBlinnPhong = true;
		
		// blinnePhong
		float3 lightDirection = direction;
		float3 cameraDirection = normalize(input.worldPos - gCamera3D.position).xyz;

		float3 H = normalize(-cameraDirection - lightDirection);
  
		float NdotH = max(dot(normalize(input.normal), H), 0);
		float4 spec = pow(NdotH, gMaterial.specPow) * gLight.color; //!< specColor = lightColor
  
		float4 blinnPhong = saturate(spec * gLight.color * intensity);
		output.color = output.color + blinnPhong;

	}
	
	return output;
}