#pragma once
#include "ObjectBase.hpp"
#include "Material.hpp"
#include "Logger.hpp"

enum class ButtonState { IDLE, CLICKED_1, CLICKED_2 };

class ButtonFSM : public Component {
    ButtonState state = ButtonState::IDLE;
    ColorMaterial* pMat = nullptr;
    bool prevLBit = false;

public:
    ButtonFSM(ColorMaterial* m) : pMat(m) {}

    ButtonState GetState() const { return state; }

    void Input() override {
        bool currLBit = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

        // 마우스 클릭 감지 및 영역 체크
        if (currLBit && !prevLBit) {
            if (IsMouseOver()) {
                Transition();
            }
        }
        prevLBit = currLBit;

        // R 키 초기화
        if (GetAsyncKeyState('R') & 0x8000) {
            if (state != ButtonState::IDLE) {
                state = ButtonState::IDLE;
                pMat->SetColor({ 1, 1, 1, 1 }); // 백색
                Logger::Log("[ButtonFSM] Reset triggered by 'R' key. State: IDLE");
            }
        }
    }

    void Transition() {
        if (state == ButtonState::IDLE) {
            state = ButtonState::CLICKED_1;
            pMat->SetColor({ 1, 1, 0, 1 }); // 노란색
            Logger::Log("[ButtonFSM] State Changed: IDLE -> CLICKED_1");
        }
        else if (state == ButtonState::CLICKED_1) {
            state = ButtonState::CLICKED_2;
            pMat->SetColor({ 1, 0, 0, 1 }); // 빨간색
            Logger::Log("[ButtonFSM] State Changed: CLICKED_1 -> CLICKED_2");
        }
        else {
            state = ButtonState::IDLE;
            pMat->SetColor({ 1, 1, 1, 1 }); // 백색
            Logger::Log("[ButtonFSM] State Changed: CLICKED_2 -> IDLE");
        }
    }

    bool IsMouseOver() {
        POINT mouse;
        GetCursorPos(&mouse);
        ScreenToClient(GetActiveWindow(), &mouse);

        float mx = (mouse.x / 400.0f) - 1.0f; // 800x600 기준 해상도 변환
        float my = 1.0f - (mouse.y / 300.0f);

        return (mx >= pOwner->pos.x - pOwner->scale.x && mx <= pOwner->pos.x + pOwner->scale.x &&
            my >= pOwner->pos.y - pOwner->scale.y && my <= pOwner->pos.y + pOwner->scale.y);
    }

    void Start(GraphicsContext* gfx) override {}
    void Update(float dt) override {}
    void Render(GraphicsContext* gfx) override {}
};