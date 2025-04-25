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
	_graphics = new Graphics(hwnd);


	CreateGeometry();               // [4] 정점 데이터 정의 및 GPU 버퍼에 업로드, vertexBuffer 생성
	CreateVertexShader();           // [5] 정점 셰이더 컴파일 및 생성
	CreateInputLayout();            // [6] 정점 구조 정의 (셰이더와 버퍼 연결)
	CreateRasterizerState();		// [7] 래스터라이저 상태 생성 (삼각형 그리는 방식, 컬링, 채우기 모드 등 설정)
	CreatePixelShader();            // [8] 픽셀 셰이더 컴파일 및 생성
	CreateSamplerState();			// [9] 샘플러 상태 생성

	CreateShaderResourceView();		// [10] 텍스처 리소스 뷰 생성
	CreateConstantBuffer();			// [11] 상수 버퍼 생성
	CreateBlendState();				// [12] 블렌딩 상태 생성
}

void Game::Update()
{
	_localPosition.x += 0.001f;

	// Create SRT
	Matrix scaleMatrix			= Matrix::CreateScale(_localScale);
	Matrix rotationMatrix		= Matrix::CreateRotationX(_localRotation.x);
	rotationMatrix				*= Matrix::CreateRotationY(_localRotation.y);
	rotationMatrix				*= Matrix::CreateRotationZ(_localRotation.z);
	Matrix translationMatrix	= Matrix::CreateTranslation(_localPosition);

	// Create WorldMatrix
	Matrix worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
	_transformData.worldMatrix = worldMatrix;

	D3D11_MAPPED_SUBRESOURCE subResource;
	ZeroMemory(&subResource, sizeof(subResource));

	// GPU의 Constant Buffer를 CPU에서 쓰기 위한 접근 요청
	_graphics->GetDeviceContext()->Map(
		_constantBuffer.Get(),			// 업데이트할 Constant Buffer
		0,								// 서브리소스 인덱스 (일반적으로 0)
		D3D11_MAP_WRITE_DISCARD,		// 이전 내용은 버리고 새로 쓰기 (가장 일반적인 방식)
		0,								// Reserved (항상 0)
		&subResource					// 매핑 결과를 받을 구조체 (CPU가 접근 가능한 포인터 제공됨)
	);
	// _transformData값을 GPU메모리로 복사한 후, GPU의 Constant Buffer에 업로드
	::memcpy(subResource.pData, &_transformData, sizeof(_transformData));
	// 맵핑 해제 → GPU에서 읽을 수 있도록 다시 연결
	_graphics->GetDeviceContext()->Unmap(_constantBuffer.Get(), 0);
}

void Game::Render()
{
	/* 랜더링 시작 */
	_graphics->RenderBegin();

	/* IA - VS - RS - PS - OM */
	{
		// IA (Input Assembler) : 정점의 정보 전달
		uint32 stride = sizeof(Vertex);					// 정점 1개 크기 (28바이트)
		uint32 offset = 0;								// 버퍼의 시작 위치 오프셋

		/* GPU에게 정점 버퍼의 크기와 위치(stride, offset) 전달, vertices 사용함을 GPU에 알려줌 */
		_graphics->GetDeviceContext()->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
		/* 32-bit(4Byte) uint 인덱스 버퍼 GPU에 바인딩 */
		_graphics->GetDeviceContext()->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		/* GPU에게 정점 데이터의 구조 전달 */
		_graphics->GetDeviceContext()->IASetInputLayout(_inputLayout.Get());
		/* 각 정점을 어떻게 이어줄지 전달, 삼각형으로 이어주도록 설정 */
		_graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// VS (Vertex Shader) : 정점의 위치/색상 등 가공
		_graphics->GetDeviceContext()->VSSetShader(_vertexShader.Get(), nullptr, 0);
		_graphics->GetDeviceContext()->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());

		// RS (Rasterizer) : 정점 → 픽셀로 삼각형 그리기
		/* Draw()호출 시 내부적으로 GPU가 자동으로 Rasterizer 실행함 */
		_graphics->GetDeviceContext()->RSSetState(_rasterizerState.Get());

		// PS (Pixel Shader) : 픽셀 단위 색상 처리
		_graphics->GetDeviceContext()->PSSetShader(_pixelShader.Get(), nullptr, 0);
		_graphics->GetDeviceContext()->PSSetShaderResources(0, 1, _shaderResourceView.GetAddressOf());
		_graphics->GetDeviceContext()->PSSetSamplers(0, 1, _samplerState.GetAddressOf());

		// OM (Output Merger) : 최종 픽셀을 렌더 타겟에 출력
		/* 실제 삼각형 그리기 (정점 개수 3, 시작 offset 0) */
		//_deviceContext->Draw(_vertices.size(), 0);
		_graphics->GetDeviceContext()->DrawIndexed(_indices.size(), 0, 0);
	}

	/* 랜더링 끝, 화면에 출력 */
	_graphics->RenderEnd();
}

