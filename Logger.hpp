#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string>

enum class LogLevel { Info, Warning, Error };

class Logger {
private:
    FILE* file;
    Logger() : file(nullptr) {}
    ~Logger() { if (file) fclose(file); }

public:
    // ==============================================================================
    // [1. 싱글톤(Singleton) 패턴 초기화]
    // 엔진 전체에서 파일 입출력을 관리하는 객체는 단 하나만 존재해야 합니다.
    // 정적(static) 지역 변수를 활용하여 가장 안전한 형태의 싱글톤 인스턴스를 반환합니다.
    // ==============================================================================
    static Logger* Get() {
        static Logger instance;
        return &instance;
    }

    // ==============================================================================
    // [2. 로그 파일 열기 및 초기화]
    // 프로그램 시작 시 'w'(Write) 모드로 파일을 열어 이전 실행 기록을 초기화합니다.
    // 비주얼 스튜디오의 C4996 보안 에러를 방지하기 위해 안전한 함수인 fopen_s를 사용합니다.
    // ==============================================================================
    bool Initialize(const std::string& filename) {
        // 기존의 fopen 대신 안전한 fopen_s 사용
        if (fopen_s(&file, filename.c_str(), "w") != 0) {
            file = nullptr;
        }
        return file != nullptr;
    }

    // ==============================================================================
    // [3. 가변 인자 로깅(Logging) 및 파일 기록]
    // C언어의 가변 인자(va_list)를 사용하여 printf처럼 다양한 변수를 포맷팅합니다.
    // 현재 시간을 계산하여 로그 레벨(Info/Warn/Error), 메시지와 함께 조립한 뒤
    // 콘솔과 파일에 동시에 출력합니다. 
    // fflush(file)을 호출하여 프로그램이 비정상 종료되더라도 기록이 저장되도록 보장합니다.
    // ==============================================================================
    void Log(LogLevel level, const char* format, ...) {
        if (file == nullptr) return;

        time_t now = time(0);
        struct tm tstruct;
        char timeBuf[80];
        localtime_s(&tstruct, &now);
        strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", &tstruct);

        const char* levelStr = "";
        switch (level) {
        case LogLevel::Info:    levelStr = "[INFO]"; break;
        case LogLevel::Warning: levelStr = "[WARN]"; break;
        case LogLevel::Error:   levelStr = "[ERR ]"; break;
        }

        char messageBuf[1024];
        va_list args;
        va_start(args, format);
        vsnprintf(messageBuf, sizeof(messageBuf), format, args);
        va_end(args);

        fprintf(file, "%s %s %s\n", timeBuf, levelStr, messageBuf);
        printf("%s %s %s\n", timeBuf, levelStr, messageBuf);
        fflush(file);
    }
};