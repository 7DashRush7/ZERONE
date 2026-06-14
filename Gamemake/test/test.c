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
void set_color(int code);
int move_cursor(int x, int y);
void gotoxy(int x, int y);
void ShowLogo(void);
int RenderTitle(void);
void print_member_page(const char* filename, const char* description);
void draw_final_screen(void);
int People(void);
int Manual(void);
int Gamestart(void);
int Gameover(void);
void cleanup_console(void);
int calculate_visual_length(const char* str);
void get_content_stats(const char* str, int* visual_prefix, int* visual_content);
void print_ascii_file(const char* filename, int start_x, int start_y);

// 게임 전체에서 공유하며 사용할 전역 변수들입니다.
int menu = 1;
int isRunning = 1;
char playerName[50] = "Player";
int finalScore = 0;

// 게임 중 매 라운드마다 무작위로 나올 '선택지 정보'를 하나로 묶어둔 구조체 양식입니다.
typedef struct
{
    const char* art[6]; // 선택지 위에 그려질 아스키아트 그림 데이터입니다. (최대 6줄까지 저장)
    const char* text;   // 선택지에 대한 설명 글자입니다. 
    int min_damage;     // 이 행동을 골랐을 때 최소한으로 받는 피해 체력량입니다. 
    int max_damage;     // 이 행동을 골랐을 때 최대한으로 받는 피해 체력량입니다. 
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
    { {"  ::    ::  ", ": ::::::::::","..::::::::::"," :::::::::: ","   ::::::   ","     ::     "}, "앞에 커플이 지나가는 걸 본다.", 4, 10},
    { {
        "       ***++++++++#*++++++++** ",
        "       #**++++++++**++++++++** ",
        " #***********************************",
        " ***********************************#",
        " *****%@@@@%*************#@@@@@%**** ",
        " #***#@#   @@************@@   #@#***#",}, "%d의 속도로 달리는 차에 치인다.", 20, 50},
    { { "  _.-._  ", " / * * \\\\ ", " |  * | ", " '-._.-' ", "   | |   ", "   |_|   " }, "형광빛이 나는 정체불명의 버섯을 씹어먹는다.", 5, 15 },
    { { "   ___   ", "  (o_o)  ", " /|   |\\\\ ", "  |___|  ", "   ^ ^   ", "         " }, "근육질 비둘기가 날아와 내 빵을 뺏어간다.", 3, 7 },
    { { "   \\\\_    ", "   /     ", "  /___   ", "    /    ", "   /     ", "  * " }, "마른하늘에 떨어지는 벼락을 피하지 못했다.", 20, 40 },
    { { "  ___    ", " |   |   ", " |SSR|   ", " |___|   ", "         ", "         " }, "주운 스마트폰으로 가챠를 돌렸으나 대폭사했다.", 2, 8 },
    { { "  _^_    ", " /_ _\\\\   ", "  | |    ", "  | |    ", "         ", "         " }, "지나가던 외계인과 눈이 마주쳐 기가 빨린다.", 8, 15 },
    { { "         ", "  ____   ", " /o  o\\\\  ", " \\\\____/  ", "         ", "         " }, "발밑에 있던 슬라임을 밟고 화려하게 미끄러졌다.", 1, 5 },
    { { "         ", "         ", "    _    ", "  _| |_  ", " |_____| ", "  !!!    " }, "문지방에 새끼발가락을 무자비하게 찧었다.", 15, 25 },
    { { "   ( (   ", "    ) )  ", "  ____   ", " |    |  ", " |____|  ", "         " }, "새로 산 흰 옷에 뜨거운 아메리카노를 쏟는다.", 5, 10 },
    { { " _______ ", " | >_  | ", " |ERROR| ", " |_____| ", "  =====  ", "         " }, "C++ 컴파일 중 원인을 알 수 없는 오류가 뿜어져 나온다.", 8, 15 },
    { { "  ___    ", " |   |   ", " |___|   ", "   \\\\    ", " (x_x)   ", "         " }, "누워서 스마트폰을 보다 얼굴에 정통으로 떨어뜨렸다.", 3, 8 },
    { { "  ____   ", " |    |  ", " |____|  ", "  O  O   ", "   ==3   ", "         " }, "눈앞에서 타야 할 버스가 문을 닫고 무심하게 출발해버렸다.", 2, 6 },
    // === [ 회복 이벤트 ] ===
    { { "  ___    ", " |   |   ", " |ZRO|   ", " |___|   ", "         ", "         " }, "자판기에서 뽑은 제로 슈거 콜라를 시원하게 들이킨다.", -10, -5 },
    { { "  ___    ", " | + |   ", " |___|   ", "         ", "         ", "         " }, "버려진 구급상자에서 뽀로로 반창고를 찾아 붙인다.", -12, -7 },
    { { "  ++++   ", "  +HP+  ", "  ++++   ", "         ", "         ", "         " }, "약국에서 진통제를 복용한다.", -15, -10 },
    { { "  [###]  ", "  |   |  ", "  |___|  ", "         ", "         ", "         " }, "편의점에서 이온음료를 마신다.", -8, -3 },
    { { "  Zzz..  ", "  (-_-)  ", "  /| |\\  ", "         ", "         ", "         " }, "잠깐 앉아서 휴식을 취한다.", -8, -4 },

};

