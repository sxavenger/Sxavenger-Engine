#pragma once

/* ACES reference
 
*/

#ifndef ACES_HLSL_INCLUDED
#define ACES_HLSL_INCLUDED

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Math.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// ACES namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace ACES {
	// todo: ACES 2.0

	//=========================================================================================
	// Color conversiom
	//=========================================================================================

	//* constant variables *//

	/* color coordinates
	 AP0: ACES2065-1
	 AP1: ACEScg
	 XYZ: CIE 1931 XYZ
	 sRGB: Rec.709
	 Rec.2020:
	*/

	/* CAT
	 D65: sRGB white point
	 D60: ACES white point
	*/

	//!< AP0 -> XYZ
	static const float3x3 AP0_2_XYZ_Matrix = {
		0.9525523959, 0.0000000000, 0.0000936786,
		0.3439664498, 0.7281660966, -0.0721325464,
		0.0000000000, 0.0000000000, 1.0088251844,
	};

	//!< XYZ -> AP0
	static const float3x3 XYZ_2_AP0_Matrix = {
		1.0498110175, 0.0000000000, -0.0000974845,
		-0.4959030231, 1.3733130458, 0.0982400361,
		0.0000000000, 0.0000000000, 0.9912520182,
	};

	//!< AP1 -> XYZ
	static const float3x3 AP1_2_XYZ_Matrix = {
		0.6624541811, 0.1340042065, 0.1561876870,
		0.2722287168, 0.6740817658, 0.0536895174,
		-0.0055746495, 0.0040607335, 1.0103391003,
	};

	//!< XYZ -> AP1
	static const float3x3 XYZ_2_AP1_Matrix = {
		1.6410233797, -0.3248032942, -0.2364246952,
		-0.6636628587, 1.6153315917, 0.0167563477,
		0.0117218943, -0.0082844420, 0.9883948585,
	};

	//!< AP0 -> (XYZ) -> AP1
	static const float3x3 AP0_2_AP1_Matrix = /*mul(AP0_2_XYZ_Matrix, XYZ_2_AP1_Matrix)*/ {
		1.4514393161, -0.2365107469, -0.2149285693,
		-0.0765537734, 1.1762296998, -0.0996759264,
		0.0083161484, -0.0060324498, 0.9977163014,
	};

	//!< AP1 -> (XYZ) -> AP0
	static const float3x3 AP1_2_AP0_Matrix = /*mul(AP1_2_XYZ_Matrix, XYZ_2_AP0_Matrix)*/ {
		0.6954522414, 0.1406786965, 0.1638690622,
		0.0447945634, 0.8596711185, 0.0955343182,
		-0.0055258826, 0.0040252103, 1.0015006723,
	};

	//!< AP1 Luminance
	static const float3 AP1_RGB2Y = {
		0.2722287168, //AP1_2_XYZ_MAT[0][1],
		0.6740817658, //AP1_2_XYZ_MAT[1][1],
		0.0536895174, //AP1_2_XYZ_MAT[2][1]
	};

	//-----------------------------------------------------------------------------------------
	// sRGB(Rec.709) primaries
	//-----------------------------------------------------------------------------------------

	//!< XYZ -> sRGB
	static const float3x3 XYZ_2_sRGB_Matrix = {
		3.2409699419, -1.5373831776, -0.4986107603,
		-0.9692436363, 1.8759675015, 0.0415550574,
		0.0556300797, -0.2039769589, 1.0569715142,
	};

	//!< sRGB -> XYZ
	static const float3x3 sRGB_2_XYZ_Matrix = {
		0.4124564, 0.3575761, 0.1804375,
		0.2126729, 0.7151522, 0.0721750,
		0.0193339, 0.1191920, 0.9503041,
	};

	//-----------------------------------------------------------------------------------------
	// Rec.2020 primaries
	//-----------------------------------------------------------------------------------------

	//!< XYZ -> Rec.2020
	static const float3x3 XYZ_2_Rec2020_Matrix = {
		1.7166084, -0.3556621, -0.2533601,
		-0.6666829, 1.6164776, 0.0157685,
		0.0176422, -0.0427763, 0.94222867
	};

	//!< Rec.2020 -> XYZ
	static const float3x3 Rec2020_2_XYZ_Matrix = {
		0.6369736, 0.1446172, 0.1688585,
		0.2627066, 0.6779996, 0.0592938,
		0.0000000, 0.0280728, 1.0608437
	};

	//-----------------------------------------------------------------------------------------
	// CAT convertions
	//-----------------------------------------------------------------------------------------

	//!< D65 -> D60
	static const float3x3 D65_2_D60_CAT = {
		1.01303, 0.00610531, -0.014971,
		0.00769823, 0.998165, -0.00503203,
		-0.00284131, 0.00468516, 0.924507,
	};

	//!< D60 -> D65
	static const float3x3 D60_2_D65_CAT = {
		0.987224, -0.00611327, 0.0159533,
		-0.00759836, 1.00186, 0.00533002,
		0.00307257, -0.00509595, 1.08168,
	};


	//* methods *//

	float rgb_2_saturation(float3 rgb) {
		float min_rgb = min(min(rgb.r, rgb.g), rgb.b);
		float max_rgb = max(max(rgb.r, rgb.g), rgb.b);
		return (max(max_rgb, 1e-10) - max(min_rgb, 1e-10)) / max(max_rgb, 1e-2);
	}

	float rgb_2_hue(float3 rgb) {
		// Returns a geometric hue angle in degrees (0-360) based on RGB values.
		// For neutral colors, hue is undefined and the function will return a quiet NaN value.
		float hue;
		
		if (rgb[0] == rgb[1] && rgb[1] == rgb[2]) {
			//hue = FLT_NAN; // RGB triplets where RGB are equal have an undefined hue
			hue = 0;
		} else {
			hue = (180. / kPi) * atan2(sqrt(3.0) * (rgb[1] - rgb[2]), 2 * rgb[0] - rgb[1] - rgb[2]);
		}

		if (hue < 0.) {
			hue = hue + 360;
		}

		return clamp(hue, 0, 360);
	}

	float rgb_2_yc(float3 rgb, float ycRadiusWeight = 1.75) {
		// Converts RGB to a luminance proxy, here called YC
		// YC is ~ Y + K * Chroma
		// Constant YC is a cone-shaped surface in RGB space, with the tip on the 
		// neutral axis, towards white.
		// YC is normalized: RGB 1 1 1 maps to YC = 1
		//
		// ycRadiusWeight defaults to 1.75, although can be overridden in function 
		// call to rgb_2_yc
		// ycRadiusWeight = 1 -> YC for pure cyan, magenta, yellow == YC for neutral 
		// of same value
		// ycRadiusWeight = 2 -> YC for pure red, green, blue  == YC for  neutral of 
		// same value.

		float r = rgb[0];
		float g = rgb[1];
		float b = rgb[2];
  
		float chroma = sqrt(b * (b - g) + g * (g - r) + r * (r - b));

		return (b + g + r + ycRadiusWeight * chroma) / 3.;
	}

	//=========================================================================================
	// Tone mapping & Non linear Shaping
	//=========================================================================================

	float glow_fwd(float ycIn, float glowGainIn, float glowMid) {
		float glowGainOut;

		if (ycIn <= 2. / 3. * glowMid) {
			glowGainOut = glowGainIn;
			
		} else if (ycIn >= 2 * glowMid) {
			glowGainOut = 0;
			
		} else {
			glowGainOut = glowGainIn * (glowMid / ycIn - 0.5);
		}

		return glowGainOut;
	}

	float glow_inv(float ycOut, float glowGainIn, float glowMid) {
		float glowGainOut;

		if (ycOut <= ((1 + glowGainIn) * 2.0 / 3.0 * glowMid)) {
			glowGainOut = -glowGainIn / (1 + glowGainIn);
			
		} else if (ycOut >= (2. * glowMid)) {
			glowGainOut = 0.0;
			
		} else {
			glowGainOut = glowGainIn * (glowMid / ycOut - 1.0 / 2.0) / (glowGainIn / 2.0 - 1.0);
		}

		return glowGainOut;
	}

	float sigmoid_shaper(float x) {
		// Sigmoid function in the range 0 to 1 spanning -2 to +2.
		float t = max(1 - abs(0.5 * x), 0);
		float y = 1 + sign(x) * (1 - t * t);
		return 0.5 * y;
	}

	float center_hue(float hue, float centerH) {
		float hueCentered = hue - centerH;
		
		if (hueCentered < -180.0) {
			hueCentered += 360;
			
		} else if (hueCentered > 180.0) {
			hueCentered -= 360;
		}
		
		return hueCentered;
	}

	//=========================================================================================
	// Red Modifier function
	//=========================================================================================

	float cubic_basis_shaper(float x, float w /*full base width of the shaper function (in degrees)*/) {

		float M[4][4] = {
			{-1. / 6, 3. / 6, -3. / 6, 1. / 6},
			{3. / 6, -6. / 6, 3. / 6, 0. / 6},
			{-3. / 6, 0. / 6, 3. / 6, 0. / 6},
			{1. / 6, 4. / 6, 1. / 6, 0. / 6}
		};
  
		float knots[5] = {-0.5 * w, -0.25 * w, 0, 0.25 * w, 0.5 * w};
  
		float y = 0;
		
		if ((x > knots[0]) && (x < knots[4])) {
			float knot_coord = (x - knots[0]) * 4.0 / w;
			int j = knot_coord;
			float t = knot_coord - j;
	  
			float monomials[4] = {t * t * t, t * t, t, 1.0};

			// (if/else structure required for compatibility with CTL < v1.5.)
			if (j == 3) {
				y = monomials[0] * M[0][0] + monomials[1] * M[1][0] +
				monomials[2] * M[2][0] + monomials[3] * M[3][0];
				
			} else if (j == 2) {
				y = monomials[0] * M[0][1] + monomials[1] * M[1][1] +
				monomials[2] * M[2][1] + monomials[3] * M[3][1];
				
			} else if (j == 1) {
				y = monomials[0] * M[0][2] + monomials[1] * M[1][2] +
				monomials[2] * M[2][2] + monomials[3] * M[3][2];
				
			} else if (j == 0) {
				y = monomials[0] * M[0][3] + monomials[1] * M[1][3] +
				monomials[2] * M[2][3] + monomials[3] * M[3][3];
				
			} else {
				y = 0.0;
			}
		}
  
		return y * 1.5;
		
	}

	//=========================================================================================
	// Other
	//=========================================================================================

	//* constant variables *//

	static const float DIM_SURROUND_GAMMA = 0.9811; //!< Gamma compensation factor

	//* methods *//

	float3 XYZ_2_xyY(float3 XYZ) {
		float3 xyY;
		float divisor = (XYZ[0] + XYZ[1] + XYZ[2]);

		if (divisor == 0.0) {
			divisor = 1e-10;
		}
		
		xyY[0] = XYZ[0] / divisor;
		xyY[1] = XYZ[1] / divisor;
		xyY[2] = XYZ[1];
  
		return xyY;
	}

	float3 xyY_2_XYZ(float3 xyY) {
		float3 XYZ;
		XYZ[0] = xyY[0] * xyY[2] / max(xyY[1], 1e-10);
		XYZ[1] = xyY[2];
		XYZ[2] = (1.0 - xyY[0] - xyY[1]) * xyY[2] / max(xyY[1], 1e-10);

		return XYZ;
	}

	float Y_2_linCV(float Y, float Ymax, float Ymin) {
		return (Y - Ymin) / (Ymax - Ymin);
	}

	float linCV_2_Y(float linCV, float Ymax, float Ymin) {
		return linCV * (Ymax - Ymin) + Ymin;
	}

	float3 darkSurround_to_dimSurround(float3 linearCV) {
		float3 XYZ = mul(AP1_2_XYZ_Matrix, linearCV);

		float3 xyY = XYZ_2_xyY(XYZ);
		xyY[2] = clamp(xyY[2], 0, 65535);
		xyY[2] = pow(xyY[2], DIM_SURROUND_GAMMA);
		XYZ = xyY_2_XYZ(xyY);

		return mul(XYZ_2_AP1_Matrix, XYZ);
	}

	float3 dimSurround_to_darkSurround(float3 linearCV) {
		float3 XYZ = mul(linearCV, AP1_2_XYZ_Matrix);

		float3 xyY = XYZ_2_xyY(XYZ);
		xyY[2] = clamp(xyY[2], 0., 65535);
		xyY[2] = pow(xyY[2], 1. / DIM_SURROUND_GAMMA);
		XYZ = xyY_2_XYZ(xyY);

		return mul(XYZ, XYZ_2_AP1_Matrix);
	}

	//=========================================================================================
	// Segment Splines
	//=========================================================================================

	//* structures *//

	struct SegmentedSplineParams_c5 {
		float coefsLow[6];  // coefs for B-spline between minPoint and midPoint (units of log luminance)
		float coefsHigh[6]; // coefs for B-spline between midPoint and maxPoint (units of log luminance)
		float2 minPoint;    // {luminance, luminance} linear extension below this
		float2 midPoint;    // {luminance, luminance} 
		float2 maxPoint;    // {luminance, luminance} linear extension above this
		float slopeLow;     // log-log slope of low linear extension
		float slopeHigh;    // log-log slope of high linear extension
	};

	struct SegmentedSplineParams_c9 {
		float coefsLow[10];  // coefs for B-spline between minPoint and midPoint (units of log luminance)
		float coefsHigh[10]; // coefs for B-spline between midPoint and maxPoint (units of log luminance)
		float2 minPoint;     // {luminance, luminance} linear extension below this
		float2 midPoint;     // {luminance, luminance} 
		float2 maxPoint;     // {luminance, luminance} linear extension above this
		float slopeLow;      // log-log slope of low linear extension
		float slopeHigh;     // log-log slope of high linear extension
	};

	//* constants variables *//

	static const int N_KNOTS_LOW_C5  = 4;
	static const int N_KNOTS_HIGH_C5 = 4;

	static const int N_KNOTS_LOW_C9  = 8;
	static const int N_KNOTS_HIGH_C9 = 8;

	static const float3x3 M = {
		{0.5, -1.0, 0.5},
		{-1.0, 1.0, 0.5},
		{0.5, 0.0, 0.0}
	};

	//* methods *//

	float segmented_spline_c5_fwd(float x) {
		// RRT_PARAMS
		const SegmentedSplineParams_c5 C = {
			{-4.0000000000, -4.0000000000, -3.1573765773, -0.4852499958, 1.8477324706, 1.8477324706}, // coefsLow[6]
			{-0.7185482425, 2.0810307172, 3.6681241237, 4.0000000000, 4.0000000000, 4.0000000000}, // coefsHigh[6]
			{0.18 * exp2(-15.0), 0.0001}, // minPoint
			{0.18, 4.8}, // midPoint
			{0.18 * exp2(18.0), 10000.0}, // maxPoint
			0.0, // slopeLow
			0.0 // slopeHigh
		};

		// Check for negatives or zero before taking the log. If negative or zero,
		// set to ACESMIN.1
		float xCheck = x <= 0 ? exp2(-14.0) : x;

		float logx = log10(xCheck);
		float logy;

		if (logx <= log10(C.minPoint.x)) {
			logy = logx * C.slopeLow + (log10(C.minPoint.y) - C.slopeLow * log10(C.minPoint.x));
			
		} else if ((logx > log10(C.minPoint.x)) && (logx < log10(C.midPoint.x))) {
			float knot_coord = (N_KNOTS_LOW_C5 - 1) * (logx - log10(C.minPoint.x)) / (log10(C.midPoint.x) - log10(C.minPoint.x));
			int j            = knot_coord;
			float t          = knot_coord - j;

			float3 cf = {C.coefsLow[j], C.coefsLow[j + 1], C.coefsLow[j + 2]};
	
			float3 monomials = {t * t, t, 1.0};
			logy = dot(monomials, mul(cf, M));
			
		} else if ((logx >= log10(C.midPoint.x)) && (logx < log10(C.maxPoint.x))) {
			float knot_coord = (N_KNOTS_HIGH_C5 - 1) * (logx - log10(C.midPoint.x)) / (log10(C.maxPoint.x) - log10(C.midPoint.x));
			int j            = knot_coord;
			float t          = knot_coord - j;

			float3 cf = {C.coefsHigh[j], C.coefsHigh[j + 1], C.coefsHigh[j + 2]};

			float3 monomials = {t * t, t, 1.0};
			logy = dot(monomials, mul(cf, M));
			
		} else { //if ( logIn >= log10(C.maxPoint.x) ) { 
			logy = logx * C.slopeHigh + (log10(C.maxPoint.y) - C.slopeHigh * log10(C.maxPoint.x));
		}

		return pow(10, logy);
	}

	float segmented_spline_c9_fwd(float x, const SegmentedSplineParams_c9 C) {
		// Check for negatives or zero before taking the log. If negative or zero,
		// set to OCESMIN.
		float xCheck = x <= 0 ? 1e-4 : x;

		float logx = log10(xCheck);
		float logy;

		if (logx <= log10(C.minPoint.x)) {
			logy = logx * C.slopeLow + (log10(C.minPoint.y) - C.slopeLow * log10(C.minPoint.x));
			
		} else if ((logx > log10(C.minPoint.x)) && (logx < log10(C.midPoint.x))) {
			float knot_coord = (N_KNOTS_LOW_C9 - 1) * (logx - log10(C.minPoint.x)) / (log10(C.midPoint.x) - log10(C.minPoint.x));
			int j            = knot_coord;
			float t          = knot_coord - j;

			float3 cf = {C.coefsLow[j], C.coefsLow[j + 1], C.coefsLow[j + 2]};

			float3 monomials = {t * t, t, 1.0};
			logy = dot(monomials, mul(cf, M));
			
		} else if ((logx >= log10(C.midPoint.x)) && (logx < log10(C.maxPoint.x))) {
			float knot_coord = (N_KNOTS_HIGH_C9 - 1) * (logx - log10(C.midPoint.x)) / (log10(C.maxPoint.x) - log10(C.midPoint.x));
			int j            = knot_coord;
			float t          = knot_coord - j;

			float3 cf = {C.coefsHigh[j], C.coefsHigh[j + 1], C.coefsHigh[j + 2]};

			float3 monomials = {t * t, t, 1.0};
			logy = dot(monomials, mul(cf, M));
			
		} else /*if ( logIn >= log10(C.maxPoint.x) )*/ {
			logy = logx * C.slopeHigh + (log10(C.maxPoint.y) - C.slopeHigh * log10(C.maxPoint.x));
		}

		return pow(10, logy);
	}

	//=========================================================================================
	// Input Device Transformations
	//=========================================================================================

	//! @brief Input Device Transform[sRGB -> AP0]
	//! @param[in] sRGB: sRGB color space
	//! @return: ACES AP0 color space
	float3 IDT_sRGB_AP0(float3 sRGB) {
		float3 XYZ = mul(sRGB_2_XYZ_Matrix, sRGB);
		return mul(XYZ_2_AP0_Matrix, XYZ);
	}

	//! @brief Input Device Transform[sRGB -> AP1]
	//! @param[in] sRGB: sRGB color space
	//! @return: ACES AP1 color space
	float3 IDT_sRGB_AP1(float3 sRGB) {
		float3 XYZ = mul(sRGB_2_XYZ_Matrix, sRGB);
		return mul(XYZ_2_AP1_Matrix, XYZ);
	}

	//=========================================================================================
	// Reference Rendering Transformations
	//=========================================================================================

	//* constant variables *//

	//!< RRT Glow
	static const float RRT_GLOW_GAIN = 0.05;
	static const float RRT_GLOW_MID  = 0.08;

	//!< RRT Red Modifier
	static const float RRT_RED_SCALE = 0.82;
	static const float RRT_RED_PIVOT = 0.03;
	static const float RRT_RED_HUE   = 0;
	static const float RRT_RED_WIDTH = 135;

	//!< RRT Global Saturation
	static const float RRT_SAT_FACTOR = 0.96;

	//* methods *//

	//! @brief Reference Rendering Transform
	//! @param[in] aces: ACES AP0 color space
	//! @return: OCES color space
	float3 RRT(float3 aces_ap0) {

		float3 aces = aces_ap0;

		//!< Glow (luminance glow) correction
		float saturation = rgb_2_saturation(aces);
		float ycIn       = rgb_2_yc(aces);
		float s          = sigmoid_shaper((saturation - 0.4) / 0.2);
		float addedGlow  = 1 + glow_fwd(ycIn, RRT_GLOW_GAIN * s, RRT_GLOW_MID);
		aces *= addedGlow;

		//!< Red Modifier
		float hue         = rgb_2_hue(aces);
		float centeredHue = center_hue(hue, RRT_RED_HUE);
		float hueWeight   = cubic_basis_shaper(centeredHue, RRT_RED_WIDTH);
		aces.r += hueWeight * saturation * (RRT_RED_PIVOT - aces.r) * (1. - RRT_RED_SCALE);

		//!< ACES2065-1(AP0) -> ACEScg(AP1)
		float3 rgbPre = mul(AP0_2_AP1_Matrix, aces);
		rgbPre = clamp(rgbPre, 0, 65535);

		//!< Global desaturation
		rgbPre = lerp(dot(rgbPre, AP1_RGB2Y), rgbPre, RRT_SAT_FACTOR);

		//!< Tone mapping
		float3 rgbPost;
		rgbPost[0] = segmented_spline_c5_fwd(rgbPre[0]);
		rgbPost[1] = segmented_spline_c5_fwd(rgbPre[1]);
		rgbPost[2] = segmented_spline_c5_fwd(rgbPre[2]);

		//!< ACEScg(AP1) -> OCES
		float3 rgbOces = mul(AP1_2_AP0_Matrix, rgbPost);
		return rgbOces;
	}

	//=========================================================================================
	// Output Device Transformations
	//=========================================================================================

	//* constant variables *//

	static const float CINEMA_WHITE = 48.0;
	static const float CINEMA_BLACK = 0.02; // CINEMA_WHITE / 2400.

	//* methods *//

	// Summary :
	//  This transform is intended for mapping OCES onto a desktop computer monitor 
	//  typical of those used in motion picture visual effects production. These 
	//  monitors may occasionally be referred to as "sRGB" displays, however, the 
	//  monitor for which this transform is designed does not exactly match the 
	//  specifications in IEC 61966-2-1:1999.
	// 
	//  The assumed observer adapted white is D65, and the viewing environment is 
	//  that of a dim surround. 
	//
	//  The monitor specified is intended to be more typical of those found in 
	//  visual effects production.
	//
	// Device Primaries : 
	//  Primaries are those specified in Rec. ITU-R BT.709
	//  CIE 1931 chromaticities:  x         y         Y
	//              Red:          0.64      0.33
	//              Green:        0.3       0.6
	//              Blue:         0.15      0.06
	//              White:        0.3127    0.329     100 cd/m^2
	//
	// Display EOTF :
	//  The reference electro-optical transfer function specified in 
	//  IEC 61966-2-1:1999.
	//
	// Signal Range:
	//    This transform outputs full range code values.
	//
	// Assumed observer adapted white point:
	//         CIE 1931 chromaticities:    x            y
	//                                     0.3127       0.329
	//
	// Viewing Environment:
	//   This ODT has a compensation for viewing environment variables more typical 
	//   of those associated with video mastering.

	//! @brief Output Device Transform[OCES -> sRGB D65]
	//! @param[in] oces: OCES color space
	//! @return: sRGB D65 color space
	float3 ODT_sRGB_D65(float3 oces) {
		// OCES to RGB rendering space
		float3 rgbPre = mul(AP0_2_AP1_Matrix, oces);

		const SegmentedSplineParams_c9 ODT_48nits = {
			{-1.6989700043, -1.6989700043, -1.4779000000, -1.2291000000, -0.8648000000, -0.4480000000, 0.0051800000, 0.4511080334, 0.9113744414, 0.9113744414}, // coefsLow[10]
			{0.5154386965, 0.8470437783, 1.1358000000, 1.3802000000, 1.5197000000, 1.5985000000, 1.6467000000, 1.6746091357, 1.6878733390, 1.6878733390},       // coefsHigh[10]
			{segmented_spline_c5_fwd(0.18 * exp2(-6.5)), 0.02}, // minPoint
			{segmented_spline_c5_fwd(0.18), 4.8},               // midPoint  
			{segmented_spline_c5_fwd(0.18 * exp2(6.5)), 48.0},  // maxPoint
			0.0, // slopeLow
			0.04 // slopeHigh
		};

		// Apply the tonescale independently in rendering-space RGB
		float3 rgbPost;
		rgbPost[0] = segmented_spline_c9_fwd(rgbPre[0], ODT_48nits);
		rgbPost[1] = segmented_spline_c9_fwd(rgbPre[1], ODT_48nits);
		rgbPost[2] = segmented_spline_c9_fwd(rgbPre[2], ODT_48nits);
		
	
		// Scale luminance to linear code value
		float3 linearCV;
		linearCV[0] = Y_2_linCV(rgbPost[0], CINEMA_WHITE, CINEMA_BLACK);
		linearCV[1] = Y_2_linCV(rgbPost[1], CINEMA_WHITE, CINEMA_BLACK);
		linearCV[2] = Y_2_linCV(rgbPost[2], CINEMA_WHITE, CINEMA_BLACK);

		// Apply gamma adjustment to compensate for dim surround
		linearCV = darkSurround_to_dimSurround(linearCV);

		// Apply desaturation to compensate for luminance difference
		const float ODT_SAT_FACTOR = 0.93;
		linearCV = lerp(dot(linearCV, AP1_RGB2Y), linearCV, ODT_SAT_FACTOR);

		// Convert to display primary encoding
		// Rendering space RGB to XYZ
		float3 XYZ = mul(AP1_2_XYZ_Matrix, linearCV);
	
		// Apply CAT from ACES white point to assumed observer adapted white point
		XYZ = mul(D60_2_D65_CAT, XYZ);

		// CIE XYZ to display primaries
		linearCV = mul(XYZ_2_sRGB_Matrix, XYZ);

		// Handle out-of-gamut values
		// Clip values < 0 or > 1 (i.e. projecting outside the display primaries)
		linearCV = saturate(linearCV);

		return linearCV;
	}


	// Summary :
	//  This transform is intended for mapping OCES onto a Rec.2020 HDR display calibrated 
	//  to a D65 white point at 1000 cd/m^2. The assumed observer adapted white is 
	//  D65, and the viewing environment is that of a dim surround. 
	//
	// Device Primaries : 
	//  Primaries are those specified in Rec. ITU-R BT.2020
	//  CIE 1931 chromaticities:  x         y         Y
	//              Red:          0.708     0.292
	//              Green:        0.17      0.797
	//              Blue:         0.131     0.046
	//              White:        0.3127    0.329     1000 cd/m^2
	//
	// Display EOTF :
	//  The reference electro-optical transfer function specified in SMPTE ST 
	//  2084-2014. This transform makes no attempt to address the Annex functions 
	//  which address integer quantization.
	//
	// Assumed observer adapted white point:
	//         CIE 1931 chromaticities:    x            y
	//                                     0.3127       0.329
	//
	// Viewing Environment:
	//  This ODT is designed for a viewing environment more typically associated 
	//  with video mastering.

	//! @brief Output Device Transform[OCES -> Rec.2020 (1000 cd/m^2)]
	//! @param[in] oces: OCES color space
	//! @return: Rec.2020 (1000 cd/m^2) color space
	float3 ODT_1000nits(float3 oces) {

		// OCES to RGB rendering space
		float3 rgbPre = mul(AP0_2_AP1_Matrix, oces);

		const SegmentedSplineParams_c9 ODT_1000nits = {
			{-4.9706219331, -3.0293780669, -2.1262, -1.5105, -1.0578, -0.4668, 0.11938, 0.7088134201, 1.2911865799, 1.2911865799}, // coefsLow[10]
			{0.8089132070, 1.1910867930, 1.5683, 1.9483, 2.3083, 2.6384, 2.8595, 2.9872608805, 3.0127391195, 3.0127391195},        // coefsHigh[10]
			{segmented_spline_c5_fwd(0.18 * pow(2., -12.)), 0.0001}, // minPoint
			{segmented_spline_c5_fwd(0.18), 10.0},                   // midPoint  
			{segmented_spline_c5_fwd(0.18 * pow(2., 10.)), 1000.0},  // maxPoint
			3.0, // slopeLow
			0.06 // slopeHigh
		};

		// Apply the tonescale independently in rendering-space RGB
		float3 rgbPost;
		rgbPost[0] = segmented_spline_c9_fwd(rgbPre[0], ODT_1000nits);
		rgbPost[1] = segmented_spline_c9_fwd(rgbPre[1], ODT_1000nits);
		rgbPost[2] = segmented_spline_c9_fwd(rgbPre[2], ODT_1000nits);

		// Subtract small offset to allow for a code value of 0
		rgbPost -= 0.00003507384284432574;

		return rgbPost;
	}

	// Summary :
	//  This transform is intended for mapping OCES onto an HDR display calibrated 
	//  to a D60 white point at 2000 cd/m^2. The assumed observer adapted white is 
	//  D60, and the viewing environment is that of a dim surround. 
	//
	// Device Primaries : 
	//  CIE 1931 chromaticities:  x         y         Y
	//              Red:          0.68      0.32
	//              Green:        0.265     0.69
	//              Blue:         0.15      0.06
	//              White:        0.32168   0.33767   2000 cd/m^2
	//
	//  Also assumes a black level of 0.005 cd/m^2
	//
	// Display EOTF :
	//  The reference electro-optical transfer function specified in SMPTE ST 
	//  2084-2014. This transform makes no attempt to address the Annex functions 
	//  which address integer quantization.
	//
	// Assumed observer adapted white point:
	//         CIE 1931 chromaticities:    x            y
	//                                     0.32168      0.33767
	//
	// Viewing Environment:
	//  This ODT is designed for a viewing environment more typically associated 
	//  with video mastering.
	
	//! @brief Output Device Transform[OCES -> Rec.2020 (2000 cd/m^2)]
	//! @param[in] oces: OCES color space
	//! @return: Rec.2020 (2000 cd/m^2) color space
	float3 ODT_2000nits(float3 oces) {
		// OCES to RGB rendering space
		float3 rgbPre = mul(AP0_2_AP1_Matrix, oces);

		const SegmentedSplineParams_c9 ODT_2000nits = {
			{-2.3010299957, -2.3010299957, -1.9312000000, -1.5205000000, -1.0578000000, -0.4668000000, 0.1193800000, 0.7088134201, 1.2911865799, 1.2911865799}, // coefsLow[10]
			{0.8019952042, 1.1980047958, 1.5943000000, 1.9973000000, 2.3783000000, 2.7684000000, 3.0515000000, 3.2746293562, 3.3274306351, 3.3274306351},       // coefsHigh[10]
			{segmented_spline_c5_fwd(0.18 * pow(2., -12.)), 0.005}, // minPoint
			{segmented_spline_c5_fwd(0.18), 10.0},                  // midPoint
			{segmented_spline_c5_fwd(0.18 * pow(2., 11.)), 2000.0}, // maxPoint
			0.0, // slopeLow
			0.12 // slopeHigh
		};

		// Apply the tonescale independently in rendering-space RGB
		float3 rgbPost;
		rgbPost[0] = segmented_spline_c9_fwd(rgbPre[0], ODT_2000nits);
		rgbPost[1] = segmented_spline_c9_fwd(rgbPre[1], ODT_2000nits);
		rgbPost[2] = segmented_spline_c9_fwd(rgbPre[2], ODT_2000nits);

		return rgbPost;
	}
	
}

/* ACES workflow
 sRGB Linear
	↓ IDT (Input Device Transform)
 ACEScg or ACES2065-1 [main work space]
	↓ RRT（Reference Rendering Transform）
 OCES（Output Color Encoding Space）
	↓ ODT（Output Device Transform）
 Display RGB（sRGB, HDR, etc.）
*/

#endif //!< ACES_HLSL_INCLUDED

