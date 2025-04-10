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

	CreateDeviceAndSwapChain();     // [1] DirectX �ٽ� ��ü�� ����
	CreateRenderTargetView();       // [2] ����� ����� ���� Ÿ�� �� ����
	SetViewport();                  // [3] ȭ�鿡 ����� ����Ʈ ũ�� ����

	CreateGeometry();               // [4] ���� ������ ���� �� GPU ���ۿ� ���ε�, vertexBuffer ����
	CreateVertexShader();           // [5] ���� ���̴� ������ �� ����
	CreateInputLayout();            // [6] ���� ���� ���� (���̴��� ���� ����)
	CreateRasterizerState();		// [7] �����Ͷ����� ���� ���� (�ﰢ�� �׸��� ���, �ø�, ä��� ��� �� ����)
	CreatePixelShader();            // [8] �ȼ� ���̴� ������ �� ����
	CreateSamplerState();			// [9] ���÷� ���� ����

	CreateShaderResourceView();		// [10] �ؽ�ó ���ҽ� �� ����
	CreateConstantBuffer();			// [11] ��� ���� ����
	CreateBlendState();				// [12] ���� ���� ����
}

void Game::Update()
{
	//_transformData.offset.x += 0.003f;
	//_transformData.offset.y += 0.003f;

	D3D11_MAPPED_SUBRESOURCE subResource;
	ZeroMemory(&subResource, sizeof(subResource));

	// GPU�� Constant Buffer�� CPU���� ���� ���� ���� ��û
	_deviceContext->Map(
		_constantBuffer.Get(),			// ������Ʈ�� Constant Buffer
		0,								// ���긮�ҽ� �ε��� (�Ϲ������� 0)
		D3D11_MAP_WRITE_DISCARD,		// ���� ������ ������ ���� ���� (���� �Ϲ����� ���)
		0,								// Reserved (�׻� 0)
		&subResource					// ���� ����� ���� ����ü (CPU�� ���� ������ ������ ������)
	);
	// _transformData���� GPU�޸𸮷� ������ ��, GPU�� Constant Buffer�� ���ε�
	::memcpy(subResource.pData, &_transformData, sizeof(_transformData));
	// ���� ���� �� GPU���� ���� �� �ֵ��� �ٽ� ����
	_deviceContext->Unmap(_constantBuffer.Get(), 0);
}

void Game::Render()
{
	/* ������ ���� */
	RenderBegin();

	/* IA - VS - RS - PS - OM */
	{
		// IA (Input Assembler) : ������ ���� ����
		uint32 stride = sizeof(Vertex);					// ���� 1�� ũ�� (28����Ʈ)
		uint32 offset = 0;								// ������ ���� ��ġ ������

		/* GPU���� ���� ������ ũ��� ��ġ(stride, offset) ����, vertices ������� GPU�� �˷��� */
		_deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
		/* 32-bit(4Byte) uint �ε��� ���� GPU�� ���ε� */
		_deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		/* GPU���� ���� �������� ���� ���� */
		_deviceContext->IASetInputLayout(_inputLayout.Get());
		/* �� ������ ��� �̾����� ����, �ﰢ������ �̾��ֵ��� ���� */
		_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// VS (Vertex Shader) : ������ ��ġ/���� �� ����
		_deviceContext->VSSetShader(_vertexShader.Get(), nullptr, 0);
		_deviceContext->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());

		// RS (Rasterizer) : ���� �� �ȼ��� �ﰢ�� �׸���
		/* Draw()ȣ�� �� ���������� GPU�� �ڵ����� Rasterizer ������ */
		_deviceContext->RSSetState(_rasterizerState.Get());

		// PS (Pixel Shader) : �ȼ� ���� ���� ó��
		_deviceContext->PSSetShader(_pixelShader.Get(), nullptr, 0);
		_deviceContext->PSSetShaderResources(0, 1, _shaderResourceView.GetAddressOf());
		_deviceContext->PSSetSamplers(0, 1, _samplerState.GetAddressOf());

		// OM (Output Merger) : ���� �ȼ��� ���� Ÿ�ٿ� ���
		/* ���� �ﰢ�� �׸��� (���� ���� 3, ���� offset 0) */
		//_deviceContext->Draw(_vertices.size(), 0);
		_deviceContext->DrawIndexed(_indices.size(), 0, 0);
	}

	/* ������ ��, ȭ�鿡 ��� */
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
	hResult = _device->CreateRenderTargetView(backBuffer.Get(), nullptr, _renderTargetView.GetAddressOf());
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