int num_choices = sizeof(choices) / sizeof(Choice);
CHAR_INFO savedScreen[SCREEN_WIDTH * SCREEN_HEIGHT];

// === [ 함수 ] 텍스트 파일(아스키아트) 불러와서 출력 ===
void print_ascii_file(const char* filename, int start_x, int start_y) {
    FILE* file = fopen(filename, "r");
    if (file != NULL) {
        char buffer[1024];
        int y = start_y;
        while (fgets(buffer, sizeof(buffer), file)) {
            buffer[strcspn(buffer, "\r\n")] = 0; // 줄바꿈 문자 제거
            move_cursor(start_x, y++);
            printf("%s", buffer);
        }
        fclose(file);
    }
    else {
        move_cursor(start_x, start_y);
        printf("[오류] '%s' 파일을 불러올 수 없습니다.\n", filename);
    }
}

void save_console_screen()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD bufferSize = { SCREEN_WIDTH, SCREEN_HEIGHT };
    COORD bufferCoord = { 0, 0 };
    SMALL_RECT readRegion = { 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1 };
    ReadConsoleOutput(hConsole, savedScreen, bufferSize, bufferCoord, &readRegion);
}

void restore_console_screen()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD bufferSize = { SCREEN_WIDTH, SCREEN_HEIGHT };
    COORD bufferCoord = { 0, 0 };
    SMALL_RECT writeRegion = { 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1 };
    WriteConsoleOutput(hConsole, savedScreen, bufferSize, bufferCoord, &writeRegion);
}

