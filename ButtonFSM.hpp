#pragma once
#include "ObjectBase.hpp"
#include "Material.hpp"
#include "Logger.hpp"

// 상태 4가지로 확장: 대기, 호버, 1회 클릭, 2회 클릭
enum class ButtonState { IDLE, HOVER, CLICKED_1, CLICKED_2 };

class ButtonFSM : public Component {
    ButtonState state = ButtonState::IDLE;
    ColorMaterial* pMat = nullptr;

    bool prevLBit = false;
    bool prevHover = false; // 이전 프레임의 마우스 호버 상태

public:
    ButtonFSM(ColorMaterial* m) : pMat(m) {}

    ButtonState GetState() const { return state; }

    void Input() override {
        bool currHover = IsMouseOver();
        bool currLBit = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

        // --- 1. 마우스 진입/탈출 (Hover <-> Idle) 로직 ---
        if (currHover && !prevHover) {
            // 마우스가 들어왔을 때, 만약 대기 상태였다면 호버 상태로 전환
            if (state == ButtonState::IDLE) {
                ChangeState(ButtonState::HOVER);
            }
        }
        else if (!currHover && prevHover) {
            // 마우스가 나갔을 때, 
            // 클릭된 상태가 아니라 단순히 호버 상태였다면 대기 상태로 복귀
            if (state == ButtonState::HOVER) {
                ChangeState(ButtonState::IDLE);
            }
        }

        // --- 2. 클릭 상태 전환 로직 ---
        if (currLBit && !prevLBit && currHover) {
            if (state == ButtonState::HOVER) {
                ChangeState(ButtonState::CLICKED_1);
            }
            else if (state == ButtonState::CLICKED_1) {
                ChangeState(ButtonState::CLICKED_2);
            }
            else if (state == ButtonState::CLICKED_2) {
                // 2회 클릭 후 다시 누르면, 마우스가 아직 위에 있으므로 HOVER로 돌아감
                ChangeState(ButtonState::HOVER);
            }
        }

        // --- 3. R 키 초기화 ---
        if (GetAsyncKeyState('R') & 0x8000) {
            // R키를 누르면, 마우스가 현재 위에 있는지 여부에 따라 초기 상태 결정
            ChangeState(currHover ? ButtonState::HOVER : ButtonState::IDLE);
            Logger::Log("[ButtonFSM] Reset triggered by 'R' key.");
        }

        // 상태 기억
        prevHover = currHover;
        prevLBit = currLBit;
    }

    // 상태 변경 및 색상/로그 처리를 전담하는 통합 함수
    void ChangeState(ButtonState newState) {
        if (state == newState) return; // 동일 상태면 무시

        state = newState;

        switch (state) {
        case ButtonState::IDLE:
            pMat->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f }); // 백색
            Logger::Log("[ButtonFSM] State: IDLE (마우스 벗어남)");
            break;
        case ButtonState::HOVER:
            pMat->SetColor({ 0.8f, 0.9f, 1.0f, 1.0f }); // 연한 파란색 (호버 시각 효과)
            Logger::Log("[ButtonFSM] State: HOVER (마우스 진입)");
            break;
        case ButtonState::CLICKED_1:
            pMat->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f }); // 노란색
            Logger::Log("[ButtonFSM] State: CLICKED_1");
            break;
        case ButtonState::CLICKED_2:
            pMat->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f }); // 빨간색
            Logger::Log("[ButtonFSM] State: CLICKED_2");
            break;
        }
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