void Game::CreateGeometry()
{
	/* Vertex Data */
	{
		/*	사각형이니까 정점 4개, 사각형의 정점 구성도
		*	1  3
		*	0  2
		*/
		_vertices.resize(4);

		_vertices[0].position = Vec3(-0.5f, -0.5f, 0);
		_vertices[0].uv = Vec2(0.f, 1.f);
		//_vertices[0].color = Color(1.0f, 0.611f, 0.420f, 1.0f);

		_vertices[1].position = Vec3(-0.5f, 0.5f, 0);
		_vertices[1].uv = Vec2(0.f, 0.f);
		//_vertices[1].color = Color(0.447f, 0.792f, 0.820f, 1.0f);

		_vertices[2].position = Vec3(0.5f, -0.5f, 0);
		_vertices[2].uv = Vec2(1.f, 1.f);
		//_vertices[2].color = Color(0.996f, 0.969f, 0.8f, 1.0f);

		_vertices[3].position = Vec3(0.5f, 0.5f, 0);
		_vertices[3].uv = Vec2(1.f, 0.f);
		//_vertices[3].color = Color(1.0f, 0.611f, 0.420f, 1.0f);
	}
	/* Vertex Buffer 생성 */
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		/* GPU만 읽을 수 있음. CPU 접근 불가 */
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		/* Vertex Buffer 바인딩 용도로 사용 */
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		/* Vertex 크기 * 정점 개수 */
		desc.ByteWidth = static_cast<uint32>(sizeof(Vertex) * _vertices.size());
		
		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		/* 첫번째 data 시작주소, _vertices.data()로 대체 가능 */
		data.pSysMem = &_vertices[0];

		// desc와 data를 기반으로 버퍼 생성 후 _vertexBuffer에 저장
		_graphics->GetDevice()->CreateBuffer(&desc, &data, _vertexBuffer.GetAddressOf());
	}
	/* Index 구성 */
	{
		// 삼각형 1 : _vertices[0], _vertices[1], _vertices[2]
		// 삼각형 2 : _vertices[2], _vertices[1], _vertices[3]
		_indices = {
			0, 1, 2,
			2, 1, 3
		};
	}
	/* Index Buffer 생성 */
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		/* GPU만 읽을 수 있음. CPU 접근 불가 */
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		/* Index Buffer 바인딩 용도로 사용 */
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		/* uint32 크기 * 인덱스 개수 */
		desc.ByteWidth = static_cast<uint32>(sizeof(uint32) * _indices.size());

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		/* 첫번째 data 시작주소, _indices.data()로 대체 가능 */
		data.pSysMem = &_indices[0];

		// desc와 data를 기반으로 버퍼 생성 후 _vertexBuffer에 저장
		HRESULT hResult = _graphics->GetDevice()->CreateBuffer(&desc, &data, _indexBuffer.GetAddressOf());
		CHECK(hResult);
	}
}

