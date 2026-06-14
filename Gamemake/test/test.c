#pragma execution_character_set("utf-8") // 콘솔 창에서 한글이 깨지는 현상을 방지하기 위해 출력 인코딩을 UTF-8로 지정합니다.
#define _CRT_SECURE_NO_WARNINGS          // fopen 등의 표준 함수 사용 시 비주얼 스튜디오에서 보안 경고(에러)가 나는 것을 막아줍니다.

#include <stdio.h>    // printf(출력), fopen(파일 열기) 같은 기본적인 입출력 함수들을 쓰기 위해 가져옵니다.
#include <string.h>   // strlen(길이 계산), strstr(글자 찾기) 같은 문자열 처리 함수들을 쓰기 위해 가져옵니다.
#include <windows.h>  // SetConsoleCursorPosition(커서 이동) 등 윈도우 시스템 제어 기능을 쓰기 위해 가져옵니다.
#include <conio.h>    // _getch(키보드 입력 실시간 감지) 함수를 사용하기 위해 가져옵니다.
#include <stdlib.h>   // rand(랜덤 숫자), system(명령어 실행), exit(종료) 함수를 쓰기 위해 가져옵니다.
#include <time.h>     // time(현재 시간) 함수를 사용하여 매번 다른 랜덤 시드 값을 주기 위해 가져옵니다.
#include <mmsystem.h> // PlaySound 함수를 이용해 .wav 음원을 재생하기 위해 가져옵니다.

#pragma comment(lib, "winmm.lib") // Windows 멀티미디어 사운드 라이브러리를 링크 서브시스템에 등록합니다.

#define COLOR_RESET "\x1b[0m" // 변경된 글자나 배경 색상을 다시 콘솔 기본 색상으로 되돌리는 특수 문자열입니다.

// 콘솔창의 글자 색상과 배경 색상을 간편하게 바꾸기 위해 ANSI 코드를 숫자로 정의해 둔 것입니다.
#define FONT_COLOR_BLACK 30
#define BG_COLOR_BLACK 40
#define FONT_COLOR_RED 31
#define BG_COLOR_RED 41
#define FONT_COLOR_GREEN 32
#define BG_COLOR_GREEN 42
#define FONT_COLOR_YELLOW 33
#define BG_COLOR_YELLOW 43
#define FONT_COLOR_BLUE 34
#define BG_COLOR_BLUE 44
#define FONT_COLOR_MAGENTA 35
#define BG_COLOR_MAGENTA 45
#define FONT_COLOR_BRIGHTMAGENTA 95
#define BG_COLOR_BRIGHTMAGENTA 105
#define FONT_COLOR_WHITE 37
#define BG_COLOR_WHITE 47

#define Backspace 8 // 키보드의 백스페이스 키가 가지는 고유 아스키코드 번호(8)를 알아보기 쉽게 이름을 붙인 것입니다.

#define SCREEN_WIDTH 120  // 이 프로그램이 사용하는 가상 콘솔 화면의 최대 가로 칸(폭)을 120칸으로 정합니다.
#define SCREEN_HEIGHT 30  // 이 프로그램이 사용하는 가상 콘솔 화면의 최대 세로 줄(높이)을 30줄로 정합니다.

// 프로그램 안에서 사용할 기능(함수)들의 목차를 컴파일러에게 미리 알려주는 선언부입니다.
void set_color(int code);                                 // 지정한 번호의 색상으로 글자/배경색을 바꾸는 함수입니다.
int move_cursor(int x, int y);                            // ANSI 코드를 이용해 지정한 X, Y 좌표로 커서를 움직이는 함수입니다.
void gotoxy(int x, int y);                                // 윈도우 API를 이용해 지정한 X, Y 좌표로 커서를 움직이는 함수입니다.
void ShowLogo(void);                                      // 게임을 켜자마자 나오는 오프닝 로고 상자를 그려주는 함수입니다.
int RenderTitle(void);                                    // 메인 메뉴 화면을 그리고 사용자가 메뉴를 고르게 하는 함수입니다.
void print_member_page(const char* filename, const char* description); // 팀원 소개 파일(.txt)을 읽어서 화면에 이쁘게 찍어주는 함수입니다.
void draw_final_screen(void);                             // 팀원 소개 마지막 페이지인 팀 로고와 전체 명단을 그려주는 함수입니다.
int People(void);                                         // 1번 메뉴인 '팀원 소개'의 전체 화면 전환을 관리하는 함수입니다.
int Manual(void);                                         // 2번 메뉴인 '설명서' 화면을 띄우고 페이지 넘김을 처리하는 함수입니다.
int Gamestart(void);                                      // 3번 메뉴인 '진짜 게임 플레이' 전체 생존 루프를 담당하는 함수입니다.
int Gameover(void);                                       // 4번 메뉴 혹은 게임 종료 시 텍스트가 위로 올라가는 엔딩 연출 함수입니다.
void cleanup_console(void);                               // 프로그램이 꺼질 때 콘솔 상태를 게임 실행 전으로 깨끗하게 되돌려주는 함수입니다.
int calculate_visual_length(const char* str);             // 영어, 한글, 특수문자가 섞인 문장이 화면에서 실제로 차지하는 칸수를 계산합니다.
void get_content_stats(const char* str, int* visual_prefix, int* visual_content); // 아스키아트의 순수 그림 폭과 앞 공백 크기를 계산합니다.

// 게임 전체에서 공유하며 사용할 전역 변수들입니다.
int menu = 1;                   // 메인 화면에서 현재 화살표가 가리키고 있는 메뉴의 번호입니다. (기본값 1번)
int isRunning = 1;              // 이 값이 1인 동안은 프로그램이 계속 켜져 있고, 0이 되면 프로그램이 완전히 꺼집니다.
char playerName[50] = "Player"; // [복구] 플레이어의 이름을 실시간 보관하고 UI에 매핑할 닉네임 전역 변수입니다.

// 게임 중 매 라운드마다 무작위로 나올 '선택지 정보'를 하나로 묶어둔 구조체 양식입니다.
typedef struct
{
    const char* art[6]; // 선택지 위에 그려질 아스키아트 그림 데이터입니다. (최대 6줄까지 저장)
    const char* text;   // 선택지에 대한 설명 글자입니다. ("귀여운 길고양이를 쓰다듬는다" 등)
    int min_damage;     // 이 행동을 골랐을 때 최소한으로 받는 피해 체력량입니다. (음수면 회복)
    int max_damage;     // 이 행동을 골랐을 때 최대한으로 받는 피해 체력량입니다. (음수면 회복)
} Choice;

// 게임에 등장하게 될 기상천외한 선택지들의 실제 데이터들을 모아놓은 배열입니다.
Choice choices[] =
{
    { {"  /\\_/\\  ", " ( o.o ) ", "  > ^ <  ", "         ", "         ", "         "}, "귀여운 길고양이를 쓰다듬는다.", 1, 5 },
    { {"   ___   ", "  / _ \\  ", " | (_) | ", "  \\___/  ", "         ", "         "}, "수상할 정도로 빨간 버튼을 누른다.", 3, 9 },
    { {"  ====   ", " |    |  ", " |    |  ", "  ====   ", "         ", "         "}, "자판기 밑에서 동전을 줍는다.", 0, 2 },
    { {"   \\|/   ", "  - O -  ", "   /|\\   ", "         ", "         ", "         "}, "태양을 맨눈으로 10초 동안 바라본다.", 8, 12 },
    { {"  [___]  ", "  |   |  ", "  |___|  ", "         ", "         ", "         "}, "유통기한이 3년 지난 통조림을 먹는다.", 5, 15 },
    { {"  _||_   ", " |    |  ", " |    |  ", " |    |  ", " |____|  ", "         "}, "%d층에서 떨어졌다.", 2, 10 },
    { { "                                                   " }, "%d의 속도로 달리는 차에 치인다.", 20, 50 },
    // [복구] 두 번째 코드에서 누락되었던 체력 회복(HP+) 및 휴식 관련 선택지 데이터셋 3종입니다.
    { { "   +++   ", "  +HP+  ", "   +++   ", "         ", "         ", "         " }, "약국에서 진통제를 복용한다.", -15, -10 },
    { { "  [###]  ", "  |   |  ", "  |___|  ", "         ", "         ", "         " }, "편의점에서 이온음료를 마신다.", -8, -3 },
    { { "  Zzz..  ", "  (-_-)  ", "  /| |\\  ", "         ", "         ", "         " }, "잠깐 앉아서 휴식을 취한다.", -8, -4 }
};

int num_choices = sizeof(choices) / sizeof(Choice); // 위 배열에 등록된 선택지가 총 몇 개인지 개수를 계산해 저장합니다.
CHAR_INFO savedScreen[SCREEN_WIDTH * SCREEN_HEIGHT]; // 게임 일시정지 창을 띄울 때, 원래 그려져 있던 화면 정보를 임시 보관할 저장소입니다.

// 현재 눈에 보이는 콘솔 화면 전체를 그대로 복사하여 savedScreen 배열에 백업해둡니다.
void save_console_screen()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // 현재 출력 중인 콘솔창의 관리 권한(핸들)을 가져옵니다.
    COORD bufferSize = { SCREEN_WIDTH, SCREEN_HEIGHT }; // 저장할 임시 버퍼의 크기를 가로 120, 세로 30 크기로 설정합니다.
    COORD bufferCoord = { 0, 0 }; // 백업용 배열의 가장 첫 칸(0,0)부터 차곡차곡 채우겠다는 뜻입니다.
    SMALL_RECT readRegion = { 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1 }; // 콘솔 창 전체 영역(0,0 좌표부터 끝 좌표까지)을 지정합니다.
    ReadConsoleOutput(hConsole, savedScreen, bufferSize, bufferCoord, &readRegion); // 지정한 영역의 화면 데이터를 캡처해 저장합니다.
}

// 일시정지 창이 닫힐 때, 백업해두었던 savedScreen의 화면 데이터를 콘솔창에 그대로 다시 뿌려 복원합니다.
void restore_console_screen()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // 현재 출력 중인 콘솔창의 관리 권한을 가져옵니다.
    COORD bufferSize = { SCREEN_WIDTH, SCREEN_HEIGHT }; // 복원할 데이터의 규격 크기를 매칭합니다.
    COORD bufferCoord = { 0, 0 }; // 백업 배열의 첫 칸부터 읽어오겠다는 뜻입니다.
    SMALL_RECT writeRegion = { 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1 }; // 화면을 다시 덮어쓸 대상을 콘솔창 전체로 지정합니다.
    WriteConsoleOutput(hConsole, savedScreen, bufferSize, bufferCoord, &writeRegion); // 저장해둔 화면을 그대로 copy해서 화면을 복구합니다.
}

// 윈도우 시스템 제어 명령을 사용해 원하는 X(가로), Y(세로) 좌표로 텍스트 출력 커서를 바로 순간이동 시킵니다.
void gotoxy(int x, int y) {
    COORD pos = { (SHORT)x, (SHORT)y }; // 입력받은 X, Y 값을 윈도우 전용 좌표 구조체에 담습니다.
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos); // 함수를 호출해 커서를 해당 위치로 이동시킵니다.
}

