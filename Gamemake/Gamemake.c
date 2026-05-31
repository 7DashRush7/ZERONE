#pragma execution_character_set("utf-8") // 콘솔 출력 시 한글 깨짐을 완벽하게 방지하기 위한 UTF-8 인코딩 설정
#define _CRT_SECURE_NO_WARNINGS          // fopen 등 C 표준 함수의 보안 경고(C4996)를 무시하기 위한 매크로 정의

#include <stdio.h>    // 표준 입출력 함수(printf, fopen 등)를 사용하기 위한 헤더 파일
#include <string.h>   // 문자열 처리 함수(strlen, strstr 등)를 사용하기 위한 헤더 파일
#include <windows.h>  // 윈도우 API(콘솔 제어, Sleep, 색상 변경 등)를 사용하기 위한 헤더 파일
#include <conio.h>    // 콘솔 입출력 함수(_getch 등)를 사용하기 위한 헤더 파일
#include <stdlib.h>   // 난수 생성(rand, srand) 및 시스템 명령어(system)를 위한 헤더 파일
#include <time.h>     // 시간 관련 함수(time)를 사용하여 난수 시드를 설정하기 위한 헤더 파일

#define COLOR_RESET "\x1b[0m" 
// 콘솔 텍스트 색상을 기본값으로 초기화하는 ANSI 이스케이프 시퀀스

// 콘솔 텍스트 폰트(글자) 및 배경 색상을 지정하기 위한 ANSI 색상 코드 매크로 정의
#define FONT_COLOR_BLACK 30
#define BG_COLOR_BLACK 40
#define FONT_COLOR_RED 316
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

#define Backspace 8 // 키보드 Backspace 키의 아스키코드 값 정의

#define SCREEN_WIDTH 120  // 화면 캡처 및 복원을 위한 콘솔 가로 최대 크기
#define SCREEN_HEIGHT 30  // 화면 캡처 및 복원을 위한 콘솔 세로 최대 크기

// 프로그램에서 사용할 함수들의 원형(Prototype) 선언부
void set_color(int code);                 // 텍스트 색상을 변경하는 함수
int move_cursor(int x, int y);            // 콘솔 화면의 특정 좌표로 커서를 이동시키는 함수 (ANSI 방식)
void gotoxy(int x, int y);                // 콘솔 화면의 특정 좌표로 커서를 이동시키는 함수 (Windows API 방식)
void ShowLogo(void);                      // 게임 시작 전 타이틀 로고를 보여주는 함수
int RenderTitle(void);                    // 메인 메뉴를 출력하고 사용자의 선택을 받는 함수
void print_member_page(const char* filename, const char* description); // 팀원 개인의 txt 파일을 읽어 출력하는 함수 (추가됨)
void draw_final_screen(void);             // 팀원 소개의 마지막 ZERONE 로고 화면을 출력하는 함수 (추가됨)
int People(void);                         // '만든 사람 및 팀 소개' 메뉴를 처리하는 함수 (새롭게 교체됨)
int Manual(void);                         // '설명서' 메뉴를 처리하는 함수
int Gamestart(void);                      // 실제 게임 플레이 로직을 담당하는 함수
int Gameover(void);                       // 게임 오버 연출 및 프로그램 종료를 담당하는 함수

// 게임 전역 변수 설정
int menu = 1;       // 현재 선택된 메인 메뉴 번호 (기본값 1)
int isRunning = 1;  // 게임 메인 루프 실행 여부 (0이 되면 프로그램 종료)

// 각 라운드마다 등장할 선택지 데이터를 담는 구조체 선언
typedef struct
{
    const char* art[6]; // 화면에 출력될 아스키아트 배열 (최대 6줄)
    const char* text;   // 선택지에 대한 설명 텍스트
    int min_damage;     // 무작위 데미지의 최솟값 (또는 n층 데미지 계산 시 n의 최솟값)
    int max_damage;     // 무작위 데미지의 최댓값 (또는 n층 데미지 계산 시 n의 최댓값)
} Choice;

// 게임 내에서 사용할 선택지 목록 배열 선언 (새로운 선택지를 이 배열에 계속 추가 가능)
Choice choices[] =
{
    // { {아스키아트 6줄}, "텍스트 문구", 최소 데미지, 최대 데미지 }
    { {"  /\\_/\\  ", " ( o.o ) ", "  > ^ <  ", "         ", "         ", "         "}, "귀여운 길고양이를 쓰다듬는다.", 1, 5 },
    { {"   ___   ", "  / _ \\  ", " | (_) | ", "  \\___/  ", "         ", "         "}, "수상할 정도로 빨간 버튼을 누른다.", 3, 9 },
    { {"  ====   ", " |    |  ", " |    |  ", "  ====   ", "         ", "         "}, "자판기 밑에서 동전을 줍는다.", 0, 2 },
    { {"   \\|/   ", "  - O -  ", "   /|\\   ", "         ", "         ", "         "}, "태양을 맨눈으로 10초 동안 바라본다.", 8, 12 },
    { {"  [___]  ", "  |   |  ", "  |___|  ", "         ", "         ", "         "}, "유통기한이 3년 지난 통조림을 먹는다.", 5, 15 },

    // 구조체 변경 없이, 텍스트 안에 '%d'가 포함되어 있으면 프로그램이 이를 n층 변수로 자동 인식하여 처리합니다.
    { {"  _||_   ", " |    |  ", " |    |  ", " |    |  ", " |____|  ", "         "}, "%d층에서 떨어졌다.", 2, 10 },
    { { "                                                  " }, "%d의 속도로 달리는 차에 치인다.", 20, 50 }
};

