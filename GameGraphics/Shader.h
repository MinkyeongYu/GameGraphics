#pragma once

// 셰이더가 어디서 쓰이는지 구분하는 용도
enum ShaderScope
{
	SS_None = 0,
	SS_VertexShader = (1 << 0),		// 1
	SS_PixelShader	= (1 << 1),		// 왼쪽 비트로 한칸 옮김 (2^0 -> 2^1), 2
	SS_Both			= SS_PixelShader | SS_VertexShader,		// 3
};

/* Shader
 * : 셰이더(Shader)를 로드하고 컴파일하며, 셰이더 바이트코드를 관리하는 기본 클래스
 */
class Shader
{
public:
	Shader(ComPtr<ID3D11Device> device);
	virtual ~Shader();

	virtual void Create(const std::wstring& path, const std::string& name, const std::string& version) abstract;

	ComPtr<ID3DBlob> GetBlob() { return _blob; }

protected:
	/* DefaultVertexShader.hlsl에서 shader 로드하고 컴파일하여 결과를 ID3DBlob에 저장 */
	void LoadShaderFromFile(const std::wstring& path, const std::string& name, const std::string& version);

protected:
	// 정점/픽셀 셰이더의 바이트 코드 (InputLayout 생성에도 사용)
	ComPtr<ID3DBlob> _blob = nullptr;

	ComPtr<ID3D11Device> _device;
	std::wstring _path;
	std::string _name;

};

/* VertexShader
 * : Vertex Shader(정점 변환)를 생성 및 관리하는 클래스
 */
class VertexShader : public Shader
{
	using Super = Shader;
public:
	VertexShader(ComPtr<ID3D11Device> device);
	~VertexShader();

	ComPtr<ID3D11VertexShader> GetComPtr() { return _vertexShader; }

	virtual void Create(const std::wstring& path, const std::string& name, const std::string& version) override;

protected:
	ComPtr<ID3D11VertexShader> _vertexShader = nullptr;
};

/* PixelShader
 * : Pixel Shader(픽셀 색상 계산)를 생성 및 관리하는 클래스
 */
class PixelShader : public Shader
{
	using Super = Shader;
public:
	PixelShader(ComPtr<ID3D11Device> device);
	~PixelShader();

	ComPtr<ID3D11PixelShader> GetComPtr() { return _pixelShader; }

	virtual void Create(const std::wstring& path, const std::string& name, const std::string& version) override;

protected:
	ComPtr<ID3D11PixelShader> _pixelShader = nullptr;
};