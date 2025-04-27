#pragma once

/* Texture
 * : GPU 리소스(텍스처)를 로드하고, 셰이더(Shader)에서 접근할 수 있도록 Shader Resource View를 생성 및 관리하는 클래스
 */
class Texture
{
public:
	Texture(ComPtr<ID3D11Device> device);
	~Texture();

	ComPtr<ID3D11ShaderResourceView> GetComPtr() { return _shaderResourceView; }

	void CreateShaderResourceView(const std::wstring& path);

private:
	ComPtr<ID3D11Device> _device;

	/* Shader Resource View
		: 셰이더(Shader)가 GPU 리소스(텍스처, 버퍼 등)에 접근할 수 있도록 만들어주는 객체 */
	ComPtr<ID3D11ShaderResourceView> _shaderResourceView;
};