// 위에서 선언한 선택지 배열의 전체 크기를 개별 구조체의 크기로 나누어, 총 선택지의 개수를 계산
int num_choices = sizeof(choices) / sizeof(Choice);

// 콘솔 화면의 글자와 색상 정보를 백업하기 위한 버퍼 배열 선언
CHAR_INFO savedScreen[SCREEN_WIDTH * SCREEN_HEIGHT];

// 현재 콘솔 화면의 상태(글자, 색상 등)를 배열에 저장하는 함수
void save_console_screen()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // 현재 출력 중인 콘솔의 핸들을 가져옴

    COORD bufferSize = { SCREEN_WIDTH, SCREEN_HEIGHT }; // 저장할 버퍼의 가로, 세로 크기를 지정
    COORD bufferCoord = { 0, 0 }; // 배열 내에서 화면 정보를 저장하기 시작할 기준 좌표 (0,0)
    SMALL_RECT readRegion = { 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1 }; // 콘솔 창에서 실제로 읽어올 사각형 영역 지정

    // 콘솔 창의 내용을 읽어와서 savedScreen 배열에 저장하는 Windows API 함수 호출
    ReadConsoleOutput(
        hConsole,
        savedScreen,
        bufferSize,
        bufferCoord,
        &readRegion
    );
}

// 이전에 저장해 두었던 콘솔 화면 상태를 다시 콘솔 창에 복원하는 함수
void restore_console_screen()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // 현재 출력 중인 콘솔의 핸들을 가져옴

    COORD bufferSize = { SCREEN_WIDTH, SCREEN_HEIGHT }; // 복원할 버퍼의 가로, 세로 크기 지정
    COORD bufferCoord = { 0, 0 }; // 배열 내에서 읽어오기 시작할 기준 좌표 (0,0)
    SMALL_RECT writeRegion = { 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1 }; // 콘솔 창에 실제로 덮어쓸 사각형 영역 지정

    // savedScreen 배열에 저장되어 있는 내용을 콘솔 창에 출력하여 복원하는 Windows API 함수 호출
    WriteConsoleOutput(
        hConsole,
        savedScreen,
        bufferSize,
        bufferCoord,
        &writeRegion
    );
}

// X, Y 좌표를 받아 콘솔 창의 커서 위치를 해당 좌표로 즉시 이동시키는 함수 (Windows API 방식)
void gotoxy(int x, int y) {
    COORD pos = { (SHORT)x, (SHORT)y }; // X, Y 좌표를 COORD 구조체로 변환
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos); // 설정된 위치로 커서 이동
}

// 게임 시작 시 처음으로 보여지는 제작사/로고 화면 출력 함수
void ShowLogo(void)
{
    printf("\x1b[2J"); // ANSI 코드를 사용하여 콘솔 화면 전체를 깔끔하게 지움

    CONSOLE_SCREEN_BUFFER_INFO csbi; // 콘솔 창의 정보를 저장할 구조체
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi); // 현재 콘솔 창의 정보를 가져옴

    // 로고를 감싸는 배경 박스의 크기와 시작 좌표 설정
    int box_width = 60;
    int box_height = 24;
    int box_start_x = 37;
    int box_start_y = 5;

    // 설정한 크기만큼 파란색 배경 박스를 그리는 반복문
    for (int i = 0; i < box_height; i++)
    {
        printf("\x1b[%d;%dH", box_start_y + i, box_start_x); // 커서를 박스 시작 위치로 이동
        printf("\x1b[44m"); // 배경색을 파란색으로 변경
        for (int j = 0; j < box_width; j++)
        {
            printf(" "); // 공백을 출력하여 배경색으로 채움
        }
        printf("\x1b[0m"); // 색상 설정을 기본값으로 초기화
    }

    // 출력할 로고의 형태를 문자열 배열로 정의 (W=흰색, B=검은색 텍스트, X=검은색 로고 블록)
    const char* logo[] = {
       "                                                    ",
       "  WWWWWW                                             ",
       " WWW   WWWBB                                       ",
       " WWW   WWWBB                                       ",
       " WWW   WWWBB  XXXXXX  XXXXX  XXXXX                 ",
       "  WWWWWWWBB        X   X   X  X                    ",
       "    BBBBB         X    XXXXX  XXXXX                ",
       "                 X     X   X  X                    ",
       "               XXXXXX  X   X  XXXXX                ",
       "                                                    ",
       "                 XXX   X   X  XXXXX                ",
       "                X   X  XX  X  X                    ",
       "                X   X  X X X  XXXXX                ",
       "                X   X  X  XX  X                    ",
       "                 XXX   X   X  XXXXX      WWW         ",
       "                                       WWWWWBB       ",
       "                                       WW WWBB       ",
       "                                          WWBB       ",
       "                                          WWBB       ",
       "                                      WWWWWWWWBB     ",
       "                                        BBBBBB       "
    };

    // 로고 텍스트의 크기 설정
    int logo_height = 21;
    int logo_width = 52;

    // 파란색 박스 정중앙에 로고가 오도록 시작 좌표 계산
    int logo_start_x = box_start_x + (box_width - logo_width) / 2;
    int logo_start_y = box_start_y + (box_height - logo_height) / 2;

    // 로고 배열을 순회하며 특정 알파벳에 따라 색상을 입혀서 출력
    for (int i = 0; i < logo_height; i++)
    {
        printf("\x1b[%d;%dH", logo_start_y + i, logo_start_x); // 각 줄의 시작 위치로 이동
        for (int j = 0; j < logo_width; j++)
        {
            if (logo[i][j] == 'W')
            {
                printf("\x1b[47m "); // 'W'인 경우 흰색 배경
            }
            else if (logo[i][j] == 'B' || logo[i][j] == 'X')
            {
                printf("\x1b[40m "); // 'B' 또는 'X'인 경우 검은색 배경
            }
            else
            {
                printf("\x1b[44m "); // 그 외의 공간은 파란색 배경 유지
            }
        }
        printf("\x1b[0m"); // 한 줄 출력이 끝나면 색상 초기화
    }

    printf("\x1b[%d;1H\n", box_start_y + box_height + 1); // 박스 바깥쪽 아래로 커서 이동

    move_cursor(logo_start_x + 2, logo_start_y + logo_height + 1); // 안내 문구 위치로 커서 이동
    set_color(FONT_COLOR_WHITE); // 안내 문구 색상을 흰색으로 설정
    printf("전체화면으로 바꾼 뒤 Enter키를 누르면 시작합니다."); // 안내 문구 출력

    int key; // 입력받은 키 값을 저장할 변수

    // 사용자가 Enter 키(\r)를 누를 때까지 무한정 대기하는 루프
    do {
        key = _getch(); // 키보드 입력을 하나 받음
    } while (key != '\r');

    system("cls"); // Enter 키가 입력되면 화면을 깨끗하게 지움

    return 0; // 함수 종료
}