void gotoxy(int x, int y) {
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

int calculate_visual_length(const char* str) {
    int len = 0;
    for (int i = 0; str[i] != '\0'; ) {
        if ((str[i] & 0x80) == 0) { i += 1; len += 1; }
        else if ((str[i] & 0xE0) == 0xC0) { i += 2; len += 2; }
        else if ((str[i] & 0xF0) == 0xE0) { i += 3; len += 2; }
        else { i += 4; len += 2; }
    }
    return len;
}

void get_content_stats(const char* str, int* visual_prefix, int* visual_content) {
    int target_start = -1, target_end = -1;
    int i = 0, current_visual = 0, start_visual = 0;

    while (str[i] != '\0') {
        int is_blank = 0, char_len = 1, vis_len = 1;
        if (str[i] == ' ') { is_blank = 1; }
        else if ((unsigned char)str[i] == 0xE2 && (unsigned char)str[i + 1] == 0xA0 && (unsigned char)str[i + 2] == 0x80) {
            is_blank = 1; char_len = 3; vis_len = 2;
        }
        else {
            if ((unsigned char)str[i] <= 0x7F) { char_len = 1; vis_len = 1; }
            else if ((unsigned char)str[i] >= 0xC0 && (unsigned char)str[i] <= 0xDF) { char_len = 2; vis_len = 2; }
            else if ((unsigned char)str[i] >= 0xE0 && (unsigned char)str[i] <= 0xEF) { char_len = 3; vis_len = 2; }
            else { char_len = 4; vis_len = 2; }
        }
        if (!is_blank) {
            if (target_start == -1) { target_start = i; start_visual = current_visual; }
            target_end = i + char_len - 1;
        }
        current_visual += vis_len; i += char_len;
    }
    if (target_start == -1) { *visual_prefix = 0; *visual_content = 0; return; }

    int content_vis = 0; i = target_start;
    while (i <= target_end && str[i] != '\0') {
        int char_len = 1, vis_len = 1;
        if ((unsigned char)str[i] <= 0x7F) { char_len = 1; vis_len = 1; }
        else if ((unsigned char)str[i] >= 0xC0 && (unsigned char)str[i] <= 0xDF) { char_len = 2; vis_len = 2; }
        else if ((unsigned char)str[i] >= 0xE0 && (unsigned char)str[i] <= 0xEF) { char_len = 3; vis_len = 2; }
        else { char_len = 4; vis_len = 2; }
        content_vis += vis_len; i += char_len;
    }
    *visual_prefix = start_visual; *visual_content = content_vis;
}

void ShowLogo(void)
{
    printf("\x1b[2J");

    int box_width = 60;
    int box_height = 24;
    int box_start_x = 37;
    int box_start_y = 5;

    for (int i = 0; i < box_height; i++)
    {
        printf("\x1b[%d;%dH", box_start_y + i, box_start_x);
        printf("\x1b[44m");
        for (int j = 0; j < box_width; j++)
        {
            printf(" ");
        }
        printf("\x1b[0m");
    }

    const char* logo[] = {
       "                                                    ",
       "  WWWWWW                                            ",
       " WWW   WWWBB                                        ",
       " WWW   WWWBB                                        ",
       " WWW   WWWBB  XXXXXX  XXXXX  XXXXX                  ",
       "  WWWWWWWBB       X   X   X  X                      ",
       "    BBBBB        X    XXXXX  XXXXX                  ",
       "                X     X   X  X                      ",
       "              XXXXXX  X   X  XXXXX                  ",
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

    int logo_height = 21;
    int logo_width = 52;

    int logo_start_x = box_start_x + (box_width - logo_width) / 2;
    int logo_start_y = box_start_y + (box_height - logo_height) / 2;

    for (int i = 0; i < logo_height; i++)
    {
        printf("\x1b[%d;%dH", logo_start_y + i, logo_start_x);
        for (int j = 0; j < logo_width; j++)
        {
            if (logo[i][j] == 'W')
            {
                printf("\x1b[47m ");
            }
            else if (logo[i][j] == 'B' || logo[i][j] == 'X')
            {
                printf("\x1b[40m ");
            }
            else
            {
                printf("\x1b[44m ");
            }
        }
        printf("\x1b[0m");
    }

    printf("\x1b[%d;1H\n", box_start_y + box_height + 1);

    move_cursor(logo_start_x - 5, logo_start_y + logo_height + 2);
    set_color(FONT_COLOR_WHITE);
    printf(" F11키를 눌러 전체화면으로 바꾸고 Enter키를 누르면 시작합니다.");

    int key;
    do {
        key = _getch();
    } while (key != '\r');

    system("cls");
}

int RenderTitle(void)
{
    int console_width = 210;
    int console_height = 60;
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
    {
        console_width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        console_height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

        if (console_width < 180 || console_height < 50)
        {
            system("mode con cols=210 lines=60");
            if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
            {
                console_width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
                console_height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
            }
        }
    }

    printf("\x1b[2J\x1b[H");

    static char title_lines[40][256];
    static int title_line_count = 0;
    static int max_content_width = 0;
    static int title_loaded = 0;

    if (!title_loaded) {
        FILE* f = fopen("dlrjfwnrsp.txt", "r");
        if (f != NULL) {
            char temp[1024];
            while (fgets(temp, sizeof(temp), f) && title_line_count < 40) {
                temp[strcspn(temp, "\r\n")] = 0;
                int prefix = 0, content_w = 0;
                get_content_stats(temp, &prefix, &content_w);
                if (content_w > 0) {
                    if (content_w > max_content_width) max_content_width = content_w;
                    strcpy(title_lines[title_line_count++], temp);
                }
            }
            fclose(f);
        }
        title_loaded = 1;
    }

    set_color(BG_COLOR_BLACK);
    set_color(FONT_COLOR_GREEN);
    move_cursor(10, 3);
    printf("환영합니다, [%s] 님!", playerName);

    set_color(FONT_COLOR_WHITE);
    int title_start_y = 2;
    for (int i = 0; i < title_line_count; i++) {
        int prefix = 0, content_w = 0;
        get_content_stats(title_lines[i], &prefix, &content_w);
        int target_x = (SCREEN_WIDTH - max_content_width) / 2 - prefix;
        if (target_x < 1) target_x = 1;
        move_cursor(target_x + 65, title_start_y + i + 14);
        printf("%s", title_lines[i]);
    }

    int base_y = title_start_y + title_line_count + 2;
    int base_x = (SCREEN_WIDTH - max_content_width) / 2 + 6;

    move_cursor(base_x + 75, base_y + 14);
    if (menu == 1) { set_color(FONT_COLOR_YELLOW); printf("▶ 1. 만든 사람 및 팀 소개"); }
    else { set_color(FONT_COLOR_WHITE);            printf("   1. 만든 사람 및 팀 소개"); }

    move_cursor(base_x + 75, base_y + 16);
    if (menu == 2) { set_color(FONT_COLOR_YELLOW); printf("▶ 2. 설명서"); }
    else { set_color(FONT_COLOR_WHITE);            printf("   2. 설명서"); }

    move_cursor(base_x + 75, base_y + 18);
    if (menu == 3) { set_color(FONT_COLOR_YELLOW); printf("▶ 3. 게임 시작"); }
    else { set_color(FONT_COLOR_WHITE);            printf("   3. 게임 시작"); }

    move_cursor(base_x + 75, base_y + 20);
    if (menu == 4) { set_color(FONT_COLOR_YELLOW); printf("▶ 4. 게임 종료"); }
    else { set_color(FONT_COLOR_WHITE);            printf("   4. 게임 종료"); }

    set_color(FONT_COLOR_YELLOW);
    int guide_x = console_width - 20;
    int guide_y = console_height - 3;
    if (guide_x < 1) guide_x = 1;
    if (guide_y < 1) guide_y = 1;
    move_cursor(guide_x, guide_y); printf("↑: 위로 이동");
    move_cursor(guide_x, guide_y + 1); printf("↓: 밑으로 이동");
    move_cursor(guide_x, guide_y + 2); printf("Enter : 선택");
    move_cursor(guide_x, guide_y + 3); printf("ESC : 게임 종료");

    move_cursor(1, 1);

    int a = _getch();

    if (a == 0 || a == 224)
    {
        a = _getch();
    }

    switch (a)
    {
    case 72:
        if (menu > 1) menu--;
        break;
    case 80:
        if (menu < 4) menu++;
        break;
    case 27:
        isRunning = 0;
        break;
    case 13:
        if (menu >= 1 && menu <= 4) return menu;
        break;
    }

    return 0;
}

void print_member_page(const char* filename, const char* description) {
    printf("\x1b[2J\x1b[H");
    set_color(FONT_COLOR_WHITE);

    FILE* file = fopen(filename, "r");
    int y = 2;
    if (file != NULL) {
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), file) && y <= 36) {
            buffer[strcspn(buffer, "\r\n")] = 0;
            int visual_len = calculate_visual_length(buffer);
            int x = (SCREEN_WIDTH - visual_len) / 2;
            if (x < 1) x = 1;
            move_cursor(x + 45, y++);
            printf("%s", buffer);
        }
        fclose(file);
    }
    else {
        move_cursor(10, 10);
        printf("[오류] '%s' 파일을 찾을 수 없습니다.\n", filename);
    }

    move_cursor((SCREEN_WIDTH + 38) / 2, 38);
    printf("==================================================");
    move_cursor((SCREEN_WIDTH - calculate_visual_length(description)) / 1, 39);
    printf("%s", description);
    move_cursor((SCREEN_WIDTH + 38) / 2, 40);
    printf("==================================================");

    const char* nav_str = "[ <- 이전 페이지 ]        [ Backspace 메뉴로 돌아가기 ]        [ 다음 페이지 -> ]";
    move_cursor((SCREEN_WIDTH + calculate_visual_length(nav_str)) / 3, 42);
    printf("%s", nav_str);
}