// 한글(2칸), 영어/숫자/공백(1칸)이 마구 섞여 있는 문자열이 모니터 화면에서 총 몇 칸의 가로 길이를 차지하는지 정확히 계산합니다.
int calculate_visual_length(const char* str) {
    int len = 0; // 화면에 나타날 실제 글자 폭을 누적할 변수입니다.
    for (int i = 0; str[i] != '\0'; ) { // 글자 문자열이 완전히 끝날 때까지 한 글자씩 검사합니다.
        if ((str[i] & 0x80) == 0) { i += 1; len += 1; } // 아스키 문자(영어, 숫자, 일반 공백)는 1바이트이며 화면에서 1칸을 차지합니다.
        else if ((str[i] & 0xE0) == 0xC0) { i += 2; len += 2; } // 2바이트 크기의 유니코드 특수문자는 화면에서 2칸을 차지합니다.
        else if ((str[i] & 0xF0) == 0xE0) { i += 3; len += 2; } // 3바이트 크기의 UTF-8 한글 및 글자들은 화면에서 2칸을 차지합니다.
        else { i += 4; len += 2; } // 그 외의 4바이트 거대 문자들도 화면에서는 2칸으로 취급합니다.
    }
    return len; // 최종적으로 연산된 화면 상의 시각적 가로 길이를 반환합니다.
}

// 아스키아트 문자열 줄을 분석하여, 순수 그림이 시작되기 전까지의 '앞쪽 공백 크기'와 '순수 그림 글자들의 폭'을 따로 구합니다.
void get_content_stats(const char* str, int* visual_prefix, int* visual_content) {
    int target_start = -1, target_end = -1; // 그림 내용물이 시작되는 글자 위치와 끝나는 글자 위치를 담을 변수입니다.
    int i = 0, current_visual = 0, start_visual = 0; // 문자열 탐색용 인덱스와 가로 폭 누적 연산용 변수들입니다.

    while (str[i] != '\0') { // 문자열 줄이 끝날 때까지 반복해서 문자를 분석합니다.
        int is_blank = 0, char_len = 1, vis_len = 1; // 현재 문자가 공백인지, 바이트 길이는 얼마인지 저장할 임시 변수입니다.
        if (str[i] == ' ') { is_blank = 1; } // 단순 스페이스바 공백인 경우 공백 플래그를 켭니다.
        else if ((unsigned char)str[i] == 0xE2 && (unsigned char)str[i + 1] == 0xA0 && (unsigned char)str[i + 2] == 0x80) {
            is_blank = 1; char_len = 3; vis_len = 2; // 특수한 점자 형태의 빈 공백 문자(U+2800 등)도 공백으로 인정하고 바이트를 세팅합니다.
        }
        else { // 공백이 아니라 무언가 그려진 실제 그림 문자를 만났을 때의 처리입니다.
            if ((unsigned char)str[i] <= 0x7F) { char_len = 1; vis_len = 1; } // 영문/숫자 그림문자
            else if ((unsigned char)str[i] >= 0xC0 && (unsigned char)str[i] <= 0xDF) { char_len = 2; vis_len = 2; }
            else if ((unsigned char)str[i] >= 0xE0 && (unsigned char)str[i] <= 0xEF) { char_len = 3; vis_len = 2; } // 한글/기호 그림문자
            else { char_len = 4; vis_len = 2; }
        }
        if (!is_blank) { // 지금 보고 있는 글자가 공백이 아닐 때 실행됩니다.
            if (target_start == -1) { target_start = i; start_visual = current_visual; } // 맨 처음으로 그림 문자를 발견했다면 시작 지점으로 등록합니다.
            target_end = i + char_len - 1; // 그림의 끝 지점을 계속해서 업데이트해 둡니다.
        }
        current_visual += vis_len; i += char_len; // 다음 글자를 분석하기 위해 인덱스와 시각적 위치를 누적 이동시킵니다.
    }
    if (target_start == -1) { *visual_prefix = 0; *visual_content = 0; return; } // 만약 줄 전체가 공백이라 그림이 없으면 0을 채우고 끝냅니다.

    int content_vis = 0; i = target_start; // 여기서부터는 알아낸 그림 시작점부터 끝점까지 돌며 '순수 그림만의 폭'을 랲니다.
    while (i <= target_end && str[i] != '\0') {
        int char_len = 1, vis_len = 1;
        if ((unsigned char)str[i] <= 0x7F) { char_len = 1; vis_len = 1; }
        else if ((unsigned char)str[i] >= 0xC0 && (unsigned char)str[i] <= 0xDF) { char_len = 2; vis_len = 2; }
        else if ((unsigned char)str[i] >= 0xE0 && (unsigned char)str[i] <= 0xEF) { char_len = 3; vis_len = 2; }
        else { char_len = 4; vis_len = 2; }
        content_vis += vis_len; i += char_len; // 순수 그림만의 가로 칸수를 더해나갑니다.
    }
    *visual_prefix = start_visual; *visual_content = content_vis; // 포인터 변수 주소를 참조해 원본 변수에 최종 연산값을 전달합니다.
}

// 게임을 구동했을 때 유저에게 최초로 노출되는 오프닝 인트로 파란색 박스 로고 화면입니다.
void ShowLogo(void)
{
    printf("\x1b[2J"); // ANSI 특수 탈출 코드를 사용해 콘솔 스크린에 그려진 모든 내용을 깨끗이 지웁니다.

    int box_width = 60;   // 화면 중앙에 그릴 파란색 배경 박스의 가로 폭을 설정합니다.
    int box_height = 24;  // 파란색 배경 박스의 세로 높이를 설정합니다.
    int box_start_x = 37; // 파란색 박스가 그려지기 시작할 가로 X 좌표입니다.
    int box_start_y = 5;  // 파란색 박스가 그려지기 시작할 세로 Y 좌표입니다.

    for (int i = 0; i < box_height; i++) // 설정한 세로 높이만큼 아래로 내려가며 반복합니다.
    {
        printf("\x1b[%d;%dH", box_start_y + i, box_start_x); // 박스를 그릴 해당 줄의 시작 시작 위치로 커서를 이동시킵니다.
        printf("\x1b[44m"); // 배경 색상을 파란색(44)으로 변경합니다.
        for (int j = 0; j < box_width; j++)
        {
            printf(" "); // 공백 문자를 연속 출력하여 파란색 면적을 가로로 채워나갑니다.
        }
        printf("\x1b[0m"); // 한 줄 출력이 끝나면 색상 변경 설정을 기본값으로 초기화합니다.
    }

    // 파란 박스 안에 찍어줄 픽셀 그래픽 디자인 문자열 배열입니다. (W=흰칸, B/X=검은칸, 공백=파란바탕)
    const char* logo[] = {
       "                                                    ",
       "  WWWWWW                                            ",
       " WWW   WWWBB                                        ",
       " WWW   WWWBB                                        ",
       " WWW   WWWBB  XXXXXX  XXXXX  XXXXX                  ",
       "  WWWWWWWBB        X   X   X  X                     ",
       "    BBBBB         X    XXXXX  XXXXX                 ",
       "                  X     X   X  X                    ",
       "                XXXXXX  X   X  XXXXX                ",
       "                                                    ",
       "                 XXX   X   X  XXXXX                 ",
       "                X   X  XX  X  X                     ",
       "                X   X  X X X  XXXXX                 ",
       "                X   X  X  XX  X                     ",
       "                 XXX   X   X  XXXXX      WWW        ",
       "                                       WWWWWBB      ",
       "                                       WW WWBB      ",
       "                                          WWBB      ",
       "                                          WWBB      ",
       "                                      WWWWWWWWBB    ",
       "                                       BBBBBB       "
    };

    int logo_height = 21; // 픽셀 아트 이미지의 세로 라인 수입니다.
    int logo_width = 52;  // 픽셀 아트 이미지의 가로 한 줄 문자 수입니다.

    int logo_start_x = box_start_x + (box_width - logo_width) / 2; // 파란 박스의 정중앙 가로에 오도록 수학적으로 배치 시작점을 잡습니다.
    int logo_start_y = box_start_y + (box_height - logo_height) / 2; // 파란 박스의 정중앙 세로에 오도록 배치 시작점을 잡습니다.

    for (int i = 0; i < logo_height; i++) // 픽셀 아트 행 수만큼 돌려줍니다.
    {
        printf("\x1b[%d;%dH", logo_start_y + i, logo_start_x); // 픽셀을 한 줄 한 줄 찍을 시작 위치로 이동합니다.
        for (int j = 0; j < logo_width; j++) // 가로 칸만큼 스캔하며 색상을 채워 나갑니다.
        {
            if (logo[i][j] == 'W')
            {
                printf("\x1b[47m "); // 'W' 기호가 있는 자리는 흰색 배경(47) 칸으로 칠해 픽셀을 만듭니다.
            }
            else if (logo[i][j] == 'B' || logo[i][j] == 'X')
            {
                printf("\x1b[40m "); // 'B' 나 'X' 기호가 있는 자리는 검은색 배경(40) 칸으로 칠합니다.
            }
            else
            {
                printf("\x1b[44m "); // 그 외 바탕 공백 기호 자리는 박스 배경과 일치하도록 파란색 배경 칸을 유지합니다.
            }
        }
        printf("\x1b[0m"); // 로고의 한 줄 그리가 끝나면 색상 매핑을 기본으로 복원합니다.
    }

    printf("\x1b[%d;1H\n", box_start_y + box_height + 1); // 로고 그리기가 끝났으니 커서를 파란 상자 바깥쪽 하단 빈칸으로 격리합니다.

    move_cursor(logo_start_x - 5, logo_start_y + logo_height + 2); // 안내 문구를 출력하기 위해 적절한 위치로 커서를 옮깁니다.
    set_color(FONT_COLOR_WHITE); // 안내문 글자색을 흰색으로 변경합니다.
    printf(" F11키를 눌러 전체화면으로 바꾸고 Enter키를 누르면 시작합니다."); // 콘솔 안내 멘트를 노출합니다.

    int key; // 유저가 누른 키를 임시 저장할 변수입니다.
    do {
        key = _getch(); // 키보드 입력을 실시간으로 하나 읽어옵니다.
    } while (key != '\r'); // 엔터 키('\r')가 들어올 때까지 다른 키들은 무시하고 무한 대기합니다.

    system("cls"); // 엔터가 눌려 루프를 나가면 화면을 전부 밀어내어 깨끗이 지웁니다.
}

