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
// DxObjectMethod
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
		ComPtr<ID3D12Device> device,
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
	//! @return shaderBlopを返却
	IDxcBlob* CompilerShader(
		const std::wstring& filePath,
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
		ComPtr<ID3D12Device> device,
		size_t sizeInBytes
	);

	//! @brief 深度バッファを作成
	//! 
	//! @param[in] device デバイス
	//! @param[in] width  画面の縦幅
	//! @param[in] height 画面の横幅
	//! 
	//! @return 深度バッファResourceを返却
	ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(
		ComPtr<ID3D12Device> device,
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