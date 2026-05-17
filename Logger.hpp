#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>

class Logger {
public:
    // ==============================================================================
    // [1. 로그 초기화 문단]
    // 프로그램이 켜질 때 딱 한 번 호출됩니다.
    // std::ios::trunc 옵션을 사용하여 기존에 있던 engine_log.txt 파일의 내용을 
    // 싹 지우고 깨끗한 상태에서 새 기록을 시작할 준비를 합니다.
    // ==============================================================================
    static void Init() {
        std::ofstream file("engine_log.txt", std::ios::trunc);
        if (file.is_open()) {
            file << "========== [Engine Log Started] ==========\n";
            file.close();
        }
    }

    // ==============================================================================
    // [2. 실제 로그 기록 문단]
    // 아무 곳에서나 Logger::Log("메시지") 형태로 호출하면 실행됩니다.
    // 현재 시간을 구해서 [시간] 메시지 형태로 포맷팅한 뒤, 
    // std::cout으로 콘솔에 보여주고 std::ios::app(이어쓰기) 옵션으로 파일에 저장합니다.
    // ==============================================================================
    static void Log(const std::string& message) {
        // 시간 구하기 및 포맷팅
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        char timeStr[26];
        ctime_s(timeStr, sizeof(timeStr), &now_c);
        timeStr[24] = '\0'; // ctime_s가 끝에 넣는 줄바꿈(\n) 문자 제거

        std::string finalMsg = "[" + std::string(timeStr) + "] " + message + "\n";

        // 화면(콘솔) 출력
        std::cout << finalMsg;

        // 파일 뒤에 이어쓰기
        std::ofstream file("engine_log.txt", std::ios::app);
        if (file.is_open()) {
            file << finalMsg;
            file.close();
        }
    }
};