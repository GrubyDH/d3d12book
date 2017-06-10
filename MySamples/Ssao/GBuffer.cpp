#include "GBuffer.h"

GBuffer::GBuffer(ID3D12Device * device, ID3D12GraphicsCommandList * cmdList, UINT width, UINT height)
{
	md3dDevice = device;

	OnResize(width, height);
}

ID3D12Resource * GBuffer::NormalMap()
{
	return mNormalMap.Get();
}

ID3D12Resource* GBuffer::DiffuseMap()
{
	return mDiffuseMap.Get();
}

CD3DX12_CPU_DESCRIPTOR_HANDLE GBuffer::NormalMapRtv() const
{
	return mhNormalMapCpuRtv;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE GBuffer::NormalMapSrv() const
{
	return mhNormalMapGpuSrv;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE GBuffer::DiffuseMapRtv() const
{
	return mhDiffuseMapCpuRtv;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE GBuffer::DiffuseMapSrv() const
{
	return mhDiffuseMapGpuSrv;
}

void GBuffer::BuildDescriptors(ID3D12Resource * depthStencilBuffer, CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv, CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuRtv, UINT cbvSrvUavDescriptorSize, UINT rtvDescriptorSize)
{
	// Save references to the descriptors.  The GBuffer reserves heap space
	// for 3 contiguous Srvs.

	mhDepthMapCpuSrv = hCpuSrv;
	mhNormalMapCpuSrv = hCpuSrv.Offset(1, cbvSrvUavDescriptorSize);
	mhDiffuseMapCpuSrv = hCpuSrv.Offset(1, cbvSrvUavDescriptorSize);

	mhDepthMapGpuSrv = hGpuSrv;
	mhNormalMapGpuSrv = hGpuSrv.Offset(1, cbvSrvUavDescriptorSize);
	mhDiffuseMapGpuSrv = hGpuSrv.Offset(1, cbvSrvUavDescriptorSize);

	mhNormalMapCpuRtv = hCpuRtv;
	mhDiffuseMapCpuRtv = hCpuRtv.Offset(1, rtvDescriptorSize);

	//  Create the descriptors
	RebuildDescriptors(depthStencilBuffer);
}

void GBuffer::RebuildDescriptors(ID3D12Resource * depthStencilBuffer)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = GBuffer::NormalMapFormat;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	md3dDevice->CreateShaderResourceView(mNormalMap.Get(), &srvDesc, mhNormalMapCpuSrv);

	srvDesc.Format = GBuffer::DiffuseMapFormat;
	md3dDevice->CreateShaderResourceView(mDiffuseMap.Get(), &srvDesc, mhDiffuseMapCpuSrv);

	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	md3dDevice->CreateShaderResourceView(depthStencilBuffer, &srvDesc, mhDepthMapCpuSrv);

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = GBuffer::NormalMapFormat;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.Texture2D.PlaneSlice = 0;
	md3dDevice->CreateRenderTargetView(mNormalMap.Get(), &rtvDesc, mhNormalMapCpuRtv);

	rtvDesc.Format = GBuffer::DiffuseMapFormat;
	md3dDevice->CreateRenderTargetView(mDiffuseMap.Get(), &rtvDesc, mhDiffuseMapCpuRtv);
}

void GBuffer::OnResize(UINT newWidth, UINT newHeight)
{
	if (mRenderTargetWidth != newWidth || mRenderTargetHeight != newHeight)
	{
		mRenderTargetWidth = newWidth;
		mRenderTargetHeight = newHeight;

		BuildResources();
	}
}

void GBuffer::BuildResources()
{
	// Release current resources.
	mNormalMap = nullptr;
	mDiffuseMap = nullptr;

	CD3DX12_RESOURCE_DESC texDesc = {};
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = mRenderTargetWidth;
	texDesc.Height = mRenderTargetHeight;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = GBuffer::NormalMapFormat;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	float normalClearColor[] = { 0.0f, 0.0f, 1.0f, 0.0f };
	CD3DX12_CLEAR_VALUE optClear(GBuffer::NormalMapFormat, normalClearColor);
	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&optClear,
		IID_PPV_ARGS(&mNormalMap)));

	texDesc.Format = GBuffer::DiffuseMapFormat;
	float diffuseClearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	optClear = CD3DX12_CLEAR_VALUE(GBuffer::DiffuseMapFormat, diffuseClearColor);

	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&optClear,
		IID_PPV_ARGS(&mDiffuseMap)));
}
