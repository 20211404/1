#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>

class Logger {
public:
    // 프로그램 시작 시 기존 로그 파일을 초기화하는 함수
    static void Init() {
        std::ofstream file("engine_log.txt", std::ios::trunc);
        if (file.is_open()) {
            file << "========== [Engine Log Started] ==========\n";
            file.close();
        }
    }

    // 파일과 콘솔에 동시에 로그를 남기는 함수
    static void Log(const std::string& message) {
        // 1. 현재 시간 구하기
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);

        // 시간 포맷팅 (예: [14:05:32])
        char timeStr[26];
        ctime_s(timeStr, sizeof(timeStr), &now_c);
        timeStr[24] = '\0'; // ctime_s가 추가하는 줄바꿈 문자 제거

        // 2. 로그 메시지 조합
        std::string finalMsg = "[" + std::string(timeStr) + "] " + message + "\n";

        // 3. 콘솔 출력 (기존 printf 대체)
        std::cout << finalMsg;

        // 4. 파일 출력 (기존 내용 뒤에 이어쓰기 - ios::app)
        std::ofstream file("engine_log.txt", std::ios::app);
        if (file.is_open()) {
            file << finalMsg;
            file.close();
        }
    }
};