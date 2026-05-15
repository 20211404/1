#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

#include "GameLoop.hpp"
#include "MeshRenderer.hpp"
#include "ButtonFSM.hpp"
#include "InfoButton.hpp"
#include "Logger.hpp"     

LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (m == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(h, m, w, l);
}

int WINAPI WinMain(HINSTANCE hI, HINSTANCE, LPSTR, int nS) {
    // 1. 엔진 시작 전 로그 초기화
    Logger::Init();
    Logger::Log("Engine Initialization Started.");

    GameLoop engine;
    engine.Initialize(hI, WndProc);

    // 2. 셰이더 및 메쉬 초기화
    D3D11_INPUT_ELEMENT_DESC ied[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    ShaderSet shaders = engine.gfx.CompileAndCreate(L"effect.hlsl", 0, true, ied, 2);

    std::vector<Vertex> vRect = {
        {{-1, 1, 0}, {1,1,1,1}}, {{1, 1, 0}, {1,1,1,1}}, {{-1,-1, 0}, {1,1,1,1}},
        {{-1,-1, 0}, {1,1,1,1}}, {{1, 1, 0}, {1,1,1,1}}, {{1,-1, 0}, {1,1,1,1}}
    };
    Mesh* gMesh = new Mesh();
    gMesh->Create(&engine.gfx, vRect);

    // --- 3. 왼쪽 버튼 구성 (상태 제어: ButtonFSM) ---
    GameObject* leftBtn = new GameObject(-0.5f, 0, 0);
    leftBtn->scale = { 0.3f, 0.2f, 1.0f };
    ColorMaterial* matL = new ColorMaterial(shaders, { 1, 1, 1, 1 }, engine.gfx.Device); // 기본 흰색

    ButtonFSM* fsmComp = new ButtonFSM(matL); // InfoButton에 전달하기 위해 포인터 저장

    leftBtn->AddComponent(new MeshRenderer(gMesh, matL));
    leftBtn->AddComponent(fsmComp);

    // --- 4. 오른쪽 버튼 구성 (정보 출력: InfoButton) ---
    GameObject* rightBtn = new GameObject(0.5f, 0, 0);
    rightBtn->scale = { 0.3f, 0.2f, 1.0f };
    ColorMaterial* matR = new ColorMaterial(shaders, { 0.5f, 0.5f, 0.5f, 1 }, engine.gfx.Device); // 항상 회색

    rightBtn->AddComponent(new MeshRenderer(gMesh, matR));
    rightBtn->AddComponent(new InfoButton(fsmComp)); // 왼쪽 버튼의 FSM 포인터 연결

    // 5. 월드에 추가 및 실행
    engine.world.push_back(leftBtn);
    engine.world.push_back(rightBtn);

    Logger::Log("Game Loop is starting...");
    engine.Run();

    Logger::Log("Engine Terminated.");

    // 자원 해제
    delete gMesh;
    delete matL;
    delete matR;
    shaders.Release();

    return 0;
}