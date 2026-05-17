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