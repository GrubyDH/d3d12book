#include "GBuffer.h"

GBuffer::GBuffer(ID3D12Device * device, ID3D12GraphicsCommandList * cmdList, UINT width, UINT height)
{
	md3dDevice = device;

	OnResize(width, height);
}

ID3D12Resource * GBuffer::DepthStencilBuffer()
{
    return mDepthStencilBuffer.Get();
}

ID3D12Resource * GBuffer::NormalMap()
{
	return mNormalMap.Get();
}

ID3D12Resource* GBuffer::DiffuseAlbedo()
{
	return mDiffuseAlbedo.Get();
}

ID3D12Resource * GBuffer::BumpMap()
{
    return mBumpMap.Get();
}

ID3D12Resource * GBuffer::FresnelR0Roughness()
{
    return mFresnelR0AndRoughness.Get();
}

CD3DX12_CPU_DESCRIPTOR_HANDLE GBuffer::DepthStencilView() const
{
    return mhDepthMapCpuDsv;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE GBuffer::NormalMapRtv() const
{
	return mhNormalMapCpuRtv;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE GBuffer::NormalMapSrv() const
{
	return mhNormalMapGpuSrv;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE GBuffer::DiffuseAlbedoRtv() const
{
	return mhDiffuseAlbedoCpuRtv;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE GBuffer::DiffuseAlbedoSrv() const
{
	return mhDiffuseAlbedoGpuSrv;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE GBuffer::BumpMapRtv() const
{
    return mhBumpMapCpuRtv;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE GBuffer::BumpMapSrv() const
{
    return mhBumpMapGpuSrv;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE GBuffer::FresnelR0RoughnessRtv() const
{
    return mhFresnelR0AndRoughnessCpuRtv;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE GBuffer::FresnelR0RoughnessSrv() const
{
    return mhFresnelR0AndRoughnessGpuSrv;
}

void GBuffer::BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv, CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuRtv, CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDsv, UINT cbvSrvUavDescriptorSize, UINT rtvDescriptorSize)
{
	// Save references to the descriptors.  The GBuffer reserves heap space
	// for 3 contiguous Srvs.

	mhDepthMapCpuSrv = hCpuSrv;
	mhNormalMapCpuSrv = hCpuSrv.Offset(1, cbvSrvUavDescriptorSize);
	mhDiffuseAlbedoCpuSrv = hCpuSrv.Offset(1, cbvSrvUavDescriptorSize);
    mhBumpMapCpuSrv = hCpuSrv.Offset(1, cbvSrvUavDescriptorSize);
    mhFresnelR0AndRoughnessCpuSrv = hCpuSrv.Offset(1, cbvSrvUavDescriptorSize);

	mhDepthMapGpuSrv = hGpuSrv;
	mhNormalMapGpuSrv = hGpuSrv.Offset(1, cbvSrvUavDescriptorSize);
	mhDiffuseAlbedoGpuSrv = hGpuSrv.Offset(1, cbvSrvUavDescriptorSize);
    mhBumpMapGpuSrv = hGpuSrv.Offset(1, cbvSrvUavDescriptorSize);
    mhFresnelR0AndRoughnessGpuSrv = hGpuSrv.Offset(1, cbvSrvUavDescriptorSize);

	mhNormalMapCpuRtv = hCpuRtv;
	mhDiffuseAlbedoCpuRtv = hCpuRtv.Offset(1, rtvDescriptorSize);
    mhBumpMapCpuRtv = hCpuRtv.Offset(1, rtvDescriptorSize);
    mhFresnelR0AndRoughnessCpuRtv = hCpuRtv.Offset(1, rtvDescriptorSize);

    mhDepthMapCpuDsv = hCpuDsv;

	//  Create the descriptors
	RebuildDescriptors();
}

void GBuffer::RebuildDescriptors()
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = GBuffer::NormalMapFormat;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	md3dDevice->CreateShaderResourceView(mNormalMap.Get(), &srvDesc, mhNormalMapCpuSrv);

	srvDesc.Format = GBuffer::DiffuseAlbedoFormat;
	md3dDevice->CreateShaderResourceView(mDiffuseAlbedo.Get(), &srvDesc, mhDiffuseAlbedoCpuSrv);

    srvDesc.Format = GBuffer::BumpMapFormat;
    md3dDevice->CreateShaderResourceView(mBumpMap.Get(), &srvDesc, mhBumpMapCpuSrv);

    srvDesc.Format = GBuffer::FresnelR0RoughnessFormat;
    md3dDevice->CreateShaderResourceView(mFresnelR0AndRoughness.Get(), &srvDesc, mhFresnelR0AndRoughnessCpuSrv);

	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	md3dDevice->CreateShaderResourceView(mDepthStencilBuffer.Get(), &srvDesc, mhDepthMapCpuSrv);

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = GBuffer::NormalMapFormat;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.Texture2D.PlaneSlice = 0;
	md3dDevice->CreateRenderTargetView(mNormalMap.Get(), &rtvDesc, mhNormalMapCpuRtv);

	rtvDesc.Format = GBuffer::DiffuseAlbedoFormat;
	md3dDevice->CreateRenderTargetView(mDiffuseAlbedo.Get(), &rtvDesc, mhDiffuseAlbedoCpuRtv);

    rtvDesc.Format = GBuffer::BumpMapFormat;
    md3dDevice->CreateRenderTargetView(mBumpMap.Get(), &rtvDesc, mhBumpMapCpuRtv);

    rtvDesc.Format = GBuffer::FresnelR0RoughnessFormat;
    md3dDevice->CreateRenderTargetView(mFresnelR0AndRoughness.Get(), &rtvDesc, mhFresnelR0AndRoughnessCpuRtv);

    // Create descriptor to mip level 0 of entire resource using the format of the resource.
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Format = GBuffer::DepthStencilFormat;
    dsvDesc.Texture2D.MipSlice = 0;
    md3dDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), &dsvDesc, mhDepthMapCpuDsv);
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
    mDepthStencilBuffer.Reset();
    mNormalMap.Reset();
    mDiffuseAlbedo.Reset();
    mBumpMap.Reset();
    mFresnelR0AndRoughness.Reset();

	CD3DX12_RESOURCE_DESC texDesc = {};
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = mRenderTargetWidth;
	texDesc.Height = mRenderTargetHeight;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    // We require an SRV to the depth buffer to read from the depth buffer.
    // Therefore, because we need to create two views to the same resource:
    //   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
    //   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
    // we need to create the depth buffer resource with a typeless format.  
    texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

    D3D12_CLEAR_VALUE optClear;
    optClear.Format = GBuffer::DepthStencilFormat;
    optClear.DepthStencil.Depth = 1.0f;
    optClear.DepthStencil.Stencil = 0;
    ThrowIfFailed(md3dDevice->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &texDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &optClear,
        IID_PPV_ARGS(&mDepthStencilBuffer)));

    texDesc.Format = GBuffer::NormalMapFormat;
    texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	float normalClearColor[] = { 0.0f, 0.0f, 1.0f, 0.0f };
	optClear = CD3DX12_CLEAR_VALUE(GBuffer::NormalMapFormat, normalClearColor);

	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&optClear,
		IID_PPV_ARGS(&mNormalMap)));

	texDesc.Format = GBuffer::DiffuseAlbedoFormat;
	float diffuseClearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	optClear = CD3DX12_CLEAR_VALUE(GBuffer::DiffuseAlbedoFormat, diffuseClearColor);
	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&optClear,
		IID_PPV_ARGS(&mDiffuseAlbedo)));

    texDesc.Format = GBuffer::BumpMapFormat;
    float bumpClearColor[] = { 0.0f, 0.0f, 1.0f, 0.0f };
    optClear = CD3DX12_CLEAR_VALUE(GBuffer::BumpMapFormat, bumpClearColor);
    ThrowIfFailed(md3dDevice->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &texDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        &optClear,
        IID_PPV_ARGS(&mBumpMap)));

    texDesc.Format = GBuffer::FresnelR0RoughnessFormat;
    float fresnelClearColor[] = { 0.01f, 0.01f, 0.01f, 0.5f };
    optClear = CD3DX12_CLEAR_VALUE(GBuffer::FresnelR0RoughnessFormat, fresnelClearColor);
    ThrowIfFailed(md3dDevice->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &texDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        &optClear,
        IID_PPV_ARGS(&mFresnelR0AndRoughness)));
}