// 메인 메뉴 화면을 그리고 사용자의 방향키 선택을 처리하는 함수
int RenderTitle(void)
{
    set_color(BG_COLOR_BRIGHTMAGENTA); // 메인 타이틀의 배경색을 밝은 마젠타색으로 설정
    set_color(FONT_COLOR_WHITE);       // 메인 타이틀의 글자색을 흰색으로 설정

    // 화면 상단 중앙에 게임 제목 렌더링
    move_cursor(52, 9);
    printf("                       ");
    move_cursor(52, 10);
    printf("       이걸 죽네       ");
    move_cursor(52, 11);
    printf("                       ");

    set_color(BG_COLOR_BLACK); // 이후 메뉴 항목들의 기본 배경색을 검은색으로 복구

    // 1번 메뉴 (만든 사람 및 팀 소개) 출력. 현재 선택된 메뉴가 1번이면 배경을 노란색으로 강조
    if (menu == 1)
    {
        set_color(BG_COLOR_YELLOW);
    }
    move_cursor(52, 13);
    printf("  1. 만든 사람 및 팀 소개  ");
    set_color(BG_COLOR_BLACK);

    // 2번 메뉴 (설명서) 출력
    if (menu == 2)
    {
        set_color(BG_COLOR_YELLOW);
    }
    move_cursor(52, 15);
    printf("  2. 설명서  ");
    set_color(BG_COLOR_BLACK);

    // 3번 메뉴 (게임 시작) 출력
    if (menu == 3)
    {
        set_color(BG_COLOR_YELLOW);
    }
    move_cursor(52, 17);
    printf("  3. 게임 시작  ");
    set_color(BG_COLOR_BLACK);

    // 4번 메뉴 (게임 종료) 출력
    if (menu == 4)
    {
        set_color(BG_COLOR_YELLOW);
    }
    move_cursor(52, 19);
    printf("  4. 게임 종료  ");
    set_color(BG_COLOR_BLACK);

    // 화면 우측 하단에 조작법 안내 출력
    set_color(FONT_COLOR_YELLOW);
    move_cursor(106, 27);
    printf("↑: 위로 이동");
    move_cursor(106, 28);
    printf("↓: 밑으로 이동");
    move_cursor(106, 29);
    printf("Enter : 선택");
    move_cursor(106, 30);
    printf("ESC : 게임 종료");

    move_cursor(106, 100); // 사용자에게 커서가 보이지 않게 구석으로 치움

    char a = _getch(); // 사용자로부터 키보드 입력(방향키, 엔터, ESC 등)을 받음

    // 입력받은 키에 따라 동작을 수행하는 switch 문
    switch (a)
    {
    case 72: // 위쪽 화살표(↑) 키가 입력된 경우
        if (menu > 1) // 메뉴가 1보다 크면
        {
            menu = menu - 1; // 커서를 위로 한 칸 올림
        }
        break;
    case 80: // 아래쪽 화살표(↓) 키가 입력된 경우
        if (menu < 4) // 메뉴가 4보다 작으면
        {
            menu = menu + 1; // 커서를 아래로 한 칸 내림
        }
        break;

    case 27: // ESC 키가 입력된 경우
        isRunning = 0; // 전역 변수를 0으로 만들어 게임 루프(프로그램) 종료
        break;

    case 13: // Enter 키가 입력된 경우
        if (menu >= 1 && menu <= 4)
        {
            return menu; // 현재 커서가 위치한 메뉴의 번호를 반환하여 해당 기능 실행
        }
        break;
    }

    return 0; // 특별한 선택이 없었으면 0 반환
}

// -------------------------------------------------------------------------
// [추가된 부분 1] 개별 팀원 아스키아트(txt) 및 설명을 읽어와 출력하는 함수
// -------------------------------------------------------------------------
void print_member_page(const char* filename, const char* description) {
    printf("\x1b[2J\x1b[H"); // 콘솔 화면 전체를 지우고 커서를 왼쪽 맨 위(1,1)로 이동

    FILE* file = fopen(filename, "r"); // 인자로 받은 txt 파일을 읽기 모드("r")로 염
    if (file != NULL) { // 파일이 정상적으로 열렸다면
        char buffer[1024]; // 파일의 내용을 한 줄씩 저장할 임시 문자열 버퍼 생성
        while (fgets(buffer, sizeof(buffer), file)) { // 파일 끝에 도달할 때까지 한 줄씩 읽음
            printf("%s", buffer); // 읽어온 한 줄의 아스키아트를 화면에 출력
        }
        fclose(file); // 출력이 끝났으므로 파일을 닫음
    }
    else { // 파일을 찾지 못했거나 오류가 발생한 경우
        printf("\n[오류] '%s' 파일을 찾을 수 없습니다.\n", filename);
        printf("프로젝트 폴더 안에 파일 이름이 정확히 %s 인지 확인해주세요.\n\n", filename);
    }

    // 텍스트 파일(아스키아트) 출력 아래쪽에 팀원 설명 및 네비게이션 안내문 출력
    printf("\n\n==================================================\n");
    printf("  %s\n", description); // 인자로 받은 팀원 역할/학번 출력
    printf("==================================================\n");
    // 사용자가 페이지를 넘기거나 메뉴로 돌아갈 수 있도록 키 안내
    printf("\n[ <- 이전 페이지 ]        [ Backspace 메뉴로 돌아가기 ]        [ 다음 페이지 -> ]\n");
}