// 메인 타이틀 디자인 파일을 읽어와 한가운데 정렬하고, 화살표식 메뉴 선택 인터페이스를 구현하는 핵심 타이틀 함수입니다.
int RenderTitle(void)
{
    static char title_lines[40][256]; // 파일에서 읽어온 대형 메인 타이틀 아트를 저장해 둘 2차원 배열 메모리 슬롯입니다.
    static int title_line_count = 0;  // 불러온 타이틀 아트의 전체 세로 줄 수를 기억할 변수입니다.
    static int max_content_width = 0; // 불러온 타이틀 아트 중에서 가장 가로 폭이 긴 줄의 폭 크기입니다.
    static int title_loaded = 0;      // 파일을 이미 읽었는지 체크하여 중복 로딩을 막아주는 안전장치 변수입니다.

    if (!title_loaded) { // 프로그램을 켜고 타이틀을 한 번도 불러온 적이 없다면 중괄호 안이 동작합니다.
        FILE* f = fopen("dlrjfwnrsp.txt", "r"); // 외부 폴더에 있는 타이틀 텍스트 파일("이걸죽네" 초성 파일)을 읽기 모드로 엽니다.
        if (f != NULL) { // 파일이 에러 없이 무사히 성공적으로 열렸다면 작동합니다.
            char temp[1024]; // 한 줄 내용을 임시로 받아 적을 버퍼 가방입니다.
            while (fgets(temp, sizeof(temp), f) && title_line_count < 40) { // 파일 끝까지 혹은 최대 40줄까지 한 줄씩 읽어 들입니다.
                temp[strcspn(temp, "\r\n")] = 0; // 읽어온 글자 맨 끝의 불필요한 줄바꿈(\r, \n) 엔터 표시를 잘라내 지웁니다.
                int prefix = 0, content_w = 0; // 해당 줄의 앞 공백과 순수 아트를 판독할 변수입니다.
                get_content_stats(temp, &prefix, &content_w); // 줄 폭을 정밀 분석기에 집어넣어 크기를 잽니다.
                if (content_w > 0) { // 빈 줄이 아닌 무언가 그려진 진짜 유효한 라인이라면 실행합니다.
                    if (content_w > max_content_width) max_content_width = content_w; // 찾은 크기 중 가장 넓은 가로 폭을 최댓값으로 갱신해 나갑니다.
                    strcpy(title_lines[title_line_count++], temp); // 임시 보관한 줄 내용을 영구 배열 공간에 복사해 담고 줄 수를 카운트합니다.
                }
            }
            fclose(f); // 파일 분석 및 메모리 복사가 완료되었으므로 열었던 텍스트 파일을 안전하게 닫아줍니다.
        }
        title_loaded = 1; // 다음 루프 실행 시에는 이 긴 파일 로드 과정을 건너뛰도록 로딩 완료 도장을 찍습니다.
    }

    // [복구] 상단 좌측 여백에 플레이어의 입력을 환영하는 문구를 산뜻한 초록색으로 렌더링합니다.
    set_color(BG_COLOR_BLACK);
    set_color(FONT_COLOR_GREEN);
    move_cursor(10, 3);
    printf("환영합니다, [%s] 님!", playerName);

    set_color(FONT_COLOR_WHITE); // 타이틀 그래픽 폰트의 글씨 색상을 깔끔한 흰색으로 설정합니다.
    int title_start_y = 2; // 화면 맨 위 천장에서부터 2칸 아래를 시작 높이로 지정합니다.
    for (int i = 0; i < title_line_count; i++) { // 파일에서 복사해둔 타이틀 줄 수만큼 화면에 드로우하기 위해 반복합니다.
        int prefix = 0, content_w = 0; // 현재 그릴 줄의 폭 정보를 저장할 변수입니다.
        get_content_stats(title_lines[i], &prefix, &content_w); // 줄의 공백과 크기를 다시 추출합니다.
        int target_x = (SCREEN_WIDTH - max_content_width) / 2 - prefix; // 120칸 가로 화면 기준 한가운데 정렬되도록 오프셋 수학 연산을 수행합니다.
        if (target_x < 1) target_x = 1; // 연산 결과가 화면 왼쪽 바깥으로 튕겨나가지 않도록 최소 한계를 1칸으로 가둡니다.
        move_cursor(target_x + 65, title_start_y + i + 14); // 완벽한 좌우 중앙 정렬 계산 결과 좌표 위치로 커서를 옮겨 놓습니다.
        printf("%s", title_lines[i]); // 마침내 정렬된 자리에 메인 아트 문구를 화면에 출력합니다.
    }

    int base_y = title_start_y + title_line_count + 2; // 타이틀 아트 그래픽이 모두 끝난 지점 아래쪽 2칸 밑을 메뉴 시작 높이로 잡습니다.
    int base_x = (SCREEN_WIDTH - max_content_width) / 2 + 6; // 메뉴 글자들도 타이틀 정렬선에 맞춰 보기 좋게 들여쓰기 가로 시작점을 잡습니다.

    // 1번 메뉴 항목을 그립니다. 현재 menu 변수가 1이면 노란색 강조 화살표를 달아주고, 아니면 그냥 흰색 기본으로 그립니다.
    move_cursor(base_x + 75, base_y + 14);
    if (menu == 1) { set_color(FONT_COLOR_YELLOW); printf("▶ 1. 만든 사람 및 팀 소개"); }
    else { set_color(FONT_COLOR_WHITE);           printf("   1. 만든 사람 및 팀 소개"); }

    // 2번 설명서 메뉴 항목을 상황에 맞춰 스캔 출력합니다.
    move_cursor(base_x + 75, base_y + 16);
    if (menu == 2) { set_color(FONT_COLOR_YELLOW); printf("▶ 2. 설명서"); }
    else { set_color(FONT_COLOR_WHITE);           printf("   2. 설명서"); }

    // 3번 게임 시작 메뉴 항목을 상황에 맞춰 스캔 출력합니다.
    move_cursor(base_x + 75, base_y + 18);
    if (menu == 3) { set_color(FONT_COLOR_YELLOW); printf("▶ 3. 게임 시작"); }
    else { set_color(FONT_COLOR_WHITE);           printf("   3. 게임 시작"); }

    // 4번 게임 종료 메뉴 항목을 상황에 맞춰 스캔 출력합니다.
    move_cursor(base_x + 75, base_y + 20);
    if (menu == 4) { set_color(FONT_COLOR_YELLOW); printf("▶ 4. 게임 종료"); }
    else { set_color(FONT_COLOR_WHITE);           printf("   4. 게임 종료"); }

    // 화면 우측 하단 구석지에 노란색 가이드 글씨로 조작법 숏컷 안내 자막을 띄웁니다.
    set_color(FONT_COLOR_YELLOW);
    move_cursor(190, 45); printf("↑: 위로 이동");
    move_cursor(190, 46); printf("↓: 밑으로 이동");
    move_cursor(190, 47); printf("Enter : 선택");
    move_cursor(190, 48); printf("ESC : 게임 종료");

    move_cursor(106, 100); // 텍스트를 다 그려서 깜빡거리는 하얀 커서를 화면 밖 멀리 던져서 안 보이게 대피시킵니다.

    char a = _getch(); // 유저가 조작 키를 입력할 때까지 대기하며 키 값을 한 바이트 읽어옵니다.

    switch (a) // 입력된 키 값에 따라 이동 분기 처리를 수행합니다.
    {
    case 72: // 위쪽 화살표(↑) 키를 누른 경우입니다.
        if (menu > 1) menu--; // 최상단 1번 메뉴가 아닐 때만 가리키는 메뉴 번호를 한 칸 위로 올립니다.
        break;
    case 80: // 아래쪽 화살표(↓) 키를 누른 경우입니다.
        if (menu < 4) menu++; // 최하단 4번 메뉴가 아닐 때만 가리키는 메뉴 번호를 한 칸 아래로 내립니다.
        break;
    case 27: // 키보드 왼쪽 위 ESC 키를 누른 경우입니다.
        isRunning = 0; // 전역 루프 제어 스위치를 꺼서 프로그램 전체가 종료 단계를 밟도록 만듭니다.
        break;
    case 13: // 엔터(Enter) 키를 누른 경우입니다.
        if (menu >= 1 && menu <= 4) return menu; // 현재 화살표가 멈춰있던 최종 메뉴 번호를 메인 제어 루프로 반환합니다.
        break;
    }

    return 0; // 엔터를 누른 게 아니면 그냥 0을 리턴하여 메인 루프에서 타이틀을 무한 새로고침하게 만듭니다.
}

// 각 팀원의 고유 개별 아스키아트 파일명과 이름 직책 정보를 매개변수로 넘겨받아 중앙 정렬 레이아웃으로 출력해주는 함수입니다.
void print_member_page(const char* filename, const char* description) {
    printf("\x1b[2J\x1b[H"); // 화면 내용을 전체 청소하고 커서 좌표를 맨 왼쪽 위 구석(1,1)으로 즉시 강제 워프시킵니다.
    set_color(FONT_COLOR_WHITE); // 출력할 그래픽 텍스트 글자색을 흰색으로 변경합니다.

    FILE* file = fopen(filename, "r"); // 인자로 넘겨받은 텍스트 파일명("1.txt" 등)을 읽기 전용 모드로 조심스럽게 오픈합니다.
    int y = 2; // 그림이 그려질 시작 줄 위치를 화면 맨 위에서 2줄 아래로 세팅합니다.
    if (file != NULL) { // 텍스트 파일이 에러 없이 무사히 로드되었다면 내부를 렌더링합니다.
        char buffer[1024]; // 파일의 글자 줄들을 한 줄씩 임시로 쓸 가방을 준비합니다.
        while (fgets(buffer, sizeof(buffer), file) && y <= 36) { // 파일 끝까지 읽되, 화면 크기를 감안해 최대 36줄까지만 가져옵니다.
            buffer[strcspn(buffer, "\r\n")] = 0; // 문자열 끝단에 포함되어 넘어오는 엔터 기호 행바꿈 값을 완벽히 삭제합니다.
            int visual_len = calculate_visual_length(buffer); // 한글과 영어가 혼합된 현재 줄 문장의 정확한 화면 실측 폭을 측정합니다.
            int x = (SCREEN_WIDTH - visual_len) / 2; // 전체 화면 폭 120칸 기준 정확히 센터 가로 정렬을 위한 좌표 값을 수식으로 도출합니다.
            if (x < 1) x = 1; // 연산된 좌표가 왼쪽 벽을 뚫고 버그가 나지 않도록 바운더리를 제한합니다.
            move_cursor(x + 45, y++); // 계산 완료된 가로 중앙 좌표 x와 한 칸씩 증가되는 세로 y 위치로 커서를 옮깁니다.
            printf("%s", buffer); // 마침내 정교하게 정렬된 자리에 팀원 개별 아스키아트 아트를 한 줄 출력합니다.
        }
        fclose(file); // 개별 그림 로드가 무사히 종료되었으므로 오픈했던 텍스트 파일 리소스를 안전하게 닫아줍니다.
    }
    else { // 파일이 누락되었거나 이름 오타로 열리지 않았을 때 예외 안전 에러 처리를 작동시킵니다.
        move_cursor(10, 10);
        printf("[오류] '%s' 파일을 찾을 수 없습니다.\n", filename);
    }

    // 아스키아트 아래 하단 고정 스펙 영역 레이아웃 가이드를 드로우합니다.
    move_cursor((SCREEN_WIDTH + 38) / 2, 38); // 하단 영역 데코레이션 가로선 줄 위치를 정중앙으로 잡습니다.
    printf("==================================================");
    move_cursor((SCREEN_WIDTH - calculate_visual_length(description)) / 1, 39); // 팀원 직책 학번 이름 정보를 중앙으로 정렬해 출력합니다.
    printf("%s", description);
    move_cursor((SCREEN_WIDTH + 38) / 2, 40); // 닫는 데코레이션 가로 구분선 줄을 정중앙 정렬해 출력합니다.
    printf("==================================================");

    // 유저가 페이지를 제어할 수 있도록 돕는 UI 가이드 네비게이션 메시지를 중앙 정렬로 노출합니다.
    const char* nav_str = "[ <- 이전 페이지 ]        [ Backspace 메뉴로 돌아가기 ]        [ 다음 페이지 -> ]";
    move_cursor((SCREEN_WIDTH + calculate_visual_length(nav_str)) / 3, 42); // 안내 가이드 한 줄을 가로 기준 정중앙 정렬 좌표로 기동합니다.
    printf("%s", nav_str); // 조작 내비 바 텍스트를 출력합니다.
}

