//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Graphic Engine
//
//                                              (C)2017, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef elenagm_d12H
#define elenagm_d12H 1

#include "elena.h"
#include "win32_common.h"

using namespace DirectX;

namespace _ELENA_
{

class D3D12Platform : public GraphicPlatform
{
	static const UINT FrameCount = 3;

	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	path_t  _rootPath;
	int		_width;
	int		_height;
	float	_aspectRatio;

	// Pipeline objects.
	CD3DX12_VIEWPORT					_viewport;
	CD3DX12_RECT						_scissorRect;
	ComPtr<ID3D12Device>				_d3d12Device;
	ComPtr<ID3D11On12Device>			_d3d11On12Device;
	ComPtr<ID3D11DeviceContext>			_d3d11DeviceContext;
	ComPtr<ID2D1Factory3>				_d2dFactory;
	ComPtr<ID2D1Device2>				_d2dDevice;
	ComPtr<ID2D1DeviceContext2>			_d2dDeviceContext;
	ComPtr<IDWriteFactory>				_dWriteFactory;
	ComPtr<ID3D12CommandQueue>			_commandQueue;
	ComPtr<IDXGISwapChain3>				_swapChain;
	ComPtr<ID3D12DescriptorHeap>		_rtvHeap;
	ComPtr<ID3D12Resource>				_renderTargets[FrameCount];
	ComPtr<ID3D11Resource>				_wrappedBackBuffers[FrameCount];
	ComPtr<ID2D1Bitmap1>				_d2dRenderTargets[FrameCount];
	ComPtr<ID3D12CommandAllocator>		_commandAllocators[FrameCount];
	ComPtr<ID3D12PipelineState>			_pipelineState;
	ComPtr<ID3D12GraphicsCommandList>	_commandList;
	ComPtr<ID3D12RootSignature>			_rootSignature;

	// App resources.
	UINT								_rtvDescriptorSize;
	ComPtr<ID2D1SolidColorBrush>		_textBrush;
	ComPtr<IDWriteTextFormat>			_textFormat;
	ComPtr<ID3D12Resource>				_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW			_vertexBufferView;

	// Synchronization objects.
	UINT								_frameIndex;
	HANDLE								_fenceEvent;
	UINT64								_fenceValues[FrameCount];
	ComPtr<ID3D12Fence>					_fence;

	void MoveToNextFrame();

	void PopulateCommandList();
	void RenderUI();

	void WaitForGpu();

	void InitPipeline();

public:
	D3D12Platform(path_t rootPath, int width, int height);
	virtual ~D3D12Platform() {}

	virtual void Init(HWND hWnd);

	virtual void* NewWidget(void* /* parent*/, int/* type*/) { return NULL; }
	virtual int CloseWidget(void* /* handle*/) { return -1; }

	void OnRender(HWND hWnd);
	void OnDestroy();
};

} // _ELENA_

#endif // elenagm_d12H