// -------------------------------------------------------------------------
// [추가된 부분 2] 팀원 소개의 마지막 페이지 (ZERONE 로고 화면)를 출력하는 함수
// -------------------------------------------------------------------------
void draw_final_screen(void) {
    printf("\x1b[2J\x1b[H"); // 화면 전체를 지우고 커서를 맨 위로 이동

    // 가운데 파란 박스의 크기 및 위치 좌표 설정
    int box_width = 50;
    int box_height = 30;
    int box_start_x = 40;
    int box_start_y = 5;

    // 파란색 배경 박스를 화면에 그리는 루프
    for (int i = 0; i < box_height; i++) {
        printf("\x1b[%d;%dH", box_start_y + i, box_start_x); // 커서 이동
        printf("\x1b[44m"); // 배경색 파란색
        for (int j = 0; j < box_width; j++) {
            printf(" "); // 공백 채우기
        }
        printf("\x1b[0m"); // 색상 초기화
    }

    // 픽셀 아트로 표현된 'ZERONE' 로고 문자열 배열
    const char* title_text[] = {
        "XXXXX  XXXXX  XXXX   XXXXX  X   X  XXXXX",
        "   X   X      X   X  X   X  XX  X  X    ",
        "  X    XXXX   XXXX   X   X  X X X  XXXX ",
        " X     X      X  X   X   X  X  XX  X    ",
        "XXXXX  XXXXX  X   X  XXXXX  X   X  XXXXX"
    };

    // 로고를 박스 정중앙에 배치하기 위한 계산
    int title_width = (int)strlen(title_text[0]);
    int title_start_x = box_start_x + (box_width - title_width) / 2;
    int title_start_y = box_start_y + (box_height - 11) / 2;

    // 계산된 위치에 ZERONE 로고를 출력하는 루프
    for (int i = 0; i < 5; i++) {
        printf("\x1b[%d;%dH", title_start_y + i, title_start_x);
        for (int j = 0; j < title_width; j++) {
            if (title_text[i][j] == 'X') { // 'X' 문자는 검은색 블록으로 표시
                printf("\x1b[40m ");
            }
            else { // 빈칸은 배경색인 파란색으로 표시
                printf("\x1b[44m ");
            }
        }
        printf("\x1b[0m"); // 한 줄 끝날 때마다 색상 초기화
    }

    // ZERONE 팀원 전체 명단 텍스트 배열
    const char* team_text[] = {
        " team 01 (ZERONE) 팀원들",
        "마준서(202617166) : 총괄", // 오타(학번) 교정됨
        "백종화(202617139) : 코드",
        "이인욱(202619389) : 코드",
        "이준현(202619549) : 디자인"
    };

    // 로고 아래에 명단을 예쁘게 정렬하기 위한 시작 좌표 계산
    int team_start_x = box_start_x + (box_width - 24) / 2;
    int team_start_y = title_start_y + 5 + 1;

    // 명단 배열을 순회하며 화면에 출력 (검은색 폰트, 파란색 배경)
    for (int i = 0; i < 5; i++) {
        printf("\x1b[%d;%dH\x1b[30m\x1b[44m%s\x1b[0m", team_start_y + i, team_start_x, team_text[i]);
    }

    // 박스 바깥 아래쪽 공간으로 커서를 이동시킨 뒤, 네비게이션 키 안내 출력
    printf("\x1b[%d;1H\n", box_start_y + box_height + 1);

    // [요청 반영] 마지막 창 아래에 Backspace 안내 문구를 추가하여 출력
    printf("[ <- 이전 페이지 ]        [ Backspace 메뉴로 돌아가기 ]        [ ESC 종료 ]\n");
}

