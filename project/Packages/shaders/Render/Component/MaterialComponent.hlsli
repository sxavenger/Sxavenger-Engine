#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// Material namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Material {

	////////////////////////////////////////////////////////////////////////////////////////////
	// TextureParameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct TextureParameter { //!< helper structure.

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
		//!< 0: float3 color, 1: texture
		
		float3 color;
		uint index;

		//=========================================================================================
		// public methods
		//=========================================================================================

		float3 GetAlbedo(TextureParameter parameter) {
			if (type == 0) {
				return color;
				
			} else if (type == 1) {
				Texture2D<float4> texture = ResourceDescriptorHeap[index];
				return texture.Sample(parameter.samplers, parameter.texcoord).rgb;
			}

			return (float3)0; //!< exception
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

		float GetTransparency(TextureParameter parameter) {
			if (type == 0) {
				return alpha;
				
			} else if (type == 1) {
				Texture2D<float4> texture = ResourceDescriptorHeap[index];
				return texture.Sample(parameter.samplers, parameter.texcoord).r;
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

		float3 GetNormal(float3 normal, TextureParameter parameter, float3x3 tbn = (float3x3)0) {
			if (type == 0) {
				return normal;
				
			} else if (type == 1) {
				Texture2D<float4> texture = ResourceDescriptorHeap[index];
				float3 map = texture.Sample(parameter.samplers, parameter.texcoord).xyz;
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

		float GetValue(TextureParameter parameter) {
			if (type == 0) {
				return value;
				
			} else if (type == 1) {
				Texture2D<float4> texture = ResourceDescriptorHeap[index];
				return texture.Sample(parameter.samplers, parameter.texcoord).r;
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

		Property metallic;
		Property specular;
		Property roughness;
		
	};

	// todo: metallic, specular, roughness
	
}

////////////////////////////////////////////////////////////////////////////////////////////
// MaterialComponent structure
////////////////////////////////////////////////////////////////////////////////////////////
struct MaterialComponent {

	//=========================================================================================
	// public variables
	//=========================================================================================
	
	Material::Albedo albedo;
	Material::Transparency transparency;
	Material::Normal normal;
	Material::SurfaceProperties properties;
	
};
