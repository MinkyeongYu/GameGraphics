#pragma once
#include "pch.h"

class Game
{
public:
	Game();
	~Game();

public:
	void Init(HWND hwnd);
	void Update();
	void Render();

private:
	void RenderBegin();
	void RenderEnd();

private:
	/* 결과물을 그릴 백 버퍼 생성 */
	void CreateDeviceAndSwapChain();
	/* 백 퍼버에 대한 포인터, GPU가 목표 버퍼에 접근할 수 있도록 해주는 랜더타겟 뷰 객체 생성 */
	void CreateRenderTargetView();
	/* viewport 설정*/
	void SetViewport();

private:
	/* 기하학적 도형 그리기 */
	void CreateGeometry();
	/* GPU가 정점 데이터를 올바르게 해석할 수 있도록 입력 레이아웃(Input Layout)을 생성 */
	void CreateInputLayout();

	void CreateVertexShader();
	void CreatePixelShader();

	/* Shader Resource View 생성 */
	void CreateShaderResourceView();

	/* Constant Buffer 생성 */
	void CreateConstantBuffer();

	/* DefaultVertexShader.hlsl에서 shader 로드하고 컴파일하여 결과를 ID3DBlob에 저장 */
	void LoadShaderFromFile(const std::wstring& path, const std::string& name, const std::string& version, ComPtr<ID3DBlob>& blob);

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
	D3D11_VIEWPORT _viewport = {0};
	float _clearColor[4] = { 1.0f, 0.576f, 0.674f, 0.5f };

private:
	/* Geometry */
	// 정점 리스트 (CPU 메모리)
	std::vector<Vertex> _vertices;
	// 인덱스 리스트 (CPU 메모리)
	std::vector<uint32> _indices;
	// GPU Buffers
	// 정점 데이터를 GPU VRAM에 전달하기 위한 버퍼 (Vertex Buffer)
	ComPtr<ID3D11Buffer> _vertexBuffer = nullptr;
	// 인덱스 데이터를 GPU VRAM에 전달하기 위한 버퍼 (Index Buffer) 
	ComPtr<ID3D11Buffer> _indexBuffer = nullptr;

	/* Input Layout */
	// 정점 버퍼의 데이터 구조를 GPU에게 알려주는 입력 레이아웃
	ComPtr<ID3D11InputLayout> _inputLayout = nullptr;

	/* Vertex Shader */
	// 정점 셰이더 객체 (GPU에서 정점 위치/속성 처리)
	ComPtr<ID3D11VertexShader> _vertexShader = nullptr;
	// 정점 셰이더의 바이트 코드 (InputLayout 생성에도 사용)
	ComPtr<ID3DBlob> _vertexBlob = nullptr;

	/* Pixel Shader */
	// 픽셀 셰이더 객체 (GPU에서 픽셀 색상 처리)
	ComPtr<ID3D11PixelShader> _pixelShader = nullptr;
	// 픽셀 셰이더의 바이트 코드
	ComPtr<ID3DBlob> _pixelBlob = nullptr;

	/* Shader Resource View 
		: 셰이더(Shader)가 GPU 리소스(텍스처, 버퍼 등)에 접근할 수 있도록 만들어주는 객체 */
	ComPtr<ID3D11ShaderResourceView> _shaderResourceView = nullptr;

private:
	/* Constant Buffer */
	TransformData _transformData;
	ComPtr<ID3D11Buffer> _constantBuffer;
};