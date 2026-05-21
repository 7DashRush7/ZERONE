#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <conio.h>
#include <stdlib.h> // rand(), srand(), system() 함수용
#include <time.h>   // time() 함수용

#define COLOR_RESET "\x1b[0m"

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

#define Backspace 8


#define SCREEN_WIDTH 120  // 저장할 콘솔 화면의 가로 크기
#define SCREEN_HEIGHT 30  // 저장할 콘솔 화면의 세로 크기


// 함수 원형 선언
void set_color(int code);
int move_cursor(int x, int y);
void ShowLogo(void);
int RenderTitle(void);
int People(void);
int Manual(void);
int Gamestart(void);
int Gameover(void);

// 전역 변수 설정
int menu = 1;
int isRunning = 1;

// 선택지 데이터를 담을 구조체 선언
typedef struct
{
    const char* art[6]; // 아스키아트 (최대 6줄)
    const char* text;   // 대사 한 줄
    int min_damage;     // 최소 데미지 (추가됨)
    int max_damage;     // 최대 데미지 (추가됨)
} Choice;


//이거 아래에 문장있고 그 뒤에 데미지 최솟값 최댓값이 있는데 저거 저런식으로말고 'n층에서 떨어졌다'로 하고 n을 변수도 두변서 n의 값에 따라 데미지를 받는 방식으로 하게 바꾸자


// 선택지 목록 (여기에 100개까지 계속 추가하시면 됩니다)
Choice choices[] =          //밑에서 바로 아래 중괄호로 묶인거 선택지
{
    { {"  /\\_/\\  ", " ( o.o ) ", "  > ^ <  ", "         ", "         ", "         "}, "귀여운 길고양이를 쓰다듬는다.", 1, 5 },//아스키아트(엔터할거면 "하고 쉼표), 밑에 선택지 말, 최소값 최댓값
    { {"   ___   ", "  / _ \\  ", " | (_) | ", "  \\___/  ", "         ", "         "}, "수상할 정도로 빨간 버튼을 누른다.", 3, 9 },
    { {"  ====   ", " |    |  ", " |    |  ", "  ====   ", "         ", "         "}, "자판기 밑에서 동전을 줍는다.", 0, 2 },
    { {"   \\|/   ", "  - O -  ", "   /|\\   ", "         ", "         ", "         "}, "태양을 맨눈으로 10초 동안 바라본다.", 8, 12 },
    { {"  [___]  ", "  |   |  ", "  |___|  ", "         ", "         ", "         "}, "유통기한이 3년 지난 통조림을 먹는다.", 5, 15 }
};



// 등록된 선택지의 총 개수 계산
int num_choices = sizeof(choices) / sizeof(Choice);

CHAR_INFO savedScreen[SCREEN_WIDTH * SCREEN_HEIGHT]; // 콘솔 화면 글자와 색 정보를 저장할 배열

void save_console_screen() // 현재 콘솔 화면을 저장하는 함수
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // 현재 콘솔 출력 화면의 핸들을 가져옴

    COORD bufferSize = { SCREEN_WIDTH, SCREEN_HEIGHT }; // 저장할 버퍼의 크기를 지정
    COORD bufferCoord = { 0, 0 }; // 버퍼의 시작 좌표를 왼쪽 위로 지정
    SMALL_RECT readRegion = { 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1 }; // 콘솔에서 읽어올 영역 지정

    ReadConsoleOutput( // 콘솔 화면 내용을 savedScreen 배열에 저장
        hConsole,      // 읽어올 콘솔 화면
        savedScreen,   // 화면 내용을 저장할 배열
        bufferSize,    // 저장할 배열의 크기
        bufferCoord,   // 배열에서 저장을 시작할 위치
        &readRegion    // 콘솔에서 읽어올 영역
    );
}

