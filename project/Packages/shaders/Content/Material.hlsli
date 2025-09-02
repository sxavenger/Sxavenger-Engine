#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../Library/ACES.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// MaterialData namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace MaterialLib {

	////////////////////////////////////////////////////////////////////////////////////////////
	// common methods
	////////////////////////////////////////////////////////////////////////////////////////////

	float4 SampleTexture(Texture2D<float4> texture, SamplerState sample, float2 texcoord) {
#ifdef _COMPUTE
		return texture.SampleLevel(sample, texcoord, 0);
#else
		return texture.Sample(sample, texcoord);
#endif
		//!< - pixel shader: texture.Sample(_samples, _texcoord)
		//!< - compute shader: texture.SampleLevel(_samples, _texcoord, 0)
	}
	
	////////////////////////////////////////////////////////////////////////////////////////////
	// TextureSampler structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct TextureSampler { //!< helper structure.

		//=========================================================================================
		// public variables
		//=========================================================================================

		SamplerState samplers;
		float2 texcoord;

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Set(float2 _texcoord, SamplerState _samplers) {
			texcoord = _texcoord;
			samplers = _samplers;
		}
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Albedo structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Albedo {

		//=========================================================================================
		// public variables
		//=========================================================================================
		
		uint type;
		//!< 0: float3 color, 1: texture, 2: multiply
		
		float3 color;
		uint index;

		//=========================================================================================
		// public methods
		//=========================================================================================

		float3 GetAlbedo(TextureSampler parameter) {

			float3 output = float3(0.0f, 0.0f, 0.0f);

			switch (type) {
				case 0:
					output = color;
					break;
				
				case 1:
					{
						Texture2D<float4> texture = ResourceDescriptorHeap[index];
						output = SampleTexture(texture, parameter.samplers, parameter.texcoord).rgb;
					}
					break;
				
				case 2:
					{
						Texture2D<float4> texture = ResourceDescriptorHeap[index];
						output = SampleTexture(texture, parameter.samplers, parameter.texcoord).rgb * color;
					}
					break;
			}

			return ACES::IDT_sRGB_AP1(output);
		}
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Transparency structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Transparency {

		//=========================================================================================
		// public variables
		//=========================================================================================

		uint type;
		//!< 0: float value, 1: texture

		float alpha;
		uint index;

		//=========================================================================================
		// public methods
		//=========================================================================================

		float GetTransparency(TextureSampler parameter) {
			if (type == 0) {
				return alpha;
				
			} else if (type == 1) {
				Texture2D<float4> texture = ResourceDescriptorHeap[index];
				return SampleTexture(texture, parameter.samplers, parameter.texcoord).a;
			}

			return 0.0f; //!< exception
		}
		
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Normal structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Normal {

		//=========================================================================================
		// public variables
		//=========================================================================================

		uint type;
		//!< 0: none, 1: texture

		uint index;

		//=========================================================================================
		// public methods
		//=========================================================================================

		float3 GetNormal(float3 normal, TextureSampler parameter, float3x3 tbn = (float3x3)0) {
			if (type == 0) {
				return normal;
				
			} else if (type == 1) {
				Texture2D<float4> texture = ResourceDescriptorHeap[index];
				float3 map = SampleTexture(texture, parameter.samplers, parameter.texcoord).xyz;
				return normalize(mul(map * 2.0f - 1.0f, tbn)); //!< fix...? test plz.
			}

			return (float3)0; //!< exception
		}
		
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Property structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Property { //!< helper structure.
			
		uint type;
		//!< 0: float value, 1: texture

		float value;
		
		uint index;

		float GetValue(TextureSampler parameter, uint channel) {
			if (type == 0) {
				return value;
				
			} else if (type == 1) {
				Texture2D<float4> texture = ResourceDescriptorHeap[index];
				return SampleTexture(texture, parameter.samplers, parameter.texcoord)[channel];
			}

			return 0.0f; //!< exception
		}
			
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// SurfaceProperties structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct SurfaceProperties {

		//=========================================================================================
		// public variables
		//=========================================================================================

		Property ao;
		Property roughness;
		Property metallic;
		
	};

	// todo: metallic, specular, roughness

	////////////////////////////////////////////////////////////////////////////////////////////
	// Transform structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Transform {
		
		//=========================================================================================
		// public variables
		//=========================================================================================

		float4x4 mat;

		//=========================================================================================
		// public methods
		//=========================================================================================

		float2 Transformation(float2 texcoord) {
			return mul(float4(texcoord, 0.0f, 1.0f), mat).xy;
		}
		
	};
	
}

////////////////////////////////////////////////////////////////////////////////////////////
// Material structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Material {

	//=========================================================================================
	// public variables
	//=========================================================================================

	MaterialLib::Transform transform;
	MaterialLib::Albedo albedo;
	MaterialLib::Transparency transparency;
	MaterialLib::Normal normal;
	MaterialLib::SurfaceProperties properties;
	
};