// -------------------------------------------------------------------------
// [교체된 부분] 메뉴 1번을 눌렀을 때 실행되는 팀원 소개 로직 메인 함수
// -------------------------------------------------------------------------
int People(void)
{
    // 각 페이지에서 읽어올 팀원 아스키아트 텍스트 파일들의 이름 배열
    const char* filenames[] = {
        "1.txt",
        "2.txt",
        "3.txt",
        "4.txt"
    };

    // 각 팀원 텍스트 파일 하단에 띄울 소개글 배열
    const char* descriptions[] = {
        "마준서(202617166) : 총괄",
        "백종화(202617139) : 코드",
        "이인욱(202619389) : 코드",
        "이준현(202619549) : 디자인"
    };

    int current_page = 0; // 현재 보여주고 있는 페이지의 인덱스 (0~4)
    int total_pages = 5;  // 팀원 4명(0~3) + 마지막 로고 화면(4) = 총 5페이지

    while (1) // 사용자가 메뉴로 나가거나 종료할 때까지 화면을 반복해서 그림
    {
        // current_page 인덱스에 따라 알맞은 화면을 렌더링
        if (current_page < 4) {
            // 인덱스가 0~3인 경우, 파일과 소개글을 넘겨 팀원 개인 페이지 출력
            print_member_page(filenames[current_page], descriptions[current_page]);
        }
        else if (current_page == 4) {
            // 인덱스가 4인 경우, 마지막 ZERONE 전체 로고 페이지 출력
            draw_final_screen();
        }

        // 사용자의 키보드 입력을 받음
        int ch = _getch();

        // 방향키 등 확장 키보드 코드가 들어온 경우 (224 또는 0)
        if (ch == 224 || ch == 0) {
            ch = _getch(); // 실제 방향키 식별 코드를 한 번 더 읽어옴

            if (ch == 75) {         // '왼쪽 화살표(←)' 키를 누른 경우
                if (current_page > 0) {
                    current_page--; // 이전 페이지로 이동
                }
            }
            else if (ch == 77) {    // '오른쪽 화살표(→)' 키를 누른 경우
                if (current_page < total_pages - 1) {
                    current_page++; // 다음 페이지로 이동
                }
            }
        }
        else if (ch == 8) { // Backspace 키(아스키코드 8)를 누른 경우
            system("cls");  // 화면을 깨끗이 지우고
            return 0;       // 함수를 종료하여 (main 루프로 돌아가) 메인 메뉴 화면으로 복귀함
        }
        else if (ch == 27) { // ESC 키(아스키코드 27)를 누른 경우
            exit(0);         // 프로그램 전체를 즉시 강제 종료
        }
    }

    return 0;
}

// 메인 메뉴 2번 '설명서'를 눌렀을 때 실행되는 함수
int Manual(void)
{
    int key = 0;          // 키보드 입력을 받을 변수 초기화
    int Manual_page = 1;  // 설명서의 현재 페이지 상태 (1 또는 2)
    system("cls");        // 메뉴 진입 전 화면을 지움

    // Backspace(8)를 눌러 메뉴로 나가기 전까지 반복
    while (key != 8)
    {
        system("cls"); // 화면을 갱신하기 위해 지움

        if (Manual_page == 1) // 1페이지일 때 그릴 UI
        {
            move_cursor(111, 50); // 우측 하단에 '다음장' 안내 출력
            printf("\033[1m다음장 (→)\033[0m");

            // 제목 박스 출력
            set_color(BG_COLOR_BRIGHTMAGENTA);
            set_color(FONT_COLOR_WHITE);
            move_cursor(48, 7);
            printf("=========== 설명서 ===========");

            set_color(BG_COLOR_BLACK); // 배경 복구

            // 게임 룰 설명 텍스트들을 각 위치에 출력
            set_color(FONT_COLOR_YELLOW);
            move_cursor(43, 10);
            printf("게임 제목 : 이걸 죽네");

            set_color(FONT_COLOR_RED);
            move_cursor(43, 12);
            printf("HP");

            set_color(FONT_COLOR_WHITE);
            printf("가 0 이하가 되기 전까지 최대한 많은 턴을 버티는 게임입니다.");

            move_cursor(43, 14);
            printf("매 턴마다 2개 또는 3개의 선택지가 나옵니다.");

            set_color(FONT_COLOR_WHITE);
            move_cursor(43, 15);
            printf("선택지 안의 숫자는 무작위로 정해집니다.");

            move_cursor(43, 16);
            printf("선택한 행동에 따라 ");

            set_color(FONT_COLOR_RED);
            printf("HP");

            set_color(FONT_COLOR_WHITE);
            printf("가 다르게 감소합니다.");

            // 페이지 좌측 하단에 뒤로가기 키 안내 출력
            set_color(FONT_COLOR_YELLOW);
            move_cursor(43, 17);
            printf("Backspace");

            set_color(FONT_COLOR_WHITE);
            printf("를 눌러 메뉴로 돌아가시오");
        }

        if (Manual_page == 2) // 2페이지일 때 그릴 조작법 UI
        {
            // 하단 조작 안내
            move_cursor(0, 50);
            printf("\033[1m이전장 (←)\033[0m");

            move_cursor(103, 50);
            printf("\033[1m나가기 (Backspace)\033[0m");

            // 각종 키 설명 텍스트들을 각 위치에 출력
            move_cursor(60, 7);
            printf("\033[1m키 설명\033[0m");
            move_cursor(53, 12);
            printf("↑: 위로 이동");
            move_cursor(53, 13);
            printf("↓: 밑으로 이동");
            move_cursor(53, 14);
            printf("←: 왼쪽 선택");
            move_cursor(53, 15);
            printf("→: 오른쪽 선택");
            move_cursor(53, 16);
            printf("Enter : 선택");
            move_cursor(53, 17);
            printf("ESC : 게임 종료");
            move_cursor(53, 18);
            printf("Backspace : 뒤로 가기");
        }

        key = _getch(); // 페이지를 보고 난 뒤 키 입력 대기

        // 방향키 등 입력에 따른 페이지 이동 또는 종료
        switch (key)
        {
        case 75: // ← 키
            if (Manual_page > 1) // 1페이지보다 크면
            {
                Manual_page = Manual_page - 1; // 1페이지로 돌아감
            }
            break;
        case 77: // → 키
            if (Manual_page < 2) // 2페이지보다 작으면
            {
                Manual_page = Manual_page + 1; // 2페이지로 넘어감
            }
            break;

        case 27: // ESC 키
            exit(0); // 프로그램 즉시 종료
            break;
        }
    }

    system("cls"); // 백스페이스를 눌러 루프를 탈출하면 화면을 지우고 메뉴로 돌아감
    return 0;
}

