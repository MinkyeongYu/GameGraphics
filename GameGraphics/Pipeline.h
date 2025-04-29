#pragma once

/* PipelineInfo
 * : 파이프라인(Pipeline)에서 공통적으로 사용할 객체들(InputLayout, VertexShader, PixelShader 등)과 Topology 정보를 모아둔 구조체
 */
struct PipelineInfo
{
	/* Input Layout
	 * : 정점 버퍼(Vertex Buffer)의 데이터 구조를 GPU 셰이더에게 알려주는 객체
	 */
	std::shared_ptr<InputLayout> _inputLayout;

	/* Vertex Shader
	 * : 정점 변환 단계 (월드/뷰/프로젝션 행렬 적용)에서 실행되는 셰이더
	 */
	std::shared_ptr<VertexShader> _vertexShader;

	/* Pixel Shader
	 * : 픽셀 색상 계산 단계 (텍스처, 라이팅 적용)에서 실행되는 셰이더
	 */
	std::shared_ptr<PixelShader> _pixelShader;

	/* Rasterizer State
	 * : 삼각형을 화면에 어떻게 그릴지(채우기, 컬링 등) 설정하는 래스터라이저 상태 객체
	 */
	std::shared_ptr<RasterizerState> _rasterizerState;

	/* Blend State
	 * : 픽셀 색상 혼합(알파 블렌딩 등) 방식을 정의하는 블렌딩 상태 객체
	 */
	std::shared_ptr<BlendState> _blendState;


	/* Primitive Topology
	 * : 정점을 어떤 형태(삼각형 리스트, 라인 스트립 등)로 그릴지 정의하는 설정
	 */
	D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};

/* Pipeline
 * : PipelineInfo를 기반으로 렌더링 파이프라인 상태를 구성하고 설정하는 클래스
 */
class Pipeline
{
public:
	Pipeline(ComPtr<ID3D11DeviceContext> deviceContext);
	~Pipeline();

public:
	/* UpdatePipeline
	 * : IA ~ OM 단계까지 PipelineInfo를 기반으로 GPU 상태를 한 번에 설정
	 */
	void UpdatePipeline(PipelineInfo info);

	/* SetVertexBuffer
	 * : VertexBuffer를 Input Assembler 단계에 설정
	 */
	void SetVertexBuffer(std::shared_ptr<VertexBuffer> buffer);
	/* SetIndexBuffer
	 * : IndexBuffer를 Input Assembler 단계에 설정
	 */
	void SetIndexBuffer(std::shared_ptr<IndexBuffer> buffer);

	/* SetConstantBuffer
	* : VS/PS 셰이더에 공통 데이터를 전달하는 Constant Buffer를 설정
	*/
	template <typename T>
	void SetConstantBuffer(uint32 slot, uint32 scope, std::shared_ptr<ConstantBuffer<T>> buffer)
	{
		if (scope & SS_VertexShader)
		{
			_deviceContext->VSSetConstantBuffers(slot, 1, buffer->GetComPtr().GetAddressOf());
		}
		if (scope & SS_PixelShader)
		{
			_deviceContext->PSSetConstantBuffers(slot, 1, buffer->GetComPtr().GetAddressOf());
		}
	}

	/* SetTexture
	 * : VertexShader와 PixelShader에 텍스처(Shader Resource View)를 바인딩
	 */
	void SetShaderResources(uint32 slot, uint32 scope, std::shared_ptr<Texture> shaderResouceView);
	/* SetSamplerState
	 * : VertexShader와 PixelShader에 샘플러(Sampler State)를 바인딩
	 */
	void SetSamplerState(uint32 slot, uint32 scope, std::shared_ptr<SamplerState> samplerState);

	/* Draw
	 * : OM단계에 정점(Vertex) 데이터만으로 드로우 (인덱스 없이)
	 */
	void Draw(uint32 vertexCount, uint32 startVertexLocation);
	/* DrawIndexed
	 * : OM단계에 인덱스(Index) 데이터를 사용하여 드로우
	 */
	void DrawIndexed(uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation);

private:
	ComPtr<ID3D11DeviceContext> _deviceContext;
};