void draw_final_screen(void) {
    printf("\x1b[2J\x1b[H");

    int box_width = 60;
    int box_height = 30;
    int box_start_x = (SCREEN_WIDTH - box_width) / 1;
    int box_start_y = 1;

    for (int i = 0; i < box_height; i++) {
        printf("\x1b[%d;%dH\x1b[44m", box_start_y + i + 6, box_start_x + 15);
        for (int j = 0; j < box_width; j++) { printf(" "); }
        printf("\x1b[0m");
    }

    const char* title_text[] = {
        "XXXXX  XXXXX  XXXX   XXXXX  X   X  XXXXX",
        "   X   X      X   X  X   X  XX  X  X    ",
        "  X    XXXX   XXXX   X   X  X X X  XXXX ",
        " X     X      X  X   X   X  X  XX  X    ",
        "XXXXX  XXXXX  X   X  XXXXX  X   X  XXXXX"
    };

    int title_width = (int)strlen(title_text[0]);
    int title_start_x = box_start_x + 16 + (box_width - title_width) / 2;
    int title_start_y = box_start_y + 15;

    for (int i = 0; i < 5; i++) {
        printf("\x1b[%d;%dH", title_start_y + i, title_start_x);
        for (int j = 0; j < title_width; j++) {
            if (title_text[i][j] == 'X') { printf("\x1b[40m "); }
            else { printf("\x1b[44m "); }
        }
        printf("\x1b[0m");
    }

    const char* team_text[] = {
        " team 01 (ZERONE) 팀원들",
        "마준서(202617166) : 총괄",
        "백종화(202617139) : 코드",
        "이인욱(202619389) : 코드",
        "이준현(202619549) : 디자인"
    };

    int team_start_y = title_start_y + 7;
    for (int i = 0; i < 5; i++) {
        int text_vis_len = calculate_visual_length(team_text[i]);
        int team_line_x = box_start_x + 15 + (box_width - text_vis_len) / 2;
        printf("\x1b[%d;%dH\x1b[30m\x1b[44m%s\x1b[0m", team_start_y + i, team_line_x, team_text[i]);
    }

    const char* final_nav = "[ <- 이전 페이지 ]        [ Backspace 메뉴로 돌아가기 ]        [ ESC 종료 ]";
    int final_nav_x = (SCREEN_WIDTH - calculate_visual_length(final_nav)) / 2;
    move_cursor(final_nav_x + 45, box_start_y + 9 + box_height + 2);
    set_color(FONT_COLOR_WHITE);
    printf("%s", final_nav);
}

int People(void)
{
    const char* filenames[] = { "1.txt", "2.txt", "3.txt", "4.txt" };
    const char* descriptions[] = {
        "마준서(202617166) : 총괄   ",
        "백종화(202617139) : 코드   ",
        "이인욱(202619389) : 코드   ",
        "이준현(202619549) : 디자인  "
    };

    int current_page = 0;
    int total_pages = 5;

    while (1)
    {
        if (current_page < 4) {
            print_member_page(filenames[current_page], descriptions[current_page]);
        }
        else if (current_page == 4) {
            draw_final_screen();
        }

        int ch = _getch();

        if (ch == 224 || ch == 0) {
            ch = _getch();

            if (ch == 75) {
                if (current_page > 0) {
                    current_page--;
                }
            }
            else if (ch == 77) {
                if (current_page < total_pages - 1) {
                    current_page++;
                }
            }
        }
        else if (ch == 8) {
            system("cls");
            return 0;
        }
        else if (ch == 27) {
            exit(0);
        }
    }

    return 0;
}

