#include "Object3d.hlsli"

struct Material
{
    float4 color;
    float4x4 uvTransform;
    int lambertType;
    int enableBlinnPhong;
    float specPow;
};
ConstantBuffer<Material> gMaterial : register(b0);

struct DirectionalLight
{
    float4 color;
    float3 direction;
    float intensity;
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

struct Camera3D
{
    float4 position;
};
ConstantBuffer<Camera3D> gCamera3D : register(b2);

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PSOutput
{
    float4 color : SV_TARGET0;
};

PSOutput main(VSOutput input)
{
    PSOutput output;
    
    // textureColor
    float4 transformUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformUV.xy);
    
    float4 defaultColor = gMaterial.color * textureColor;
    output.color = defaultColor;
    
    // lighting
    if (gMaterial.lambertType == 0) //!< lambert
    {
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = saturate(NdotL);
        
        float4 lambert = defaultColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
        output.color = lambert;
    }
    else if (gMaterial.lambertType == 1) //!< halfLambert
    {
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        
        float4 lambert = defaultColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
        output.color = lambert;
    }
    
    if (gMaterial.enableBlinnPhong != 0) //!< enableBlinnPhong = true;
    {
        // blinnePhong
        
        float3 lightDirection = gDirectionalLight.direction;
        float3 cameraDirection = normalize(input.worldPos - gCamera3D.position).xyz;
        
        float3 H = normalize(-cameraDirection - lightDirection);
  
        float NdotH = max(dot(normalize(input.normal), H), 0);
        float4 spec = pow(NdotH, gMaterial.specPow) * gDirectionalLight.color; //!< specColor = lightColor
  
        float4 blinnPhong = saturate(spec * gDirectionalLight.color * gDirectionalLight.intensity);
        output.color = output.color + blinnPhong;

    }
    
    return output;
}