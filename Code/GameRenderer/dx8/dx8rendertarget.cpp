#include "../GameRenderer/GameRenderer.h"

bool wwRenderTarget::Initialize(
	UINT                width,
	UINT                height,
	D3DFORMAT           colorFormat,
	D3DFORMAT           depthFormat,
	D3DMULTISAMPLE_TYPE msaaType,
	DWORD               msaaQuality)
{

	Release(); // Clean up if re-initializing

	m_pDevice = DX8Wrapper::D3DDevice;
	m_pDevice->AddRef();

	this->colorFormat = colorFormat;
	this->depthFormat = depthFormat;

	m_Width = width;
	m_Height = height;
	m_bUseMSAA = (msaaType != D3DMULTISAMPLE_NONE);

	HRESULT hr = S_OK;

	hr = m_pDevice->CreateRenderTarget(
		m_Width,
		m_Height,
		colorFormat,
		D3DMULTISAMPLE_NONE,
		0,
		FALSE,
		&m_pRT_Resolved,
		nullptr
	);
	if (FAILED(hr))
	{
		Release();
		return false;
	}

	if (depthFormat != D3DFMT_UNKNOWN)
	{
		hr = m_pDevice->CreateDepthStencilSurface(
			m_Width,
			m_Height,
			depthFormat,
			D3DMULTISAMPLE_NONE,
			0,
			TRUE,
			&m_pDS_Resolved,
			nullptr
		);
		if (FAILED(hr))
		{
			Release();
			return false;
		}
	}

	if (m_bUseMSAA)
	{
		hr = m_pDevice->CreateRenderTarget(
			m_Width,
			m_Height,
			colorFormat,
			msaaType,
			msaaQuality,
			FALSE,
			&m_pRT_MSAA,
			nullptr
		);
		if (FAILED(hr))
		{
			Release();
			return false;
		}

		if (depthFormat != D3DFMT_UNKNOWN)
		{
			hr = m_pDevice->CreateDepthStencilSurface(
				m_Width,
				m_Height,
				depthFormat,
				msaaType,
				msaaQuality,
				TRUE,
				&m_pDS_MSAA,
				nullptr
			);
			if (FAILED(hr))
			{
				Release();
				return false;
			}
		}
	}

	auto UnwrapSurface = [&](IDirect3DSurface9* pSurf, ID3D12Resource** ppOut) -> void
		{
			if (!pSurf || !ppOut) return;

			DX8Wrapper::device9On12->UnwrapUnderlyingResource(
				pSurf,
				DX8Wrapper::D3D12Renderer->graphics_queue->dx_queue,
				__uuidof(ID3D12Resource),
				reinterpret_cast<void**>(ppOut)
			);
		};

	// Unwrap each surface if valid
	UnwrapSurface(m_pRT_Resolved, &m_pRT_Resolved_12);
	UnwrapSurface(m_pDS_Resolved, &m_pDS_Resolved_12);
	UnwrapSurface(m_pRT_MSAA, &m_pRT_MSAA_12);
	UnwrapSurface(m_pDS_MSAA, &m_pDS_MSAA_12);

	return true;
}

void wwRenderTarget::Release()
{
	// Release D3D12 resources
	SAFE_RELEASE(m_pRT_MSAA_12);
	SAFE_RELEASE(m_pDS_MSAA_12);
	SAFE_RELEASE(m_pRT_Resolved_12);
	SAFE_RELEASE(m_pDS_Resolved_12);

	// Release D3D9 surfaces
	SAFE_RELEASE(m_pRT_MSAA);
	SAFE_RELEASE(m_pDS_MSAA);
	SAFE_RELEASE(m_pRT_Resolved);
	SAFE_RELEASE(m_pDS_Resolved);

	// Finally, release D3D9 device
	SAFE_RELEASE(m_pDevice);

	m_Width = 0;
	m_Height = 0;
	m_bUseMSAA = false;
}

bool wwRenderTarget::BeginRender()
{
	if (!m_pDevice)
		return false;

	HRESULT hr = S_OK;

	// If we have MSAA surfaces, set them. Otherwise, set single-sample surfaces.
	if (m_bUseMSAA && m_pRT_MSAA)
	{
		hr = m_pDevice->SetRenderTarget(0, m_pRT_MSAA);
		if (FAILED(hr)) return false;

		// Depth
		hr = m_pDevice->SetDepthStencilSurface(m_pDS_MSAA);
		if (FAILED(hr)) return false;
	}
	else
	{
		// Single-sample
		if (!m_pRT_Resolved)
			return false;

		hr = m_pDevice->SetRenderTarget(0, m_pRT_Resolved);
		if (FAILED(hr)) return false;

		hr = m_pDevice->SetDepthStencilSurface(m_pDS_Resolved);
		if (FAILED(hr)) return false;
	}

	return true;
}

