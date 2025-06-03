PSOutput main(PSInput input) {

	PSOutput output = (PSOutput)0;

	//* Deferred Pass情報の取得
	Surface surface;
	surface.GetSurface(input.position.xy);

	//* Lightの情報を取得
	float3 c_light = gParameter.GetColor(); //!< lightのcolor
	float3 l       = -gTransforms[input.instanceId].GetDirection(); //!< surfaceからlightへの方向ベクトル

	float NdotL = dot(surface.normal, l); //!< 法線とlightの方向ベクトルの内積

	RayQuery<0> q;

	const float4 main_col   = float4(1.0f, 1.0f, 1.0f, 1.0f);
	const float4 thard_col  = float4(0.2f, 0.2f, 0.2f, 1.0f);
	const float4 shadow_col = float4(0.1f, 0.1f, 0.1f, 1.0f);

	if (NdotL >= 0.0f) {

		// 影の計算
		RayDesc desc;
		desc.Origin    = surface.position.xyz; //!< surfaceの位置
		desc.Direction = l;
		desc.TMin      = kTMin;
		desc.TMax      = kTMax;

		q.TraceRayInline(
			gScene,
			gShadow.flag,
			0xFF,
			desc
		);

		if (q.Proceed()) { //!< hit判定
			float t = q.CandidateTriangleRayT();
			output.color = lerp(shadow_col, main_col, saturate(t / 10.0f));

		} else {
			output.color = main_col;
		}
		
	} else {

		// 影の計算
		RayDesc desc;
		desc.Origin    = surface.position.xyz; //!< surfaceの位置
		desc.Direction = -l;
		desc.TMin      = kTMin;
		desc.TMax      = kTMax;

		q.TraceRayInline(
			gScene,
			gShadow.flag,
			0xFF,
			desc
		);

		
		if (q.Proceed()) { //!< hit判定
			float t = q.CandidateTriangleRayT();
			output.color = lerp(shadow_col, thard_col, saturate(t / 10.0f));
			
		} else {
			output.color = thard_col;
		}
	}

	return output;
}
