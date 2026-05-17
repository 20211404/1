#pragma once
#include "ObjectBase.hpp"
#include "Material.hpp"
#include "Logger.hpp"

// ==============================================================================
// [1. 상태 정의 및 변수 선언 문단]
// enum class를 사용하여 4가지 상태를 안전하게 격리합니다.
// 또한 이전 프레임의 마우스 상태(prevLBit, prevHover)를 기억하여 
// '누르고 있는 중'이 아니라 '방금 눌린 순간'을 감지(Edge Detection)할 준비를 합니다.
// ==============================================================================
enum class ButtonState { IDLE, HOVER, CLICKED_1, CLICKED_2 };

class ButtonFSM : public Component {
    ButtonState state = ButtonState::IDLE;
    ColorMaterial* pMat = nullptr;

    bool prevLBit = false;
    bool prevHover = false;

public:
    ButtonFSM(ColorMaterial* m) : pMat(m) {}
    ButtonState GetState() const { return state; }

    // ==============================================================================
    // [2. 입력 감지 문단 (Input)]
    // 매 프레임마다 마우스 좌표와 클릭 여부를 검사합니다.
    // 이전 프레임 상태와 비교하여 마우스가 '들어온 순간', '나간 순간', '클릭한 순간'을
    // 구분해 내고, 조건이 맞으면 ChangeState()를 호출합니다.
    // ==============================================================================
    void Input() override {
        bool currHover = IsMouseOver();
        bool currLBit = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

        // 호버 상태 진입/탈출 로직
        if (currHover && !prevHover) {
            if (state == ButtonState::IDLE) ChangeState(ButtonState::HOVER);
        }
        else if (!currHover && prevHover) {
            if (state == ButtonState::HOVER) ChangeState(ButtonState::IDLE);
        }

        // 클릭 로직
        if (currLBit && !prevLBit && currHover) {
            if (state == ButtonState::HOVER) ChangeState(ButtonState::CLICKED_1);
            else if (state == ButtonState::CLICKED_1) ChangeState(ButtonState::CLICKED_2);
            else if (state == ButtonState::CLICKED_2) ChangeState(ButtonState::HOVER);
        }

        // R 키 초기화
        if (GetAsyncKeyState('R') & 0x8000) {
            ChangeState(currHover ? ButtonState::HOVER : ButtonState::IDLE);
            Logger::Log("[ButtonFSM] Reset triggered by 'R' key.");
        }

        prevHover = currHover;
        prevLBit = currLBit;
    }

    // ==============================================================================
    // [3. 상태 전환 및 시각 효과 문단 (Transition)]
    // 상태가 변할 때 해야 할 일(색상 변경, 로그 출력)을 한 곳으로 모았습니다.
    // 유지보수를 쉽게 만들고 스파게티 코드를 방지하는 FSM 패턴의 핵심입니다.
    // ==============================================================================
    void ChangeState(ButtonState newState) {
        if (state == newState) return;
        state = newState;

        switch (state) {
        case ButtonState::IDLE:
            pMat->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f }); Logger::Log("[FSM] IDLE"); break;
        case ButtonState::HOVER:
            pMat->SetColor({ 0.8f, 0.9f, 1.0f, 1.0f }); Logger::Log("[FSM] HOVER"); break;
        case ButtonState::CLICKED_1:
            pMat->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f }); Logger::Log("[FSM] CLICK_1"); break;
        case ButtonState::CLICKED_2:
            pMat->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f }); Logger::Log("[FSM] CLICK_2"); break;
        }
    }

    // ==============================================================================
    // [4. 마우스 피킹(충돌 판정) 문단]
    // 윈도우 스크린 좌표를 게임 공간(NDC) 좌표로 변환한 뒤,
    // 마우스(점)와 버튼(AABB 사각형)의 충돌 검사 공식을 적용하여 참/거짓을 반환합니다.
    // ==============================================================================
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