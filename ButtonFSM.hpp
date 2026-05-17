#pragma once
#include "ObjectBase.hpp"
#include "Material.hpp"
#include "Logger.hpp"

// ==============================================================================
// [1. 상태 정의 및 멤버 변수 설정]
// enum class를 사용하여 버튼의 4가지 상태를 독립적으로 정의합니다.
// prevHover와 prevLBit 변수를 통해 이전 프레임의 마우스 상태를 저장하고, 
// 현재 프레임과 비교하기 위한 기반을 마련합니다.
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
    // [2. 입력 감지 및 엣지 디텍션(Edge Detection)]
    // 매 프레임마다 마우스가 버튼 영역에 있는지, 클릭되었는지 검사합니다.
    // 현재 상태(curr)와 이전 상태(prev)가 다를 때(즉, 상태가 변한 순간)만 반응하도록 
    // 설계하여 1초에 60번씩 이벤트가 중복 실행되는 것을 방지합니다.
    // ==============================================================================
    void Input() override {
        bool currHover = IsMouseOver();
        bool currLBit = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

        if (currHover && !prevHover) {
            if (state == ButtonState::IDLE) ChangeState(ButtonState::HOVER);
        }
        else if (!currHover && prevHover) {
            if (state == ButtonState::HOVER) ChangeState(ButtonState::IDLE);
        }

        if (currLBit && !prevLBit && currHover) {
            if (state == ButtonState::HOVER) ChangeState(ButtonState::CLICKED_1);
            else if (state == ButtonState::CLICKED_1) ChangeState(ButtonState::CLICKED_2);
            else if (state == ButtonState::CLICKED_2) ChangeState(ButtonState::HOVER);
        }

        if (GetAsyncKeyState('R') & 0x8000) {
            ChangeState(currHover ? ButtonState::HOVER : ButtonState::IDLE);
            Logger::Get()->Log(LogLevel::Info, "[ButtonFSM] Reset triggered by 'R' key.");
        }

        prevHover = currHover;
        prevLBit = currLBit;
    }

    // ==============================================================================
    // [3. 중앙 통제식 상태 전환]
    // 상태가 변경될 때 실행되어야 하는 로직(색상 변경, 로그 출력)을 한 곳에서 관리합니다.
    // 이를 통해 코드가 분산되는 것을 막고 유지보수성을 높입니다.
    // ==============================================================================
    void ChangeState(ButtonState newState) {
        if (state == newState) return;
        state = newState;

        switch (state) {
        case ButtonState::IDLE:
            pMat->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
            Logger::Get()->Log(LogLevel::Info, "[FSM] IDLE (마우스 벗어남)"); break;
        case ButtonState::HOVER:
            pMat->SetColor({ 0.8f, 0.9f, 1.0f, 1.0f });
            Logger::Get()->Log(LogLevel::Info, "[FSM] HOVER (마우스 진입)"); break;
        case ButtonState::CLICKED_1:
            pMat->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f });
            Logger::Get()->Log(LogLevel::Info, "[FSM] CLICK_1 (노란색)"); break;
        case ButtonState::CLICKED_2:
            pMat->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });
            Logger::Get()->Log(LogLevel::Info, "[FSM] CLICK_2 (빨간색)"); break;
        }
    }

    // ==============================================================================
    // [4. AABB 충돌 판정 (마우스 피킹)]
    // 윈도우 스크린 좌표를 NDC(-1 ~ 1) 좌표계로 변환합니다.
    // 이후 버튼 오브젝트의 위치(pos)와 크기(scale)를 기준으로 상하좌우 경계선을 계산해,
    // 마우스 좌표(점)가 AABB(축 정렬 사각형) 내부에 있는지 수학적으로 판별합니다.
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