// 메인 메뉴 3번 '게임 시작'을 눌렀을 때 실행되는 실제 게임 로직 함수
int Gamestart(void)
{
    // ============================================================
    // ★ 추가된 오프닝 (ta.txt 파일 출력) 연출 시작 부분 ★
    // ============================================================
    system("mode con cols=210 lines=60"); // 텍스트 아가 깨지지 않게 콘솔 크기를 넉넉히 변경
    system("cls"); // 화면 지우기

    FILE* fp = fopen("ta.txt", "r"); // 오프닝 텍스트 파일(ta.txt) 읽기 모드로 열기
    char buffer[1024]; // 한 줄을 저장할 버퍼
    int start_x = 40;  // 파일 텍스트를 출력할 시작 x 좌표
    int current_y = 10; // 파일 텍스트를 출력할 시작 y 좌표

    if (fp != NULL) { // 파일이 무사히 열렸다면
        while (fgets(buffer, sizeof(buffer), fp) != NULL) { // 파일 끝까지 한 줄씩 읽기
            // 줄바꿈 문자(엔터, \n, \r)를 제거하여 깨끗한 문자열로 만듦
            buffer[strcspn(buffer, "\r\n")] = 0;

            gotoxy(start_x, current_y); // 지정된 위치로 커서 이동
            printf("%s", buffer);       // 읽은 텍스트 출력
            current_y++;                // 다음 줄을 위해 y 좌표 1 증가
        }
        fclose(fp); // 파일 다 읽었으니 닫기

        // 그림이 다 출력된 밑 공간에 안내 메시지 출력
        gotoxy(start_x, current_y + 2);
        printf("아무 키나 누르면 게임이 시작됩니다...");
        _getch(); // 유저가 확인할 수 있도록 아무 키나 누를 때까지 정지 대기
    }
    else { // 파일을 찾지 못한 경우 에러 처리
        gotoxy(start_x, current_y);
        printf("오류: ta.txt 파일을 찾을 수 없습니다.\n");
        Sleep(2000); // 2초간 에러 메시지를 보여주고 넘어감
    }

    // 원래 게임 UI 규격(SCREEN_WIDTH=120, SCREEN_HEIGHT=30)이 
    // 정상적으로 렌더링되도록 콘솔 크기를 다시 원래대로 축소 복구
    system("mode con cols=120 lines=30");
    system("cls");
    // ============================================================
    // ★ 추가된 오프닝 (ta.txt 파일 출력) 연출 끝 부분 ★
    // ============================================================

    // 매 게임마다 선택지가 다르게 나오도록 난수 생성 시드를 현재 시간 기반으로 초기화
    srand((unsigned int)time(NULL));

    int hp = 100;    // 플레이어의 초기 HP 설정
    int score = 0;   // 플레이어의 초기 점수 설정 (버틴 라운드 수)
    int key = 0;     // 입력받을 키 저장용 변수

    // 플레이어의 HP가 0보다 큰(살아있는) 동안 무한 반복되는 라운드 루프
    while (hp > 0)
    {
        system("cls"); // 새 라운드 시작 전 화면 지우기

        // 2개의 중복되지 않는 랜덤 선택지 뽑기 로직
        int left_idx = rand() % num_choices; // 왼쪽 선택지를 0~총개수-1 범위 내 랜덤 뽑기
        int right_idx;
        do {
            right_idx = rand() % num_choices; // 오른쪽 선택지도 뽑되
        } while (left_idx == right_idx);      // 왼쪽과 똑같은 게 나오면 다시 뽑음

        // n 변수가 텍스트에 포함되어(%d 존재) 매번 값이 달라져야 하는 선택지일 경우
        // 화면 출력 전 미리 n 값을 랜덤(min_damage ~ max_damage)으로 뽑아 둡니다.
        int left_n = 0, right_n = 0;
        if (strstr(choices[left_idx].text, "%d") != NULL) { // 왼쪽에 %d가 있으면
            left_n = (rand() % (choices[left_idx].max_damage - choices[left_idx].min_damage + 1)) + choices[left_idx].min_damage;
        }
        if (strstr(choices[right_idx].text, "%d") != NULL) { // 오른쪽에 %d가 있으면
            right_n = (rand() % (choices[right_idx].max_damage - choices[right_idx].min_damage + 1)) + choices[right_idx].min_damage;
        }

        // 상단 UI (현재 HP 출력)
        set_color(FONT_COLOR_RED);
        move_cursor(40, 2);
        printf("HP : %d", hp);

        // 상단 UI (현재 SCORE 출력)
        set_color(FONT_COLOR_WHITE);
        move_cursor(70, 2);
        printf("SCORE : %d", score);

        // 화면 중앙에 'VS' 문자열 출력
        set_color(FONT_COLOR_YELLOW);
        move_cursor(58, 12);
        printf("VS");
        set_color(FONT_COLOR_WHITE);

        // 왼쪽 선택지의 아스키아트 이미지(6줄) 화면에 출력
        for (int i = 0; i < 6; i++) {
            move_cursor(25, 8 + i);
            printf("%s", choices[left_idx].art[i]);
        }
        // 왼쪽 선택지의 설명 텍스트 출력
        move_cursor(15, 18);
        if (strstr(choices[left_idx].text, "%d") != NULL) { // n층 계열이면
            printf(choices[left_idx].text, left_n);         // 뽑아둔 left_n을 포맷에 넣어 출력
        }
        else {
            printf("%s", choices[left_idx].text);           // 아니면 원본 그대로 출력
        }

        // 오른쪽 선택지의 아스키아트 이미지(6줄) 화면에 출력
        for (int i = 0; i < 6; i++) {
            move_cursor(80, 8 + i);
            printf("%s", choices[right_idx].art[i]);
        }
        // 오른쪽 선택지의 설명 텍스트 출력
        move_cursor(70, 18);
        if (strstr(choices[right_idx].text, "%d") != NULL) { // n층 계열이면
            printf(choices[right_idx].text, right_n);        // 뽑아둔 right_n을 포맷에 넣어 출력
        }
        else {
            printf("%s", choices[right_idx].text);           // 아니면 원본 그대로 출력
        }

        // 화면 하단에 조작 안내 문구 초록색으로 출력
        set_color(FONT_COLOR_GREEN);
        move_cursor(35, 25);
        printf("방향키(←, →)로 선택하세요. (메뉴로 가기: Backspace)");
        set_color(FONT_COLOR_WHITE); // 색상 초기화

        // 플레이어가 선택할 때까지 대기하는 입력 판별 로직
        int has_selected = 0; // 선택이 완료되었는지 확인하는 플래그 (0=미선택, 1=선택완료)
        int selected_idx = 0; // 유저가 최종적으로 고른 선택지의 인덱스를 저장할 변수

        while (!has_selected) // 사용자가 무언가를 선택할 때까지 무한루프
        {
            key = _getch(); // 키보드 입력 받음

            if (key == 224) // 방향키 같은 확장 키의 접두사
            {
                key = _getch(); // 실제 키 코드를 읽음
                if (key == 75) // 왼쪽 (←) 키
                {
                    selected_idx = left_idx; // 고른 선택지를 왼쪽 것으로 확정
                    has_selected = 1;        // 선택 완료 플래그 활성화
                }
                else if (key == 77) // 오른쪽 (→) 키
                {
                    selected_idx = right_idx; // 고른 선택지를 오른쪽 것으로 확정
                    has_selected = 1;         // 선택 완료 플래그 활성화
                }
            }
            else if (key == 8) // 메뉴로 돌아가기 위해 Backspace를 누른 경우
            {
                save_console_screen(); // 메뉴로 나가기 전 현재 게임 화면(질문들)을 임시 버퍼에 백업 저장

                // 일시 정지(게임 중단) 확인 창 루프
                while (1)
                {
                    // 화면 중앙에 검은색 배경 박스를 덮어씌워 팝업창처럼 만듦
                    set_color(BG_COLOR_BLACK);
                    move_cursor(20, 7);
                    // 빈 공간(공백) 문자열로 기존 그림을 덮어서 가림
                    printf("                                                                                                                       \n                                                                                                                       \n                                                                                                                       \n                                                                                                                       \n                                                                                                                       \n                                                                                                                       \n                                                                                                                       \n");

                    // 팝업 내용 출력
                    set_color(FONT_COLOR_RED);
                    move_cursor(50, 12);
                    printf("게임을 중지하시겠습니까?");
                    move_cursor(40, 15);
                    printf("게임을 계속하려면 t, 중지하려면 r를 누르시오.");

                    key = _getch(); // y/n 대신 t/r 입력을 대기

                    if (key == 'r') // r을 누르면 완전히 중지하고 메뉴로 나감
                    {
                        system("cls"); // 지우고
                        return 0;      // Gamestart 함수를 빠져나가 메인 화면으로 돌아감
                    }
                    if (key == 't') // t를 누르면 중지 취소, 게임 재개
                    {
                        restore_console_screen(); // 팝업 띄우기 전 백업해둔 원래 게임화면을 다시 덮어씌워 복원
                        break; // 팝업창 루프를 빠져나가고 기존 선택 판별 대기 화면으로 돌아감
                    }
                }
            }
            else if (key == 27) // ESC를 누른 경우
            {
                exit(0); // 프로그램 강제 종료
            }
        } // while(!has_selected) 끝

        // 유저가 고른 선택지에 따른 데미지 계산 처리
        int damage = 0;

        // 만약 고른 선택지의 텍스트 안에 '%d' 가 들어있다면 (n층에서 떨어졌다 등)
        if (strstr(choices[selected_idx].text, "층에서 떨어졌다") != NULL)
        {
            // 유저가 고른 쪽의 뽑아두었던 n값(층수)을 가져옴
            int n = (selected_idx == left_idx) ? left_n : right_n;
            damage = n * 4; // n의 값에 비례하여 데미지 계산 (예: 1층당 HP 4씩 감소)
        }
        else if (strstr(choices[selected_idx].text, "속도") != NULL)
        {
            int n = (selected_idx == left_idx) ? left_n : right_n;
            damage = n * 2;
        }
        else {
            // 일반 선택지의 경우 설정된 min_damage 와 max_damage 사이에서 랜덤으로 데미지를 정함
            int min = choices[selected_idx].min_damage;
            int max = choices[selected_idx].max_damage;
            damage = (rand() % (max - min + 1)) + min;
        }

        hp -= damage; // 플레이어의 현재 체력에서 계산된 데미지를 차감
        score += 1;   // 무사히(?) 라운드를 한 턴 버텼으므로 점수 1 증가

        // 유저에게 입은 데미지 결과를 잠시 보여주는 화면 갱신
        system("cls"); // 화면 한 번 지우고
        move_cursor(50, 12); // 중앙으로 가서
        printf("선택 완료! HP가 %d 감소했습니다.", damage); // 결과 출력
        Sleep(2000); // 사용자가 읽을 수 있도록 2초(2000ms) 대기 후 다음 라운드로 루프 반복
    } // while(hp > 0) 끝

    // HP가 0 이하가 되어 반복문을 빠져나온 경우 (게임 오버 처리)
    system("cls");
    set_color(FONT_COLOR_RED);
    move_cursor(54, 12);
    printf("GAME OVER"); // 강렬하게 붉은색 게임오버 표시

    set_color(FONT_COLOR_WHITE);
    move_cursor(50, 14);
    printf("최종 버틴 점수 : %d", score); // 그동안 버틴 횟수(최종 점수) 출력

    move_cursor(43, 18);
    printf("Backspace를 누르면 메뉴로 돌아갑니다."); // 메뉴 복귀 안내

    // Backspace를 누를 때까지 게임오버 화면을 유지하는 무한 대기 루프
    while (1)
    {
        key = _getch();
        if (key == 8) // Backspace 입력 시
        {
            break; // 루프 탈출
        }
    }

    system("cls"); // 지우고
    return 0;      // Gamestart 함수 끝, 메인메뉴 복귀
}

