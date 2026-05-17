#pragma once
#include "ObjectBase.hpp"
#include "ButtonFSM.hpp"
#include "Logger.hpp"

class InfoButton : public Component {
    // ==============================================================================
    // [1. 의존성 포인터 문단]
    // 자신이 감시해야 할 왼쪽 버튼(ButtonFSM)의 포인터를 가지고 있습니다.
    // 생성자를 통해 이 포인터를 주입받아 연결(Link)됩니다.
    // ==============================================================================
    ButtonFSM* targetFSM = nullptr;
    bool prevLBit = false;

public:
    InfoButton(ButtonFSM* target) : targetFSM(target) {}

    // ==============================================================================
    // [2. 입력 및 출력 실행 문단]
    // 마우스가 현재 내 버튼(오른쪽) 위에 있고 && 막 클릭되었다면 
    // PrintStatus()를 호출하여 정보를 빼옵니다.
    // ==============================================================================
    void Input() override {
        bool currLBit = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

        if (currLBit && !prevLBit && IsMouseOver()) {
            PrintStatus();
        }
        prevLBit = currLBit;
    }

    // ==============================================================================
    // [3. 데이터 참조 문단]
    // targetFSM->GetState()를 호출하여 왼쪽 버튼의 상태 정보만 "읽기 전용"으로 가져옵니다.
    // 왼쪽 버튼의 데이터를 직접 수정하지 않으므로 결합도가 낮아져 안전합니다.
    // ==============================================================================
    void PrintStatus() {
        if (!targetFSM) return;
        ButtonState s = targetFSM->GetState();

        Logger::Log("--- Info Button Clicked ---");
        switch (s) {
        case ButtonState::IDLE:      Logger::Log("[INFO] IDLE 상태"); break;
        case ButtonState::HOVER:     Logger::Log("[INFO] HOVER 상태"); break;
        case ButtonState::CLICKED_1: Logger::Log("[INFO] CLICKED_1 상태"); break;
        case ButtonState::CLICKED_2: Logger::Log("[INFO] CLICKED_2 상태"); break;
        }
    }

    // 마우스 피킹 (ButtonFSM과 동일한 로직)
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