void restore_console_screen() // 저장했던 콘솔 화면을 다시 복원하는 함수
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // 현재 콘솔 출력 화면의 핸들을 가져옴

    COORD bufferSize = { SCREEN_WIDTH, SCREEN_HEIGHT }; // 복원할 버퍼의 크기를 지정
    COORD bufferCoord = { 0, 0 }; // 버퍼의 시작 좌표를 왼쪽 위로 지정
    SMALL_RECT writeRegion = { 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1 }; // 콘솔에 다시 쓸 영역 지정

    WriteConsoleOutput( // savedScreen 배열에 저장된 화면 내용을 콘솔에 다시 출력
        hConsole,       // 복원할 콘솔 화면
        savedScreen,    // 저장되어 있던 화면 배열
        bufferSize,     // 배열의 크기
        bufferCoord,    // 배열에서 읽기 시작할 위치
        &writeRegion    // 콘솔에 쓸 영역
    );
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
       "                                                   ",
       "  WWWWWW                                           ",
       " WWW   WWWBB                                       ",
       " WWW   WWWBB                                       ",
       " WWW   WWWBB   XXXXXX  XXXXX  XXXXX                 ",
       "  WWWWWWWBB        X   X   X  X                     ",
       "    BBBBB         X    XXXXX  XXXXX                 ",
       "                 X     X   X  X                     ",
       "               XXXXXX  X   X  XXXXX                 ",
       "                                                    ",
       "                 XXX   X   X  XXXXX                 ",
       "                X   X  XX  X  X                     ",
       "                X   X  X X X  XXXXX                 ",
       "                X   X  X  XX  X                     ",
       "                 XXX   X   X  XXXXX      WWW        ",
       "                                       WWWWWBB       ",
       "                                       WW WWBB       ",
       "                                          WWBB       ",
       "                                          WWBB       ",
       "                                      WWWWWWWWBB     ",
       "                                        BBBBBB       "
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

    Sleep(2000);
    system("cls");

    return 0;
}

int RenderTitle(void)
{

    set_color(BG_COLOR_BRIGHTMAGENTA);
    set_color(FONT_COLOR_WHITE);

    move_cursor(52, 9);
    printf("                       ");
    move_cursor(52, 10);
    printf("       이걸 죽네       ");
    move_cursor(52, 11);
    printf("                       ");

    set_color(BG_COLOR_BLACK);

    if (menu == 1)
    {
        set_color(BG_COLOR_YELLOW);
    }
    move_cursor(52, 13);
    printf("  1. 만든 사람 및 팀 소개  ");
    set_color(BG_COLOR_BLACK);

    if (menu == 2)
    {
        set_color(BG_COLOR_YELLOW);
    }
    move_cursor(52, 15);
    printf("  2. 설명서  ");
    set_color(BG_COLOR_BLACK);

    if (menu == 3)
    {
        set_color(BG_COLOR_YELLOW);
    }
    move_cursor(52, 17);
    printf("  3. 게임 시작  ");
    set_color(BG_COLOR_BLACK);

    if (menu == 4)
    {
        set_color(BG_COLOR_YELLOW);
    }
    move_cursor(52, 19);
    printf("  4. 게임 종료  ");
    set_color(BG_COLOR_BLACK);

    set_color(FONT_COLOR_YELLOW);
    move_cursor(106, 27);
    printf("↑: 위로 이동");
    move_cursor(106, 28);
    printf("↓: 밑으로 이동");
    move_cursor(106, 29);
    printf("Enter : 선택");
    move_cursor(106, 30);
    printf("ESC : 게임 종료");

    move_cursor(106, 100);

    char a = _getch(); // 경고 방지를 위해 getch()를 _getch()로 변경

    switch (a)
    {
    case 72: // ↑
        if (menu > 1)
        {
            menu = menu - 1;
        }
        break;
    case 80: // ↓
        if (menu < 4)
        {
            menu = menu + 1;
        }
        break;

    case 27: // ESC
        isRunning = 0;
        break;

    case 13: // ENTER
        if (menu >= 1 && menu <= 4)
        {
            return menu;
        }
        break;
    }

    return 0;
}