void Game::CreateGeometry()
{
	/* Vertex Data */
	{
		/*	�簢���̴ϱ� ���� 4��, �簢���� ���� ������
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
	/* Vertex Buffer ���� */
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		/* GPU�� ���� �� ����. CPU ���� �Ұ� */
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		/* Vertex Buffer ���ε� �뵵�� ��� */
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		/* Vertex ũ�� * ���� ���� */
		desc.ByteWidth = static_cast<uint32>(sizeof(Vertex) * _vertices.size());
		
		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		/* ù��° data �����ּ�, _vertices.data()�� ��ü ���� */
		data.pSysMem = &_vertices[0];

		// desc�� data�� ������� ���� ���� �� _vertexBuffer�� ����
		_device->CreateBuffer(&desc, &data, _vertexBuffer.GetAddressOf());
	}
	/* Index ���� */
	{
		// �ﰢ�� 1 : _vertices[0], _vertices[1], _vertices[2]
		// �ﰢ�� 2 : _vertices[2], _vertices[1], _vertices[3]
		_indices = {
			0, 1, 2,
			2, 1, 3
		};
	}
	/* Index Buffer ���� */
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		/* GPU�� ���� �� ����. CPU ���� �Ұ� */
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		/* Index Buffer ���ε� �뵵�� ��� */
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		/* uint32 ũ�� * �ε��� ���� */
		desc.ByteWidth = static_cast<uint32>(sizeof(uint32) * _indices.size());

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		/* ù��° data �����ּ�, _indices.data()�� ��ü ���� */
		data.pSysMem = &_indices[0];

		// desc�� data�� ������� ���� ���� �� _vertexBuffer�� ����
		HRESULT hResult = _device->CreateBuffer(&desc, &data, _indexBuffer.GetAddressOf());
		CHECK(hResult);
	}
}

void Game::CreateInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC layouts[] = 
	{
		/* ���� ��ġ(POSITION): float 3�� (X, Y, Z), ������ 0���� ���� */
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		/*  UV : float 2�� (R, G), ������ 12���� ���� (POSITION ��) */
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	/* (�迭 / ����)�� ���� ���� ���ϱ� */
	const int32 count = sizeof(layouts) / sizeof(D3D11_INPUT_ELEMENT_DESC);

	/* �Է� ���̾ƿ� ���� */
	_device->CreateInputLayout(
		layouts, count, _vertexBlob->GetBufferPointer(), _vertexBlob->GetBufferSize(), _inputLayout.GetAddressOf()
	);
}

void Game::CreateVertexShader()
{
	/* vertex shader load �� _vertexBlob�� ��� ���� */
	LoadShaderFromFile(L"DefaultVertexShader.hlsl", "VS_main", "vs_5_0", _vertexBlob);

	/* ������ _vertexBlob ������ ���� _vertexShader ���� */
	HRESULT hResult = _device->CreateVertexShader(
		_vertexBlob->GetBufferPointer(),
		_vertexBlob->GetBufferSize(),
		nullptr,
		_vertexShader.GetAddressOf()
	);
	CHECK(hResult);
}

