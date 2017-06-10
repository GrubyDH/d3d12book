#pragma once

#include "../../Common/d3dUtil.h"

class GBuffer {
public:
	GBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, UINT width, UINT height);
	GBuffer(const GBuffer& rhs) = delete;
	GBuffer& operator=(const GBuffer& rhs) = delete;
	~GBuffer() = default;

	static const DXGI_FORMAT NormalMapFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
	static const DXGI_FORMAT DiffuseMapFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	ID3D12Resource* NormalMap();
	ID3D12Resource* DiffuseMap();

	CD3DX12_CPU_DESCRIPTOR_HANDLE NormalMapRtv() const;
	CD3DX12_GPU_DESCRIPTOR_HANDLE NormalMapSrv() const;
	CD3DX12_CPU_DESCRIPTOR_HANDLE DiffuseMapRtv() const;
	CD3DX12_GPU_DESCRIPTOR_HANDLE DiffuseMapSrv() const;

	void BuildDescriptors(
		ID3D12Resource* depthStencilBuffer,
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
		CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuRtv,
		UINT cbvSrvUavDescriptorSize,
		UINT rtvDescriptorSize);

	void RebuildDescriptors(ID3D12Resource* depthStencilBuffer);

	// Call when the backbuffer is resized.
	void OnResize(UINT newWidth, UINT newHeight);

	void BuildResources();

private:
	ID3D12Device* md3dDevice;

	Microsoft::WRL::ComPtr<ID3D12Resource> mNormalMap;
	Microsoft::WRL::ComPtr<ID3D12Resource> mDiffuseMap;

	CD3DX12_CPU_DESCRIPTOR_HANDLE mhDepthMapCpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mhDepthMapGpuSrv;

	CD3DX12_CPU_DESCRIPTOR_HANDLE mhNormalMapCpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mhNormalMapGpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhNormalMapCpuRtv;

	CD3DX12_CPU_DESCRIPTOR_HANDLE mhDiffuseMapCpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mhDiffuseMapGpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhDiffuseMapCpuRtv;

	UINT mRenderTargetWidth = 0;
	UINT mRenderTargetHeight = 0;
};