// 만든 사람 및 팀 소개 메뉴의 맨 마지막 5페이지를 채우게 될 종합 크레딧 블루 패널 창 렌더러 함수입니다.
void draw_final_screen(void) {
    printf("\x1b[2J\x1b[H"); // 화면창 전역을 한번 깨끗이 지우고 홈 좌표로 커서를 이동시킵니다.

    int box_width = 60;  // 크레딧을 감쌀 배경 상자의 가로 폭 크기를 설정합니다.
    int box_height = 30; // 크레딧을 감쌀 배경 상자의 세로 줄 높이를 설정합니다.
    int box_start_x = (SCREEN_WIDTH - box_width) / 1; // 전체 콘솔 창 120칸 기준 한가운데 정확히 박스가 오도록 가로 시작 좌표를 연산합니다.
    int box_start_y = 1; // 천장 아래 3번째 세로 줄을 시작선으로 잡습니다.

    for (int i = 0; i < box_height; i++) { // 계산한 상자 세로 범위만큼 한 줄씩 도강하며 렌더합니다.
        printf("\x1b[%d;%dH\x1b[44m", box_start_y + i + 6, box_start_x + 15); // 커서를 상자 시작 위치로 보내고 배경색을 파란색(44)으로 마킹합니다.
        for (int j = 0; j < box_width; j++) { printf(" "); } // 상자 가로 면적만큼 스페이스바를 쏘아 파란 가득 채운 사각형 면을 만듭니다.
        printf("\x1b[0m"); // 채색 렌더링이 끝나면 색상 마스킹 설정을 초기화합니다.
    }

    // 파란 박스 상단에 찍어줄 팀 타이틀 "ZERONE" 이니셜 그래픽 매핑용 배열입니다.
    const char* title_text[] = {
        "XXXXX  XXXXX  XXXX   XXXXX  X   X  XXXXX",
        "   X   X      X   X  X   X  XX  X  X    ",
        "  X    XXXX   XXXX   X   X  X X X  XXXX ",
        " X     X      X  X   X   X  X  XX  X    ",
        "XXXXX  XXXXX  X   X  XXXXX  X   X  XXXXX"
    };

    int title_width = (int)strlen(title_text[0]); // ZERONE 픽셀 자막의 가로 글자 길이를 뽑아옵니다.
    int title_start_x = box_start_x + 16 + (box_width - title_width) / 2; // 파란 박스 가로 폭 내부에서 다시 정확히 정중앙에 오도록 내부 마진을 연산합니다.
    int title_start_y = box_start_y + 15; // 파란 박스 상단 모서리로부터 10칸 아래를 타이틀 찍기 시작선으로 잡습니다.

    for (int i = 0; i < 5; i++) { // 5줄짜리 자막 그래픽 루프를 돌려줍니다.
        printf("\x1b[%d;%dH", title_start_y + i, title_start_x); // 매 줄의 출력 정렬 좌표 위치로 실시간 커서 이동을 처리합니다.
        for (int j = 0; j < title_width; j++) {
            if (title_text[i][j] == 'X') { printf("\x1b[40m "); } // 기호 'X' 가 박힌 배열 칸은 검은색 배경(40) 블록으로 색칠 픽셀 처리를 수행합니다.
            else { printf("\x1b[44m "); } // 빈칸은 박스 원경과 자연스레 합치되도록 파란색 배경 칸 상태를 유지해 줍니다.
        }
        printf("\x1b[0m"); // 줄 처리 완료 후 색상 설정을 기본값으로 돌립니다.
    }

    // 팀의 영광스러운 전체 구성원 학번 직책 이름 명단 문자열 목록 데이터셋입니다.
    const char* team_text[] = {
        " team 01 (ZERONE) 팀원들",
        "마준서(202617166) : 총괄",
        "백종화(202617139) : 코드",
        "이인욱(202619389) : 코드",
        "이준현(202619549) : 디자인"
    };

    int team_start_y = title_start_y + 7; // ZERONE 픽셀 타이틀 그래픽 출력 완료선 아래로 7칸 밑을 명단 띄우기 세로 시작 위치로 지정합니다.
    for (int i = 0; i < 5; i++) { // 명단 수만큼 반복 주행합니다.
        int text_vis_len = calculate_visual_length(team_text[i]); // 명단 텍스트가 화면에서 차지하는 가로 길이를 각각 계측합니다.
        int team_line_x = box_start_x + 15 + (box_width - text_vis_len) / 2; // 파란 박스 내부 안에서 각각 가로 정중앙 라인 정렬선에 결속되게 좌표를 뽑습니다.
        printf("\x1b[%d;%dH\x1b[30m\x1b[44m%s\x1b[0m", team_start_y + i, team_line_x, team_text[i]); // 파란 배경(44) 위에 가독성을 확보한 검은색 폰트(30)로 스태프 명단을 찍습니다.
    }

    // 박스 레이아웃 설계 밖 아래쪽 마진 빈 곳에 최종 내비게이션 바 안내를 하단 정중앙에 배치합니다.
    const char* final_nav = "[ <- 이전 페이지 ]        [ Backspace 메뉴로 돌아가기 ]        [ ESC 종료 ]";
    int final_nav_x = (SCREEN_WIDTH - calculate_visual_length(final_nav)) / 2; // 전체 스크린 기준 센터 좌표값을 수학 연산합니다.
    move_cursor(final_nav_x + 45, box_start_y + 9 + box_height + 2); // 하단 빈 여백선 줄로 최종 이동 기동을 명령합니다.
    set_color(FONT_COLOR_WHITE); // 조작 가이드 안내용 폰트색을 깨끗한 흰색으로 바꿉니다.
    printf("%s", final_nav); // 최종 종합 크레딧창 안내 가이드 라인을 출력 완료합니다.
}

// 1번 메뉴를 선택해 내부 진입에 성공했을 시 작동하는 팀원 소개 네비게이션 라우팅 컨트롤 핵심 함수입니다.
int People(void)
{
    const char* filenames[] = { "1.txt", "2.txt", "3.txt", "4.txt" }; // 개별로 읽어 들일 팀원 4명의 텍스트 파일명 배열 데이터셋입니다.
    const char* descriptions[] = { // 각 팀원의 아스키아트 하단에 찍어줄 정보 매칭 설명 문자열 모음 배열입니다.
        "마준서(202617166) : 총괄   ",
        "백종화(202617139) : 코드   ",
        "이인욱(202619389) : 코드   ",
        "이준현(202619549) : 디자인  "
    };

    int current_page = 0; // 유저가 가장 먼저 마주하게 될 스타트 페이지 넘버 인덱스입니다. (0번 = 1페이지)
    int total_pages = 5;  // 팀원 개인장 4장 + 마지막 ZERONE 전체 종합장 1장 = 합산 총 5개의 페이지 구조를 선언합니다.

    while (1) // 유저가 백스페이스나 ESC를 눌러 화면을 이탈하기 전까지 무한 루프로 돌며 화면을 렌더 제어합니다.
    {
        if (current_page < 4) { // 현재 인덱스가 0~3 범위 안에 있을 때는 개별 팀원 상세 소개 페이지 화면으로 취급해 분기 처리합니다.
            print_member_page(filenames[current_page], descriptions[current_page]); // 해당 파일명과 설명을 매개변수로 매핑해 던져서 출력합니다.
        }
        else if (current_page == 4) { // 인덱스가 4를 터치해 찍었을 때는 마지막 페이지 특수 상태이므로 종합 크레딧창을 전용으로 렌더시킵니다.
            draw_final_screen();
        }

        int ch = _getch(); // 키보드 키 입력을 실시간 가로채기하여 한 글자 판독을 시도합니다.

        if (ch == 224 || ch == 0) { // 방향키 같은 특수 기능 확장 키들이 입력되었을 때 들어오는 리딩 접두사 조건 판정 블록입니다.
            ch = _getch(); // 접두사 뒤에 연속해서 붙어 유입되는 진짜 방향키 고유 식별 코드를 한 번 더 수령 가공합니다.

            if (ch == 75) { // 왼쪽 화살표(←) 방향키를 클릭했을 경우 작동되는 로직입니다.
                if (current_page > 0) {
                    current_page--; // 가장 첫 페이지가 아닐 때에 한해서 현재 보고 있는 페이지 번호를 뒤로 한 칸 후퇴 이동시킵니다.
                }
            }
            else if (ch == 77) { // 오른쪽 화살표(→) 방향키를 클릭했을 경우 작동되는 로직입니다.
                if (current_page < total_pages - 1) {
                    current_page++; // 맨 마지막 종합 페이지가 아닐 때에 한해서 보고 있는 페이지 번호를 한 단계 앞으로 전진 이동시킵니다.
                }
            }
        }
        else if (ch == 8) { // 특수 방향 확장키가 아니라 단순 백스페이스(Backspace, 아스키코드 8) 키를 입력받은 긴급 탈출 조건식입니다.
            system("cls");  // 팀원 보기 창 내부를 흔적 없이 깨끗하게 세정하고 지워냅니다.
            return 0;       // 메인 main 함수의 본래 메뉴 스위치 선택 대기소 상태로 리턴 복귀하여 메인화면을 복구시킵니다.
        }
        else if (ch == 27) { // 컴퓨터 자판 왼쪽 맨 위 ESC(아스키코드 27) 키를 누른 원클릭 프로세스 즉각 종료 강제 분기 처리입니다.
            exit(0);         // 지체 없이 실행 중인 콘솔 프로그램 인스턴스 전체 프로세스를 완벽히 강제 킬(Kill) 종료 폐쇄합니다.
        }
    }

    return 0;
}