void Game::CreatePixelShader()
{
	/* pixel shader load �� _pixelBlob�� ��� ���� */
	LoadShaderFromFile(L"DefaultVertexShader.hlsl", "PS", "ps_5_0", _pixelBlob);

	/* ������ _pixelBlob ������ ���� _pixelShader ���� */
	HRESULT hResult = _device->CreatePixelShader(
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
	// �ȼ��� ä��� ���: SOLID(ä���) �Ǵ� WIREFRAME(���� �׸���)
	desc.FillMode = D3D11_FILL_SOLID;
	// �ø� ���: BACK(���� �� ����), FRONT(�ո� ����), NONE(���� �� ��)
	desc.CullMode = D3D11_CULL_BACK;
	// �ﰢ���� �ո� �ð����/�ݽð���� ���� (false = �ð������ �ո�)
	desc.FrontCounterClockwise = false;

	HRESULT hResult = _device->CreateRasterizerState(&desc, _rasterizerState.GetAddressOf());
	CHECK(hResult);
}

void Game::CreateSamplerState()
{
	// ���÷� ����(Sampler State) ���� ����ü �ʱ�ȭ
	D3D11_SAMPLER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	// �ؽ�ó ��ǥ ������ ����� �� �׵θ� ����(Border Color) ��� (U, V, W ������ ����)
	desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;

	// �׵θ�(Border) ���� ���� R,G,B,A (����: 1,0,0,1)
	desc.BorderColor[0] = 1;
	desc.BorderColor[1] = 0;
	desc.BorderColor[2] = 0;
	desc.BorderColor[3] = 1;

	// �� �Լ� (������� ���� �� �׻� ���)
	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;

	// ���� ���� (�ؽ�ó ���/Ȯ��/�Ӹ� ���� ���� ����)
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	// �̹漺 ���͸� �ִ�ġ (�Ϲ������� 16 ���)
	desc.MaxAnisotropy = 16;

	// ����� �Ӹ� LOD ���� ����
	desc.MinLOD = FLT_MIN;
	desc.MaxLOD = FLT_MAX;

	// �Ӹ� LOD ������ (�⺻�� 0)
	desc.MipLODBias = 0.f;

	// �� ������ ������� ���÷� ���� ��ü ����
	HRESULT hResult = _device->CreateSamplerState(&desc, _samplerState.GetAddressOf());
	CHECK(hResult);
}

void Game::CreateBlendState()
{
	D3D11_BLEND_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	// ��Ƽ ���ø� �� ���� Ŀ������ ��� ���� (�Ϲ������� false)
	desc.AlphaToCoverageEnable = false;

	// MRT(Multiple Render Targets) ������ �ٸ� ���� ������ ���� ���� (false = ���� ���� ���)
	desc.IndependentBlendEnable = false;

	// ���� Ÿ�� 0�� ���� ����
	desc.RenderTarget[0].BlendEnable = true;								// ���� Ȱ��ȭ

	// �÷� ���� ���� (�� �ȼ� �� * SrcBlend + ���� �ȼ� �� * DestBlend)
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;					// �� �ȼ� ���İ�
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;				// (1 - �� �ȼ� ���İ�)
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;						// Src + Dest

	// ���� ���� ���� (���� ���� ������ �����ϰ� ó��)
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;					// ���� 100%
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;					// ���� ����
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;					// Src + Dest

	// ����� ���� ä�� ���� (R, G, B, A ���� ���)
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// �� ������ ������� BlendState ����
	HRESULT hResult = _device->CreateBlendState(&desc, _blendState.GetAddressOf());
	CHECK(hResult);
}

void Game::CreateShaderResourceView()
{
	// PNG �̹��� ���� �ε�
	DirectX::TexMetadata md;		// �ؽ�ó ��Ÿ������ (ũ��, ���� ��)
	DirectX::ScratchImage img;		// �̹��� �����͸� �ӽ� ������ ��ü

	// WIC(Windows Imaging Component)�� ���� PNG ������ �а� img�� ����
	HRESULT hResult = ::LoadFromWICFile(L"hamster_latte.png", WIC_FLAGS_NONE, &md, img);
	CHECK(hResult);

	// �о�� �̹��� �����͸� ������� Shader Resource View ����
	hResult = ::CreateShaderResourceView(_device.Get(), img.GetImages(), img.GetImageCount(), md, _shaderResourceView.GetAddressOf());
	CHECK(hResult);
}

void Game::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	// CPU ���� ����, GPU �б� ���� (�� �����Ӹ��� CPU���� ������Ʈ �� ���)
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = sizeof(TransformData);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// ������ desc�� ������� GPU�� Constant Buffer ���� �� _constantBuffer�� ����
	HRESULT hResult = _device->CreateBuffer(&desc, nullptr, _constantBuffer.GetAddressOf());
	CHECK(hResult);
}

void Game::LoadShaderFromFile(const std::wstring& path, const std::string& name, const std::string& version, ComPtr<ID3DBlob>& blob)
{
	const uint32 comileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	
	HRESULT hResult = ::D3DCompileFromFile(
		path.c_str(),								// path: HLSL ���̴� ���� ���
		nullptr,									// pDefines: ���̴� ��ó�� ��ũ�� (��� �� ��)
		D3D_COMPILE_STANDARD_FILE_INCLUDE,			// pInclude: #include ��� (���� ���� ���� ����)
		name.c_str(),								// entryPoint: ���̴� ���� ���� �Լ� �̸� (��: "main")
		version.c_str(),							// target: ���̴� �� (��: "vs_5_0", "ps_5_0")
		comileFlag,									// Flags1: ������ �ɼ� (����׿� �÷��� ���)
		0,											// Flags2: ������ (��� �� ��)
		blob.GetAddressOf(),						// ppCode: ������ ���(ID3DBlob)�� ������ ������
		nullptr										// ppErrorMsgs: ���� �޽��� Blob (��� �� ��)
	);
	CHECK(hResult);
}