#pragma once

/*
 # reference
 - [XeGTAO](https://github.com/GameTechDev/XeGTAO.git)
*/

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* process
#include "../../Process.hlsli"
#include "XeGTAO.hlsli"

//* component
#include "../../../../Component/CameraComponent.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

//* camera
ConstantBuffer<CameraComponent> gCamera : register(b0, space1);

////////////////////////////////////////////////////////////////////////////////////////////
// SxGTAO [Sxavenger XeGTAO] namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace SxGTAO {

	////////////////////////////////////////////////////////////////////////////////////////////
	// GTAOQuality structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct GTAOQuality {

		//=========================================================================================
		// public variables
		//=========================================================================================

		uint sliceCount;
		uint stepsPerSlice;

		//=========================================================================================
		// public methods
		//=========================================================================================

		static GTAOQuality GetLow() {
			GTAOQuality quality = (GTAOQuality)0;
			quality.sliceCount    = 1;
			quality.stepsPerSlice = 2;
			return quality;
		}

		static GTAOQuality GetMedium() {
			GTAOQuality quality = (GTAOQuality)0;
			quality.sliceCount    = 2;
			quality.stepsPerSlice = 2;
			return quality;
		}

		static GTAOQuality GetHigh() {
			GTAOQuality quality = (GTAOQuality)0;
			quality.sliceCount    = 3;
			quality.stepsPerSlice = 3;
			return quality;
		}

		static GTAOQuality GetUltra() {
			GTAOQuality quality = (GTAOQuality)0;
			quality.sliceCount    = 4;
			quality.stepsPerSlice = 4;
			return quality;
		}
		
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// GTAOSettings structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct GTAOSettings {
		//!< [XeGTAO.h] GTAOSettings structure

		//=========================================================================================
		// public variables
		//=========================================================================================

		uint qualityLevel;  // 0: low; 1: medium; 2: high; 3: ultra
		uint denoisePasses; // 0: disabled; 1: sharp; 2: medium; 3: soft
		float radius;       // [0.0,  ~ ]   World (view) space size of the occlusion sphere.

		// auto-tune-d settings
		float radiusMultiplier;
		float falloffRange;
		float sampleDistributionPower;
		float thinOccluderCompensation;
		float finalValuePower;
		float depthMIPSamplingOffset;

		//=========================================================================================
		// public methods
		//=========================================================================================

		static GTAOSettings GetDefault() {
		
			GTAOSettings settings = (GTAOSettings)0;
			
			settings.qualityLevel  = 2;
			settings.denoisePasses = 1;
			settings.radius        = 0.5f;

			settings.radiusMultiplier         = XE_GTAO_DEFAULT_RADIUS_MULTIPLIER;
			settings.falloffRange             = XE_GTAO_DEFAULT_FALLOFF_RANGE;
			settings.sampleDistributionPower  = XE_GTAO_DEFAULT_SAMPLE_DISTRIBUTION_POWER;
			settings.thinOccluderCompensation = XE_GTAO_DEFAULT_THIN_OCCLUDER_COMPENSATION;
			settings.finalValuePower          = XE_GTAO_DEFAULT_FINAL_VALUE_POWER;
			settings.depthMIPSamplingOffset   = XE_GTAO_DEFAULT_DEPTH_MIP_SAMPLING_OFFSET;

			return settings;
		}

		static GTAOConstants GetConstants(GTAOSettings settings, uint2 dimension, float4x4 proj) {
			 //!< [XeGTAO.h] GTAOGetConstants constants. not temporal storage.

			GTAOConstants constants = (GTAOConstants)0;

			constants.ViewportSize      = dimension;
			constants.ViewportPixelSize = 1.0f / dimension;

			float depthLinearizeMul = -proj[3][2]; //!< = (far * near) / (far - near)
			float depthLinearizeAdd =  proj[2][2]; //!< = far / (far - near)

			if (depthLinearizeMul * depthLinearizeAdd < 0) {
				depthLinearizeAdd = -depthLinearizeAdd;
			}

			constants.DepthUnpackConsts = float2(depthLinearizeMul, depthLinearizeAdd);

			float tanHalfFOVY = 1.0f / proj[1][1];
			float tanHalfFOVX = 1.0f / proj[0][0];
			constants.CameraTanHalfFOV = float2(tanHalfFOVX, tanHalfFOVY);

			constants.NDCToViewMul = float2(constants.CameraTanHalfFOV.x * 2.0f,  constants.CameraTanHalfFOV.y * -2.0f);
			constants.NDCToViewAdd = float2(constants.CameraTanHalfFOV.x * -1.0f, constants.CameraTanHalfFOV.y * 1.0f);

			constants.NDCToViewMul_x_PixelSize = float2(constants.NDCToViewMul.x * constants.ViewportPixelSize.x, constants.NDCToViewMul.y * constants.ViewportPixelSize.y);

			constants.EffectRadius = settings.radius;

			constants.EffectFalloffRange = settings.falloffRange;
			constants.DenoiseBlurBeta    = (settings.denoisePasses == 0) ? 1e4f : 1.2f; //!< high value disables denoise - more elegant & correct way would be do set all edges to 0

			constants.RadiusMultiplier         = settings.radiusMultiplier;
			constants.SampleDistributionPower  = settings.sampleDistributionPower;
			constants.ThinOccluderCompensation = settings.thinOccluderCompensation;
			constants.FinalValuePower          = settings.finalValuePower;
			constants.DepthMIPSamplingOffset   = settings.depthMIPSamplingOffset;
			constants.NoiseIndex               = 0; //!< = (settings.DenoisePasses > 0) ? frameCounter % 64 : 0; <= maybe [0 ~ 63] random noise...?

			return constants;
		}
		
	};

	static GTAOSettings sGTAOSettings = GTAOSettings::GetDefault(); //!< Global GTAOSettings instance. not temporal storage.

	////////////////////////////////////////////////////////////////////////////////////////////
	// XeGTAO intermediate functions [XeGTAO.hlsli]
	////////////////////////////////////////////////////////////////////////////////////////////

	void XeGTAO_OutputWorkingTerm(const uint2 pixcoord, lpfloat visibility, RWTexture2D<float> ao) {
		ao[pixcoord] = visibility;
	}

	void XeGTAO_DecodeGather(const float4 value, out AOTermType outDecoded[4]) {
		for (uint i = 0; i < 4; ++i) {
			outDecoded[i] = (AOTermType)value[i];
		}
	}

	void XeGTAO_Output(const uint2 pixcoord, RWTexture2D<float> output, AOTermType outputValue) {
		output[pixcoord.xy] = outputValue;
	}

	float2 SpatioTemporalNoise(const uint2 pixcoord, const uint temporalIndex = 0 /* without TAA, temporalIndex is always 0 */) {

		float2 noise = float2(0, 0);

		uint index = HilbertIndex(pixcoord.x, pixcoord.y);
		index += 288 * (temporalIndex % 64); // why 288? tried out a few and that's the best so far (with XE_HILBERT_LEVEL 6U) - but there's probably better :)

		// R2 sequence - see http://extremelearning.com.au/unreasonable-effectiveness-of-quasirandom-sequences/
		return frac(0.5 + index * float2(0.75487766624669276005, 0.5698402909980532659114));
		
	}

	////////////////////////////////////////////////////////////////////////////////////////////
	// XeGTAO Pass methods [XeGTAO.hlsli]
	////////////////////////////////////////////////////////////////////////////////////////////

	groupshared lpfloat g_scratchDepths[8][8];
	void XeGTAO_PrefilterDepths16x16( //!< [numthreas(8, 8, 1)] - each thread computes 2x2 blocks so processing 16x16 block: Dispatch needs to be called with (width + 16-1) / 16, (height + 16-1) / 16
		const uint2 dispatchThreadId /* = SV_DispathThreadID */, const uint2 groupThreadId /* = SV_GroupThreadID */,
		GTAOConstants constants,
		Texture2D<float> depthTexture, SamplerState pointSampler,
		RWTexture2D<float> outDepthMip0, RWTexture2D<float> outDepthMip1, RWTexture2D<float> outDepthMip2, RWTexture2D<float> outDepthMip3, RWTexture2D<float> outDepthMip4) {

		// MIP 0
		const uint2 baseCoord = dispatchThreadId;
		const uint2 pixCoord  = baseCoord * 2;
		float4 depths4 = depthTexture.GatherRed(pointSampler, float2(pixCoord * constants.ViewportPixelSize), int2(1, 1));
		lpfloat depth0 = XeGTAO_ClampDepth(XeGTAO_ScreenSpaceToViewSpaceDepth(depths4.w, constants));
		lpfloat depth1 = XeGTAO_ClampDepth(XeGTAO_ScreenSpaceToViewSpaceDepth(depths4.z, constants));
		lpfloat depth2 = XeGTAO_ClampDepth(XeGTAO_ScreenSpaceToViewSpaceDepth(depths4.x, constants));
		lpfloat depth3 = XeGTAO_ClampDepth(XeGTAO_ScreenSpaceToViewSpaceDepth(depths4.y, constants));
		outDepthMip0[pixCoord + uint2(0, 0)] = (float)depth0;
		outDepthMip0[pixCoord + uint2(1, 0)] = (float)depth1;
		outDepthMip0[pixCoord + uint2(0, 1)] = (float)depth2;
		outDepthMip0[pixCoord + uint2(1, 1)] = (float)depth3;

		// MIP 1
		lpfloat dm1 = XeGTAO_DepthMIPFilter(depth0, depth1, depth2, depth3, constants);
		outDepthMip1[baseCoord] = (float)dm1;
		g_scratchDepths[groupThreadId.x][groupThreadId.y] = dm1;

		GroupMemoryBarrierWithGroupSync();

		// MIP 2
		[branch]
		if (all((groupThreadId.xy % 2.xx) == 0)) {
			lpfloat inTL = g_scratchDepths[groupThreadId.x + 0][groupThreadId.y + 0];
			lpfloat inTR = g_scratchDepths[groupThreadId.x + 1][groupThreadId.y + 0];
			lpfloat inBL = g_scratchDepths[groupThreadId.x + 0][groupThreadId.y + 1];
			lpfloat inBR = g_scratchDepths[groupThreadId.x + 1][groupThreadId.y + 1];

			lpfloat dm2 = XeGTAO_DepthMIPFilter( inTL, inTR, inBL, inBR, constants);
			outDepthMip2[baseCoord / 2] = (float)dm2;
			g_scratchDepths[groupThreadId.x][groupThreadId.y] = dm2;
		}

		GroupMemoryBarrierWithGroupSync();

		// MIP 3
		[branch]
		if (all((groupThreadId.xy % 4.xx) == 0)) {
			lpfloat inTL = g_scratchDepths[groupThreadId.x + 0][groupThreadId.y + 0];
			lpfloat inTR = g_scratchDepths[groupThreadId.x + 2][groupThreadId.y + 0];
			lpfloat inBL = g_scratchDepths[groupThreadId.x + 0][groupThreadId.y + 2];
			lpfloat inBR = g_scratchDepths[groupThreadId.x + 2][groupThreadId.y + 2];

			lpfloat dm3 = XeGTAO_DepthMIPFilter(inTL, inTR, inBL, inBR, constants);
			outDepthMip3[baseCoord / 4] = (float)dm3;
			g_scratchDepths[groupThreadId.x][groupThreadId.y] = dm3;
		}

		GroupMemoryBarrierWithGroupSync( );

		// MIP 4
		[branch]
		if (all((groupThreadId.xy % 8.xx) == 0)) {
			lpfloat inTL = g_scratchDepths[groupThreadId.x + 0][groupThreadId.y + 0];
			lpfloat inTR = g_scratchDepths[groupThreadId.x + 4][groupThreadId.y + 0];
			lpfloat inBL = g_scratchDepths[groupThreadId.x + 0][groupThreadId.y + 4];
			lpfloat inBR = g_scratchDepths[groupThreadId.x + 4][groupThreadId.y + 4];

			lpfloat dm4 = XeGTAO_DepthMIPFilter( inTL, inTR, inBL, inBR, constants);
			outDepthMip4[baseCoord / 8] = (lpfloat)dm4;
		}
		
	}

	void XeGTAO_MainPass(
		const uint2 pixcoord,
		const GTAOQuality quality, const GTAOConstants constants,
		float3 normal, float2 localNoise,
		Texture2D<float> depth, SamplerState pointSampler,
		RWTexture2D<float> ao, RWTexture2D<float> edge) {

		float2 normalizedScreenPos = (pixcoord + 0.5.xx) * constants.ViewportPixelSize;

		lpfloat4 valuesUL = (lpfloat4)depth.GatherRed(pointSampler, float2(pixcoord * constants.ViewportPixelSize));
		lpfloat4 valuesBR = (lpfloat4)depth.GatherRed(pointSampler, float2(pixcoord * constants.ViewportPixelSize), int2(1, 1));

		// viewspace Z at the center
		lpfloat viewspaceZ  = valuesUL.y; //sourceViewspaceDepth.SampleLevel( depthSampler, normalizedScreenPos, 0 ).x; 

		// viewspace Zs left top right bottom
		const lpfloat pixLZ = valuesUL.x;
		const lpfloat pixTZ = valuesUL.z;
		const lpfloat pixRZ = valuesBR.z;
		const lpfloat pixBZ = valuesBR.x;

		lpfloat4 edgesLRTB  = XeGTAO_CalculateEdges((lpfloat)viewspaceZ, (lpfloat)pixLZ, (lpfloat)pixRZ, (lpfloat)pixTZ, (lpfloat)pixBZ);
		edge[pixcoord]      = XeGTAO_PackEdges(edgesLRTB);

		// Move center pixel slightly towards camera to avoid imprecision artifacts due to depth buffer imprecision; offset depends on depth texture format used
#ifdef XE_GTAO_FP32_DEPTHS
		viewspaceZ *= 0.99999; // this is good for FP32 depth buffer
#else
		viewspaceZ *= 0.99920; // this is good for FP16 depth buffer
#endif

		const float3 pixCenterPos = XeGTAO_ComputeViewspacePosition(normalizedScreenPos, viewspaceZ, constants);
		const lpfloat3 viewVec = (lpfloat3)normalize(-pixCenterPos);

		// prevents normals that are facing away from the view vector - xeGTAO struggles with extreme cases, but in Vanilla it seems rare so it's disabled by default
		// viewspaceNormal = normalize( viewspaceNormal + max( 0, -dot( viewspaceNormal, viewVec ) ) * viewVec );

		const lpfloat effectRadius             = (lpfloat)constants.EffectRadius * (lpfloat)constants.RadiusMultiplier;
		const lpfloat sampleDistributionPower  = (lpfloat)constants.SampleDistributionPower;
		const lpfloat thinOccluderCompensation = (lpfloat)constants.ThinOccluderCompensation;
		const lpfloat falloffRange             = (lpfloat)constants.EffectFalloffRange * effectRadius;

		const lpfloat falloffFrom = effectRadius * ((lpfloat)1 - (lpfloat)constants.EffectFalloffRange);

		// fadeout precompute optimisation
		const lpfloat falloffMul = (lpfloat)-1.0 / (falloffRange);
		const lpfloat falloffAdd = falloffFrom / (falloffRange) + (lpfloat)1.0;

		lpfloat visibility = 0;

		lpfloat3 bentNormal = (lpfloat)normal;

		// see "Algorithm 1" in https://www.activision.com/cdn/research/Practical_Real_Time_Strategies_for_Accurate_Indirect_Occlusion_NEW%20VERSION_COLOR.pdf
		{
			const lpfloat noiseSlice  = (lpfloat)localNoise.x;
			const lpfloat noiseSample = (lpfloat)localNoise.y;

			 // quality settings / tweaks / hacks
			const lpfloat pixelTooCloseThreshold = 1.3; // if the offset is under approx pixel size (pixelTooCloseThreshold), push it out to the minimum distance

			// approx viewspace pixel size at pixCoord; approximation of NDCToViewspace( normalizedScreenPos.xy + consts.ViewportPixelSize.xy, pixCenterPos.z ).xy - pixCenterPos.xy;
			const float2 pixelDirRBViewspaceSizeAtCenterZ = viewspaceZ.xx * constants.NDCToViewMul_x_PixelSize;

			lpfloat screenspaceRadius = effectRadius / (lpfloat)pixelDirRBViewspaceSizeAtCenterZ.x;

			// fade out for small screen radii 
			visibility += saturate((10 - screenspaceRadius) / 100) * 0.5;

			// this is the min distance to start sampling from to avoid sampling from the center pixel (no useful data obtained from sampling center pixel)
			const lpfloat minS = (lpfloat)pixelTooCloseThreshold / screenspaceRadius;

			//[unroll]
			for (uint slice = 0; slice < quality.sliceCount; ++slice) {

				lpfloat sliceK = ((lpfloat)slice + noiseSlice) / (lpfloat)quality.sliceCount;
				// lines 5, 6 from the paper
				lpfloat phi = sliceK * XE_GTAO_PI;
				lpfloat cosPhi = cos(phi);
				lpfloat sinPhi = sin(phi);
				lpfloat2 omega = lpfloat2(cosPhi, -sinPhi); //lpfloat2 on omega causes issues with big radii

				// convert to screen units (pixels) for later use
				omega *= screenspaceRadius;

				// line 8 from the paper
				const lpfloat3 directionVec = lpfloat3(cosPhi, sinPhi, 0);

				// line 9 from the paper
				const lpfloat3 orthoDirectionVec = directionVec - (dot(directionVec, viewVec) * viewVec);

				// line 10 from the paper
				//axisVec is orthogonal to directionVec and viewVec, used to define projectedNormal
				const lpfloat3 axisVec = normalize(cross(orthoDirectionVec, viewVec));

				// alternative line 9 from the paper
				// float3 orthoDirectionVec = cross( viewVec, axisVec );

				// line 11 from the paper
				lpfloat3 projectedNormalVec = (lpfloat3)normal - axisVec * dot((lpfloat3)normal, axisVec);

				// line 13 from the paper
				lpfloat signNorm = (lpfloat)sign(dot(orthoDirectionVec, projectedNormalVec));

				// line 14 from the paper
				lpfloat projectedNormalVecLength = length(projectedNormalVec);
				lpfloat cosNorm = (lpfloat)saturate(dot(projectedNormalVec, viewVec) / projectedNormalVecLength);

				// line 15 from the paper
				lpfloat n = signNorm * XeGTAO_FastACos(cosNorm);

				// this is a lower weight target; not using -1 as in the original paper because it is under horizon, so a 'weight' has different meaning based on the normal
				const lpfloat lowHorizonCos0 = cos(n + XE_GTAO_PI_HALF);
				const lpfloat lowHorizonCos1 = cos(n - XE_GTAO_PI_HALF);

				// lines 17, 18 from the paper, manually unrolled the 'side' loop
				lpfloat horizonCos0 = lowHorizonCos0; //-1;
				lpfloat horizonCos1 = lowHorizonCos1; //-1;

				[unroll]
				for (uint step = 0; step < quality.stepsPerSlice; ++step) {

					// R1 sequence (http://extremelearning.com.au/unreasonable-effectiveness-of-quasirandom-sequences/)
					const lpfloat stepBaseNoise = lpfloat(slice + step * quality.stepsPerSlice) * 0.6180339887498948482; // <- this should unroll
					lpfloat stepNoise = frac(noiseSample + stepBaseNoise);

					// approx line 20 from the paper, with added noise
					lpfloat s = ((lpfloat)step + stepNoise) / (lpfloat)quality.stepsPerSlice; // + (lpfloat2)1e-6f);

					// additional distribution modifier
					s = (lpfloat)pow(s, (lpfloat)sampleDistributionPower);

					// avoid sampling center pixel
					s += minS;

					// approx lines 21-22 from the paper, unrolled
					lpfloat2 sampleOffset = s * omega;

					lpfloat sampleOffsetLength = length(sampleOffset);

					// note: when sampling, using point_point_point or point_point_linear sampler works, but linear_linear_linear will cause unwanted interpolation between neighbouring depth values on the same MIP level!
					const lpfloat mipLevel = (lpfloat)clamp(log2(sampleOffsetLength) - constants.DepthMIPSamplingOffset, 0, XE_GTAO_DEPTH_MIP_LEVELS);

					// Snap to pixel center (more correct direction math, avoids artifacts due to sampling pos not matching depth texel center - messes up slope - but adds other 
					// artifacts due to them being pushed off the slice). Also use full precision for high res cases.
					sampleOffset = round(sampleOffset) * (lpfloat2)constants.ViewportPixelSize;

					float2 sampleScreenPos0 = normalizedScreenPos + sampleOffset;
					float SZ0               = depth.SampleLevel(pointSampler, sampleScreenPos0, mipLevel).x;
					float3 samplePos0       = XeGTAO_ComputeViewspacePosition(sampleScreenPos0, SZ0, constants);

					float2 sampleScreenPos1 = normalizedScreenPos - sampleOffset;
					float SZ1               = depth.SampleLevel(pointSampler, sampleScreenPos1, mipLevel).x;
					float3 samplePos1       = XeGTAO_ComputeViewspacePosition(sampleScreenPos1, SZ1, constants);

					float3 sampleDelta0 = (samplePos0 - float3(pixCenterPos)); // using lpfloat for sampleDelta causes precision issues
					float3 sampleDelta1 = (samplePos1 - float3(pixCenterPos)); // using lpfloat for sampleDelta causes precision issues
					lpfloat sampleDist0 = (lpfloat)length(sampleDelta0);
					lpfloat sampleDist1 = (lpfloat)length(sampleDelta1);

					// approx lines 23, 24 from the paper, unrolled
					lpfloat3 sampleHorizonVec0 = (lpfloat3)(sampleDelta0 / sampleDist0);
					lpfloat3 sampleHorizonVec1 = (lpfloat3)(sampleDelta1 / sampleDist1);

					lpfloat falloffBase0 = length(lpfloat3(sampleDelta0.x, sampleDelta0.y, sampleDelta0.z * (1 + thinOccluderCompensation)));
					lpfloat falloffBase1 = length(lpfloat3(sampleDelta1.x, sampleDelta1.y, sampleDelta1.z * (1 + thinOccluderCompensation)));
					lpfloat weight0      = saturate(falloffBase0 * falloffMul + falloffAdd);
					lpfloat weight1      = saturate(falloffBase1 * falloffMul + falloffAdd);

					// sample horizon cos
					lpfloat shc0 = (lpfloat)dot(sampleHorizonVec0, viewVec);
					lpfloat shc1 = (lpfloat)dot(sampleHorizonVec1, viewVec);

					// discard unwanted samples
					shc0 = lerp(lowHorizonCos0, shc0, weight0); // this would be more correct but too expensive: cos(lerp( acos(lowHorizonCos0), acos(shc0), weight0 ));
					shc1 = lerp(lowHorizonCos1, shc1, weight1); // this would be more correct but too expensive: cos(lerp( acos(lowHorizonCos1), acos(shc1), weight1 ));

					horizonCos0 = max(horizonCos0, shc0);
					horizonCos1 = max(horizonCos1, shc1);
				}

				// I can't figure out the slight overdarkening on high slopes, so I'm adding this fudge - in the training set, 0.05 is close (PSNR 21.34) to disabled (PSNR 21.45)
				projectedNormalVecLength = lerp(projectedNormalVecLength, 1, 0.05);

				// line ~27, unrolled
				lpfloat h0 = -XeGTAO_FastACos((lpfloat)horizonCos1);
				lpfloat h1 = XeGTAO_FastACos((lpfloat)horizonCos0);

				lpfloat iarc0 = ((lpfloat)cosNorm + (lpfloat)2 * (lpfloat)h0 * (lpfloat)sin(n) - (lpfloat)cos((lpfloat)2 * (lpfloat)h0 - n)) / (lpfloat)4;
				lpfloat iarc1 = ((lpfloat)cosNorm + (lpfloat)2 * (lpfloat)h1 * (lpfloat)sin(n) - (lpfloat)cos((lpfloat)2 * (lpfloat)h1 - n)) / (lpfloat)4;
				lpfloat localVisibility = (lpfloat)projectedNormalVecLength * (lpfloat)(iarc0 + iarc1);
				visibility += localVisibility;
			}

			visibility /= (lpfloat)quality.sliceCount;
			visibility = pow(visibility, (lpfloat)constants.FinalValuePower);
			visibility = max((lpfloat)0.03, visibility); // disallow total occlusion (which wouldn't make any sense anyhow since pixel is visible but also helps with packing bent normals)
		}

		XeGTAO_OutputWorkingTerm(pixcoord, visibility, ao);
	}

	void XeGTAO_Denoise(
		const uint2 pixcoord /* = pixel * uint2(2, 1) */,
		const GTAOConstants constants,
		Texture2D<float> edge, Texture2D<float> ao, SamplerState pointSampler,
		RWTexture2D<float> output, const bool isFinalApply) {

		const lpfloat blurAmount = (isFinalApply) ? ((lpfloat)constants.DenoiseBlurBeta) : ((lpfloat)constants.DenoiseBlurBeta / (lpfloat)5.0);
		const lpfloat diagWeight = 0.85 * 0.5;

		AOTermType aoTerm[2]; // pixel pixCoordBase and pixel pixCoordBase + int2( 1, 0 )
		lpfloat4 edgesC_LRTB[2];
		lpfloat weightTL[2];
		lpfloat weightTR[2];
		lpfloat weightBL[2];
		lpfloat weightBR[2];

		// gather edge and visibility quads, used later
		const float2 gatherCenter = float2(pixcoord.x, pixcoord.y) * constants.ViewportPixelSize;
		lpfloat4 edgesQ0 = (lpfloat4)edge.GatherRed(pointSampler, gatherCenter, int2(0, 0));
		lpfloat4 edgesQ1 = (lpfloat4)edge.GatherRed(pointSampler, gatherCenter, int2(2, 0));
		lpfloat4 edgesQ2 = (lpfloat4)edge.GatherRed(pointSampler, gatherCenter, int2(1, 2));

		AOTermType visQ0[4];
		XeGTAO_DecodeGather(ao.GatherRed(pointSampler, gatherCenter, int2(0, 0)), visQ0);
	
		AOTermType visQ1[4];
		XeGTAO_DecodeGather(ao.GatherRed(pointSampler, gatherCenter, int2(2, 0)), visQ1);
	
		AOTermType visQ2[4];
		XeGTAO_DecodeGather(ao.GatherRed(pointSampler, gatherCenter, int2(0, 2)), visQ2);
	
		AOTermType visQ3[4];
		XeGTAO_DecodeGather(ao.GatherRed(pointSampler, gatherCenter, int2(2, 2)), visQ3);

		for (int side = 0; side < 2; ++side) {

			const int2 pixel = int2(pixcoord.x + side, pixcoord.y);

			lpfloat4 edgesL_LRTB = XeGTAO_UnpackEdges((side == 0) ? (edgesQ0.x) : (edgesQ0.y));
			lpfloat4 edgesT_LRTB = XeGTAO_UnpackEdges((side == 0) ? (edgesQ0.z) : (edgesQ1.w));
			lpfloat4 edgesR_LRTB = XeGTAO_UnpackEdges((side == 0) ? (edgesQ1.x) : (edgesQ1.y));
			lpfloat4 edgesB_LRTB = XeGTAO_UnpackEdges((side == 0) ? (edgesQ2.w) : (edgesQ2.z));

			edgesC_LRTB[side] = XeGTAO_UnpackEdges((side == 0) ? (edgesQ0.y) : (edgesQ1.x));

			// Edges aren't perfectly symmetrical: edge detection algorithm does not guarantee that a left edge on the right pixel will match the right edge on the left pixel (although
			// they will match in majority of cases). This line further enforces the symmetricity, creating a slightly sharper blur. Works real nice with TAA.
			edgesC_LRTB[side] *= lpfloat4(edgesL_LRTB.y, edgesR_LRTB.x, edgesT_LRTB.w, edgesB_LRTB.z);

			// this allows some small amount of AO leaking from neighbours if there are 3 or 4 edges; this reduces both spatial and temporal aliasing
			const lpfloat leak_threshold = 2.5;
			const lpfloat leak_strength = 0.5;
			lpfloat edginess = (saturate(4.0 - leak_threshold - dot(edgesC_LRTB[side], 1.xxxx)) / (4 - leak_threshold)) * leak_strength;
			edgesC_LRTB[side] = saturate(edgesC_LRTB[side] + edginess);

			// for diagonals; used by first and second pass
			weightTL[side] = diagWeight * (edgesC_LRTB[side].x * edgesL_LRTB.z + edgesC_LRTB[side].z * edgesT_LRTB.x);
			weightTR[side] = diagWeight * (edgesC_LRTB[side].z * edgesT_LRTB.y + edgesC_LRTB[side].y * edgesR_LRTB.z);
			weightBL[side] = diagWeight * (edgesC_LRTB[side].w * edgesB_LRTB.x + edgesC_LRTB[side].x * edgesL_LRTB.w);
			weightBR[side] = diagWeight * (edgesC_LRTB[side].y * edgesR_LRTB.w + edgesC_LRTB[side].w * edgesB_LRTB.y);

			// first pass
			AOTermType ssaoValue = (side == 0) ? (visQ0[1]) : (visQ1[0]);
			AOTermType ssaoValueL = (side == 0) ? (visQ0[0]) : (visQ0[1]);
			AOTermType ssaoValueT = (side == 0) ? (visQ0[2]) : (visQ1[3]);
			AOTermType ssaoValueR = (side == 0) ? (visQ1[0]) : (visQ1[1]);
			AOTermType ssaoValueB = (side == 0) ? (visQ2[2]) : (visQ3[3]);
			AOTermType ssaoValueTL = (side == 0) ? (visQ0[3]) : (visQ0[2]);
			AOTermType ssaoValueBR = (side == 0) ? (visQ3[3]) : (visQ3[2]);
			AOTermType ssaoValueTR = (side == 0) ? (visQ1[3]) : (visQ1[2]);
			AOTermType ssaoValueBL = (side == 0) ? (visQ2[3]) : (visQ2[2]);

			lpfloat sumWeight = blurAmount;
			AOTermType sum = ssaoValue * sumWeight;

			XeGTAO_AddSample(ssaoValueL, edgesC_LRTB[side].x, sum, sumWeight);
			XeGTAO_AddSample(ssaoValueR, edgesC_LRTB[side].y, sum, sumWeight);
			XeGTAO_AddSample(ssaoValueT, edgesC_LRTB[side].z, sum, sumWeight);
			XeGTAO_AddSample(ssaoValueB, edgesC_LRTB[side].w, sum, sumWeight);

			XeGTAO_AddSample(ssaoValueTL, weightTL[side], sum, sumWeight);
			XeGTAO_AddSample(ssaoValueTR, weightTR[side], sum, sumWeight);
			XeGTAO_AddSample(ssaoValueBL, weightBL[side], sum, sumWeight);
			XeGTAO_AddSample(ssaoValueBR, weightBR[side], sum, sumWeight);

			aoTerm[side] = sum / sumWeight;

			XeGTAO_Output(pixel, output, aoTerm[side]);
		}
		
	}

}
