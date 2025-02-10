#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// Material namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Material {

	SamplerState defaultSampler;

	////////////////////////////////////////////////////////////////////////////////////////////
	// Albedo structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Albedo {

		//* members *//
		
		uint   type;
		//!< 0: float3 color, 1: texture
		
		float3 color;
		uint   index;

		//* methods *//

		float3 GetAlbedo(float2 texcoord = float2(0.0f, 0.0f), SamplerState samplers = defaultSampler) {
			if (type == 0) {
				return color;
				
			} else if (type == 1) {
				Texture2D<float4> texture = ResourceDescriptorHeap[index];
				return texture.Sample(samplers, texcoord).rgb;
			}

			return (float3)0; //!< exception

		}
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Transparency structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Transparency {

		//* members *//

		uint type;
		//!< 0: float value, 1: texture

		float alpha;
		uint  index;

		//* methods *//

		float GetTrnasparency(float2 texcoord = float2(0.0f, 0.0f), SamplerState samplers = defaultSampler) {
			if (type == 0) {
				return alpha;
				
			} else if (type == 1) {
				Texture2D<float4> texture = ResourceDescriptorHeap[index];
				return texture.Sample(samplers, texcoord).r;
			}

			return 0.0f; //!< exception
		}
		
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Normal structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Normal {
		//* todo: bitangent process

		//* members *//

		uint type;
		//!< 0: none, 1: texture

		uint index;

		//* methods *//

		float3 GetNormal(float3 normal, float2 texcoord = float2(0.0f, 0.0f), SamplerState samplers = defaultSampler) {
			if (type == 0) {
				return normal;
				
			} else if (type == 1) {
				Texture2D<float4> texture = ResourceDescriptorHeap[index];
				return texture.Sample(defaultSampler, texcoord.xy).xyz;
			}

			return (float3)0; //!< exception

		}
		
	};

	// todo: metallic, specular, roughness
	
}

////////////////////////////////////////////////////////////////////////////////////////////
// MaterialComponent structure
////////////////////////////////////////////////////////////////////////////////////////////
struct MaterialComponent {
	Material::Albedo       albedo;
	Material::Transparency transparency;
	Material::Normal       normal;
};
