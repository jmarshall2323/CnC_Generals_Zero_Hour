// GameRenderer.h
//

#pragma once

#define D3DXFX_LARGEADDRESS_HANDLE

#include <d3d9.h>
#include <d3d12.h>
#include <assert.h>

class wwDeviceTexture
{
public:
	wwDeviceTexture(IDirect3DTexture9* pTexture, ID3D12Resource* pResource12)
		: m_pTexture(pTexture),
		m_pResource12(pResource12)
	{
		if (m_pTexture)
		{
	//		m_pTexture->AddRef(); // hold onto the texture
		}
	}

	ULONG AddRef()
	{
		return m_pTexture ? m_pTexture->AddRef() : 0;
	}

	ULONG Release()
	{
		// If we don't have a valid texture, there's nothing to do
		if (!m_pTexture)
			return 0;

		// Decrement COM's reference count
		ULONG refCount = m_pTexture->Release();

		// If COM has fully released it (refCount == 0), delete ourselves
		if (refCount == 0)
		{
			if (m_pResource12 != NULL)
			{
				m_pResource12->Release();
			}

			// Setting m_pTexture to nullptr here is optional safety
			m_pTexture = nullptr;

			// **Self-delete** – after this, the object is gone
			delete this;

			// Once we call `delete this;`, the current object is destroyed.
			// Typically you don't want to access any member variables or
			// methods after this line. Returning 0 here is usually fine.
			return 0;
		}

		// Otherwise, return the new ref count
		return refCount;
	}

	// Optionally expose the wrapped pointer if needed externally
	IDirect3DTexture9* GetWrappedTexture() const
	{
		return m_pTexture;
	}

	ID3D12Resource* GetWrappedTexture12() const
	{
		return m_pResource12;
	}

	//--------------------------------------------------------------------------
	// IUnknown-like methods
	//--------------------------------------------------------------------------

	HRESULT QueryInterface(REFIID riid, void** ppvObj)
	{
		return m_pTexture ? m_pTexture->QueryInterface(riid, ppvObj)
			: E_POINTER;
	}

	//--------------------------------------------------------------------------
	// IDirect3DBaseTexture9-like methods
	//--------------------------------------------------------------------------

	HRESULT GetDevice(IDirect3DDevice9** ppDevice)
	{
		return m_pTexture ? m_pTexture->GetDevice(ppDevice)
			: E_POINTER;
	}

	HRESULT SetPrivateData(REFGUID refguid, const void* pData,
		DWORD SizeOfData, DWORD Flags)
	{
		return m_pTexture ? m_pTexture->SetPrivateData(refguid, pData, SizeOfData, Flags)
			: E_POINTER;
	}

	HRESULT GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
	{
		return m_pTexture ? m_pTexture->GetPrivateData(refguid, pData, pSizeOfData)
			: E_POINTER;
	}

	HRESULT FreePrivateData(REFGUID refguid)
	{
		return m_pTexture ? m_pTexture->FreePrivateData(refguid)
			: E_POINTER;
	}

	DWORD SetPriority(DWORD PriorityNew)
	{
		return m_pTexture ? m_pTexture->SetPriority(PriorityNew)
			: 0;
	}

	DWORD GetPriority()
	{
		return m_pTexture ? m_pTexture->GetPriority()
			: 0;
	}

	void PreLoad()
	{
		if (m_pTexture)
			m_pTexture->PreLoad();
	}

	D3DRESOURCETYPE GetType()
	{
		return m_pTexture ? m_pTexture->GetType()
			: D3DRTYPE_SURFACE; // or some default
	}

	DWORD SetLOD(DWORD LODNew)
	{
		return m_pTexture ? m_pTexture->SetLOD(LODNew)
			: 0;
	}

	DWORD GetLOD()
	{
		return m_pTexture ? m_pTexture->GetLOD()
			: 0;
	}

	DWORD GetLevelCount()
	{
		return m_pTexture ? m_pTexture->GetLevelCount()
			: 0;
	}