int People(void)
{

    system("cls");

    printf("\x1b[2J");

    int box_width = 50;
    int box_height = 30;

    int box_start_x = 40;
    int box_start_y = 5;

    for (int i = 0; i < box_height; i++) {
        printf("\x1b[%d;%dH", box_start_y + i, box_start_x);

        printf("\x1b[44m");
        for (int j = 0; j < box_width; j++) {
            printf(" ");
        }
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
    int title_start_x = box_start_x + (box_width - title_width) / 2;
    int title_start_y = box_start_y + (box_height - 11) / 2;

    for (int i = 0; i < 5; i++)
    {
        printf("\x1b[%d;%dH", title_start_y + i, title_start_x);
        for (int j = 0; j < title_width; j++)
        {
            if (title_text[i][j] == 'X')
            {
                printf("\x1b[40m ");
            }
            else {
                printf("\x1b[44m ");
            }
        }
        printf("\x1b[0m");
    }
    const char* team_text[] =
    {
        " team 01 (ZERONE) 팀원들 ",
        "마준서(202617166) : 총괄",
        "백종화(202617139) : 코드",
        "이인욱(202619389) : 코드",
        "이준현(202619549) : 디자인"
    };

    int team_start_x = box_start_x + (box_width - 24) / 2;
    int team_start_y = title_start_y + 5 + 1;

    for (int i = 0; i < 5; i++)
    {
        printf("\x1b[%d;%dH\x1b[30m\x1b[44m%s\x1b[0m", team_start_y + i, team_start_x, team_text[i]);
    }

    printf("\x1b[%d;1H\n", box_start_y + box_height + 1);

    move_cursor(48, 30);
    printf("Backspace를 눌러 메뉴로 돌아가시오");

    int key = 0;

    while (key != 8)
    {
        key = _getch();

        if (key == 27)
        {
            exit(0);
        }
    }

    system("cls");

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
            move_cursor(111, 50);
            printf("\033[1m다음장 (→)\033[0m");

            set_color(BG_COLOR_BRIGHTMAGENTA);
            set_color(FONT_COLOR_WHITE);
            move_cursor(48, 7);
            printf("=========== 설명서 ===========");

            set_color(BG_COLOR_BLACK);

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

            set_color(FONT_COLOR_YELLOW);
            move_cursor(43, 17);
            printf("Backspace");

            set_color(FONT_COLOR_WHITE);
            printf("를 눌러 메뉴로 돌아가시오");
        }
        if (Manual_page == 2)
        {
            move_cursor(0, 50);
            printf("\033[1m이전장 (←)\033[0m");

            move_cursor(103, 50);
            printf("\033[1m나가기 (Backspace)\033[0m");

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

        key = _getch();
        switch (key)
        {
        case 75: // ←
            if (Manual_page > 1)
            {
                Manual_page = Manual_page - 1;
            }
            break;
        case 77: // →
            if (Manual_page < 2)
            {
                Manual_page = Manual_page + 1;
            }
            break;

        case 27: // ESC
            exit(0);
            break;
        }
    }

    system("cls");
    return 0;
}