// 2번 설명서 메뉴 진입에 트리거 성공 시 2페이지 분량의 게임 안내 규칙 가이드를 생성 노출하는 함수입니다.
int Manual(void)
{
    int key = 0;          // 사용자가 누르는 가이드 제어 조작 키를 매핑받을 스토리지 변수입니다.
    int Manual_page = 1;  // 유저에게 현재 표기 중인 설명서 가이드의 페이지 상태를 관리합니다. (초기값 1페이지 지정)
    system("cls");        // 설명서 인터페이스를 깔끔히 배치하기 전 이전 흔적을 싹 비워냅니다.

    while (key != 8) // 유저가 이전 메인 메뉴로 리턴 복귀하기 위해 백스페이스(8) 키를 입력하기 전까지 가이드 가둠 루프가 돕니다.
    {
        system("cls"); // 설명서 페이지가 갱신 전환될 때 글자가 잔상으로 겹쳐 오염되지 않도록 도화지를 깨끗하게 샤워 청소합니다.

        if (Manual_page == 1) // 현재 가이드북 상태가 1페이지일 경우 렌더링할 룰북 그래픽 인터페이스 모듈 블록입니다.
        {
            move_cursor(106, 26); // 화면 우하단 지정 정렬 좌표선 위치로 기동합니다.
            printf("\033[1m다음장 (→)\033[0m"); // 진하게 속성 옵션을 준 특수 ANSI 문구를 쏘아 다음장 유도 내비 힌트를 표기합니다.

            set_color(BG_COLOR_BRIGHTMAGENTA); // 설명서 타이틀 상자 배경 색상을 화사한 밝은 마젠타색(95)으로 칠합니다.
            set_color(FONT_COLOR_WHITE); // 타이틀 상자 내부 자막 글씨색을 선명한 흰색(37)으로 매스킹 변경합니다.
            move_cursor(45, 4); // 상단 중앙 배치 좌표선 줄 위치로 날아갑니다.
            printf("=========== 설명서 ==========="); // 가이드북 대형 대문 간판을 프린트합니다.

            set_color(BG_COLOR_BLACK); // 타이틀 상자 그리기가 완전히 마감되었으므로 배경 기본색을 기본인 검은색(40)으로 환원시킵니다.

            set_color(FONT_COLOR_YELLOW); // 항목 요약 헤드라인을 강조하기 위해 노란색(33)으로 글자색을 교체 주입합니다.
            move_cursor(40, 8); printf("게임 제목 : 이걸 죽네"); // 게임의 메인 타이틀 네임을 각인 노출합니다.

            set_color(FONT_COLOR_RED); move_cursor(40, 11); printf("HP"); // 플레이어 중요 자원 스탯 지표인 체력을 뜻하는 단어를 붉은색(31)으로 찍어경고감을 줍니다.
            set_color(FONT_COLOR_WHITE); printf("가 0 이하가 되기 전까지 최대한 많은 턴을 버티는 게임입니다."); // 기본 매뉴얼 텍스트를 출력합니다.

            move_cursor(40, 13); printf("매 턴마다 2개 또는 3개의 선택지가 나옵니다."); // 게임 진행 방식 규칙 내용을 각 줄 위치에 차례로 노출합니다.
            move_cursor(40, 14); printf("선택지 안의 숫자는 무작위로 정해집니다.");
            move_cursor(40, 15); printf("선택한 행동에 따라 ");

            set_color(FONT_COLOR_RED); printf("HP"); set_color(FONT_COLOR_WHITE); printf("가 다르게 감소합니다."); // 경고 단어 조합 문자열을 컬러 레이아웃에 맞게 출력합니다.

            set_color(FONT_COLOR_YELLOW); move_cursor(40, 18); printf("Backspace"); // 복귀 조작 숏컷 버튼 이름을 노란색으로 하이라이팅하여 가인식성을 확보합니다.
            set_color(FONT_COLOR_WHITE); printf("를 눌러 메뉴로 돌아가시오"); // 1페이지 매뉴얼 드로우 모듈 작동을 완전 마감합니다.
        }

        if (Manual_page == 2) // 사용 설명서의 현재 상태가 2페이지(키 조합 입력 가이드라인)일 경우 작동되는 렌더 블록입니다.
        {
            move_cursor(5, 26); printf("\033[1m이전장 (←)\033[0m"); // 화면 좌하단 마진으로 기동하여 이전 쪽으로 가기 방향키 제어법 자막을 표시합니다.
            move_cursor(98, 26); printf("\033[1m나가기 (Backspace)\033[0m"); // 화면 우하단 마진으로 기동하여 나가기 버튼 사용법 자막을 표시합니다.

            move_cursor(55, 4); printf("\033[1m키 설명\033[0m"); // 진하게 타이틀 속성을 먹여 키 가이드 대문 소제목을 표기합니다.
            move_cursor(48, 9);  printf("↑: 위로 이동"); // 인게임 키 세팅 리스트를 한 행씩 가독성을 보장하는 간격 좌표선에 순차 프린트합니다.
            move_cursor(48, 10); printf("↓: 밑으로 이동");
            move_cursor(48, 11); printf("←: 왼쪽 선택");
            move_cursor(48, 12); printf("→: 오른쪽 선택");
            move_cursor(48, 13); printf("Enter : 선택");
            move_cursor(48, 14); printf("ESC : 게임 종료");
            move_cursor(48, 15); printf("Backspace : 뒤로 가기"); // 2페이지 키 조작 매뉴얼 드로우 모듈을 완전 마감합니다.
        }

        key = _getch(); // 가이드를 다 본 유저가 페이지를 핸들링하기 위해 넣는 단일 키보드 모션을 홀딩 대기 입수합니다.

        switch (key) // 가이드 제어 수령 입력값에 맞게 상태 전이 기동을 처리합니다.
        {
        case 75: // 키보드 왼쪽 화살표(←) 키를 누른 트리거 조건 분기입니다.
            if (Manual_page > 1) { Manual_page = Manual_page - 1; } // 현재 보고 있는 상태가 1페이지보다 높을 때만 가이드북 상태를 앞장으로 롤백 복귀시킵니다.
            break;
        case 77: // 키보드 오른쪽 화살표(→) 키를 누른 트리거 조건 분기입니다.
            if (Manual_page < 2) { Manual_page = Manual_page + 1; } // 현재 보고 있는 상태가 2페이지 미만일 때만 가이드북 상태를 다음장 상태로 격상 이동시킵니다.
            break;
        case 27: // 매뉴얼 탐독 중 즉각 완전 아웃 탈출을 위해 ESC 키를 누른 경우입니다.
            exit(0); // 프로그램 가동 엔진 인스턴스를 즉각 전면 올 클로즈 폐쇄 조치합니다.
            break;
        }
    }

    system("cls"); // 백스페이스가 마침내 입력되어 매뉴얼 메인 루프 조건이 깨지고 나가질 때 콘솔 내벽을 청정 비우기 처리합니다.
    return 0; // 메인 타이틀을 다시 깔끔하게 다시 소환하도록 안전 신호값 정수 0을 리턴 반환합니다.
}

