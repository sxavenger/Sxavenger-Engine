////////////////////////////////////////////////////////////////////////////////////////////
// ToonShading methods
////////////////////////////////////////////////////////////////////////////////////////////

float3 ToonShading(float3 color, float tone) {
	float3 result;
	result = round(color * tone) / tone;
	return result;
}

float4 Vignette(float4 position, float2 size) {
	
	float2 uv = position.xy / size;
	float2 uvCenter = float2(0.5f, 0.5f);
	
	float2 t = abs(uv - uvCenter); //!< min: 0.0f, max: 0.5f
	
	float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f); // viggnette color
	color.a = lerp(0.0f, 1.0f, (t.x + t.y));
	
	return color;
	
}