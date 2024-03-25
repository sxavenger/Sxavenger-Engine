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
	float4 color;            //!< 色
	float4 position;         //!< world座標
	float3 direction;        //!< 向き
	float intensity;         //!< 輝度 0 ~ 1
	float range;             //!< 範囲
	float decay;             //!< 減衰率
	float angle;             //!< 角度 -1 ~ 1
	float falloffStartAngle; //!< falloffの制御角度 -1 ~ 1
	
	int lightType; //!< ライトの種類
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

////////////////////////////////////////////////////////////////////////////////////////////
// 関数
////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////
// メイン
////////////////////////////////////////////////////////////////////////////////////////////
PSOutput main(GSOutput input) {
	
	PSOutput output;
	
	// textureColor
	float4 transformUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float4 textureColor = gTexture.Sample(gSampler, transformUV.xy);

	float4 defaultColor = gMaterial.color * textureColor;
	output.color = defaultColor;
	
	if (defaultColor.a == 0.0f) {
		discard;
	}
	

	//-----------------------------------------------------------------------------------------
	// light
	//-----------------------------------------------------------------------------------------
	float intensity;
	float3 direction;
	
	if (gLight.lightType == 0) { //!< directionLight
		intensity = gLight.intensity;
		direction = gLight.direction;
		
	} else if (gLight.lightType == 1) { //!< PointLight
		
		float distance = length(gLight.position - input.worldPos);
		float factor = pow(saturate(-distance / gLight.range + 1.0f), gLight.decay);
		
		intensity = gLight.intensity * factor;
		direction = normalize(input.worldPos - gLight.position).xyz;
		
	} else if (gLight.lightType == 2) {
		
		direction = normalize(input.worldPos - gLight.position).xyz;
		
		float angle = dot(direction, gLight.direction);
		float falloffFactor = saturate((angle - gLight.angle) / (gLight.falloffStartAngle - gLight.angle));
		
		float distance = length(gLight.position - input.worldPos);
		float distanceFactor = pow(saturate(-distance / gLight.range + 1.0f), gLight.decay);
		
		intensity = gLight.intensity * falloffFactor * distanceFactor;
	}

	//-----------------------------------------------------------------------------------------
	// lambert
	//-----------------------------------------------------------------------------------------
	if (gMaterial.lambertType == 0) { //!< lambert
		
		float NdotL = dot(normalize(input.normal), -direction);
		float cos = saturate(NdotL);

		float4 lambert = defaultColor * gLight.color * cos * intensity;
		output.color.rgb = lambert.rgb;
		
	} else if (gMaterial.lambertType == 1) { //!< halfLambert
		
		float NdotL = dot(normalize(input.normal), -direction);
		float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);

		float4 lambert = defaultColor * gLight.color * cos * intensity;
		output.color.rgb = lambert.rgb;
	}
	
	//-----------------------------------------------------------------------------------------
	// phong
	//-----------------------------------------------------------------------------------------
	if (gMaterial.phongType == 0) { //!< phong
		
		float3 toEye = normalize(gCamera3D.position - input.worldPos).xyz;
		float3 reflectLight = reflect(gLight.direction, normalize(input.normal));
		
		float RdotE = saturate(dot(reflectLight, toEye));
		float4 spec = pow(RdotE, gMaterial.specPow) * gLight.color; //!< specColor = lightColor
		
		float4 blinnPhong = saturate(spec * gLight.color * intensity);
		output.color.rgb = output.color.rgb + blinnPhong.rgb;
		
		
	} else if (gMaterial.phongType == 1) { //!< blinnPhong
		
		// blinnePhong
		float3 lightDirection = direction;
		float3 cameraDirection = normalize(input.worldPos - gCamera3D.position).xyz;

		float3 H = normalize(-cameraDirection - lightDirection);
  
		float NdotH = saturate(dot(normalize(input.normal), H));
		float4 spec = pow(NdotH, gMaterial.specPow) * gLight.color; //!< specColor = lightColor

		float4 blinnPhong = saturate(spec * gLight.color * intensity);
		output.color.rgb = output.color.rgb + blinnPhong.rgb;
		
	}

	return output;
}