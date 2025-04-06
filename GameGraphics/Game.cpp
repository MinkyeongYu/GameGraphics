#include "pch.h"
#include "Game.h"

Game::Game()
{
}

Game::~Game()
{
}

void Game::Init(HWND hwnd)
{
	_hwnd = hwnd;
	_width = GWinSizeX;
	_height = GWinSizeY;

	CreateDeviceAndSwapChain();
	CreateRenderTargetView();
	SetViewport();
}

void Game::Update()
{
}

void Game::Render()
{
	/* ������ ���� */
	RenderBegin();

	//TODO

	/* ������ �� */
	RenderEnd();
}

void Game::RenderBegin()
{
	/* view ����, renderTargetView �ּ�, DepthStencilView(�ȼ��� �󸶳� ���� �ִ���(Z��) */
	_deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), nullptr);
	/* renderTargetView �� �ʱ�ȭ */
	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), _clearColor);
	/* viewport ���� */
	_deviceContext->RSSetViewports(1, &_viewport);
}

void Game::RenderEnd()
{
	/* rendering ������� ������ۿ� ������ �� ȭ�鿡 ��� */
	HRESULT hResult = _swapChain->Present(1, 0);
	CHECK(hResult);
}

void Game::CreateDeviceAndSwapChain()
{
	DXGI_SWAP_CHAIN_DESC desc;
	// memset�� ����, �迭 �� 0���� ��� �ʱ�ȭ
	ZeroMemory(&desc, sizeof(desc));

	{
		/* ���� ũ�� �ʱ�ȭ */
		desc.BufferDesc.Width = _width;
		desc.BufferDesc.Height = _height;
		/* ȭ�� �ֻ���, 1�ʿ� ȭ�鿡 ǥ�õǴ� �̹��� ��. (Numerator = ����, Denominator = �и�) */
		desc.BufferDesc.RefreshRate.Numerator = 60;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		/* Display Format : RGBA 8-bit */
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		/* �ֻ缱 �׸��� ��� unspecified */
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		/* �ȼ����� ������� ���� �ɼ�, 1�̸� MSAA ��Ȱ��ȭ */
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		/* �ش� ���۸� ���� ������� �׸��� �뵵�� ��� */
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		/* �� ���� 1�� ��� */
		desc.BufferCount = 1;
		/* ��� â = _hwnd */
		desc.OutputWindow = _hwnd;
		/* â���� ���� */
		desc.Windowed = TRUE;
		/* ������ ������ ������Ʈ(ȭ�鿡 �����ֱ�)�� ����, �� ���� ������ ���� */
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	}

	/*	DirectX 11 ����̽�(Device), ����̽� ���ؽ�Ʈ(Context), ���� ü��(SwapChain) ����
	* 
		D3D_DRIVER_TYPE : hardware (�׷���ī�带 ����ϰڴ�),
		D3D_FEATURE_LEVEL : DX�� ������ �ش��ϴ� ��� ����,
	*/
	HRESULT hResult = ::D3D11CreateDeviceAndSwapChain(
		nullptr,							// �⺻ ����� ���
		D3D_DRIVER_TYPE_HARDWARE,			// GPU(�ϵ���� ����) ���
		nullptr,							// ����Ʈ���� ����̹� ����
		0,									// �÷��� (����� ��� ��)
		nullptr, 0,							// ��ó ���� �ڵ� ���� (DX11 �켱)
		D3D11_SDK_VERSION,					// SDK ����
		&desc,								// ���� ü�� ���� ����
		_swapChain.GetAddressOf(),			// ������ ���� ü�� ����
		_device.GetAddressOf(),				// ������ ����̽� ����
		nullptr,							// ���� ���õ� ��ó ������ ����
		_deviceContext.GetAddressOf()		// ������ ����̽� ���ؽ�Ʈ ����
	);

	/* false��� crash�߻���Ŵ assert(SUCCEEDED(hResult))�� CHECK(hResult). pch�� ������ */
	CHECK(hResult);
}

void Game::CreateRenderTargetView()
{
	HRESULT hResult;
	ComPtr<ID3D11Texture2D> backBuffer = nullptr;
	// _swapChain���� 0��° ����(ID3D11Texture2D)�� ��� backBuffer �����Ϳ� ����
	hResult = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());
	CHECK(hResult);

	// backBuffer�� ������� ���� Ÿ�� �� ���� �� _renderTargetView�� ����
	_device->CreateRenderTargetView(backBuffer.Get(), nullptr, _renderTargetView.GetAddressOf());
	CHECK(hResult);
}

void Game::SetViewport()
{
	/* ���� ��ġ */
	_viewport.TopLeftX = 0.f;
	_viewport.TopLeftY = 0.f;
	/* �ʺ�� ���� */
	_viewport.Width = static_cast<float>(_width);
	_viewport.Height = static_cast<float>(_height);
	/* ���� */
	_viewport.MinDepth = 0.f;
	_viewport.MaxDepth = 1.f;
}