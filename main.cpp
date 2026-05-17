#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

#include "GameLoop.hpp"
#include "MeshRenderer.hpp"
#include "ButtonFSM.hpp"
#include "InfoButton.hpp"
#include "Logger.hpp"

// ==============================================================================
// [1. 윈도우 메시지 처리기 (WndProc)]
// 창의 'X' 버튼을 누르면 프로그램을 정상적으로 종료시키는 신호를 보냅니다.
// ==============================================================================
LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (m == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(h, m, w, l);
}

int WINAPI WinMain(HINSTANCE hI, HINSTANCE, LPSTR, int nS) {
    // ==============================================================================
    // [2. 엔진 초기화 및 리소스 준비]
    // 윈도우 창과 GPU 통신을 시작하고, 셰이더 파일과 버튼 모양의 직사각형 
    // 정점 데이터를 메모리에 올립니다.
    // ==============================================================================
    Logger::Init();
    Logger::Log("Engine Initialization Started.");

    GameLoop engine;
    engine.Initialize(hI, WndProc);

    D3D11_INPUT_ELEMENT_DESC ied[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    ShaderSet shaders = engine.gfx.CompileAndCreate(L"effect.hlsl", 0, true, ied, 2);

    std::vector<Vertex> vRect = {
        {{-1.0f,  1.0f, 0.0f}, {1,1,1,1}},
        {{ 1.0f,  1.0f, 0.0f}, {1,1,1,1}},
        {{-1.0f, -1.0f, 0.0f}, {1,1,1,1}},
        {{-1.0f, -1.0f, 0.0f}, {1,1,1,1}},
        {{ 1.0f,  1.0f, 0.0f}, {1,1,1,1}},
        {{ 1.0f, -1.0f, 0.0f}, {1,1,1,1}}
    };
    Mesh* gMesh = new Mesh();
    gMesh->Create(&engine.gfx, vRect);

    // ==============================================================================
    // [3. 왼쪽 버튼 컴포넌트 조립]
    // FSM 상태 기계 기능을 달아주고 화면 왼쪽에 배치합니다.
    // ==============================================================================
    GameObject* leftBtn = new GameObject(-0.5f, 0, 0);
    leftBtn->scale = { 0.3f, 0.2f, 1.0f };
    ColorMaterial* matL = new ColorMaterial(shaders, { 1, 1, 1, 1 }, engine.gfx.Device);

    ButtonFSM* fsmComp = new ButtonFSM(matL);

    leftBtn->AddComponent(new MeshRenderer(gMesh, matL));
    leftBtn->AddComponent(fsmComp);

    // ==============================================================================
    // [4. 오른쪽 버튼 컴포넌트 조립]
    // 화면 오른쪽에 배치하고, InfoButton을 장착할 때 왼쪽 버튼의 주소를 넘겨줍니다.
    // =============================================================================
    GameObject* rightBtn = new GameObject(0.5f, 0, 0);
    rightBtn->scale = { 0.3f, 0.2f, 1.0f };
    ColorMaterial* matR = new ColorMaterial(shaders, { 0.5f, 0.5f, 0.5f, 1 }, engine.gfx.Device);

    rightBtn->AddComponent(new MeshRenderer(gMesh, matR));
    rightBtn->AddComponent(new InfoButton(fsmComp));

    // ==============================================================================
    // [5. 루프 실행 및 자원 해제]
    // ==============================================================================
    engine.world.push_back(leftBtn);
    engine.world.push_back(rightBtn);

    Logger::Log("Game Loop is starting...");
    engine.Run();

    Logger::Log("Engine Terminated.");

    delete gMesh;
    delete matL;
    delete matR;
    shaders.Release();

    return 0;
}