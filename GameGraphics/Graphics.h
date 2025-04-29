#pragma once

/* Graphics
 * : Device, DeviceContext, SwapChain, RenderTargetView, Viewport 등을 생성 및 관리하는 클래스
 */
class Graphics
{
public:
	Graphics(HWND hwnd);
	~Graphics();

public:
	void RenderBegin();
	void RenderEnd();

public:
	/* 결과물을 그릴 백 버퍼 생성 */
	void CreateDeviceAndSwapChain();
	/* 백 퍼버에 대한 포인터, GPU가 목표 버퍼에 접근할 수 있도록 해주는 랜더타겟 뷰 객체 생성 */
	void CreateRenderTargetView();
	/* viewport 설정*/
	void SetViewport();

public:
	ComPtr<ID3D11Device> GetDevice() { return _device; }
	ComPtr<ID3D11DeviceContext> GetDeviceContext() { return _deviceContext; }

private:
	HWND _hwnd;
	uint32 _width = 0;
	uint32 _height = 0;

private:
	/* DirectX */
	// ComPtr을 사용하면 ref count 신경쓰지 않아도 알아서 관리됨, wrl에 포함됨
	// ID3D11Device : 디바이스 인터페이스, 지원 점검과 자원 할당에 사용됨, unit 생성
	// ID3D11DeviceContext : 렌더링 명령을 생성하는 디바이스 컨텍스트, unit에 명령
	ComPtr<ID3D11Device> _device = nullptr;
	ComPtr<ID3D11DeviceContext> _deviceContext = nullptr;

	// SwapChain : 렌더링된 데이터를 출력에 표시하기 전에 저장하기 위해 하나 이상의 표면을 구현
	ComPtr<IDXGISwapChain> _swapChain = nullptr;

	/* RenderTarget View
		: GPU가 백 버퍼를 렌더링 대상(Target)으로 인식하고,
		  해당 버퍼에 그림을 그릴 수 있도록 연결해주는 뷰(View) 객체 */
	ComPtr<ID3D11RenderTargetView> _renderTargetView;

	/* Misc : 잡다한 것들 */
	// D3D11_VIEWPORT : 화면을 묘사하는 구조체
	D3D11_VIEWPORT _viewport = { 0 };
	// 분홍색
	float _clearColor[4] = { 1.0f, 0.576f, 0.674f, 0.5f };
};