// 메인 메뉴 4번 '게임 종료'를 눌렀을 때 실행되는 독특한 게임오버(엔딩) 크레딧 연출 함수
int Gameover(void)
{
    int y = 30;  // 텍스트가 올라올 첫 번째 y 좌표 초기화
    int yy = 30; // 텍스트가 올라올 두 번째 y 좌표 초기화
    int While = 1; // 첫 번째 루프의 실행 조건 (1=참)
    int Thile = 1; // 두 번째 루프의 실행 조건 (1=참)

    system("cls"); // 화면 지우기

    // 첫 번째 줄의 텍스트가 화면 밑바닥(y=30)에서부터 y=1까지 스르륵 올라가는 연출
    while (While)
    {
        if (y != 1)
        {
            system("cls"); // 매 프레임마다 이전 화면 지움
            move_cursor(42, y); // 텍스트 출력 위치를 한 칸 위로 갱신
            printf("여기에 마무리 되는거 추가로 넣고 꺼지게 하기");
            y--; // y좌표 1 감소 (화면상으로는 위로 이동)

            Sleep(100); // 0.1초마다 갱신하여 애니메이션 효과 부여

            if (y == 1) // 꼭대기에 도달하면
            {
                While = 0; // 루프 탈출
            }
        }
    }

    // 두 번째 줄의 텍스트가 밑에서부터 y=3까지 스르륵 올라오는 연출 (첫 번째 줄은 y=1에 고정)
    while (Thile)
    {
        if (yy != 3)
        {
            system("cls");
            move_cursor(42, y); // 위에서 꼭대기(y=1)에 멈춰있는 첫 번째 줄 계속 그림
            printf("여기에 마무리 되는거 추가로 넣고 꺼지게 하기");

            move_cursor(42, yy); // 밑바닥부터 올라오는 두 번째 줄 그리기
            printf("여기에 마무리 되는거 추가로 넣고 꺼지게 하기");
            yy--; // 두 번째 줄의 y좌표 1 감소 (위로 이동)

            Sleep(100); // 0.1초 딜레이

            if (yy == 3) // 목적지인 y=3에 도달하면
            {
                Thile = 0; // 두 번째 루프 탈출
            }
        }
    }

    Sleep(10000); // 두 줄이 완성된 화면 상태에서 10초(10000ms) 동안 대기하며 감상하게 함

    exit(0); // C 표준 라이브러리 함수를 호출하여 콘솔 프로그램(게임) 자체를 완전히 종료함
}

