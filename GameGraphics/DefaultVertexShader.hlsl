// Vertex Shader

struct VS_INPUT
{
	// x, y, z, w
	float4 position : Position;
	// r, g, b, a
	//float4 color	: COLOR;
	float2 uv : TEXCOORD;
};

struct	VS_OUTPUT
{
	float4 position : SV_Position;
	//float4 color	: COLOR;
	float2 uv : TEXCOORD;
};

cbuffer TransformData : register(b0)
{
	float4 offset;
}

// IA - VS - RS(VS_main에서 자동 처리) - PS - OM
// VS_main은 정점 단위로 실행, 위치 관련 처리
VS_OUTPUT VS_main(VS_INPUT input)
{
	VS_OUTPUT output;
	output.position = input.position + offset;
	output.uv = input.uv;

	return output;
}

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

// Pixel Shader, 모든 픽셀 대상으로 실행, 색상 관련 처리
// 픽셀 셰이더 PS의 결과값을 렌더 타겟 0번에 저장
float4 PS(VS_OUTPUT input) : SV_Target
{
	// texture0의 input.uv좌표에 해당하는 색을 color에 저장 
	float4 color = texture0.Sample(sampler0, input.uv);	
	
	// texture0의 resource의 알파값이 0.1보다 작다면 화면에 출력X
	if(color.a <= 0.1f)
		discard;

	return color;
}