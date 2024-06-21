#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// directX
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxcapi.h>

// c++
#include <cstdint>
#include <string>
#include <array>

#include <ComPtr.h>

// DxObject
#include <DxObjectMethod.h>
#include <DxShaderManager.h>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxcompiler.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// DxObject namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace DxObject {

	////////////////////////////////////////////////////////////////////////////////////////////
	// GraphicShaderType enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum GraphicShaderType { //!< DxShaderManagerのShaderTypeと合わせること
		GRAPHICS_VERTEX,
		GRAPHICS_GEOMETRY,
		GRAPHICS_MESH,

		GRAPHICS_PIXEL,

		kCountOfGraphicShaderType
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// GraphicBlob class 
	////////////////////////////////////////////////////////////////////////////////////////////
	class GraphicsBlob {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		GraphicsBlob() = default;

		~GraphicsBlob() { Term(); }

		void Create(const std::wstring& fileName, GraphicShaderType type);

		void Term();

		const std::array<IDxcBlob*, kCountOfGraphicShaderType>& GetGraphicsBlobs() const { return graphicsBlobs_; }

		bool IsUseMeshPipeline() const { return isUseMeshPipeline_; }

		static void SetShaderManager(ShaderManager* manager) { manager_ = manager; }

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		/* shaderManager */
		static ShaderManager* manager_;

		/* blob container */
		std::array<IDxcBlob*, kCountOfGraphicShaderType> graphicsBlobs_ = { nullptr };

		/* parameter */
		bool isUseMeshPipeline_ = false; //!< mesh shader, amp shaderを使う際はpipelineが変わるのでtrueになる。

	};

}