// 매개변수로 ANSI 컬러 코드를 받아 콘솔 텍스트의 색상을 바꿔주는 유틸리티 함수
void set_color(int code)
{
    printf("\x1b[%dm", code); // 콘솔에 ANSI 이스케이프 코드를 쏘아 터미널 색상을 변경
}

// 매개변수로 받은 X, Y 좌표값을 통해 ANSI 이스케이프 코드로 콘솔의 커서를 이동시키는 유틸리티 함수
int move_cursor(int x, int y)
{
    printf("\033[%d;%dH", y, x); // y행, x열로 이동하는 ANSI 코드 출력
    return 0;
}

// C 프로그램의 실제 시작점(Entry Point)
int main(void)
{
    // [중요] 프로그램 내내 콘솔 출력을 UTF-8 모드로 고정. (ta.txt 등 읽을 때 한글 특수문자 깨짐 방지용)
    SetConsoleOutputCP(CP_UTF8);

    int gameStatus = 0; // 현재 진행해야 하는 게임의 상태 및 메뉴 코드를 기억할 변수

    ShowLogo(); // 게임 실행 직후, 웅장한 로고 및 "전체화면" 안내 메시지를 한 번 띄움

    // isRunning 플래그가 참(1)인 동안은 게임 메인 메뉴가 꺼지지 않고 계속 동작하는 무한루프
    while (isRunning)
    {
        // gameStatus 변수의 값에 따라 어떤 화면(함수)을 실행할지 결정
        switch (gameStatus)
        {
        case 0:
            // 0일 땐 메인 메뉴를 그리고, 사용자가 고른 메뉴의 반환값(1~4)을 gameStatus에 대입함
            gameStatus = RenderTitle();
            break;

        case 1:
            // 반환값이 1이면, [교체된] txt 파일 기반 '만든 사람 및 팀 소개' 로직 진입
            gameStatus = People();
            break;
        case 2:
            // 반환값이 2이면, 2페이지짜리 '설명서' 화면 진입
            gameStatus = Manual();
            break;
        case 3:
            // 반환값이 3이면, 실제 게임 플레이 루프 (Gamestart) 진입
            gameStatus = Gamestart();
            break;
        case 4:
            // 반환값이 4이면, 게임 오버/엔딩 크레딧 화면(Gameover) 재생 후 자동 프로그램 종료
            gameStatus = Gameover();
            break;
        }
    }

    // (ESC 등을 눌러 isRunning이 0이 되어 정상적으로 while을 빠져나온 경우)
    system("cls"); // 콘솔 화면을 완전히 지움
    move_cursor(0, 25); // 마지막 안내(혹은 종료 메시지)가 깔끔하게 보이도록 콘솔 밑으로 커서를 치움

    return 0; // 프로그램 운영체제에 정상 종료(0) 신호를 반환하며 프로그램 마무리
}