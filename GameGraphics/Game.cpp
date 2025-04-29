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
	_graphics = std::make_shared<Graphics>(hwnd);
	_pipeline = std::make_shared<Pipeline>(_graphics->GetDeviceContext());
	_gameObject = std::make_shared<GameObject>(_graphics->GetDevice(), _graphics->GetDeviceContext());
}

void Game::Update()
{
	_gameObject->Update();
}

void Game::Render()
{
	/* 랜더링 시작 */
	_graphics->RenderBegin();

	{
		_gameObject->Render(_pipeline);
	}

	/* 랜더링 끝, 화면에 출력 */
	_graphics->RenderEnd();
}