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
#include <cassert>

// ComPtr
#include <ComPtr.h>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxcompiler.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// DxObjectMethod namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace DxObjectMethod {

	//! @brief DescriptorHeapの生成
	//! 
	//! @param[in] device         デバイス
	//! @param[in] heapType       type
	//! @param[in] numDescriptors ディスクリプタ数
	//! @param[in] shaderVisible  shader内で使用するか
	//! 
	//! @return DesriptorHeapの返却
	ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(
		ID3D12Device* device,
		D3D12_DESCRIPTOR_HEAP_TYPE heapType,
		UINT numDescriptors,
		bool shaderVisible
	);

	//! @brief シェーダーのコンパイル
	//! 
	//! @param[in] filePath       shaderファイルパス
	//! @param[in] profile        Compilerに使用するProfile
	//! @param[in] dxcUtils       IDxcUtils*
	//! @param[in] dxcCompiler    IDxcCompiler3*
	//! @param[in] includeHandler IDxcIncludeHandler*
	//! 
	//! @return shaderBlobを返却
	ComPtr<IDxcBlob> CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile,
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler
	);

	//! @brief シェーダーのコンパイル
	//! 
	//! @param[in] filePath       shaderファイルパス
	//! @param[in] entryPoint     main関数名
	//! @param[in] profile        Compilerに使用するProfile
	//! @param[in] dxcUtils       IDxcUtils*
	//! @param[in] dxcCompiler    IDxcCompiler3*
	//! @param[in] includeHandler IDxcIncludeHandler*
	//! 
	//! @return shaderBlopを返却
	ComPtr<IDxcBlob> CompileShader(
		const std::wstring& filePath,
		const std::wstring& entryPoint,
		const wchar_t* profile,
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler
	);

	//! @brief バッファ確保したResourceを生成
	//! 
	//! @param[in] device      ID3D12Device
	//! @param[in] sizeInBytes バイトサイズ
	//! 
	//! @return バッファ確保したResourceを返却
	ComPtr<ID3D12Resource> CreateBufferResource(
		ID3D12Device* device,
		size_t sizeInBytes
	);

	ComPtr<ID3D12RootSignature> CreateRootSignature(
		ID3D12Device* device,
		const D3D12_ROOT_SIGNATURE_DESC& desc
	);

	//! @brief 深度バッファを作成
	//! 
	//! @param[in] device デバイス
	//! @param[in] width  画面の縦幅
	//! @param[in] height 画面の横幅
	//! 
	//! @return 深度バッファResourceを返却
	ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(
		ID3D12Device* device,
		int32_t width,
		int32_t height
	);

	//! @brief descriptorHeapのindex数のhandleCPUを取得
	//! 
	//! @param[in] descriptorHeap
	//! @param[in] descriptorSize
	//! @param[in] index
	//! 
	//! @return descriptorHeapのindex数のhandleCPUを返却
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(
		ID3D12DescriptorHeap* descriptorHeap,
		uint32_t descriptorSize,
		uint32_t index
	);

	//! @brief descriptorHeapのindex数のhandleGPUを取得
	//! 
	//! @param[in] descriptorHeap
	//! @param[in] descriptorSize
	//! @param[in] index
	//! 
	//! @return descriptorHeapのindex数のhandleGPUを返却
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(
		ID3D12DescriptorHeap* descriptorHeap,
		uint32_t descriptorSize,
		uint32_t index
	);
}

////////////////////////////////////////////////////////////////////////////////////////////
// DxObject namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace DxObject {

	////////////////////////////////////////////////////////////////////////////////////////////
	// DescriptorType enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum DescriptorType {
		RTV, //!< RenderTargetView
		SRV, //!< ShaderResourceView
		DSV, //!< DepthStencilView

		kDescriptorHeapCount,
		NotUseDescriptor,

		// other //
		UAV = SRV,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Descriptor structrure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Descriptor {
		uint32_t index;
		DescriptorType type = NotUseDescriptor;

		D3D12_CPU_DESCRIPTOR_HANDLE handleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE handleGPU;

		void Term() {
			index = NULL;
			type = NotUseDescriptor;
		}
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// TextureMode enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum TextureMode {
		MODE_WRAP        = 1,
		MODE_MIRROR      = 2,
		MODE_CLAMP       = 3,
		MODE_BORDER      = 4,
		MODE_MIRROR_ONCE = 5
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Configs
	////////////////////////////////////////////////////////////////////////////////////////////
	static const DXGI_FORMAT defaultFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	static const DXGI_FORMAT defFor[1] = { DXGI_FORMAT_R8G8B8A8_UNORM_SRGB };

}