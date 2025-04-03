#include <windows.h>
#include <d3d9.h>
#include <d3d9on12.h>
#include <d3d12.h>

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p) = nullptr; } }
#endif

class wwRenderTarget
{
public:
	wwRenderTarget() = default;
	~wwRenderTarget() { Release(); }

	// Initializes the render target system:
	//  - Always creates a single-sample color RT (m_pRT_Resolved).
	//  - If msaaType != D3DMULTISAMPLE_NONE, also creates an MSAA color RT (m_pRT_MSAA).
	//  - If depthFormat != D3DFMT_UNKNOWN, creates a depth buffer (single-sample or MSAA).
	//  - Unwraps each IDirect3DSurface9 to an ID3D12Resource* if device supports D3D9On12.
	bool Initialize(
		UINT                 width,
		UINT                 height,
		D3DFORMAT            colorFormat = D3DFMT_X8R8G8B8,
		D3DFORMAT            depthFormat = D3DFMT_UNKNOWN,            // e.g. D3DFMT_D24S8 if needed
		D3DMULTISAMPLE_TYPE  msaaType = D3DMULTISAMPLE_NONE,       // e.g. D3DMULTISAMPLE_4_SAMPLES
		DWORD                msaaQuality = 0
	);

	void Release();

	bool BeginRender();
	bool EndRender();
	bool EndRender12(ID3D12GraphicsCommandList* cmdList, ID3D12Resource *backBuffer);

	// Accessors for the D3D9 surfaces
	IDirect3DSurface9* GetMSAARenderTarget()     const { return m_pRT_MSAA; }
	IDirect3DSurface9* GetMSAADepthStencil()     const { return m_pDS_MSAA; }
	IDirect3DSurface9* GetResolvedRenderTarget() const { return m_pRT_Resolved; }
	IDirect3DSurface9* GetResolvedDepthStencil() const { return m_pDS_Resolved; }

	// Accessors for the underlying D3D12 resources (unwrapped)
	ID3D12Resource* GetMSAARenderTarget12()      const { return m_pRT_MSAA_12; }
	ID3D12Resource* GetMSAADepthStencil12()      const { return m_pDS_MSAA_12; }
	ID3D12Resource* GetResolvedRenderTarget12()  const { return m_pRT_Resolved_12; }
	ID3D12Resource* GetResolvedDepthStencil12()  const { return m_pDS_Resolved_12; }

private:
	// Device
	IDirect3DDevice9* m_pDevice = nullptr;

	// Surfaces (D3D9)
	IDirect3DSurface9* m_pRT_MSAA = nullptr; // multi-sample color
	IDirect3DSurface9* m_pDS_MSAA = nullptr; // multi-sample depth
	IDirect3DSurface9* m_pRT_Resolved = nullptr; // single-sample color
	IDirect3DSurface9* m_pDS_Resolved = nullptr; // single-sample depth

	// Corresponding D3D12 resources (unwrapped)
	ID3D12Resource* m_pRT_MSAA_12 = nullptr;
	ID3D12Resource* m_pDS_MSAA_12 = nullptr;
	ID3D12Resource* m_pRT_Resolved_12 = nullptr;
	ID3D12Resource* m_pDS_Resolved_12 = nullptr;

	// State
	UINT               m_Width = 0;
	UINT               m_Height = 0;
	bool               m_bUseMSAA = false;

	D3DFORMAT           colorFormat;
	D3DFORMAT           depthFormat;
};
