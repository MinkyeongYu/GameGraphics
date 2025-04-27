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

	// [1] DirectX 핵심 객체들 생성
	// [2] 백버퍼 기반의 렌더 타겟 뷰 생성
	// [3] 화면에 출력할 뷰포트 크기 설정
	//_graphics = make_shared<Graphics>(hwnd);
	_graphics = std::make_shared<Graphics>(hwnd);

	{
		// Create Geometry
		_geometry = std::make_shared<Geometry<VertexTextureData>>();
		// Create Vertex Buffer
		_vertexBuffer = std::make_shared<VertexBuffer>(_graphics->GetDevice());
		// Create Index Buffer
		_indexBuffer = std::make_shared<IndexBuffer>(_graphics->GetDevice());
		// Create Input Layout
		_inputLayout = std::make_shared<InputLayout>(_graphics->GetDevice());
		// Create Vertex Shader
		_vertexShader = std::make_shared<VertexShader>(_graphics->GetDevice());
		// Create Pixel Shader
		_pixelShader = std::make_shared<PixelShader>(_graphics->GetDevice());
		// Create Shader Resource View
		_shaderResoureView = std::make_shared<Texture>(_graphics->GetDevice());
		// Create Constant Buffer
		_constantBuffer = std::make_shared<ConstantBuffer<TransformData>>(_graphics->GetDevice(), _graphics->GetDeviceContext());
	}


	// [4] 정점 데이터 정의 및 GPU 버퍼에 업로드, vertexBuffer 생성
	/* Create Vertex Data & Index Data */				
	{
		/* Index Data */
		// 삼각형 1 : _vertices[0], _vertices[1], _vertices[2]
		// 삼각형 2 : _vertices[2], _vertices[1], _vertices[3]
		GeometryHelper::CreateRectangle(_geometry);
	}
	/* Vertex Buffer 생성 */
	{
		_vertexBuffer->Create(_geometry->GetVertices());
	}
	/* Index Buffer 생성 - 정점을 재사용하여 메모리 사용을 줄이고, 렌더링 성능을 최적화하기 위함 */
	{
		_indexBuffer->Create(_geometry->GetIndices());
	}
	
	// [5] 정점 셰이더 컴파일 및 생성
	/* vertex shader load 후 _vertexBlob에 결과 저장 */
	_vertexShader->Create(L"DefaultVertexShader.hlsl", "VS_main", "vs_5_0");  
	// [6] 정점 구조 정의 (셰이더와 버퍼 연결), CreateInputLayout()
	_inputLayout->Create(VertexTextureData::descs, _vertexShader->GetBlob());	
	// [7] 래스터라이저 상태 생성 (삼각형 그리는 방식, 컬링, 채우기 모드 등 설정)
	CreateRasterizerState();														
	
	// [8] 픽셀 셰이더 컴파일 및 생성
	/* pixel shader load 후 _pixelBlob에 결과 저장 */
	_pixelShader->Create(L"DefaultVertexShader.hlsl", "PS", "ps_5_0");	
	// [9] 샘플러 상태 생성
	CreateSamplerState();															

	// [10] 쉐이더 리소스 뷰 생성
	_shaderResoureView->CreateShaderResourceView(L"hamster_latte.png");
	// [11] 상수 버퍼 생성
	_constantBuffer->CreateConstantBuffer();
	// [12] 블렌딩 상태 생성
	CreateBlendState();																
}

void Game::Update()
{
	_localPosition.x += 0.001f;

	// Create SRT
	Matrix scaleMatrix			= Matrix::CreateScale(_localScale / 3);
	Matrix rotationMatrix		= Matrix::CreateRotationX(_localRotation.x);
	rotationMatrix				*= Matrix::CreateRotationY(_localRotation.y);
	rotationMatrix				*= Matrix::CreateRotationZ(_localRotation.z);
	Matrix translationMatrix	= Matrix::CreateTranslation(_localPosition);

	// Create WorldMatrix
	Matrix worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
	_transformData.worldMatrix = worldMatrix;

	// 버퍼에 데이터 복사
	_constantBuffer->CopyData(_transformData);
}