// 3번 메뉴 '게임 시작' 발동 시 전개되는 실제 유저 인터랙티브 텍스트 서바이벌 메인 게임 제어 스레드 함수입니다.
int Gamestart(void)
{
    system("mode con cols=210 lines=60"); // 스토리 인트로 오프닝 텍스트 아트가 거대하므로 글자가 모니터에서 찢어지거나 깨지지 않게 화면창 레이아웃 규격을 임시로 가로 210, 세로 60의 초대형 비율 크기로 확장합니다.
    system("cls"); // 사이즈 리사이징 조정으로 터미널 내벽에 잔류하게 된 잔상 먼지들을 완전 세척 클리어 처리합니다.

    FILE* fp = fopen("ta.txt", "r"); // 오프닝 타이틀 스토리 서사를 간직하고 있는 외부 텍스트 자원 파일("ta.txt")을 읽기전용으로 노크 오픈합니다.
    char buffer[1024]; // 스토리 파일 내의 줄들을 한 줄씩 담아 올 임시 이동용 대형 컨테이너 캐리어 가방입니다.
    int start_x = 40; // 인트로 대형 그래픽 텍스트가 노출되기 시작할 베이스 가로 X 정렬선 여백 좌표입니다.
    int current_y = 10; // 인트로 대형 그래픽 텍스트가 노출되기 시작할 베이스 세로 Y 여백 높이입니다.

    if (fp != NULL) { // 인트로 스토리 파일이 누락 없이 정상 경로에서 확실하게 오픈 작동에 성공했다면 실행합니다.
        while (fgets(buffer, sizeof(buffer), fp) != NULL) { // 파일 내부에 기록된 스토리 텍스트 라인의 끝이 드러날 때까지 연속으로 루프 버퍼링을 돌립니다.
            buffer[strcspn(buffer, "\r\n")] = 0; // 문자열 우측 맨 끝 단에 묻어 유입되는 불필요한 줄바꿈 행 엔터 제어값을 깔끔하게 소멸 제거합니다.
            gotoxy(start_x, current_y); printf("%s", buffer); current_y++; // 지정한 여백 가로 정렬선 x좌표 자리에 맞춰 매 줄마다 세로축을 한 칸씩 낙하 전개하며 화려한 인트로 아트를 순차 드로우 렌더링합니다.
        }
        fclose(fp); // 인트로 스토리 데이터의 로딩 처리가 전면 종료되었으므로 오픈했던 텍스트 파일 리소스 포인터를 완전히 닫아 수거합니다.

        gotoxy(start_x, current_y + 2); // 스토리 드로우가 전면 완료된 최종 바닥 라인선 아래쪽 여백 좌표 공간으로 이동 기동합니다.
        printf("아무 키나 누르면 게임이 시작됩니다..."); _getch(); // 유저가 시각적으로 상황을 온전히 감상 및 인지하고 직접 넘길 수 있도록 입력 홀딩 락을 걸어 정지 대기시킵니다.
    }
    else { // 혹시나 파일이 분실 유실되어 로딩에 실패했을 때 프로그램이 굳어 튕기지 않도록 방어하는 안전 예외 처리입니다.
        gotoxy(start_x, current_y); printf("오류: ta.txt 파일을 찾을 수 없습니다.\n"); Sleep(2000); // 웅장한 에러 자막 메시지를 노출하고 유저가 볼 수 있게 2초간 화면을 강제 동결 유지합니다.
    }

    system("mode con cols=120 lines=30"); // 스토리가 지나가고 본격적인 핵심 인게임 화면 규격 모드로 진입하기 위해 콘솔 해상도 화면 창 규격 크기를 원래의 컴팩트 비율(가로 120, 세로 30)로 재조정 축소 복구합니다.
    system("cls"); // 리사이징 여파로 남은 거대 화면의 인트로 잔상 데이터들을 깔끔하게 샤워 정리하여 스크린을 초기화합니다.

    srand((unsigned int)time(NULL)); // 매 게임 판을 플레이할 때마다 매번 완벽히 예측 불허한 새로운 랜덤 난수들이 생성되도록 현재 시스템 시각 초 단위를 계산해 난수 발생 장치 시드값을 초기화 세팅합니다.

    int hp = 100; // 주인공 플레이어 영웅의 시동 라이프 초기 체력 스탯 수치를 최고치인 100점 점수로 만듭니다.
    int score = 0; // 유저가 무사히 위험 행동을 도피해 버텨 생존해 나간 누적 라운드 턴 생존 횟수 점수 변수입니다. (초기값 0점 점수 부여)
    int key = 0; // 플레이어가 갈림길 선택지에서 고르는 방향 입력 모션 값을 담을 실시간 수령 스토리지입니다.

    while (hp > 0) // 주인공의 라이프 체력이 조금이라도 남아 숨줄기가 붙어 수치가 양수인 살아있는 상태 동안 무한 전개되는 우주 핵심 게임 루프 엔진입니다.
    {
        system("cls"); // 매 라운드 새로운 갈림길 매치업 대결 구도를 깔끔 명징하게 노출하기 위해 기존 턴의 흔적 자막을 완전 청소 밀어버립니다.

        int left_idx = rand() % num_choices; // 구축된 선택지 총수 개수 범위 안에서 임의의 왼쪽 돌발 선택지 번호 인덱스 하나를 완전 무작위로 추첨 선발합니다.
        int right_idx; // 매칭 상대로 등판시킬 오른쪽 돌발 선택지 번호 인덱스 보관고입니다.
        do {
            right_idx = rand() % num_choices; // 오른쪽 갈림길 선택지도 임의로 랜덤 뽑기 추첨을 진행하되 중괄호 조건식을 검증합니다.
        } while (left_idx == right_idx); // 만약 좌측 갈림길과 우측 갈림길에 똑같은 중복 복사판 선택지가 동시 등판 추첨되었다면, 다를 때까지 계속 다시 재추첨을 강제 반복 돌립니다.

        int left_n = 0, right_n = 0; // "n층에서 낙하했다", "n의 속도로 치인다" 같이 서식 가변 변수 기호인 %d 양식을 포함한 조건문일 때 주입해 줄 랜덤 수치 변수 칸입니다.
        if (strstr(choices[left_idx].text, "%d") != NULL) { // 왼쪽 선발된 선택지의 원본 글귀 안에 %d 가변 기호 양식이 실재하는지 문자열 수색 검사를 돌립니다.
            left_n = (rand() % (choices[left_idx].max_damage - choices[left_idx].min_damage + 1)) + choices[left_idx].min_damage; // 실재함이 판독되면 해당 선택지가 미리 규정한 피해 연산 최소 범위와 최대 범위 사잇값을 랜덤 연산해 무작위 배정 변수 숫자를 미리 추출해 확정해 둡니다.
        }
        if (strstr(choices[right_idx].text, "%d") != NULL) { // 오른쪽 선발된 선택지의 원본 글귀 안에도 %d 가변 기호 양식이 박혀있는지 수색합니다.
            right_n = (rand() % (choices[right_idx].max_damage - choices[right_idx].min_damage + 1)) + choices[right_idx].min_damage; // 실재함이 판독되면 우측 전용 사잇값 범위 내의 무작위 배정 변수 숫자를 계산해 확정 확보해 둡니다.
        }

        // 유저 모니터 스크린 최상단 영역에 실시간 플레이어 체력 현황과 생존 누적 턴 점수 스탯 정보를 시각화 표기합니다.
        // [복구] 첫 번째 코드 사양에 따라 닉네임과 간격 오프셋 UI 가독성을 완벽 조율했습니다.
        set_color(FONT_COLOR_GREEN); move_cursor(15, 2); printf("Player : %s", playerName);
        set_color(FONT_COLOR_RED); move_cursor(45, 2); printf("HP : %d", hp); // 중요 생명 수치 지표인 라이프 체력을 박진감 넘치는 붉은색 글씨로 상단 좌측선에 배치 출력합니다.
        set_color(FONT_COLOR_WHITE); move_cursor(75, 2); printf("SCORE : %d", score); // 현재 영광의 생존 라운드 점수 스코어를 단정한 흰색 글씨로 상단 우측선에 대칭 매핑합니다.
        set_color(FONT_COLOR_YELLOW); move_cursor(58, 12); printf("VS"); // 좌우 선택지의 운명의 서바이벌 매치업 데코레이션 문구 단어를 노란색으로 정중앙에 수놓습니다.
        set_color(FONT_COLOR_WHITE); // 채색 렌더링 세팅 속성을 일반 흰색으로 원상 복구 초기화합니다.

        // 고도화된 문자열 자동 결합 포맷팅 모듈 프로세스를 가동합니다.
        char left_msg[256], right_msg[256]; // 무작위로 뽑아낸 가변 숫자가 융합 완료되어 완전히 문장 조립이 끝난 최종 가이드라인 자막 메시지를 구워 담을 깨끗한 메모리 냄비 공간 둘을 개설합니다.
        if (strstr(choices[left_idx].text, "%d") != NULL) sprintf(left_msg, choices[left_idx].text, left_n); // 만약 가변 숫자가 적용되는 양식 문장이라면, %d 자리에 아까 정밀 뽑기해 둔 가변 숫자(left_n)를 완벽히 대입 조립 결합하여 최종 완성 문장으로 변환해 left_msg 용기에 주입합니다.
        else strcpy(left_msg, choices[left_idx].text); // 일반 단순 정적 문장 형태의 선택지라면 원본 텍스트 내용을 토씨 하나 틀리지 않게 있는 그대로 안전하게 복사 카피해 left_msg 용기에 채워 넣습니다.
        if (strstr(choices[right_idx].text, "%d") != NULL) sprintf(right_msg, choices[right_idx].text, right_n); // 우측 선택지 자막 문장도 가변 숫자 대입 조립 양식 매칭 여부를 판독해 동일하게 조립 가공을 대칭 수행합니다.
        else strcpy(right_msg, choices[right_idx].text); // 우측 단순 일반 정적 문장도 있는 그대로 안전하게 복사 카피하여 조립을 끝마칩니다.

        int left_msg_len = calculate_visual_length(left_msg); // 조립 완성된 최종 왼쪽 자막 문장의 실제 시각적 총 문자 가로 길이를 마이크론 단위로 실측 추출합니다.
        int right_msg_len = calculate_visual_length(right_msg); // 조립 완성된 최종 오른쪽 자막 문장의 실제 시각적 총 문자 가로 길이를 대칭 실측 추출합니다.
        int left_x = 28 - (left_msg_len / 2);   if (left_x < 2) left_x = 2; // 왼쪽 아스키 아트 출력 영역 구역의 가로 중심선 기준점(28칸)에 정확히 대칭 중심 무게추가 딱 잡히도록 자막 시작 X좌표를 동적 기하학 연산하고 화면 이탈 마진 버그를 강제 조정 차단 보정합니다.
        int right_x = 90 - (right_msg_len / 2); if (right_x < 62) right_x = 62; // 오른쪽 아스키 아트 출력 영역 구역의 가로 중심선 기준점(90칸)에 중심 무게추가 딱 부합되도록 시작 X좌표를 동적 연산하고 중앙 분리선 침범 이탈 마진 버그를 자동 조정 보정합니다.

        set_color(FONT_COLOR_WHITE); // 드로우 컬러 속성을 흰색으로 확정 세팅합니다.
        for (int i = 0; i < 6; i++) {
            move_cursor(25, 8 + i); printf("%s", choices[left_idx].art[i]); // 왼쪽 지정 배치 영역 도화지 라인 위치로 한 줄씩 하강 이동하며 할당된 선택지의 전용 아스키 아트 그래픽 그림 6줄을 순차 프린팅 구현합니다.
        }
        move_cursor(left_x, 18); printf("%s", left_msg); // 정밀 기하학 연산으로 좌우 대칭 중앙 정렬점이 완벽하게 확보 완료된 바로 그 가로 left_x 좌표 줄 자리에 최종 가공 완료된 왼쪽 선택지 행동 가이드 자막 문장을 마킹 출력합니다.

        for (int i = 0; i < 6; i++) {
            move_cursor(80, 8 + i); printf("%s", choices[right_idx].art[i]); // 오른쪽 지정 배치 영역 도화지 라인 위치로 대칭 이동하며 할당된 우측 선택지 전용 아스키 아트 그래픽 그림 6줄을 순차 프린팅 구현합니다.
        }
        move_cursor(right_x, 18); printf("%s", right_msg); // 좌우 대칭 중앙 정렬점이 완벽히 잡힌 바로 그 가로 right_x 좌표 줄 자리에 최종 가공 완료된 오른쪽 선택지 행동 가이드 자막 문장을 마킹 출력합니다.

        set_color(FONT_COLOR_GREEN); move_cursor(35, 25); printf("방향키(←, →)로 선택하세요. (메뉴로 가기: Backspace)"); // 게임 진행 조작 가이드 안내 텍스트 힌트 문구를 눈에 편안함을 주는 초록색(32) 자막 글씨로 하단 중앙 영역선 줄에 이쁘게 사출 노출합니다.
        set_color(FONT_COLOR_WHITE); // 색상 속성 타깃을 다시 기본 하얀색으로 리셋 복원합니다.

        int has_selected = 0; // 유저가 숙고 끝에 최종 운명의 생존 결단을 내려 버튼을 완벽히 눌렀는지 확인 식별하는 스위치 플래그입니다. (0=미결정 방황 상태, 1=결정 완료 탈출 스위치 온)
        int selected_idx = 0; // 유저가 기어코 최종 터치해 선택 접수한 생존 행동 선택지의 배열 고유 번호 인덱스를 저장할 안전 금고 보관함입니다.

        while (!has_selected) // 결단을 내리기 전까지는 이 안의 키 감지 루프 스레드 영역에 유저를 임시 가두어 키 입력을 연속 스캔 대기합니다.
        {
            key = _getch(); // 키보드 눌림 인터랙티브 모션을 가로채기하여 한 글자 수령 수신합니다.

            if (key == 224) // 키보드 방향키를 딸깍 터치했을 때 시스템 내부에서 유입 신호 맨 앞에 달고 들어오는 확장 특수 키 접두사 트리거인지 검사합니다.
            {
                key = _getch(); // 접두사 필터 조건이 승인되면 즉시 연달아 사출되는 진짜 방향키만의 고유 리얼 식별 주소 코드 번호를 완벽히 캐치 입수합니다.
                if (key == 75) // 실시간 가로챈 고유 주소 코드가 75번인 경우, 즉 '왼쪽 화살표(←) 방향키'를 정확하게 타격 입력한 조건 성립 상황입니다.
                {
                    selected_idx = left_idx; // 내가 고른 운명의 결과 행동 대상을 미리 선발해 좌측에 띄워뒀던 질문지 정보(left_idx)로 최종 확정 고정합니다.
                    has_selected = 1;        // 결단을 확실히 내렸으므로 감금 스캔 루프를 해제하고 빠져나가기 위해 결정 완료 스위치를 참(1)으로 변경합니다.
                }
                else if (key == 77) // 실시간 가로챈 고유 주소 코드가 77번인 경우, 즉 '오른쪽 화살표(→) 방향키'를 정확하게 타격 입력한 조건 성립 상황입니다.
                {
                    selected_idx = right_idx; // 내가 고른 운명의 결과 행동 대상을 우측에 배치해뒀던 질문지 정보(right_idx)로 최종 확정 고정합니다.
                    has_selected = 1;          // 결정 완료 스위치를 참(1)으로 변경해 락을 해제합니다.
                }
            }
            else if (key == 8) // 방향키 조작 모션이 아니라 게임 중단을 원해서 뒤로가기 버튼인 '백스페이스(Backspace, 코드 8) 키'를 긴급하게 누른 비상 트리거 작동 조건식입니다.
            {
                save_console_screen(); // 열정적으로 목숨을 걸고 브레인 생존 대결 게임을 치열하게 벌이던 현재 화면 도화지 그래픽 상태가 단 일말도 다치거나 깨지지 않게 가상 임시 버퍼 배열에 원형 그대로 캡처 백업 보존 처리합니다.

                while (1) // 일시 중지 중단 안내 팝업 메시지 창을 제어하는 로컬 무한 서스펜드 대기 루프창을 개설 가동합니다.
                {
                    set_color(BG_COLOR_BLACK); move_cursor(20, 7); // 배경색 속성을 정적 칠흑 검은색(40)으로 변경 마스킹하고 화면 정중앙 팝업 전용 드로우 시작 줄 레이아웃 좌표로 이동 기동합니다.
                    printf("                                                                                                                                                                                                            \n                                                                                                                                                                                                            \n                                                                                                                                                                                                            \n                                                                                                                                                                                                            \n                                                                                                                                                                                                            \n                                                                                                                                                                                                            \n                                                                                                                                                                                                            \n"); // 기존에 화려하게 그려져 있던 게임 갈림길 질문 그림들을 스크린에서 일시 가려 차단해 버리기 위해 대형 검은색 공백 자막 포탄을 연속 사출하여 팝업 다이얼로그 전용 클린 빈 슬롯 창 면적 공간을 강제 밀어버려 개척합니다.

                    set_color(FONT_COLOR_RED); move_cursor(50, 12); printf("게임을 중지하시겠습니까?"); // 유저에게 긴장감을 주는 강렬한 레드 선명 컬러 자막으로 한가운데 중단 의사를 타진 물어봅니다.
                    move_cursor(40, 15); printf("게임을 계속하려면 t, 중지하려면 r를 누르시오."); // 중단 처리 분기 액션을 유도하기 위한 최종 옵션 제어 안내를 출력 배치합니다.

                    key = _getch(); // 일시정지 분기점 명령 처리를 승인받기 위해 키보드 인터랙션 모션을 긴급 대기 홀딩 수령합니다.

                    if (key == 'r') // 단호하게 리턴 탈출 나가기를 원하여 컴퓨터 자판 'r' 소문자 버튼을 클릭 접수한 비상 중단 조건식 성립 상황입니다.
                    {
                        system("cls"); // 난장판이 된 중단 서스펜드 다이얼로그 팝업창 내벽을 완전 제거 소독 세정 처리합니다.
                        return 0;      // 인게임 플레이 전체 연산 함수인 Gamestart 함수 엔진 작동을 중도 전면 포기 파기하고 0값을 들고 메인 타이틀 메뉴 스위치 대기소로 원상 복귀 리턴합니다.
                    }
                    if (key == 't') // 다시 마음을 다잡고 재개 복귀를 원하여 컴퓨터 자판 't' 소문자 버튼을 클릭 접수한 전개 재개 조건식 성립 상황입니다.
                    {
                        restore_console_screen(); // 팝업창 띄우기 비상 직전 가상 버퍼 배열 메모리에 한 치 오차 없이 완벽 백업 보존해두었던 원래 인게임 갈림길 라운드 화면 그래픽 스크린 상태를 그대로 복사해 와 콘솔창 위에 한순간에 원형 복구 복원 덮어버립니다.
                        break; // 일시정지 서스펜드 전용 로컬 무한 루프창을 완전히 깨부수고 탈출하여 기존의 라운드 생존 결단 대기 홀딩 상태 스레드로 유연하게 복귀 컴백합니다.
                    }
                }
            }
            else if (key == 27) { exit(0); } // 도중에 완전한 게임 완전 포기 클로즈 아웃을 위해 ESC 키를 누르면 시스템 프로세스 자체를 즉각 소멸 강제 오프 종료시킵니다.
        }

        int damage = 0; // 이번 라운드 유저가 내린 선택의 대가로 주인공 생명체에 실질적으로 가해질 최종 정밀 타격 대미지 피해 수치 보관고 변수입니다.

        if (strstr(choices[selected_idx].text, "층에서 떨어졌다") != NULL) // 유저가 최종 선택 완료해 접수한 행동 패턴의 문장 카테고리가 높은 곳에서 추락하는 낙하 물리 낙하 시나리오 상황인지 조건 판독식으로 검증합니다.
        {
            int n = (selected_idx == left_idx) ? left_n : right_n; // 플레이어가 결정을 내린 갈림길 방향(좌/우)에 맞추어 아까 턴 초입에 미리 추첨 고정해 확보해 두었던 가변 변수 정수층수 값 n을 유연하게 매칭 추적해 인출해 가져옵니다.
            damage = n * 4; // 가혹한 물리 낙하 중력 가속도 중폭 법칙을 코드로 산정 적용하여 추락한 건물 층수의 무려 4배수로 타격을 곱 연산해 최종 대미지 점수로 수립 산출합니다.
        }
        else if (strstr(choices[selected_idx].text, "속도") != NULL) // 유저가 최종 선택 완료해 접수한 행동 패턴 카테고리가 고속으로 질주하는 광속 차량에 충돌 타격당하는 교통사고 물리 시나리오 상황인지 조건식으로 검증합니다.
        {
            int n = (selected_idx == left_idx) ? left_n : right_n; // 유저가 고른 갈림길 방향에 알맞은 고유 시동 난수 차량 가속 속도 변수 값 n을 실시간 추적 매칭 인출합니다.
            damage = n * 2; // 막강한 물리 운동 에너지 충격량 증폭 공식을 연산 산정 적용하여 차량 충돌 속도 수치의 무려 2배수로 타격을 곱 연산해 최종 대미지 피해 점수로 수립 산출합니다.
        }
        else { // 특수 물리 증폭 계산식 시나리오 문장들이 아닌, 일반 보편적이고 기상천외한 텍스트 행동 양식들을 골랐을 때 전개되는 기본 범용 피해 밸런싱 연산 블록입니다.
            int min = choices[selected_idx].min_damage; // 선택한 고유 행동 데이터 속에 내장 정의되어 있는 본래 지정 최소 대미지 상수를 추출합니다.
            int max = choices[selected_idx].max_damage; // 선택한 고유 행동 데이터 속에 내장 정의되어 있는 본래 지정 최대 대미지 상수를 추출합니다.
            damage = (rand() % (max - min + 1)) + min; // 정밀 공정 랜덤 난수 가공 산식을 대입 작동시켜 사전 규정된 최소값과 최대값 수치 사잇값 범위 한계선 내부에서 단 하나의 실질 피해 타격 대미지 점수를 무작위로 완전 공정하게 추출 산정해 냅니다.
        }

        hp -= damage; // 주인공 영웅의 소중한 현재 라이프 실질 체력 스탯 수치에서 최종 산정 완료된 피해 타격 대미지 수치를 차감 삭감 적용합니다. (음수 피해인 경우 체력 회복 적용)

        // [복구] 체력이 회복되었을 시 최대 수치 선 한계선인 100점을 절대 초과하지 못하도록 제한하는 오버플로우 한계 가이딩 로직입니다.
        if (hp > 100)
        {
            hp = 100;
        }

        if (strstr(choices[selected_idx].text, "귀여운 길고양이를 쓰다듬는다.") != NULL)
        {
            // PlaySound 함수를 사용하여 비동기(SND_ASYNC)로 wav 파일을 재생합니다.
            // 인게임 중에는 타이틀 BGM이 꺼져 있으므로 사운드 충돌 없이 깔끔하게 재생됩니다.
            PlaySound(TEXT("angry_cat.wav"), NULL, SND_FILENAME | SND_ASYNC);
        }

        if (strstr(choices[selected_idx].text, "수상할 정도로 빨간 버튼을 누른다.") != NULL)
        {
            // PlaySound 함수를 사용하여 비동기(SND_ASYNC)로 wav 파일을 재생합니다.
            // 인게임 중에는 타이틀 BGM이 꺼져 있으므로 사운드 충돌 없이 깔끔하게 재생됩니다.
            PlaySound(TEXT("boom.wav"), NULL, SND_FILENAME | SND_ASYNC);
        }
        score += 1;   // 죽음의 피격 타격을 받고도 기어코 이번 위험 턴 라운드를 무사히(?) 버티고 생존해 넘겼으므로 유저의 생존 업적 누적 턴 스코어 점수를 영광의 1점 증량 누적 획득 처리합니다.

        system("cls"); // 피격 대미지 리포트 결과 화면을 집중도 높고 깔끔하게 연출하기 위해 인게임 질문 화면 그래픽을 싹 지워 초기화합니다.
        move_cursor(50, 12); // 결과 보고 글귀가 화면 정중앙 최적의 시각 존에 위치하도록 정렬 좌표 커서 기동을 수행합니다.

        // [복구] 데미지가 음수(회복)냐 양수(피해)냐에 따라 최종 스크린 출력 멘트를 깔끔하게 자동 분기 처리해 줍니다.
        if (damage < 0)
        {
            printf("선택 완료! HP가 %d 회복되었습니다.", -damage);
        }
        else
        {
            printf("선택 완료! HP가 %d 감소했습니다.", damage); // 이번 라운드 결정으로 주인공의 심장에 입힌 누적 타격 피해 대미지 리포트 수치를 유저에게 노출 가인식시킵니다.
        }

        Sleep(2000); // 유저가 피격 충격 수치 결과를 눈으로 똑똑히 읽고 정신을 가다듬을 수 있도록 2초(2000ms) 동안 화면 작동을 강제 일시 동결 연출합니다.

        // 입력 버퍼 비우기 (연타로 밀려 들어온 버그성 잔여 키값을 지워 다음 라운드 오작동을 완벽히 방어합니다)
        while (_kbhit())
        {
            _getch();
        }
    } // 핵심 생존 게임 연산 while 무한 휠 루프의 마감점입니다. 주인공의 라이프 체력이 영(0) 이하 숫자가 찍혀 완전히 사망할 때까지 이 회전 바퀴가 계속 돌며 라운드를 생성 주행합니다.

    // 주인공 영웅의 라이프 체력이 기어코 완전 바닥나 영(0) 이하 사망 판정을 터치하고 코어 생존 루프 바퀴를 슬프게 탈출해 내려왔을 때 집행되는 비극적인 최종 게임 오버 게임 패배 드로우 렌더 연출부입니다.
    system("cls"); // 아비규환이 된 피격 연출 흔적을 스크린에서 완전 비우기 탈색 청소 처리합니다.
    set_color(FONT_COLOR_RED); move_cursor(54, 12); printf("GAME OVER"); // 전장의 공포감과 비극을 선사하는 강렬하고 묵직한 붉은색(31) 컬러 폰트로 게임오버 종막 대문 간판 자막을 정중앙에 각인 선언합니다.

    set_color(FONT_COLOR_WHITE); move_cursor(50, 14); printf("최종 버틴 점수 : %d", score); // 그동안 유저가 뇌를 풀가동해 핏빛 사투를 벌이며 위대하게 버텨낸 영광의 최종 누적 생존 라운드 스코어 스탯 점수 기록을 단정한 흰색 자막으로 명시 표기합니다.
    move_cursor(43, 18); printf("Backspace를 누르면 메뉴로 돌아갑니다."); // 종막을 확인한 유저가 다시 홈 화면으로 돌아가 재도전을 설계할 수 있도록 리턴 숏컷 조작법을 정중앙선 좌표에 가이드라인 안내합니다.

    while (1) // 유저가 결과를 겸허히 수용하고 인증 도장을 찍는 의미로 백스페이스 확인 버튼을 누를 때까지 화면을 고정 동결해 가두는 마감 무한 대기 루프창입니다.
    {
        key = _getch(); // 키보드 확인 타격 인터랙션 무브먼트를 홀딩 가로채기 수령합니다.
        if (key == 8) // 입력 확인 접수한 자판 고유 주소가 뒤로가기 버튼인 '백스페이스 키' 임이 정확히 매칭 검증 판독된 수용 상황입니다.
        {
            break; // 화면 고정용 마감 무한 대기 루프 쇠사슬을 완전히 깨부수고 탈출 스위치를 올립니다.
        }
    }

    system("cls"); // 눈물겨운 패배 흔적이 고스란히 남아있던 최종 게임오버 렌더 전역 화면창 내벽을 완전 클리어 청소 처리합니다.
    return 0; // 메인 main 함수 코어 허브 센터 스위치의 대기소 상태로 당당하고 안전하게 복귀 귀환 리턴 탈출합니다.
}

