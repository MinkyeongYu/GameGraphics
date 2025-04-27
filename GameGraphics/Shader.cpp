#include "pch.h"
#include "Shader.h"

Shader::Shader(ComPtr<ID3D11Device> device)
	:_device(device)
{

}

Shader::~Shader()
{

}

void Shader::LoadShaderFromFile(const std::wstring& path, const std::string& name, const std::string& version)
{
	_path = path;
	_name = name;

	const uint32 comileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

	HRESULT hResult = ::D3DCompileFromFile(
		path.c_str(),								// path: HLSL 셰이더 파일 경로
		nullptr,									// pDefines: 셰이더 전처리 매크로 (사용 안 함)
		D3D_COMPILE_STANDARD_FILE_INCLUDE,			// pInclude: #include 허용 (로컬 파일 포함 가능)
		name.c_str(),								// entryPoint: 셰이더 진입 지점 함수 이름 (예: "main")
		version.c_str(),							// target: 셰이더 모델 (예: "vs_5_0", "ps_5_0")
		comileFlag,									// Flags1: 컴파일 옵션 (디버그용 플래그 사용)
		0,											// Flags2: 고정값 (사용 안 함)
		_blob.GetAddressOf(),						// ppCode: 컴파일 결과(ID3DBlob)를 저장할 포인터
		nullptr										// ppErrorMsgs: 에러 메시지 Blob (사용 안 함)
	);
	CHECK(hResult);
}

VertexShader::VertexShader(ComPtr<ID3D11Device> device) 
	: Super(device)
{
}

VertexShader::~VertexShader()
{
}

void VertexShader::Create(const std::wstring& path, const std::string& name, const std::string& version)
{
	/* vertex shader load 후 _vertexBlob에 결과 저장 */
	LoadShaderFromFile(path, name, version);

	/* 생성된 _vertexBlob 정보를 통해 _vertexShader 생성 */
	HRESULT hResult = _device->CreateVertexShader(
		_blob->GetBufferPointer(),
		_blob->GetBufferSize(),
		nullptr,
		_vertexShader.GetAddressOf()
	);
	CHECK(hResult);
}

PixelShader::PixelShader(ComPtr<ID3D11Device> device) 
	: Super(device)
{
}

PixelShader::~PixelShader()
{
}

void PixelShader::Create(const std::wstring& path, const std::string& name, const std::string& version)
{
	/* pixel shader load 후 _pixelBlob에 결과 저장 */
	LoadShaderFromFile(path, name, version);

	/* 생성된 _pixelBlob 정보를 통해 _pixelShader 생성 */
	HRESULT hResult = _device->CreatePixelShader(
		_blob->GetBufferPointer(),
		_blob->GetBufferSize(),
		nullptr,
		_pixelShader.GetAddressOf()
	);
	CHECK(hResult);
}