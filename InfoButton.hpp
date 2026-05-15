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

        if (currLBit && !prevLBit) {
            if (IsMouseOver()) {
                PrintStatus();
            }
        }
        prevLBit = currLBit;
    }

    void PrintStatus() {
        if (!targetFSM) return;

        ButtonState s = targetFSM->GetState();

        Logger::Log("--- Info Button Clicked ---");
        switch (s) {
        case ButtonState::IDLE:
            Logger::Log("[INFO] State: 0 Clicks (IDLE) | Message: 현재 대기 중입니다. 왼쪽 버튼을 클릭해 보세요!");
            break;
        case ButtonState::CLICKED_1:
            Logger::Log("[INFO] State: 1 Click (CLICKED_1) | Message: 한 번 클릭되었습니다. 버튼이 노란색으로 변했습니다.");
            break;
        case ButtonState::CLICKED_2:
            Logger::Log("[INFO] State: 2 Clicks (CLICKED_2) | Message: 두 번 클릭되었습니다. 버튼이 빨간색입니다.");
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