int Manual(void)
{
    int key = 0;
    int Manual_page = 1;
    system("cls");

    while (key != 8)
    {
        system("cls");

        if (Manual_page == 1)
        {
            move_cursor(106, 26);
            printf("\033[1m다음장 (→)\033[0m");

            set_color(BG_COLOR_BRIGHTMAGENTA);
            set_color(FONT_COLOR_WHITE);
            move_cursor(45, 4);
            printf("=========== 설명서 ===========");

            set_color(BG_COLOR_BLACK);

            set_color(FONT_COLOR_YELLOW);
            move_cursor(40, 8); printf("게임 제목 : 이걸 죽네");

            set_color(FONT_COLOR_RED); move_cursor(40, 11); printf("HP");
            set_color(FONT_COLOR_WHITE); printf("가 0 이하가 되기 전까지 최대한 많은 턴을 버티는 게임입니다.");

            move_cursor(40, 13); printf("매 턴마다 2개 또는 3개의 선택지가 나옵니다.");
            move_cursor(40, 14); printf("선택지 안의 숫자는 무작위로 정해집니다.");
            move_cursor(40, 15); printf("선택한 행동에 따라 ");

            set_color(FONT_COLOR_RED); printf("HP"); set_color(FONT_COLOR_WHITE); printf("가 다르게 감소합니다.");

            set_color(FONT_COLOR_YELLOW); move_cursor(40, 18); printf("Backspace");
            set_color(FONT_COLOR_WHITE); printf("를 눌러 메뉴로 돌아가시오");
        }

        if (Manual_page == 2)
        {
            move_cursor(5, 26); printf("\033[1m이전장 (←)\033[0m");
            move_cursor(98, 26); printf("\033[1m나가기 (Backspace)\033[0m");

            move_cursor(55, 4); printf("\033[1m키 설명\033[0m");
            move_cursor(48, 9);  printf("↑: 위로 이동");
            move_cursor(48, 10); printf("↓: 밑으로 이동");
            move_cursor(48, 11); printf("←: 왼쪽 선택");
            move_cursor(48, 12); printf("→: 오른쪽 선택");
            move_cursor(48, 13); printf("Enter : 선택");
            move_cursor(48, 14); printf("ESC : 게임 종료");
            move_cursor(48, 15); printf("Backspace : 뒤로 가기");
        }

        key = _getch();

        switch (key)
        {
        case 75:
            if (Manual_page > 1) { Manual_page = Manual_page - 1; }
            break;
        case 77:
            if (Manual_page < 2) { Manual_page = Manual_page + 1; }
            break;
        case 27:
            exit(0);
            break;
        }
    }

    system("cls");
    return 0;
}

