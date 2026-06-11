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
#define SCREEN_HEIGHT 50  // 거대 아트를 담기 위해 세로 화면을 50줄로 대폭 확장합니다.

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

// 게임 전체에서 공유하며 사용할 전역 변수들입니다.
int menu = 1;                   // 메인 화면에서 현재 화살표가 가리키고 있는 메뉴의 번호입니다.
int isRunning = 1;              // 이 값이 1인 동안은 프로그램이 계속 켜져 있고, 0이 되면 꺼집니다.
char playerName[50] = "Player"; // 플레이어의 이름을 실시간 보관하고 UI에 매핑할 닉네임 전역 변수입니다.

// 30줄에 달하는 초대형 아스키아트를 담을 수 있도록 art 배열 크기를 확장했습니다.
typedef struct
{
    const char* art[30];
    const char* text;
    int min_damage;
    int max_damage;
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
    // 30줄짜리 대형 캡슐 아스키 아트 적용 완료
    { {
        "        %.............................%%       ",
        "        %                             %%       ",
        "        %                             %%       ",
        "        %                             %%       ",
        "      %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%     ",
        "   %%+====================================%%%  ",
        "  %*========================================%% ",
        " %%==========================================% ",
        " %%==========================================% ",
        " %%==========================================% ",
        " %%==========================================% ",
        " %%..........................................% ",
        " %%                                          % ",
        " %%                  =====:                  % ",
        " %%                  =====:                  % ",
        " %%             :==============              % ",
        " %%             :==============              % ",
        " %%             :==============              % ",
        " %%                  =====:                  % ",
        " %%                  =====:                  % ",
        " %%                                          % ",
        " %%******************************************% ",
        " %%==========================================% ",
        " %%==========================================% ",
        " %%==========================================% ",
        " %%==========================================% ",
        "  %#========================================%% ",
        "   %%#===================================+%%   ",
        "      %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%      ",
        "                                               "
    }, "약국에서 진통제를 복용한다.", -15, -10 },
    { { "  [###]  ", "  |   |  ", "  |___|  ", "         ", "         ", "         " }, "편의점에서 이온음료를 마신다.", -8, -3 },
    { { "  Zzz..  ", "  (-_-)  ", "  /| |\\  ", "         ", "         ", "         " }, "잠깐 앉아서 휴식을 취한다.", -8, -4 }
};

int num_choices = sizeof(choices) / sizeof(Choice);
CHAR_INFO savedScreen[SCREEN_WIDTH * SCREEN_HEIGHT]; // 일시정지 창을 띄울 때 사용할 스크린 백업 데이터

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
        for (int j = 0; j < box_width; j++) { printf(" "); }
        printf("\x1b[0m");
    }

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
       "                                        WWWWWWWWBB  ",
       "                                         BBBBBB     "
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
            if (logo[i][j] == 'W') { printf("\x1b[47m "); }
            else if (logo[i][j] == 'B' || logo[i][j] == 'X') { printf("\x1b[40m "); }
            else { printf("\x1b[44m "); }
        }
        printf("\x1b[0m");
    }

    printf("\x1b[%d;1H\n", box_start_y + box_height + 1);
    move_cursor(logo_start_x - 5, logo_start_y + logo_height + 2);
    set_color(FONT_COLOR_WHITE);
    printf(" F11키를 눌러 전체화면으로 바꾸고 Enter키를 누르면 시작합니다.");

    int key;
    do { key = _getch(); } while (key != '\r');
    system("cls");
}