// 4번 메뉴 '게임 종료'를 셀렉트하거나 프로그램 이탈 단계를 밟을 때 스크린에 전개되는 작별 크레딧 텍스트 스크롤링 입체 그래픽 애니메이션 연출 함수입니다.
int Gameover(void)
{
    // 1. 엔딩 크레딧에 출력할 텍스트 라인들을 배열로 구성합니다.
    const char* credits[] = {
        "======================================",
        "              GAME OVER               ",
        "======================================",
        "",
        "       플레이 해주셔서 감사합니다!     ",
        "",
        "--------------------------------------",
        "          [ PLAYER SCORE ]            ",
        "",
        "          플레이어: %s", // playerName이 매핑될 자리
        "          최종 점수: 100 점",
        "--------------------------------------",
        "",
        "           [ DEVELOPERS ]             ",
        "",
        "        team 01 (ZERONE) 팀원들       ",
        "",
        "        마준서 (202617166) : 총괄     ",
        "        백종화 (202617139) : 코드     ",
        "        이인욱 (202619389) : 코드     ",
        "        이준현 (202619549) : 디자인   ",
        "",
        "======================================",
        "      잠시 후 게임이 종료됩니다.      ",
        "====================================--"
    };

    int num_lines = sizeof(credits) / sizeof(credits[0]);

    // 텍스트가 화면 아래(SCREEN_HEIGHT)에서 나타나서 화면 위(-num_lines)로 완전히 사라질 때까지 루프
    for (int i = SCREEN_HEIGHT; i >= -num_lines; i--)
    {
        // ANSI escape code로 화면을 깔끔하게 지우고 홈 위치로 복귀
        printf("\x1b[2J\x1b[H");
        set_color(FONT_COLOR_WHITE);

        // 현재 프레임에서 화면에 보여야 하는 줄들만 계산해서 출력
        for (int j = 0; j < num_lines; j++)
        {
            int line_y = i + j; // 각 줄이 그려질 세로 Y 좌표 계산

            // 콘솔 화면 범위(0 ~ SCREEN_HEIGHT-1) 내에 있는 줄만 화면에 그립니다.
            if (line_y >= 0 && line_y < SCREEN_HEIGHT)
            {
                // 화면 가로 120칸 기준, 중앙에 정렬하기 위해 가로 시작 좌표 계산
                int visual_len = calculate_visual_length(credits[j]);
                int line_x = (SCREEN_WIDTH - visual_len) / 2;
                if (line_x < 1) line_x = 1;

                move_cursor(line_x, line_y);

                // 플레이어 이름 텍스트 줄(9번째 줄 인덱스) 처리
                if (j == 9)
                {
                    char formatted_line[256];
                    sprintf(formatted_line, credits[j], playerName);
                    printf("%s", formatted_line);
                }
                else
                {
                    printf("%s", credits[j]);
                }
            }
        }

        // 커서를 화면 오른쪽 아래 구석으로 대피시켜 깜빡임을 방지
        move_cursor(SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);

        // 스크롤 속도 조절 (150ms 단위로 한 줄씩 이동)
        Sleep(150);
    }

    // [수정된 부분] 
    // 크레딧 스크롤이 화면 위로 모두 올라가서 완전히 사라진 후, 2초(2000밀리초) 동안 대기합니다.
    Sleep(2000);

    // 화면을 최종적으로 깨끗하게 비워줍니다.
    system("cls");

    // 프로그램을 강제로 완전 종료(정상 종료 코드 0) 시킵니다.
    exit(0);

    return 0; // exit(0)에서 프로그램이 끝나므로 실행되지는 않지만, 컴파일러 경고 방지용으로 유지합니다.
}