int Gamestart(void)
{
    system("mode con cols=210 lines=60");
    system("cls");

    FILE* fp = fopen("ta.txt", "r");
    char buffer[1024];
    int start_x = 40;
    int current_y = 10;

    if (fp != NULL) {
        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            buffer[strcspn(buffer, "\r\n")] = 0;
            gotoxy(start_x, current_y); printf("%s", buffer); current_y++;
        }
        fclose(fp);

        gotoxy(start_x, current_y + 2);
        printf("아무 키나 누르면 게임이 시작됩니다..."); _getch();
    }
    else {
        gotoxy(start_x, current_y); printf("오류: ta.txt 파일을 찾을 수 없습니다.\n"); Sleep(2000);
    }

    int random_bgm = rand() % 3; // 0 또는 1 생성
    if (random_bgm == 0) {
        PlaySound(TEXT("White Hats - Wayne Jones_[cut_99sec].wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
    }
    else  {
        PlaySound(TEXT("Propellerheads - Spybreak!_[cut_240sec].wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
    }

    system("mode con cols=120 lines=30");
    system("cls");

    srand((unsigned int)time(NULL));

    int hp = 100;
    int score = 0;
    finalScore = 0;
    int key = 0;

    while (hp > 0)
    {
        system("cls");

        int left_idx = rand() % num_choices;
        int right_idx;
        do {
            right_idx = rand() % num_choices;
        } while (left_idx == right_idx);

        int left_n = 0, right_n = 0;
        if (strstr(choices[left_idx].text, "%d") != NULL) {
            left_n = (rand() % (choices[left_idx].max_damage - choices[left_idx].min_damage + 1)) + choices[left_idx].min_damage;
        }
        if (strstr(choices[right_idx].text, "%d") != NULL) {
            right_n = (rand() % (choices[right_idx].max_damage - choices[right_idx].min_damage + 1)) + choices[right_idx].min_damage;
        }

        int center_x = 100;
        int left_center = center_x - 35;  // 65
        int right_center = center_x + 35; // 135

        set_color(FONT_COLOR_GREEN); move_cursor(60, 2); printf("Player : %s", playerName);
        set_color(FONT_COLOR_RED); move_cursor(96, 2); printf("HP : %d", hp);
        set_color(FONT_COLOR_WHITE); move_cursor(130, 2); printf("SCORE : %d", score);

        char left_msg[256], right_msg[256];
        if (strstr(choices[left_idx].text, "%d") != NULL) sprintf(left_msg, choices[left_idx].text, left_n);
        else strcpy(left_msg, choices[left_idx].text);
        if (strstr(choices[right_idx].text, "%d") != NULL) sprintf(right_msg, choices[right_idx].text, right_n);
        else strcpy(right_msg, choices[right_idx].text);

        int left_msg_len = calculate_visual_length(left_msg);
        int right_msg_len = calculate_visual_length(right_msg);
        int left_x = left_center - (left_msg_len / 2);
        int right_x = right_center - (right_msg_len / 2);

        set_color(FONT_COLOR_YELLOW); move_cursor(center_x - 1, 12); printf("VS");
        set_color(FONT_COLOR_WHITE);

        for (int i = 0; i < 6; i++) {
            move_cursor(left_center - 4, 8 + i); printf("%s", choices[left_idx].art[i]);
        }
        move_cursor(left_x, 18); printf("%s", left_msg);

        for (int i = 0; i < 6; i++) {
            move_cursor(right_center - 4, 8 + i); printf("%s", choices[right_idx].art[i]);
        }
        move_cursor(right_x, 18); printf("%s", right_msg);

        const char* help_msg = "방향키(←, →)로 선택하세요. (메뉴로 가기: Backspace)";
        int help_msg_len = calculate_visual_length(help_msg);
        int help_x = center_x - (help_msg_len / 2);
        set_color(FONT_COLOR_GREEN); move_cursor(help_x, 25); printf("%s", help_msg);
        set_color(FONT_COLOR_WHITE);

        int has_selected = 0;
        int selected_idx = 0;

        while (!has_selected)
        {
            key = _getch();

            if (key == 224)
            {
                key = _getch();
                if (key == 75)
                {
                    selected_idx = left_idx;
                    has_selected = 1;
                }
                else if (key == 77)
                {
                    selected_idx = right_idx;
                    has_selected = 1;
                }
            }
            else if (key == 8)
            {
                save_console_screen();

                while (1)
                {
                    set_color(BG_COLOR_BLACK); move_cursor(20 + 40, 7);
                    printf("                                                                                                                                                                                                                                                                          \n                                                                                                                                                                                                                                                                          \n                                                                                                                                                                                                                                                                          \n                                                                                                                                                                                                                                                                          \n                                                                                                                                                                                                                                                                          \n                                                                                                                                                                                                                                                                          \n                                                                                                                                                                                                                                                                          \n");

                    set_color(FONT_COLOR_RED); move_cursor(center_x - 12, 12); printf("게임을 중지하시겠습니까?");
                    move_cursor(center_x - 22, 15); printf("게임을 계속하려면 t, 중지하려면 r를 누르시오.");

                    key = _getch();

                    if (key == 'r')
                    {
                        system("cls");
                        return 0;
                    }
                    if (key == 't')
                    {
                        restore_console_screen();
                        break;
                    }
                }
            }
            else if (key == 27)
            {
                // Ignore ESC on the choice screen so it does not close the game.
                continue;
            }
        }

        int damage = 0;

        if (strstr(choices[selected_idx].text, "층에서 떨어졌다") != NULL)
        {
            int n = (selected_idx == left_idx) ? left_n : right_n;
            damage = n * 4;
        }
        else if (strstr(choices[selected_idx].text, "속도") != NULL)
        {
            int n = (selected_idx == left_idx) ? left_n : right_n;
            damage = n * 2;
        }
        else {
            int min = choices[selected_idx].min_damage;
            int max = choices[selected_idx].max_damage;
            damage = (rand() % (max - min + 1)) + min;
        }

        // ==========================================
        // [ 신규 코드 ] 고양이 참참참 미니게임 시작
        // ==========================================
        if (strstr(choices[selected_idx].text, "귀여운 길고양이를 쓰다듬는다.") != NULL)
        {
            PlaySound(TEXT("Misirlou Pulp Fiction Theme_[cut_134sec].wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

            // 전체화면(F11) 레이아웃이 깨지지 않도록 콘솔 버퍼를 매우 넓게(200칸) 확장합니다.
            system("mode con cols=200 lines=70");
            system("cls");

            // 게임의 중앙 좌표를 더 파격적으로 우측(110)으로 잡았습니다.
            int mini_center = 110;

            // 1. 고양이 중앙 출력 (110 기준으로 약 29칸 좌측으로 당겨야 딱 중앙 정렬됨)
            print_ascii_file("ascii-art (5).txt", mini_center - 29, 2);

            // 2. 기본 손 모양 중앙 출력 (마찬가지로 아스키아트 길이 보정)
            print_ascii_file("ascii-art (6).txt", mini_center - 29, 30);

            int player_dir = 0;
            set_color(FONT_COLOR_WHITE);

            // 안내 문구를 중앙에 배치
            const char* prompt_msg = "고양이와 참참참! 방향을 선택하세요 (1: 오른쪽, 2: 중앙, 3: 왼쪽) : ";
            int p_len = calculate_visual_length(prompt_msg);
            move_cursor(mini_center - (p_len / 2), 65);
            printf("%s", prompt_msg);

            // 플레이어 입력 받기
            while (1)
            {
                if (scanf("%d", &player_dir) == 1 && player_dir >= 1 && player_dir <= 3) {
                    while (getchar() != '\n');
                    break;
                }
                while (getchar() != '\n');

                const char* error_msg = "잘못된 입력입니다. 1, 2, 3 중 하나만 입력해주세요: ";
                int err_len = calculate_visual_length(error_msg);
                move_cursor(mini_center - (err_len / 2), 66);
                set_color(FONT_COLOR_RED);
                printf("%s", error_msg);
                set_color(FONT_COLOR_WHITE);
            }

            const char* wait_msg = "3초 뒤 결과가 공개됩니다...";
            int w_len = calculate_visual_length(wait_msg);
            move_cursor(mini_center - (w_len / 2), 68);
            set_color(FONT_COLOR_YELLOW);
            printf("%s", wait_msg);
            Sleep(3000);

            system("cls");

            // 고양이 방향 난수 생성 (1: 오른쪽, 2: 중앙, 3: 왼쪽)
            int cat_dir = (rand() % 3) + 1;

            // 고양이 방향에 맞춰 파격적으로 X좌표 이동
            int cat_x = mini_center - 29; // 중앙 고정점
            if (cat_dir == 1) cat_x = mini_center + 15; // 오른쪽으로 확 이동
            else if (cat_dir == 3) cat_x = mini_center - 75; // 왼쪽으로 확 이동

            // 결과 화면 - 고양이가 선택한 방향으로 휙 이동하여 출력됨
            print_ascii_file("ascii-art (5).txt", cat_x, 2);

            set_color(FONT_COLOR_BRIGHTMAGENTA);
            move_cursor(mini_center - 10, 28);
            if (cat_dir == 1) printf("고양이: (오른쪽 휙!) =>");
            else if (cat_dir == 2) printf("고양이: (가만히 중앙)");
            else printf("고양이: <= (왼쪽 휙!)");

            // 플레이어 선택 손 모양 중앙 출력
            if (player_dir == 1) print_ascii_file("ascii-art (8).txt", mini_center - 29, 30);      // 오른쪽 손
            else if (player_dir == 2) print_ascii_file("ascii-art (6).txt", mini_center - 29, 30); // 중앙 손
            else if (player_dir == 3) print_ascii_file("ascii-art (7).txt", mini_center - 29, 30); // 왼쪽 손

            set_color(FONT_COLOR_WHITE);

            // 승패 판정 로직
            if (player_dir == cat_dir)
            {
                // [수정] 성공 시 기존 브금을 멈추고 cat.wav(고양이 소리) 재생
                PlaySound(TEXT("cat.wav"), NULL, SND_FILENAME | SND_ASYNC);
                const char* win_msg = "참참참 성공! 고양이가 기분 좋게 그르릉 거립니다. (HP 15 회복)";
                int msg_len = calculate_visual_length(win_msg);
                move_cursor(mini_center - (msg_len / 2), 66);
                set_color(FONT_COLOR_GREEN);
                printf("%s", win_msg);
                damage = -15;
            }
            else
            {
                // 실패 시 하악질 소리 재생 
                PlaySound(TEXT("angry_cat.wav"), NULL, SND_FILENAME | SND_ASYNC);
                int cat_damage = (rand() % 11) + 10;
                char lose_msg[128];
                sprintf(lose_msg, "참참참 실패! 고양이가 하악질을 하며 할큅니다. (HP %d 감소)", cat_damage);
                int msg_len = calculate_visual_length(lose_msg);
                move_cursor(mini_center - (msg_len / 2), 66);
                set_color(FONT_COLOR_RED);
                printf("%s", lose_msg);
                damage = cat_damage;
            }

            Sleep(3000);
            system("mode con cols=120 lines=30"); // 원래 창 크기로 복구
            system("cls");

            if (random_bgm == 0) {
                PlaySound(TEXT("White Hats - Wayne Jones_[cut_99sec].wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
            }
            else {
                PlaySound(TEXT("Propellerheads - Spybreak!_[cut_240sec].wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
            }


        }
        // ==========================================

        hp -= damage;

        if (hp > 100)
        {
            hp = 100;
        }

        if (strstr(choices[selected_idx].text, "수상할 정도로 빨간 버튼을 누른다.") != NULL)
        {
            // 효과음 재생
            PlaySound(TEXT("boom.wav"), NULL, SND_FILENAME | SND_ASYNC);
             // 효과음이 대략 2초라고 가정
            // 다시 BGM 루프 재생
            // (현재 재생 중이던 BGM 파일명을 다시 넣어줍니다)
            if (random_bgm == 0) 
            {
                Sleep(1900);
                PlaySound(TEXT("White Hats - Wayne Jones_[cut_99sec].wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
            }
            else {
                Sleep(1900);
                PlaySound(TEXT("Propellerheads - Spybreak!_[cut_240sec].wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
            }

        }

        if (strstr(choices[selected_idx].text, "태양을 맨눈으로 10초 동안 바라본다.") != NULL)
        {
            PlaySound(TEXT("myeye!.wav"), NULL, SND_FILENAME | SND_ASYNC);
            
            // 다시 BGM 루프 재생
            // (현재 재생 중이던 BGM 파일명을 다시 넣어줍니다)
            if (random_bgm == 0) {
                Sleep(1900);
                PlaySound(TEXT("White Hats - Wayne Jones_[cut_99sec].wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
            }
            else {
                Sleep(1900);
                PlaySound(TEXT("Propellerheads - Spybreak!_[cut_240sec].wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
            }
        }

        if (strstr(choices[selected_idx].text, "%d의 속도로 달리는 차에 치인다.") != NULL)
        {
            // 효과음 재생
            PlaySound(TEXT("car.wav"), NULL, SND_FILENAME | SND_ASYNC);
            // 효과음이 대략 2초라고 가정
           // 다시 BGM 루프 재생
           // (현재 재생 중이던 BGM 파일명을 다시 넣어줍니다)
            if (random_bgm == 0)
            {
                Sleep(4000);
                PlaySound(TEXT("White Hats - Wayne Jones_[cut_99sec].wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
            }
            else {
                Sleep(4000);
                PlaySound(TEXT("Propellerheads - Spybreak!_[cut_240sec].wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
            }

        }

        if (strstr(choices[selected_idx].text, "자판기에서 뽑은 제로 슈거 콜라를 시원하게 들이킨다.") != NULL)
        {
            // 효과음 재생
            PlaySound(TEXT("Drinking.wav"), NULL, SND_FILENAME | SND_ASYNC);
            // 효과음이 대략 2초라고 가정
           // 다시 BGM 루프 재생
           // (현재 재생 중이던 BGM 파일명을 다시 넣어줍니다)
            if (random_bgm == 0)
            {
                Sleep(3000);
                PlaySound(TEXT("White Hats - Wayne Jones_[cut_99sec].wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
            }
            else {
                Sleep(3000);
                PlaySound(TEXT("Propellerheads - Spybreak!_[cut_240sec].wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
            }

        }

        if (strstr(choices[selected_idx].text, "편의점에서 이온음료를 마신다.") != NULL)
        {
            // 효과음 재생
            PlaySound(TEXT("Drinking.wav"), NULL, SND_FILENAME | SND_ASYNC);
            // 효과음이 대략 2초라고 가정
           // 다시 BGM 루프 재생
           // (현재 재생 중이던 BGM 파일명을 다시 넣어줍니다)
            if (random_bgm == 0)
            {
                Sleep(3000);
                PlaySound(TEXT("White Hats - Wayne Jones_[cut_99sec].wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
            }
            else {
                Sleep(3000);
                PlaySound(TEXT("Propellerheads - Spybreak!_[cut_240sec].wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
            }

        }

        

        score += 1;
        finalScore = score;

        system("cls");

        char result_msg[128];
        if (damage < 0)
        {
            sprintf(result_msg, "선택 완료! HP가 %d 회복되었습니다.", -damage);
        }
        else
        {
            sprintf(result_msg, "선택 완료! HP가 %d 감소했습니다.", damage);
        }

        int res_len = calculate_visual_length(result_msg);
        int res_x = center_x - (res_len / 2);

        move_cursor(res_x, 12);
        printf("%s", result_msg);

        Sleep(2000);

        while (_kbhit())
        {
            _getch();
        }
    }

    finalScore = score;

    system("cls");
    set_color(FONT_COLOR_RED); move_cursor(95, 12); printf("GAME OVER");

    set_color(FONT_COLOR_WHITE); move_cursor(90, 14); printf("최종 버틴 점수 : %d", score);
    move_cursor(83, 18); printf("Backspace를 누르면 메뉴로 돌아갑니다.");

    while (1)
    {
        key = _getch();
        if (key == 8)
        {
            break;
        }
    }

    system("cls");
    return 0;
}

int Gameover(void)
{
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
        "       플레이어: %s",
        "       최종 점수: %d 점",
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

    for (int i = SCREEN_HEIGHT; i >= -num_lines; i--)
    {

        printf("\x1b[2J\x1b[H");
        set_color(FONT_COLOR_WHITE);

        for (int j = 0; j < num_lines; j++)
        {
            int line_y = i + j;

            if (line_y >= 0 && line_y < SCREEN_HEIGHT)
            {
                char formatted_line[256];

                if (j == 9)
                {
                    sprintf(formatted_line, credits[j], playerName);
                }
                else if (j == 10)
                {
                    sprintf(formatted_line, credits[j], finalScore);
                }
                else
                {
                    strcpy(formatted_line, credits[j]);
                }

                int visual_len = calculate_visual_length(formatted_line);
                int line_x = ((SCREEN_WIDTH - visual_len) / 2) + 40;
                if (line_x < 1) line_x = 1;

                move_cursor(line_x, line_y);
                printf("%s", formatted_line);
            }
        }

        move_cursor(SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
        Sleep(150);
    }

    Sleep(2000);
    system("cls");
    exit(0);

    return 0;
}

void set_color(int code)
{
    printf("\x1b[%dm", code);
}

int move_cursor(int x, int y)
{
    printf("\033[%d;%dH", y, x);
    return 0;
}

void cleanup_console(void) {
    printf("\x1b[?1049l\x1b[0m");
}

int main(void)
{
    printf("\x1b[2J\x1b[?1049h");
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    atexit(cleanup_console);

    int gameStatus = 0;
    int isBgmPlaying = 0;

    ShowLogo();

    set_color(FONT_COLOR_WHITE);
    move_cursor(80, 26);
    printf("플레이어의 닉네임을 입력하세요: ");
    set_color(FONT_COLOR_YELLOW);
    scanf("%49s", playerName);

    while (getchar() != '\n');
    system("cls");

    while (isRunning)
    {
        switch (gameStatus)
        {
        case 0:

            if (!isBgmPlaying) {
                PlaySound(TEXT("il-vento-doro.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
                isBgmPlaying = 1;
            }

            printf("\x1b[H");
            gameStatus = RenderTitle();
            break;

        case 1:
            gameStatus = People();
            break;

        case 2:
            gameStatus = Manual();
            break;

        case 3:
            PlaySound(NULL, NULL, 0);
            isBgmPlaying = 0;

            gameStatus = Gamestart();
            break;

        case 4:
            gameStatus = Gameover();
            break;
        }
    }

    system("cls");
    move_cursor(0, 25);

    return 0;
}