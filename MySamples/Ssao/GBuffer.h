#pragma once

#include "../../Common/d3dUtil.h"

class GBuffer {
public:
	GBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, UINT width, UINT height);
	GBuffer(const GBuffer& rhs) = delete;
	GBuffer& operator=(const GBuffer& rhs) = delete;
	~GBuffer() = default;

    static const DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	static const DXGI_FORMAT NormalMapFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
	static const DXGI_FORMAT DiffuseAlbedoFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    static const DXGI_FORMAT BumpMapFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    static const DXGI_FORMAT FresnelR0RoughnessFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;

    ID3D12Resource* DepthStencilBuffer();
    ID3D12Resource* NormalMap();
    ID3D12Resource* DiffuseAlbedo();
    ID3D12Resource* BumpMap();
    ID3D12Resource* FresnelR0Roughness();

    CD3DX12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;
	CD3DX12_CPU_DESCRIPTOR_HANDLE NormalMapRtv() const;
	CD3DX12_GPU_DESCRIPTOR_HANDLE NormalMapSrv() const;
	CD3DX12_CPU_DESCRIPTOR_HANDLE DiffuseAlbedoRtv() const;
	CD3DX12_GPU_DESCRIPTOR_HANDLE DiffuseAlbedoSrv() const;
    CD3DX12_CPU_DESCRIPTOR_HANDLE BumpMapRtv() const;
    CD3DX12_GPU_DESCRIPTOR_HANDLE BumpMapSrv() const;
    CD3DX12_CPU_DESCRIPTOR_HANDLE FresnelR0RoughnessRtv() const;
    CD3DX12_GPU_DESCRIPTOR_HANDLE FresnelR0RoughnessSrv() const;

	void BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
		CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuRtv,
        CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDsv,
		UINT cbvSrvUavDescriptorSize,
		UINT rtvDescriptorSize);

	void RebuildDescriptors();

	// Call when the backbuffer is resized.
	void OnResize(UINT newWidth, UINT newHeight);

	void BuildResources();

private:
	ID3D12Device* md3dDevice;

    Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> mNormalMap;
	Microsoft::WRL::ComPtr<ID3D12Resource> mDiffuseAlbedo;
    Microsoft::WRL::ComPtr<ID3D12Resource> mBumpMap;
    Microsoft::WRL::ComPtr<ID3D12Resource> mFresnelR0AndRoughness;

	CD3DX12_CPU_DESCRIPTOR_HANDLE mhDepthMapCpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mhDepthMapGpuSrv;
    CD3DX12_CPU_DESCRIPTOR_HANDLE mhDepthMapCpuDsv;

	CD3DX12_CPU_DESCRIPTOR_HANDLE mhNormalMapCpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mhNormalMapGpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhNormalMapCpuRtv;

	CD3DX12_CPU_DESCRIPTOR_HANDLE mhDiffuseAlbedoCpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mhDiffuseAlbedoGpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhDiffuseAlbedoCpuRtv;

    CD3DX12_CPU_DESCRIPTOR_HANDLE mhBumpMapCpuSrv;
    CD3DX12_GPU_DESCRIPTOR_HANDLE mhBumpMapGpuSrv;
    CD3DX12_CPU_DESCRIPTOR_HANDLE mhBumpMapCpuRtv;

    CD3DX12_CPU_DESCRIPTOR_HANDLE mhFresnelR0AndRoughnessCpuSrv;
    CD3DX12_GPU_DESCRIPTOR_HANDLE mhFresnelR0AndRoughnessGpuSrv;
    CD3DX12_CPU_DESCRIPTOR_HANDLE mhFresnelR0AndRoughnessCpuRtv;

	UINT mRenderTargetWidth = 0;
	UINT mRenderTargetHeight = 0;
};
