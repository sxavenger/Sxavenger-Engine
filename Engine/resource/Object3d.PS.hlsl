#include "Object3d.hlsli"

struct Material {
	float4 color;
	float4x4 uvTransform;
	int lambertType;
	int phongType;
	float specPow;
};
ConstantBuffer<Material> gMaterial : register(b0);

struct Light {
	float4 color;
	float4 position;
	float3 direction;
	float range;
	float shininess;
	
	int lightType;
};
ConstantBuffer<Light> gLight : register(b1);

struct Camera3D {
	float4 position;
};
ConstantBuffer<Camera3D> gCamera3D : register(b2);

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PSOutput {
	float4 color : SV_TARGET0;
};

PSOutput main(VSOutput input) {
	
	PSOutput output;
    
    // textureColor
	float4 transformUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float4 textureColor = gTexture.Sample(gSampler, transformUV.xy);
    
	float4 defaultColor = gMaterial.color * textureColor;
	output.color = defaultColor;
	
	if (defaultColor.a == 0.0f) {
		discard;
	}
	
	float shininess;
	float3 direction;
	
	if (gLight.lightType == 0) { //!< directionLight
		shininess = gLight.shininess;
		direction = gLight.direction;
		
	} else { //!< PointLight
		
		float distance;
		
		if (any(input.worldPos - gLight.position != float4(0.0f, 0.0f, 0.0f, 0.0f))) {
			distance = length(input.worldPos - gLight.position);
			
		} else {
			distance = 0.0f;
		}
		
		shininess = 1.0f - (1.0f / gLight.range) * distance;
		shininess = clamp(shininess, 0.0f, 1.0f);

		shininess = pow(shininess, 2.0f); // 指数関数敵に変化
		
		direction = normalize(input.worldPos - gLight.position).xyz;

	}
    
    // lighting
	if (gMaterial.lambertType == 0) { //!< lambert
		
		float NdotL = dot(normalize(input.normal), -direction);
		float cos = saturate(NdotL);
        
		float4 lambert = defaultColor * gLight.color * cos * shininess;
		output.color.rgb = lambert.rgb;
		
	} else if (gMaterial.lambertType == 1) { //!< halfLambert
		
		float NdotL = dot(normalize(input.normal), -direction);
		float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        
		float4 lambert = defaultColor * gLight.color * cos * shininess;
		output.color.rgb = lambert.rgb;
	}
    
	if (gMaterial.phongType == 0) { //!< phong
		
		
		
	} else if (gMaterial.phongType == 1) { //!< blinnPhong
		
		// blinnePhong
		float3 lightDirection = direction;
		float3 cameraDirection = normalize(input.worldPos - gCamera3D.position).xyz;

		float3 H = normalize(-cameraDirection - lightDirection);
  
		float NdotH = max(dot(normalize(input.normal), H), 0);
		float4 spec = pow(NdotH, gMaterial.specPow) * gLight.color; //!< specColor = lightColor
  
		float4 blinnPhong = saturate(spec * gLight.color * shininess);
		output.color.rgb = output.color.rgb + blinnPhong.rgb;
		
	}

	return output;
}