int RenderTitle(void)
{
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
    move_cursor(190, 45); printf("↑: 위로 이동");
    move_cursor(190, 46); printf("↓: 밑으로 이동");
    move_cursor(190, 47); printf("Enter : 선택");
    move_cursor(190, 48); printf("ESC : 게임 종료");

    move_cursor(106, 100);

    char a = _getch();

    switch (a)
    {
    case 72: if (menu > 1) menu--; break;
    case 80: if (menu < 4) menu++; break;
    case 27: isRunning = 0; break;
    case 13: if (menu >= 1 && menu <= 4) return menu; break;
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
        if (current_page < 4) { print_member_page(filenames[current_page], descriptions[current_page]); }
        else if (current_page == 4) { draw_final_screen(); }

        int ch = _getch();

        if (ch == 224 || ch == 0) {
            ch = _getch();
            if (ch == 75) { if (current_page > 0) { current_page--; } }
            else if (ch == 77) { if (current_page < total_pages - 1) { current_page++; } }
        }
        else if (ch == 8) { system("cls"); return 0; }
        else if (ch == 27) { exit(0); }
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
            move_cursor(106, 26); printf("\033[1m다음장 (→)\033[0m");
            set_color(BG_COLOR_BRIGHTMAGENTA); set_color(FONT_COLOR_WHITE);
            move_cursor(45, 4); printf("=========== 설명서 ===========");
            set_color(BG_COLOR_BLACK);
            set_color(FONT_COLOR_YELLOW); move_cursor(40, 8); printf("게임 제목 : 이걸 죽네");
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
        case 75: if (Manual_page > 1) { Manual_page = Manual_page - 1; } break;
        case 77: if (Manual_page < 2) { Manual_page = Manual_page + 1; } break;
        case 27: exit(0); break;
        }
    }
    system("cls"); return 0;
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
    else { gotoxy(start_x, current_y); printf("오류: ta.txt 파일을 찾을 수 없습니다.\n"); Sleep(2000); }

    // 거대한 아스키아트를 표시하기 위해 세로 크기를 50으로 넓힙니다.
    system("mode con cols=120 lines=50");
    system("cls");

    srand((unsigned int)time(NULL));

    int hp = 100;
    int score = 0;
    int key = 0;

    while (hp > 0)
    {
        system("cls");

        int left_idx = rand() % num_choices;
        int right_idx;
        do { right_idx = rand() % num_choices; } while (left_idx == right_idx);

        int left_n = 0, right_n = 0;
        if (strstr(choices[left_idx].text, "%d") != NULL) {
            left_n = (rand() % (choices[left_idx].max_damage - choices[left_idx].min_damage + 1)) + choices[left_idx].min_damage;
        }
        if (strstr(choices[right_idx].text, "%d") != NULL) {
            right_n = (rand() % (choices[right_idx].max_damage - choices[right_idx].min_damage + 1)) + choices[right_idx].min_damage;
        }

        set_color(FONT_COLOR_GREEN); move_cursor(15, 2); printf("Player : %s", playerName);
        set_color(FONT_COLOR_RED); move_cursor(45, 2); printf("HP : %d", hp);
        set_color(FONT_COLOR_WHITE); move_cursor(75, 2); printf("SCORE : %d", score);

        // 화면이 넓어졌으므로 VS 좌표를 아래로 조정합니다.
        set_color(FONT_COLOR_YELLOW); move_cursor(58, 22); printf("VS");
        set_color(FONT_COLOR_WHITE);

        char left_msg[256], right_msg[256];
        if (strstr(choices[left_idx].text, "%d") != NULL) sprintf(left_msg, choices[left_idx].text, left_n);
        else strcpy(left_msg, choices[left_idx].text);
        if (strstr(choices[right_idx].text, "%d") != NULL) sprintf(right_msg, choices[right_idx].text, right_n);
        else strcpy(right_msg, choices[right_idx].text);

        int left_msg_len = calculate_visual_length(left_msg);
        int right_msg_len = calculate_visual_length(right_msg);

        // 화면 분할 좌표를 넓어진 레이아웃에 맞춰 조정합니다.
        int left_x = 32 - (left_msg_len / 2);   if (left_x < 2) left_x = 2;
        int right_x = 87 - (right_msg_len / 2); if (right_x < 50) right_x = 50;

        set_color(FONT_COLOR_WHITE);
        // 최대 30줄까지 그림 렌더링 루프 (비어있으면 무시)
        for (int i = 0; i < 30; i++) {
            if (choices[left_idx].art[i] != NULL) {
                move_cursor(10, 6 + i);
                printf("%s", choices[left_idx].art[i]);
            }
        }
        move_cursor(left_x, 38); printf("%s", left_msg); // 그림 밑으로 자막 내림

        for (int i = 0; i < 30; i++) {
            if (choices[right_idx].art[i] != NULL) {
                move_cursor(65, 6 + i);
                printf("%s", choices[right_idx].art[i]);
            }
        }
        move_cursor(right_x, 38); printf("%s", right_msg); // 자막 내림

        // 안내문도 화면 맨 밑으로 내립니다.
        set_color(FONT_COLOR_GREEN); move_cursor(35, 42); printf("방향키(←, →)로 선택하세요. (메뉴로 가기: Backspace)");
        set_color(FONT_COLOR_WHITE);

        int has_selected = 0;
        int selected_idx = 0;

        while (!has_selected)
        {
            key = _getch();

            if (key == 224)
            {
                key = _getch();
                if (key == 75) { selected_idx = left_idx; has_selected = 1; }
                else if (key == 77) { selected_idx = right_idx; has_selected = 1; }
            }
            else if (key == 8)
            {
                save_console_screen();
                while (1)
                {
                    set_color(BG_COLOR_BLACK); move_cursor(20, 15); // 일시정지 창 위치 조정
                    printf("                                                                                                                        \n                                                                                                                        \n                                                                                                                        \n                                                                                                                        \n                                                                                                                        \n                                                                                                                        \n                                                                                                                        \n");
                    set_color(FONT_COLOR_RED); move_cursor(50, 20); printf("게임을 중지하시겠습니까?");
                    move_cursor(40, 23); printf("게임을 계속하려면 t, 중지하려면 r를 누르시오.");

                    key = _getch();
                    if (key == 'r') { system("cls"); return 0; }
                    if (key == 't') { restore_console_screen(); break; }
                }
            }
            else if (key == 27) { exit(0); }
        }

        int damage = 0;
        if (strstr(choices[selected_idx].text, "층에서 떨어졌다") != NULL) {
            int n = (selected_idx == left_idx) ? left_n : right_n; damage = n * 4;
        }
        else if (strstr(choices[selected_idx].text, "속도") != NULL) {
            int n = (selected_idx == left_idx) ? left_n : right_n; damage = n * 2;
        }
        else {
            int min = choices[selected_idx].min_damage;
            int max = choices[selected_idx].max_damage;
            damage = (rand() % (max - min + 1)) + min;
        }

        hp -= damage;
        if (hp > 100) { hp = 100; }
        score += 1;

        system("cls");
        move_cursor(50, 22); // 데미지 결과 리포트 위치 조정
        if (damage < 0) { printf("선택 완료! HP가 %d 회복되었습니다.", -damage); }
        else { printf("선택 완료! HP가 %d 감소했습니다.", damage); }

        Sleep(2000);
        while (_kbhit()) { _getch(); }
    }

    system("cls");
    set_color(FONT_COLOR_RED); move_cursor(54, 22); printf("GAME OVER"); // 오버 멘트 위치 내림
    set_color(FONT_COLOR_WHITE); move_cursor(50, 24); printf("최종 버틴 점수 : %d", score);
    move_cursor(43, 28); printf("Backspace를 누르면 메뉴로 돌아갑니다.");

    while (1) { key = _getch(); if (key == 8) { break; } }
    system("cls"); return 0;
}

