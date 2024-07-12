#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// directX
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxcapi.h>
#include <d3d12shader.h>

// c++
#include <cstdint>

// ComPtr
#include <ComPtr.h>

// DxObject
#include <DxObjectMethod.h>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxcompiler.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// DxObject namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace DxObject { //!< test obejct

	////////////////////////////////////////////////////////////////////////////////////////////
	// VoidBufferResouce class
	////////////////////////////////////////////////////////////////////////////////////////////
	class VoidBufferResouce { //!< test resource
	public:

		VoidBufferResouce(uint32_t indexSize, uint32_t bufferSize) {
			Init(indexSize, bufferSize);
		}

		~VoidBufferResouce() { Term(); }

		void Init(uint32_t indexSize, uint32_t bufferSize);

		void Term();

		template <typename T>
		void SetValue(uint32_t index, uint32_t offset, const T& value);

	private:

		ComPtr<ID3D12Resource> resource_;
		void** map_ = nullptr;

		uint32_t indexSize_;

	};

	template<typename T>
	inline void VoidBufferResouce::SetValue(uint32_t index, uint32_t offset, const T& value) {

		assert(index < indexSize_);

		reinterpret_cast<float*>(reinterpret_cast<char*>(map_[index]) + offset) = value;
	}

	//-----------------------------------------------------------------------------------------
	// forward
	//-----------------------------------------------------------------------------------------
	class ShaderManager;

	////////////////////////////////////////////////////////////////////////////////////////////
	// ShaderReflection class
	////////////////////////////////////////////////////////////////////////////////////////////
	class ShaderReflection {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		ShaderReflection() = default;

		~ShaderReflection() { Term(); }

		void Init(IDxcBlob* blob);

		void Term();

		static void SetShaderManager(ShaderManager* manager) { manager_ = manager; }

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		/* externals */
		static ShaderManager* manager_;

		ComPtr<ID3D12ShaderReflection> reflection_;

	};

}