void Game::Render()
{
	/* 랜더링 시작 */
	_graphics->RenderBegin();

	/* IA - VS - RS - PS - OM */
	{
		// IA (Input Assembler) : 정점의 정보 전달
		uint32 stride = sizeof(VertexTextureData);					// 정점 1개 크기 (28바이트)
		uint32 offset = 0;											// 버퍼의 시작 위치 오프셋

		/* GPU에게 정점 버퍼의 크기와 위치(stride, offset) 전달, vertices 사용함을 GPU에 알려줌 */
		_graphics->GetDeviceContext()->IASetVertexBuffers(0, 1, _vertexBuffer->GetComPtr().GetAddressOf(), &stride, &offset);
		/* 32-bit(4Byte) uint 인덱스 버퍼 GPU에 바인딩 */
		_graphics->GetDeviceContext()->IASetIndexBuffer(_indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);
		/* GPU에게 정점 데이터의 구조 전달 */
		_graphics->GetDeviceContext()->IASetInputLayout(_inputLayout->GetComPtr().Get());
		/* 각 정점을 어떻게 이어줄지 전달, 삼각형으로 이어주도록 설정 */
		_graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// VS (Vertex Shader) : 정점의 위치/색상 등 가공
		_graphics->GetDeviceContext()->VSSetShader(_vertexShader->GetComPtr().Get(), nullptr, 0);
		_graphics->GetDeviceContext()->VSSetConstantBuffers(0, 1, _constantBuffer->GetComPtr().GetAddressOf());

		// RS (Rasterizer) : 정점 → 픽셀로 삼각형 그리기
		/* Draw()호출 시 내부적으로 GPU가 자동으로 Rasterizer 실행함 */
		_graphics->GetDeviceContext()->RSSetState(_rasterizerState.Get());

		// PS (Pixel Shader) : 픽셀 단위 색상 처리
		_graphics->GetDeviceContext()->PSSetShader(_pixelShader->GetComPtr().Get(), nullptr, 0);
		_graphics->GetDeviceContext()->PSSetShaderResources(0, 1, _shaderResoureView->GetComPtr().GetAddressOf());
		_graphics->GetDeviceContext()->PSSetSamplers(0, 1, _samplerState.GetAddressOf());

		// OM (Output Merger) : 최종 픽셀을 렌더 타겟에 출력
		/* 실제 삼각형 그리기 (정점 개수 3, 시작 offset 0) */
		//_deviceContext->Draw(_vertices.size(), 0);
		_graphics->GetDeviceContext()->DrawIndexed(_geometry->GetIndexCount(), 0, 0);
	}

	/* 랜더링 끝, 화면에 출력 */
	_graphics->RenderEnd();
}

void Game::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	// 픽셀을 채우는 방식: SOLID(채우기) 또는 WIREFRAME(선만 그리기)
	desc.FillMode = D3D11_FILL_SOLID;
	// 컬링 방식: BACK(뒤쪽 면 제거), FRONT(앞면 제거), NONE(제거 안 함)
	desc.CullMode = D3D11_CULL_BACK;
	// 삼각형의 앞면 시계방향/반시계방향 여부 (false = 시계방향이 앞면)
	desc.FrontCounterClockwise = false;

	HRESULT hResult = _graphics->GetDevice()->CreateRasterizerState(&desc, _rasterizerState.GetAddressOf());
	CHECK(hResult);
}

void Game::CreateSamplerState()
{
	// 샘플러 상태(Sampler State) 설정 구조체 초기화
	D3D11_SAMPLER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	// 텍스처 좌표 범위를 벗어났을 때 테두리 색상(Border Color) 사용 (U, V, W 각각에 적용)
	desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;

	// 테두리(Border) 색상 설정 R,G,B,A (빨강: 1,0,0,1)
	desc.BorderColor[0] = 1;
	desc.BorderColor[1] = 0;
	desc.BorderColor[2] = 0;
	desc.BorderColor[3] = 1;

	// 비교 함수 (사용하지 않음 → 항상 통과)
	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;

	// 필터 설정 (텍스처 축소/확대/밉맵 전부 선형 보간)
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	// 이방성 필터링 최대치 (일반적으로 16 사용)
	desc.MaxAnisotropy = 16;

	// 사용할 밉맵 LOD 범위 설정
	desc.MinLOD = FLT_MIN;
	desc.MaxLOD = FLT_MAX;

	// 밉맵 LOD 오프셋 (기본값 0)
	desc.MipLODBias = 0.f;

	// 위 설정을 기반으로 샘플러 상태 객체 생성
	HRESULT hResult = _graphics->GetDevice()->CreateSamplerState(&desc, _samplerState.GetAddressOf());
	CHECK(hResult);
}

void Game::CreateBlendState()
{
	D3D11_BLEND_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	// 멀티 샘플링 시 알파 커버리지 사용 여부 (일반적으로 false)
	desc.AlphaToCoverageEnable = false;

	// MRT(Multiple Render Targets) 각각에 다른 블렌드 설정을 할지 여부 (false = 동일 설정 사용)
	desc.IndependentBlendEnable = false;

	// 렌더 타겟 0번 블렌딩 설정
	desc.RenderTarget[0].BlendEnable = true;								// 블렌딩 활성화

	// 컬러 블렌딩 연산 (새 픽셀 색 * SrcBlend + 기존 픽셀 색 * DestBlend)
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;					// 새 픽셀 알파값
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;				// (1 - 새 픽셀 알파값)
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;						// Src + Dest

	// 알파 블렌딩 연산 (보통 색상 블렌딩과 동일하게 처리)
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;					// 알파 100%
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;					// 알파 없음
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;					// Src + Dest

	// 출력할 색상 채널 설정 (R, G, B, A 전부 출력)
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// 위 설정을 기반으로 BlendState 생성
	HRESULT hResult = _graphics->GetDevice()->CreateBlendState(&desc, _blendState.GetAddressOf());
	CHECK(hResult);
}