int Gameover(void)
{
    // 화면 높이가 50이 되었으므로 엔딩 멘트도 맨 아래(50)부터 솟아오르게 합니다.
    int y = 50;
    int yy = 50;
    int While = 1;
    int Thile = 1;

    system("cls");

    while (While)
    {
        if (y != 1)
        {
            system("cls");
            move_cursor(42, y); printf("여기에 마무리 되는거 추가로 넣고 꺼지게 하기");
            y--;
            Sleep(100);
            if (y == 1) { While = 0; }
        }
    }

    while (Thile)
    {
        if (yy != 3)
        {
            system("cls");
            move_cursor(42, y); printf("여기에 마무리 되는거 추가로 넣고 꺼지게 하기");
            move_cursor(42, yy); printf("여기에 마무리 되는거 추가로 넣고 꺼지게 하기");
            yy--;
            Sleep(100);
            if (yy == 3) { Thile = 0; }
        }
    }

    Sleep(10000);
    exit(0);
}

void set_color(int code) { printf("\x1b[%dm", code); }
int move_cursor(int x, int y) { printf("\033[%d;%dH", y, x); return 0; }
void cleanup_console(void) { printf("\x1b[?1049l\x1b[0m"); }

int main(void)
{
    printf("\x1b[2J\x1b[?1049h");
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);        // 한글 닉네임 입력을 깨짐 없이 받기 위한 설정 추가!
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

        case 1: gameStatus = People(); break;
        case 2: gameStatus = Manual(); break;
        case 3:
            PlaySound(NULL, NULL, 0);
            isBgmPlaying = 0;
            gameStatus = Gamestart();
            break;

        case 4: gameStatus = Gameover(); break;
        }
    }

    system("cls");
    move_cursor(0, 48);
    return 0;
}