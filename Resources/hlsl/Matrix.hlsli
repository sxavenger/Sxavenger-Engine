//=========================================================================================
// methods
//=========================================================================================

float4x4 MakeScale(float3 scale) {
	return float4x4(
		scale.x, 0.0f, 0.0f, 0.0f,
		0.0f, scale.y, 0.0f, 0.0f,
		0.0f, 0.0f, scale.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

float4x4 MakeRotateX(float radian) {
	return float4x4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cos(radian), sin(radian), 0.0f,
		0.0f, -sin(radian), cos(radian), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

float4x4 MakeRotateY(float radian) {
	return float4x4(
		cos(radian), 0.0f, -sin(radian), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		sin(radian), 0.0f, cos(radian), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

float4x4 MakeRotateZ(float radian) {
	return float4x4(
		cos(radian), sin(radian), 0.0f, 0.0f,
		-sin(radian), cos(radian), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

float4x4 MakeRotate(float3 rotate) {
	
	float4x4 x = MakeRotateX(rotate.x);
	float4x4 y = MakeRotateX(rotate.y);
	float4x4 z = MakeRotateX(rotate.z);

	return mul(mul(x, y), z);
}

float4x4 MakeTranslate(float3 translate) {
	return float4x4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		translate.x, translate.y, translate.z, 1.0f
	);
}

float4x4 MakeAffine(float3 scale, float3 rotate, float3 translate) {
	return mul(mul(MakeScale(scale), MakeRotate(rotate)), MakeTranslate(translate));
}