void Game::CreateInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC layouts[] = 
	{
		/* 정점 위치(POSITION): float 3개 (X, Y, Z), 오프셋 0부터 시작 */
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		/*  UV : float 2개 (R, G), 오프셋 12부터 시작 (POSITION 뒤) */
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	/* (배열 / 원소)로 원소 개수 구하기 */
	const int32 count = sizeof(layouts) / sizeof(D3D11_INPUT_ELEMENT_DESC);

	/* 입력 레이아웃 생성 */
	_graphics->GetDevice()->CreateInputLayout(
		layouts, count, _vertexBlob->GetBufferPointer(), _vertexBlob->GetBufferSize(), _inputLayout.GetAddressOf()
	);
}

void Game::CreateVertexShader()
{
	/* vertex shader load 후 _vertexBlob에 결과 저장 */
	LoadShaderFromFile(L"DefaultVertexShader.hlsl", "VS_main", "vs_5_0", _vertexBlob);

	/* 생성된 _vertexBlob 정보를 통해 _vertexShader 생성 */
	HRESULT hResult = _graphics->GetDevice()->CreateVertexShader(
		_vertexBlob->GetBufferPointer(),
		_vertexBlob->GetBufferSize(),
		nullptr,
		_vertexShader.GetAddressOf()
	);
	CHECK(hResult);
}

void Game::CreatePixelShader()
{
	/* pixel shader load 후 _pixelBlob에 결과 저장 */
	LoadShaderFromFile(L"DefaultVertexShader.hlsl", "PS", "ps_5_0", _pixelBlob);

	/* 생성된 _pixelBlob 정보를 통해 _pixelShader 생성 */
	HRESULT hResult = _graphics->GetDevice()->CreatePixelShader(
		_pixelBlob->GetBufferPointer(),
		_pixelBlob->GetBufferSize(),
		nullptr,
		_pixelShader.GetAddressOf()
	);
	CHECK(hResult);
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

void Game::CreateShaderResourceView()
{
	// PNG 이미지 파일 로드
	DirectX::TexMetadata md;		// 텍스처 메타데이터 (크기, 포맷 등)
	DirectX::ScratchImage img;		// 이미지 데이터를 임시 저장할 객체

	// WIC(Windows Imaging Component)를 통해 PNG 파일을 읽고 img에 저장
	HRESULT hResult = ::LoadFromWICFile(L"hamster_latte.png", WIC_FLAGS_NONE, &md, img);
	CHECK(hResult);

	// 읽어온 이미지 데이터를 기반으로 Shader Resource View 생성
	hResult = ::CreateShaderResourceView(_graphics->GetDevice().Get(), img.GetImages(), img.GetImageCount(), md, _shaderResourceView.GetAddressOf());
	CHECK(hResult);
}

void Game::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	// CPU 쓰기 가능, GPU 읽기 가능 (매 프레임마다 CPU에서 업데이트 할 경우)
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = sizeof(TransformData);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// 설정된 desc를 기반으로 GPU에 Constant Buffer 생성 후 _constantBuffer에 저장
	HRESULT hResult = _graphics->GetDevice()->CreateBuffer(&desc, nullptr, _constantBuffer.GetAddressOf());
	CHECK(hResult);
}

void Game::LoadShaderFromFile(const std::wstring& path, const std::string& name, const std::string& version, ComPtr<ID3DBlob>& blob)
{
	const uint32 comileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	
	HRESULT hResult = ::D3DCompileFromFile(
		path.c_str(),								// path: HLSL 셰이더 파일 경로
		nullptr,									// pDefines: 셰이더 전처리 매크로 (사용 안 함)
		D3D_COMPILE_STANDARD_FILE_INCLUDE,			// pInclude: #include 허용 (로컬 파일 포함 가능)
		name.c_str(),								// entryPoint: 셰이더 진입 지점 함수 이름 (예: "main")
		version.c_str(),							// target: 셰이더 모델 (예: "vs_5_0", "ps_5_0")
		comileFlag,									// Flags1: 컴파일 옵션 (디버그용 플래그 사용)
		0,											// Flags2: 고정값 (사용 안 함)
		blob.GetAddressOf(),						// ppCode: 컴파일 결과(ID3DBlob)를 저장할 포인터
		nullptr										// ppErrorMsgs: 에러 메시지 Blob (사용 안 함)
	);
	CHECK(hResult);
}