int Gamestart(void)
{
    // 난수 시드 초기화 (게임 시작 시 한 번만)
    srand((unsigned int)time(NULL));

    int hp = 100;    // 초기 HP
    int score = 0;   // 초기 점수
    int key = 0;

    // HP가 0보다 큰 동안 라운드 반복
    while (hp > 0)
    {
        system("cls");

        // 2개의 중복되지 않는 랜덤 선택지 뽑기
        int left_idx = rand() % num_choices;
        int right_idx;
        do {
            right_idx = rand() % num_choices;
        } while (left_idx == right_idx);

        // 상단 UI 출력 (HP, Score)
        set_color(FONT_COLOR_RED);
        move_cursor(40, 2);
        printf("HP : %d", hp);

        set_color(FONT_COLOR_WHITE);
        move_cursor(70, 2);
        printf("SCORE : %d", score);

        // 중앙 VS 출력
        set_color(FONT_COLOR_YELLOW);
        move_cursor(58, 12);
        printf("VS");
        set_color(FONT_COLOR_WHITE);

        // 왼쪽 선택지 출력
        for (int i = 0; i < 6; i++) {
            move_cursor(25, 8 + i);
            printf("%s", choices[left_idx].art[i]);
        }
        move_cursor(15, 18);
        printf("%s", choices[left_idx].text);

        // 오른쪽 선택지 출력
        for (int i = 0; i < 6; i++) {
            move_cursor(80, 8 + i);
            printf("%s", choices[right_idx].art[i]);
        }
        move_cursor(70, 18);
        printf("%s", choices[right_idx].text);

        // 안내 문구 출력
        set_color(FONT_COLOR_GREEN);
        move_cursor(35, 25);
        printf("방향키(←, →)로 선택하세요. (메뉴로 가기: Backspace)");
        set_color(FONT_COLOR_WHITE);

        // 플레이어 입력 대기 및 선택 판별
        int has_selected = 0;
        int selected_idx = 0; // 유저가 최종적으로 고른 선택지의 인덱스


        while (!has_selected)
        {
            key = _getch();

            if (key == 224) // 방향키 확장 코드
            {
                key = _getch();
                if (key == 75) // 왼쪽 (←)
                {
                    selected_idx = left_idx;
                    has_selected = 1;
                }
                else if (key == 77) // 오른쪽 (→)
                {
                    selected_idx = right_idx;
                    has_selected = 1;
                }
            }

            else if (key == 8) // Backspace
            {
                save_console_screen(); // Backspace를 눌렀을 때, 확인창 띄우기 직전 화면 저장



                while (1)
                {
                    set_color(BG_COLOR_BLACK);
                    move_cursor(20, 7);
                    printf("                                                                                           \n                                                                                           \n                                                                                           \n                                                                                           \n                                                                                           \n                                                                                           \n                                                                                           \n");
                    set_color(FONT_COLOR_RED);
                    move_cursor(50, 12);
                    printf("게임을 중지하시겠습니까?");
                    move_cursor(40, 15);
                    printf("게임을 계속하려면 t, 중지하려면 r를 누르시오.");

                    key = _getch();

                    if (key == 'r')
                    {
                        system("cls");
                        return 0;
                    }
                    if (key == 't')
                    {
                        restore_console_screen(); // Backspace 누르기 직전에 저장한 화면을 복원
                        break; // while문을 빠져나가고 게임을 계속 진행                        
                    }
                }
            }
            else if (key == 27) // ESC
            {
                exit(0); // 게임 강제 종료
            }
        }


        // 선택한 항목의 지정된 범위 안에서 데미지 계산
        int min = choices[selected_idx].min_damage;
        int max = choices[selected_idx].max_damage;

        // rand() % (최대값 - 최소값 + 1) + 최소값 -> 지정된 범위 난수 공식
        int damage = (rand() % (max - min + 1)) + min;

        hp -= damage;
        score += 1; // 라운드 통과 시 점수 증가

        // 데미지 입은 결과 잠깐 보여주기
        system("cls");
        move_cursor(50, 12);
        printf("선택 완료! HP가 %d 감소했습니다.", damage);
        Sleep(2000);
    }

    // 게임 오버 처리
    system("cls");
    set_color(FONT_COLOR_RED);
    move_cursor(54, 12);
    printf("GAME OVER");

    set_color(FONT_COLOR_WHITE);
    move_cursor(50, 14);
    printf("최종 버틴 점수 : %d", score);

    move_cursor(43, 18);
    printf("Backspace를 누르면 메뉴로 돌아갑니다.");



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
    int y = 30;
    int yy = 30;
    int While = 1;
    int Thile = 1;

    system("cls");
    while (While)
    {
        if (y != 1)
        {
            system("cls");
            move_cursor(42, y);
            printf("여기에 마무리 되는거 추가로 넣고 꺼지게 하기");
            y = y--;

            Sleep(100);

            if (y == 1)
            {
                While = 0;
            }
        }

    }

    while (Thile)
    {
        if (yy != 3)
        {
            system("cls");
            move_cursor(42, y);
            printf("여기에 마무리 되는거 추가로 넣고 꺼지게 하기");
            move_cursor(42, yy);
            printf("여기에 마무리 되는거 추가로 넣고 꺼지게 하기");
            yy = yy--;

            Sleep(100);

            if (yy == 3)
            {
                Thile = 0;
            }
        }

    }



    Sleep(10000);

    exit(0);
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

int main(void)
{
    int gameStatus = 0;

    ShowLogo();

    while (isRunning)
    {
        switch (gameStatus)
        {
        case 0:
            gameStatus = RenderTitle();
            break;

        case 1:
            gameStatus = People();
            break;
        case 2:
            gameStatus = Manual();
            break;
        case 3:
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