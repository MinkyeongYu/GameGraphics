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
	/* 랜더링 시작 */
	RenderBegin();

	//TODO

	/* 랜더링 끝 */
	RenderEnd();
}

void Game::RenderBegin()
{
	/* view 개수, renderTargetView 주소, DepthStencilView(픽셀이 얼마나 깊이 있는지(Z값) */
	_deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), nullptr);
	/* renderTargetView 색 초기화 */
	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), _clearColor);
	/* viewport 설정 */
	_deviceContext->RSSetViewports(1, &_viewport);
}

void Game::RenderEnd()
{
	/* rendering 결과물을 전면버퍼에 복사한 후 화면에 출력 */
	HRESULT hResult = _swapChain->Present(1, 0);
	CHECK(hResult);
}

void Game::CreateDeviceAndSwapChain()
{
	DXGI_SWAP_CHAIN_DESC desc;
	// memset과 같음, 배열 값 0으로 모두 초기화
	ZeroMemory(&desc, sizeof(desc));

	{
		/* 버퍼 크기 초기화 */
		desc.BufferDesc.Width = _width;
		desc.BufferDesc.Height = _height;
		/* 화면 주사율, 1초에 화면에 표시되는 이미지 수. (Numerator = 분자, Denominator = 분모) */
		desc.BufferDesc.RefreshRate.Numerator = 60;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		/* Display Format : RGBA 8-bit */
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		/* 주사선 그리기 모드 unspecified */
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		/* 픽셀단위 계단현상 보간 옵션, 1이면 MSAA 비활성화 */
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		/* 해당 버퍼를 최종 결과물을 그리는 용도로 사용 */
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		/* 백 버퍼 1개 사용 */
		desc.BufferCount = 1;
		/* 출력 창 = _hwnd */
		desc.OutputWindow = _hwnd;
		/* 창모드로 실행 */
		desc.Windowed = TRUE;
		/* 렌더링 끝나고 프레젠트(화면에 보여주기)한 다음, 백 버퍼 내용은 버림 */
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	}

	/*	DirectX 11 디바이스(Device), 디바이스 컨텍스트(Context), 스왑 체인(SwapChain) 생성
	* 
		D3D_DRIVER_TYPE : hardware (그래픽카드를 사용하겠다),
		D3D_FEATURE_LEVEL : DX몇 버전에 해당하는 기능 지원,
	*/
	HRESULT hResult = ::D3D11CreateDeviceAndSwapChain(
		nullptr,							// 기본 어댑터 사용
		D3D_DRIVER_TYPE_HARDWARE,			// GPU(하드웨어 가속) 사용
		nullptr,							// 소프트웨어 드라이버 없음
		0,									// 플래그 (디버그 모드 등)
		nullptr, 0,							// 피처 레벨 자동 선택 (DX11 우선)
		D3D11_SDK_VERSION,					// SDK 버전
		&desc,								// 스왑 체인 설정 정보
		_swapChain.GetAddressOf(),			// 생성된 스왑 체인 저장
		_device.GetAddressOf(),				// 생성된 디바이스 저장
		nullptr,							// 실제 선택된 피처 레벨은 무시
		_deviceContext.GetAddressOf()		// 생성된 디바이스 컨텍스트 저장
	);

	/* false라면 crash발생시킴 assert(SUCCEEDED(hResult))는 CHECK(hResult). pch에 정의함 */
	CHECK(hResult);
}

void Game::CreateRenderTargetView()
{
	HRESULT hResult;
	ComPtr<ID3D11Texture2D> backBuffer = nullptr;
	// _swapChain에서 0번째 버퍼(ID3D11Texture2D)를 얻어 backBuffer 포인터에 저장
	hResult = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());
	CHECK(hResult);

	// backBuffer를 기반으로 렌더 타겟 뷰 생성 후 _renderTargetView에 저장
	_device->CreateRenderTargetView(backBuffer.Get(), nullptr, _renderTargetView.GetAddressOf());
	CHECK(hResult);
}

void Game::SetViewport()
{
	/* 시작 위치 */
	_viewport.TopLeftX = 0.f;
	_viewport.TopLeftY = 0.f;
	/* 너비와 높이 */
	_viewport.Width = static_cast<float>(_width);
	_viewport.Height = static_cast<float>(_height);
	/* 깊이 */
	_viewport.MinDepth = 0.f;
	_viewport.MaxDepth = 1.f;
}