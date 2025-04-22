#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// MaterialData namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace MaterialLib {

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

		//!< HACK: texture sample functions.
		//!< - pixel shader: texture.Sample(_samples, _texcoord)
		//!< - compute shader: texture.SampleLevel(_samples, _texcoord, 0)
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
			switch (type) {
				case 0:
					return color;
				
				case 1:{
						Texture2D<float4> texture = ResourceDescriptorHeap[index];
						return texture.SampleLevel(parameter.samplers, parameter.texcoord, 0).rgb;
					}
				
				case 2:{
						Texture2D<float4> texture = ResourceDescriptorHeap[index];
						return texture.SampleLevel(parameter.samplers, parameter.texcoord, 0).rgb * color;
					}
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

		float GetTransparency(TextureSampler parameter) {
			if (type == 0) {
				return alpha;
				
			} else if (type == 1) {
				Texture2D<float4> texture = ResourceDescriptorHeap[index];
				return texture.SampleLevel(parameter.samplers, parameter.texcoord, 0).a;
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
				float3 map = texture.SampleLevel(parameter.samplers, parameter.texcoord, 0).xyz;
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

		float GetValue(TextureSampler parameter, uint channel = 0) {
			if (type == 0) {
				return value;
				
			} else if (type == 1) {
				Texture2D<float4> texture = ResourceDescriptorHeap[index];
				return texture.SampleLevel(parameter.samplers, parameter.texcoord, 0)[channel];
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