// 매개변수로 ANSI 컬러 고유 정수 번호 코드값 하나를 수령 수신하여, 그 즉시 콘솔 터미널에 뿌려지는 텍스트 폰트/배경 채색 상태 설정을 실시간 변경 전환해 주는 유틸리티 인터페이스 함수입니다.
void set_color(int code)
{
    printf("\x1b[%dm", code); // 콘솔 터미널 출력 가상 스레드 파이프라인 줄에 ANSI 에스케이프 컬러 마스킹 코드를 즉각 정밀 사출 사이어 발사하여 이후의 채색 렌더링 환경 설정을 변환 전환시킵니다.
}

// 매개변수로 타깃 X(가로 열 칸 번호), Y(세로 행 줄 번호) 정수 좌표값을 안전하게 수령하여, 콘솔창 내에 문자 출력 시작점을 잡아주는 깜빡이는 하얀색 텍스트 입력 커서의 기동 위치를 ANSI 표준 코드를 통해 해당 스크린 좌표 정확한 스팟 자리로 즉시 순간이동 날려보내는 고속 숏컷 기동 유틸리티 함수입니다.
int move_cursor(int x, int y)
{
    printf("\033[%d;%dH", y, x); // 가상 터미널 환경에 매개변수로 넘어온 y줄 행, x칸 열 정보 포맷 양식을 정확히 일치 주입 빌드한 가서 커서 이동 하드웨어 제어 시그널 신호 탄환을 즉시 사출 발사하여 커서 기동 기동을 완료합니다.
    return 0; // 함수 임무 안전 완수를 뜻하는 정수 리턴값 0을 반환하며 깔끔히 기능을 마감합니다.
}

// 유저가 게임 도중 ESC를 누르거나 프로그램 가동 엔진이 정상/비정상 경로를 통해 운영체제 제어권 반환 단계로 빠져나가 탈출 클로즈 종료될 때 사후 작동하는 최종 청소 예외 안전 백업 복원 클린업 함수입니다.
void cleanup_console(void) {
    printf("\x1b[?1049l\x1b[0m"); // 게임 기동 초기 main 진입 스레드 초입 단계에서 강제로 임시 전개 할당해 기동시켰던 가상 대체 화면 버퍼 모드(Alternative Screen Buffer) 인스턴스를 완벽하게 탈출 클로즈 폐쇄(`?1049l`)하고, 색상 매핑 설정을 순수 원초적 시스템 기본값(`0m`)으로 깔끔하게 리셋 원상복구 복원해 주어 유저가 게임을 켜기 전 평화롭게 쓰고 있던 본래의 윈도우 도스 명령 프롬프트 터미널 창 화면 상태를 일말의 훼손 깨짐 잔상 흔적 전혀 없이 완벽하게 보호해 줍니다.
}

// C 프로그래밍 언어로 설계된 시스템이 빌드 및 컴파일 완료되어 컴퓨터 윈도우 OS 운영체제 환경 위에 인스턴스로 로드될 때 가장 최초로 진입하여 메인 동력 구동 스레드 휠을 돌리는 심장부 메인 진입 엔트리 포인트(Entry Point) 함수입니다.
int main(void)
{
    printf("\x1b[2J\x1b[?1049h"); // 프로그램 시동과 동시에 가로 120 세로 30 터미널 스크린 내벽을 완전히 전면 소독 클리어(`2J`)하는 동시에, 게임용 전용 독립 가상 공간 대체 버퍼 스크린 모드 기동 신호(`?1049h`)를 시스템에 전격 강제 주입 주하 개시합니다.
    SetConsoleOutputCP(CP_UTF8);  // 마이크로소프트 윈도우 OS 콘솔 환경 터미널 시스템이 유니코드 UTF-8 코드페이지 문자셋을 강제 표준 규격으로 고정 락을 걸어 복잡한 한글 자막 문장이나 팀원 아스키아트 파일 내부 한글 특수문자 그래픽 데이터가 절대 일말도 허투루 깨지거나 뭉개지지 않도록 강제 인코딩 수입 주입 조치를 취합니다.
    SetConsoleCP(CP_UTF8);        // 키보드 입력 인코딩을 UTF-8로 설정
    atexit(cleanup_console);      // 유저가 어떠한 돌발 돌발 비상 경로를 거쳐 프로그램 X 종료 창을 누르거나 강제 탈출 아웃 종료되더라도, 메모리 프로세스가 완전히 공중 분해되어 나가떨어지기 직전 사후 안전 조치로 위의 `cleanup_console` 함수가 시스템 내부에서 강제로 사후 자동 강제 트리거 호출 작동하도록 운영체제 사후 예약을 완벽하게 조치 선행 등록해 둡니다.

    int gameStatus = 0; // 프로그램 내부가 메인 타이틀 상태인지, 인게임 생존 상태인지, 설명서 보기 상태인지 전체 컴포넌트의 진행 화면 분기 상황 상태를 중앙 관리 제어하는 핵심 네비게이션 제어 코드 번호 변수입니다. (초기 구동값 0번 = 메인 타이틀 메뉴 화면 상태 부여)
    int isBgmPlaying = 0; // BGM이 중복되어 여러 번 틀어지는 스택 버그 현상을 방어하기 위한 재생 확인 스위치 플래그입니다.

    ShowLogo(); // 프로그램 메인 엔진 가동 휠이 돌기 직전, 단 한 번 웅장하고 아름다운 파란색 박스 제로원 제작사 인트로 픽셀 로고와 전체화면 전환 권고 안내 가이딩 멘트 화면 모듈을 스크린에 전격 노출 집행합니다.

    // [복구] 대기 박스가  끝나면 닉네임을 타자하여 입력받을 사용자 닉네임 입력 전용 UI 프레임을 로딩합니다.
    set_color(FONT_COLOR_WHITE);
    move_cursor(80, 26);
    printf("플레이어의 닉네임을 입력하세요: ");
    set_color(FONT_COLOR_YELLOW); // 입력 필드는 가시성 확보를 위해 노란색 폰트로 마크업합니다.
    scanf("%49s", playerName);    // 가상 버퍼 오버플로우 침범 버그 예방 수식(%49s) 탑재 완료

    // 키보드 엔터 찌꺼기 텍스트 청소 파이프라인 가동 (이후 메뉴 _getch 튐 오작동 방어)
    while (getchar() != '\n');
    system("cls");

    while (isRunning) // 프로그램 가동 전역 스위치 플래그 변수 수치가 참(1)을 굳건히 유지 유지하며 살아 숨 쉬는 동안 멈춤 없이 24시간 실시간 무한 고속 회전 구동되는 프로그램 전반 핵심 메인 기어 가동 가동 휠 루프입니다.
    {
        switch (gameStatus) // 현재 중앙 관리 코드 번호 변수에 매핑 입력되어 있는 수치 값 상태에 매칭되는 타깃 컴포넌트 화면으로 고속 정밀 스위칭 분기 라우팅을 집행합니다.
        {
        case 0: // 진행 화면 분기 상태 코드 번호가 0번(메인 타이틀) 자리에 고정 안착되어 머물러 있는 상황 제어 모듈입니다.

            // 닉네임 입력을 마친 후, 메인 타이틀 화면이 첫 호출될 때 딱 한 번만 음원을 무한 반복(SND_LOOP)으로 배경 재생합니다.
            if (!isBgmPlaying) {
                PlaySound(TEXT("il-vento-doro.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
                isBgmPlaying = 1; // 음악이 구동 중이므로 다음 새로고침 루프에서는 재생 명령을 타지 않도록 플래그를 고정합니다.
            }

            printf("\x1b[H"); // 타이틀을 실시간으로 새로 그리며 키보드 방향키 조작 모션을 실시간 인터셉트할 때 화면이 위아래로 찢어지거나 흔들리는 깜빡임 현상을 정밀 방어하기 위해 화면 전체를 system("cls")로 무식하게 지우지 않고, 커서 출력 위치만 매번 맨 왼쪽 맨 위 끝자리 천장 홈(Home) 좌표 위치로 실시간 리턴 회항(`H`) 고정시킨 뒤 덮어쓰기 그리기를 선행 명령합니다.
            gameStatus = RenderTitle(); // 메인 타이틀 메뉴 화면 렌더러 함수를 호출 기동해 스크린을 교체 드로우하고, 사용자가 신중히 숙고해 메뉴를 골라 엔터 도장을 찍어 반환해 올리는 고유 다음 분기 화면 번호 결과 수치 값(1~4)을 실시간으로 전달 수령받아 `gameStatus` 변수 금고에 실시간 덮어써 대입 대입하며 화면 전환을 이끌어 냅니다.
            break; // 해당 화면 상태 스캔 단계를 안전하게 마감하고 본 전역 기어 루프 바퀴를 한 바퀴 다음 회전으로 회전시킵니다.

        case 1: // 만든 사람 및 팀 소개
            gameStatus = People();
            break;

        case 2: // 사용 설명서
            gameStatus = Manual();
            break;

        case 3: // 진짜 게임 시작
            // [BGM 수정] 사용자가 3번 메뉴(게임 시작)를 누르고 진입하는 순간 메뉴 배경음악을 완전히 멈춥니다.
            PlaySound(NULL, NULL, 0);
            isBgmPlaying = 0; // 추후 게임오버 후 메인 메뉴로 복귀했을 때 BGM이 다시 살아나도록 연동 장치 해제

            gameStatus = Gamestart();
            break;

        case 4: // 게임 종료 최종 엔딩
            gameStatus = Gameover();
            break;
        }
    } // 메인 core 휠 while 루프의 종착 하단 마감점 줄입니다.

    system("cls"); // 콘솔 터미널 도화지 내벽에 묻어 잔류하는 마지막 메뉴판 잔상 글자 자국 흔적들을 완벽 타파 청소 세정 처리합니다.
    move_cursor(0, 25); // 프로그램이 완전히 셧다운 닫히기 직전 윈도우 운영체제 기본 명령줄 포커스 텍스트 커서가 터미널 맨 아래 구석 빈칸 안전지대 자리(0, 25 좌표)에 단정하게 안착 대기할 수 있도록 커서 위치를 하단 마진선 밖으로 격리 보냅니다.

    return 0;
}