	HRESULT SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType)
	{
		return m_pTexture ? m_pTexture->SetAutoGenFilterType(FilterType)
			: E_POINTER;
	}

	D3DTEXTUREFILTERTYPE GetAutoGenFilterType()
	{
		return m_pTexture ? m_pTexture->GetAutoGenFilterType()
			: D3DTEXF_NONE; // or some default
	}

	void GenerateMipSubLevels()
	{
		if (m_pTexture)
			m_pTexture->GenerateMipSubLevels();
	}

	//--------------------------------------------------------------------------
	// IDirect3DTexture9-like methods
	//--------------------------------------------------------------------------

	HRESULT GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc)
	{
		return m_pTexture ? m_pTexture->GetLevelDesc(Level, pDesc)
			: E_POINTER;
	}

	HRESULT GetSurfaceLevel(UINT Level, IDirect3DSurface9** ppSurfaceLevel)
	{
		return m_pTexture ? m_pTexture->GetSurfaceLevel(Level, ppSurfaceLevel)
			: E_POINTER;
	}

	HRESULT LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect,
		const RECT* pRect, DWORD Flags)
	{
		return m_pTexture ? m_pTexture->LockRect(Level, pLockedRect, pRect, Flags)
			: E_POINTER;
	}

	HRESULT UnlockRect(UINT Level)
	{
		return m_pTexture ? m_pTexture->UnlockRect(Level)
			: E_POINTER;
	}

	HRESULT AddDirtyRect(const RECT* pDirtyRect)
	{
		return m_pTexture ? m_pTexture->AddDirtyRect(pDirtyRect)
			: E_POINTER;
	}

private:
	// Destructor
	~wwDeviceTexture()
	{

	}

	// Underlying real texture pointer
	IDirect3DTexture9* m_pTexture;
	ID3D12Resource* m_pResource12;
};

inline void TransitionResource(
	ID3D12GraphicsCommandList* cmdList,
	ID3D12Resource* pResource,
	D3D12_RESOURCE_STATES beforeState,
	D3D12_RESOURCE_STATES afterState)
{
	if (!cmdList || !pResource || beforeState == afterState)
		return; // No-op if states match or invalid args

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = pResource;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = beforeState;
	barrier.Transition.StateAfter = afterState;

	cmdList->ResourceBarrier(1, &barrier);
}

inline DXGI_FORMAT ConvertD3DFormatToDXGIFormat(D3DFORMAT d3dFormat)
{
	switch (d3dFormat)
	{
		// Common 32-bit color formats:
	case D3DFMT_A8R8G8B8:
		// Usually this maps to BGRA in D3D9.  
		// If you are actually creating typical 8-bit RGBA surfaces in DX12, 
		// you might prefer DXGI_FORMAT_R8G8B8A8_UNORM. However, many D3D9
		// engines treat A8R8G8B8 as BGRA. Some drivers automatically handle 
		// the swizzle. If it looks inverted, try DXGI_FORMAT_B8G8R8A8_UNORM.
		return DXGI_FORMAT_B8G8R8A8_UNORM;

	case D3DFMT_X8R8G8B8:
		return DXGI_FORMAT_B8G8R8X8_UNORM;

		// 16-bit formats:
	case D3DFMT_R5G6B5:
		// There's no direct DXGI R5G6B5, so we often use B5G6R5 instead.
		return DXGI_FORMAT_B5G6R5_UNORM;

	case D3DFMT_A1R5G5B5:
		return DXGI_FORMAT_B5G5R5A1_UNORM;

		// Typical 24-bit or 16-bit are less common in DX12, might need a fallback or 
		// a custom path. 24-bit alone is rarely used directly in DX12.

		// Depth-stencil formats:
	case D3DFMT_D24S8:
	case D3DFMT_D24X8:
		return DXGI_FORMAT_D24_UNORM_S8_UINT;

	case D3DFMT_D16:
		// Some GPUs might not support D16 natively as a resource in DX12. 
		// If they do, it might be mapped to DXGI_FORMAT_D16_UNORM.
		return DXGI_FORMAT_D16_UNORM;

	case D3DFMT_D32:
		// Typically maps to DXGI_FORMAT_D32_FLOAT or similar. 
		// But note: some older D3D9 D32 was an integer depth. 
		// Usually you'd want to test or pick D32_FLOAT if that matches your usage.
		return DXGI_FORMAT_D32_FLOAT;

		// Fallback
	default:
		return DXGI_FORMAT_UNKNOWN;
	}
}


#include "dx8/dx8rendertarget.h"
#include "dx8/dx8wrapper.h"
#include "dx8/dx8list.h"
#include "dx8/dx8renderer.h"
#include "dx8/dx8fvf.h"
#include "dx8/dx8vertexbuffer.h"
#include "dx8/dx8indexbuffer.h"
#ifndef EDITOR
#include "dx8/dx8caps.h"
#endif
#include "dx8/dx8polygonrenderer.h"
#include "dx8/dx8renderer.h"
#include "dx8/dx8texman.h"
#ifndef EDITOR
#include "dx8/dx8WebBrowser.h"
#endif

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx12.h"

extern ImFont* g_BigConsoleFont;