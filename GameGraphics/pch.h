// GameGrphics 프로젝트에 필요한, 자주 쓰이는 파일 모두 소유
#pragma once

/* Custom Files */
#include "Types.h"
#include "Values.h"
#include "Struct.h"

/* STL */
#include <unordered_map>
#include <vector>
#include <list>
#include <map>

//using namespace std;

/* WIN API */
#include <windows.h>
#include <assert.h>
#include <wrl.h>
#include <sal.h>  

/* DirectX */
#include <DirectXMath.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>

using namespace DirectX;
using namespace Microsoft::WRL;


#pragma comment(lib, "d3d11.lib")       
#pragma comment(lib, "d3dcompiler.lib")

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex\\DirectXTex_debug.lib")
#else
#pragma comment(lib, "DirectXTex\\DirectXTex.lib")
#endif

#define CHECK(p) assert(SUCCEEDED(p));