#pragma once
#include "pch.h"

/* Game
 * : 게임 루프(Init, Update, Render)를 관리하고, 렌더링 파이프라인 전반을 구성하는 클래스
 */
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
	HWND _hwnd;

	/* Graphics
	: Device, DeviceContext, SwapChain, RenderTargetView, Viewport 생성 */
	std::shared_ptr<Graphics> _graphics;

	/* Pipeline
	 * : 렌더링 파이프라인 상태(PipelineInfo)를 기반으로 GPU 상태를 설정하고, 그리는 역할을 하는 객체
	 */
	std::shared_ptr<Pipeline> _pipeline;

	/* GameObject
	 * : 3D 공간상에 배치되는 기본 게임 오브젝트 인스턴스를 가리키는 포인터
	 */
	std::shared_ptr<GameObject> _gameObject;
};