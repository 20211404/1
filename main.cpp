#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

#include "GameLoop.hpp"
#include "MeshRenderer.hpp"
#include "ButtonFSM.hpp"
#include "InfoButton.hpp"
#include "Logger.hpp"

LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (m == WM_DESTROY) { PostQuitMessage(0); return 0; }
    return DefWindowProc(h, m, w, l);
}

int WINAPI WinMain(HINSTANCE hI, HINSTANCE, LPSTR, int nS) {
    // ==============================================================================
    // [1. 엔진 초기화 및 리소스 준비]
    // 싱글톤 Logger를 가장 먼저 초기화하여 이후 발생하는 모든 이벤트를 기록할 준비를 합니다.
    // 윈도우 창과 DirectX 장치를 세팅하고, 버튼을 그릴 사각형 정점 데이터와 셰이더를 생성합니다.
    // ==============================================================================
    Logger::Get()->Initialize("EngineLog.txt");
    Logger::Get()->Log(LogLevel::Info, "Engine Initialization Started.");

    GameLoop engine;
    engine.Initialize(hI, WndProc);

    D3D11_INPUT_ELEMENT_DESC ied[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    ShaderSet shaders = engine.gfx.CompileAndCreate(L"effect.hlsl", 0, true, ied, 2);

    std::vector<Vertex> vRect = {
        {{-1.0f,  1.0f, 0.0f}, {1,1,1,1}}, {{ 1.0f,  1.0f, 0.0f}, {1,1,1,1}}, {{-1.0f, -1.0f, 0.0f}, {1,1,1,1}},
        {{-1.0f, -1.0f, 0.0f}, {1,1,1,1}}, {{ 1.0f,  1.0f, 0.0f}, {1,1,1,1}}, {{ 1.0f, -1.0f, 0.0f}, {1,1,1,1}}
    };
    Mesh* gMesh = new Mesh();
    gMesh->Create(&engine.gfx, vRect);

    // ==============================================================================
    // [2. ECS 기반 게임 오브젝트 조립 - 왼쪽 버튼]
    // 상속을 배제하고 빈 GameObject에 필요한 기능(컴포넌트)을 조립하는 방식을 사용합니다.
    // 렌더링을 담당하는 MeshRenderer와 상태 제어를 담당하는 ButtonFSM을 부착합니다.
    // ==============================================================================
    GameObject* leftBtn = new GameObject(-0.5f, 0, 0);
    leftBtn->scale = { 0.3f, 0.2f, 1.0f };
    ColorMaterial* matL = new ColorMaterial(shaders, { 1, 1, 1, 1 }, engine.gfx.Device);

    ButtonFSM* fsmComp = new ButtonFSM(matL);
    leftBtn->AddComponent(new MeshRenderer(gMesh, matL));
    leftBtn->AddComponent(fsmComp);

    // ==============================================================================
    // [3. ECS 기반 게임 오브젝트 조립 - 오른쪽 버튼]
    // 오른쪽 오브젝트를 구성할 때, InfoButton 컴포넌트를 부착하며
    // 앞서 생성한 왼쪽 버튼의 포인터(fsmComp)를 전달하여 두 객체 간의 데이터 통신을 구축합니다.
    // ==============================================================================
    GameObject* rightBtn = new GameObject(0.5f, 0, 0);
    rightBtn->scale = { 0.3f, 0.2f, 1.0f };
    ColorMaterial* matR = new ColorMaterial(shaders, { 0.5f, 0.5f, 0.5f, 1 }, engine.gfx.Device);

    rightBtn->AddComponent(new MeshRenderer(gMesh, matR));
    rightBtn->AddComponent(new InfoButton(fsmComp));

    // ==============================================================================
    // [4. 게임 루프 실행 및 리소스 해제]
    // 완성된 오브젝트들을 월드에 등록하고 렌더링 무한 루프를 돌립니다.
    // 프로그램 종료 시 동적 할당된 리소스들을 해제하여 메모리 누수를 방지합니다.
    // ==============================================================================
    engine.world.push_back(leftBtn);
    engine.world.push_back(rightBtn);

    Logger::Get()->Log(LogLevel::Info, "Game Loop is starting...");
    engine.Run();

    Logger::Get()->Log(LogLevel::Info, "Engine Terminated.");

    delete gMesh; delete matL; delete matR; shaders.Release();
    return 0;
}