bool wwRenderTarget::EndRender()
{
	if (!m_pDevice)
		return false;

	m_pDevice->SetRenderTarget(0, nullptr);

	return true;
}

bool wwRenderTarget::EndRender12(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* pBackBuffer12)
{
	m_pDevice = DX8Wrapper::D3DDevice;

	// If we don't have the device or the D3D12 renderer, no work to do
	if (!m_pDevice || !DX8Wrapper::D3D12Renderer)
		return false;
#if 0
	// If not using MSAA, then nothing special needed for color or depth resolves in D3D12.
	if (!m_bUseMSAA)
		return true;

	
	if (m_pRT_MSAA_12 && m_pRT_Resolved_12)
	{
		// Transition MSAA surface from RENDER_TARGET -> RESOLVE_SOURCE
		TransitionResource(cmdList,
			m_pRT_MSAA_12,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_RESOLVE_SOURCE);

		// Transition resolved surface from PIXEL_SHADER_RESOURCE (or COMMON) -> RESOLVE_DEST
		TransitionResource(cmdList,
			m_pRT_Resolved_12,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_RESOLVE_DEST);

		// Perform the color resolve (pick the matching DXGI format for your D3D9 color format)
		DXGI_FORMAT dxgiColorFormat = ConvertD3DFormatToDXGIFormat(colorFormat);
		cmdList->ResolveSubresource(
			m_pRT_Resolved_12,
			0,                  // DstSubresource
			m_pRT_MSAA_12,
			0,                  // SrcSubresource
			dxgiColorFormat
		);

		// Transition them back if needed for subsequent usage
		TransitionResource(cmdList,
			m_pRT_MSAA_12,
			D3D12_RESOURCE_STATE_RESOLVE_SOURCE,
			D3D12_RESOURCE_STATE_RENDER_TARGET);

		TransitionResource(cmdList,
			m_pRT_Resolved_12,
			D3D12_RESOURCE_STATE_RESOLVE_DEST,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	}

	if (m_pDS_MSAA_12 && m_pDS_Resolved_12)
	{
		// Same pattern of transitions. Keep in mind some hardware/drivers do not allow direct depth resolves.
		TransitionResource(cmdList,
			m_pDS_MSAA_12,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			D3D12_RESOURCE_STATE_RESOLVE_SOURCE);

		TransitionResource(cmdList,
			m_pDS_Resolved_12,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			D3D12_RESOURCE_STATE_RESOLVE_DEST);

		DXGI_FORMAT dxgiDepthFormat = ConvertD3DFormatToDXGIFormat(depthFormat);
		cmdList->ResolveSubresource(
			m_pDS_Resolved_12,
			0,
			m_pDS_MSAA_12,
			0,
			dxgiDepthFormat
		);

		// Transition back to desired states (for example, DEPTH_WRITE)
		TransitionResource(cmdList,
			m_pDS_MSAA_12,
			D3D12_RESOURCE_STATE_RESOLVE_SOURCE,
			D3D12_RESOURCE_STATE_DEPTH_WRITE);

		TransitionResource(cmdList,
			m_pDS_Resolved_12,
			D3D12_RESOURCE_STATE_RESOLVE_DEST,
			D3D12_RESOURCE_STATE_DEPTH_WRITE);
	}
#endif
	if (m_pRT_Resolved_12 && pBackBuffer12)
	{
		// Transition the back buffer from PRESENT -> COPY_DEST
		TransitionResource(cmdList,
			pBackBuffer12,
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_COPY_DEST);

		// Transition the resolved texture from PIXEL_SHADER_RESOURCE -> COPY_SOURCE
		TransitionResource(cmdList,
			m_pRT_Resolved_12,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_COPY_SOURCE);

		// Perform the copy
		cmdList->CopyResource(pBackBuffer12, m_pRT_Resolved_12);

		// Transition back to original states (e.g., back buffer -> PRESENT, resolved -> PIXEL_SHADER_RESOURCE)
		TransitionResource(cmdList,
			pBackBuffer12,
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_RENDER_TARGET);

		TransitionResource(cmdList,
			m_pRT_Resolved_12,
			D3D12_RESOURCE_STATE_COPY_SOURCE,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	}

	return true;
}

