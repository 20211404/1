#pragma once
#include "ObjectBase.hpp"
#include "ButtonFSM.hpp"
#include "Logger.hpp"

class InfoButton : public Component {
    ButtonFSM* targetFSM = nullptr;
    bool prevLBit = false;

public:
    InfoButton(ButtonFSM* target) : targetFSM(target) {}

    void Input() override {
        bool currLBit = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

        if (currLBit && !prevLBit && IsMouseOver()) {
            PrintStatus();
        }
        prevLBit = currLBit;
    }

    void PrintStatus() {
        if (!targetFSM) return;

        ButtonState s = targetFSM->GetState();

        Logger::Log("--- Info Button Clicked ---");
        switch (s) {
        case ButtonState::IDLE:
            Logger::Log("[INFO] 상태: IDLE | 문장: 마우스가 버튼 밖에 있습니다.");
            break;
        case ButtonState::HOVER:
            Logger::Log("[INFO] 상태: HOVER | 문장: 마우스가 버튼 위에 올라와 있습니다! (연한 파란색)");
            break;
        case ButtonState::CLICKED_1:
            Logger::Log("[INFO] 상태: CLICKED_1 | 문장: 한 번 클릭되었습니다. (노란색)");
            break;
        case ButtonState::CLICKED_2:
            Logger::Log("[INFO] 상태: CLICKED_2 | 문장: 두 번 클릭되었습니다. (빨간색)");
            break;
        }
        Logger::Log("---------------------------");
    }

    bool IsMouseOver() {
        POINT mouse; GetCursorPos(&mouse); ScreenToClient(GetActiveWindow(), &mouse);
        float mx = (mouse.x / 400.0f) - 1.0f;
        float my = 1.0f - (mouse.y / 300.0f);
        return (mx >= pOwner->pos.x - pOwner->scale.x && mx <= pOwner->pos.x + pOwner->scale.x &&
            my >= pOwner->pos.y - pOwner->scale.y && my <= pOwner->pos.y + pOwner->scale.y);
    }

    void Start(GraphicsContext* gfx) override {}
    void Update(float dt) override {}
    void Render(GraphicsContext* gfx) override {}
};