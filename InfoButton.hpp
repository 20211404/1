#pragma once
#include "ObjectBase.hpp"
#include "ButtonFSM.hpp"
#include "Logger.hpp"

class InfoButton : public Component {
    // ==============================================================================
    // [1. 의존성 주입을 통한 참조]
    // 왼쪽 버튼의 상태를 확인하기 위해 ButtonFSM 클래스의 포인터를 멤버 변수로 가집니다.
    // 생성자를 통해 외부에서 이 포인터를 주입받아 연결되도록 설계하여 
    // 두 컴포넌트 간의 결합도를 낮췄습니다.
    // ==============================================================================
    ButtonFSM* targetFSM = nullptr;
    bool prevLBit = false;

public:
    InfoButton(ButtonFSM* target) : targetFSM(target) {}

    // ==============================================================================
     // [2. 입력 판별]
     // 마우스가 현재 내 버튼(오른쪽 UI 버튼) 영역 위에 올라와 있고, 
     // 마우스 왼쪽 클릭(좌클릭)'이 막 발생했을 때 PrintStatus() 함수를 호출합니다.
     // ==============================================================================
    void Input() override {
        bool currLBit = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
        if (currLBit && !prevLBit && IsMouseOver()) {
            PrintStatus();
        }
        prevLBit = currLBit;
    }

    // ==============================================================================
    // [3. 데이터 참조 및 로깅]
    // targetFSM->GetState()를 호출하여 왼쪽 버튼의 데이터를 '읽기 전용'으로 가져옵니다.
    // 왼쪽 버튼의 상태를 가져와 읽어보고, 왼쪽버튼의 상태에 따라 무엇을 실행시킬지가 결정됩니다. 
    // 원본 데이터를 훼손하지 않으면서 현재 상태를 확인하고, Logger를 통해 기록합니다.
    // ==============================================================================
    void PrintStatus() {
        if (!targetFSM) return;
        ButtonState s = targetFSM->GetState();

        Logger::Get()->Log(LogLevel::Info, "--- Info Button Clicked ---");
        switch (s) {
        case ButtonState::IDLE:
            Logger::Get()->Log(LogLevel::Info, "상태: IDLE | 마우스가 버튼 밖에 있습니다."); break;
        case ButtonState::HOVER:
            Logger::Get()->Log(LogLevel::Info, "상태: HOVER | 마우스가 버튼 위에 올라와 있습니다."); break;
        case ButtonState::CLICKED_1:
            Logger::Get()->Log(LogLevel::Info, "상태: CLICKED_1 | 한 번 클릭되었습니다."); break;
        case ButtonState::CLICKED_2:
            Logger::Get()->Log(LogLevel::Info, "상태: CLICKED_2 | 두 번 클릭되었습니다."); break;
        }
        Logger::Get()->Log(LogLevel::Info, "---------------------------");
    }

    bool IsMouseOver() {
        POINT m; GetCursorPos(&m); ScreenToClient(GetActiveWindow(), &m);
        float mx = (m.x / 400.0f) - 1.0f; float my = 1.0f - (m.y / 300.0f);
        return (mx >= pOwner->pos.x - pOwner->scale.x && mx <= pOwner->pos.x + pOwner->scale.x &&
            my >= pOwner->pos.y - pOwner->scale.y && my <= pOwner->pos.y + pOwner->scale.y);
    }

    void Start(GraphicsContext* gfx) override {}
    void Update(float dt) override {}
    void Render(GraphicsContext* gfx) override {}
};