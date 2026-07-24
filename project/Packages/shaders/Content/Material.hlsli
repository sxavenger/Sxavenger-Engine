#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* library
#include "../Library/ACES.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// MaterialLib namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace MaterialLib { //!< [AssetMaterial.h] として合わせる.

	////////////////////////////////////////////////////////////////////////////////////////////
	// utility methods
	////////////////////////////////////////////////////////////////////////////////////////////

	Texture2D<float4> GetTexture(uint index) {
		return ResourceDescriptorHeap[index];
	}

	float4 SampleTexture(Texture2D<float4> texture, SamplerState sample, float2 texcoord) {
#ifdef _COMPUTE_SHADER
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

		static TextureSampler Create(float2 texcoord, SamplerState sample) {
			TextureSampler output;
			output.texcoord = texcoord;
			output.samplers = sample;
			return output;
		}
		
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// UVTransformation structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct UVTransformation {

		//=========================================================================================
		// public variables
		//=========================================================================================

		float4x4 mat;

		//=========================================================================================
		// public methods
		//=========================================================================================

		float2 Transform(float2 texcoord) {
			return mul(float4(texcoord, 0, 1), mat).xy;
		}
		
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Albedo structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Albedo {

		//=========================================================================================
		// public variables
		//=========================================================================================

		float3 color;
		uint index;

		//=========================================================================================
		// public methods
		//=========================================================================================

		float3 GetAlbedo(TextureSampler parameter, bool useTexture) {

			float3 output = color;

			if (useTexture) {
				Texture2D<float4> texture = GetTexture(index);
				output *= SampleTexture(texture, parameter.samplers, parameter.texcoord).rgb;
				//!< 乗算を前提とする.
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

		float value;
		uint index;

		//=========================================================================================
		// public methods
		//=========================================================================================

		float GetTransparency(TextureSampler parameter, bool useTexture) {
			float output = value;
			
			if (useTexture) {
				Texture2D<float4> texture = GetTexture(index);
				output *= SampleTexture(texture, parameter.samplers, parameter.texcoord).a;
				//!< 乗算を前提とする.
			}
			
			return output;
		}
		
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Normal structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Normal {

		//=========================================================================================
		// public variables
		//=========================================================================================

		uint index;

		//=========================================================================================
		// public methods
		//=========================================================================================

		float3 GetNormal(float3 normal, float3 tangent, float3 bitangent, TextureSampler parameter, bool useTexture) {
			if (!useTexture) {
				return normal; //!< 法線マップを使用しない場合は, 通常の法線を返す.
			}

			float3x3 tbn = float3x3(tangent, bitangent, normal);

			Texture2D<float4> texture = GetTexture(index);
			float3 map = SampleTexture(texture, parameter.samplers, parameter.texcoord).rgb;
			return normalize(mul(map * 2.0f - 1.0f, tbn));
		}
		
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Property structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Property { //!< Roughness, Metallicとして使用.

		//=========================================================================================
		// public variables
		//=========================================================================================

		float value;
		uint index;

		//=========================================================================================
		// public methods
		//=========================================================================================

		float GetProperty(TextureSampler parameter, bool useTexture, uint channel) {
			float output = value;
			
			if (useTexture) {
				Texture2D<float4> texture = GetTexture(index);
				output = SampleTexture(texture, parameter.samplers, parameter.texcoord)[channel];
				//!< テクスチャの値を優先する.
			}
			
			return output;
		}

	};

	
	////////////////////////////////////////////////////////////////////////////////////////////
	// Emissive structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Emissive {

		//=========================================================================================
		// public variables
		//=========================================================================================

		float3 color;
		uint index;
		float intencity;

		//=========================================================================================
		// public methods
		//=========================================================================================

		float3 GetEmissive(TextureSampler parameter, bool useTexture) {
			float3 output = color;
			
			if (useTexture) {
				Texture2D<float4> texture = GetTexture(index);
				output *= SampleTexture(texture, parameter.samplers, parameter.texcoord).rgb;
				//!< 乗算を前提とする.
			}
			
			return ACES::IDT_sRGB_AP1(output * intencity);
		}
		
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// TextureFlag namespace 
	////////////////////////////////////////////////////////////////////////////////////////////
	namespace TextureFlag {
		static const uint Albedo       = 1 << 0,
		                  Transparency = 1 << 1,
		                  Normal       = 1 << 2,
		                  Roughness    = 1 << 3,
		                  Metallic     = 1 << 4,
		                  Emissive     = 1 << 5;
	}
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// Material structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Material { //!< [AssetMaterial.h] として合わせる.

	//=========================================================================================
	// public variables
	//=========================================================================================

	MaterialLib::UVTransformation transformation;
	MaterialLib::Albedo albedo;
	MaterialLib::Property roughness;
	MaterialLib::Property metallic;
	MaterialLib::Transparency transparency;
	MaterialLib::Normal normal;
	MaterialLib::Emissive emissive;

	uint flags;

	//=========================================================================================
	// public methods
	//=========================================================================================

	bool CheckFlag(uint flag) {
		return (flags & flag) != 0;
	}

	MaterialLib::TextureSampler CreateTransformedSampler(float2 texcoord, SamplerState sample) {
		return MaterialLib::TextureSampler::Create(transformation.Transform(texcoord), sample);
	}

	float3 GetAlbedo(MaterialLib::TextureSampler parameter) {
		return albedo.GetAlbedo(parameter, CheckFlag(MaterialLib::TextureFlag::Albedo));
	}

	float GetTransparency(MaterialLib::TextureSampler parameter) {
		return transparency.GetTransparency(parameter, CheckFlag(MaterialLib::TextureFlag::Transparency));
	}

	float GetRoughness(MaterialLib::TextureSampler parameter) {
		return roughness.GetProperty(parameter, CheckFlag(MaterialLib::TextureFlag::Roughness), 1);
		//!< ARMテクスチャ想定. (roughnessはgreenチャンネル)
	}

	float GetMetallic(MaterialLib::TextureSampler parameter) {
		return metallic.GetProperty(parameter, CheckFlag(MaterialLib::TextureFlag::Metallic), 2);
		//!< ARMテクスチャ想定. (metallicはblueチャンネル)
	}

	float3 GetNormal(float3 _normal, float3 _tangent, float3 _bitangent, MaterialLib::TextureSampler parameter) {
		return normal.GetNormal(_normal, _tangent, _bitangent, parameter, CheckFlag(MaterialLib::TextureFlag::Normal));
	}

	float3 GetEmissive(MaterialLib::TextureSampler parameter) {
		return emissive.GetEmissive(parameter, CheckFlag(MaterialLib::TextureFlag::Emissive));
	}

};
