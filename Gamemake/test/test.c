#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <conio.h>

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

void set_color(int code);
int move_cursor(int x, int y);
void ShowLogo(void);
int RenderTitle(void);
int People(void);
int Manual(void);
int Gamestart(void);
int Gameover(void);

int menu = 1;
int isRunning = 1;

void ShowLogo(void)
{
    printf("\x1b[2J");

    int box_width = 60;
    int box_height = 24;
    int box_start_x = 37;
    int box_start_y = 5;

    for (int i = 0; i < box_height; i++) {
        printf("\x1b[%d;%dH", box_start_y + i, box_start_x);
        printf("\x1b[44m");
        for (int j = 0; j < box_width; j++) {
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

    for (int i = 0; i < logo_height; i++) {
        printf("\x1b[%d;%dH", logo_start_y + i, logo_start_x);
        for (int j = 0; j < logo_width; j++) {
            if (logo[i][j] == 'W') {
                printf("\x1b[47m ");
            }
            else if (logo[i][j] == 'B' || logo[i][j] == 'X') {
                printf("\x1b[40m ");
            }
            else {
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

    char a = getch();

    switch (a)
    {
    case 'w':
    case 72: // ↑
        if (menu > 1)
        {
            menu = menu - 1;
        }
        break;
    case 's':
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

    for (int i = 0; i < 5; i++) {
        printf("\x1b[%d;%dH", title_start_y + i, title_start_x);
        for (int j = 0; j < title_width; j++) {
            if (title_text[i][j] == 'X') {
                printf("\x1b[40m ");
            }
            else {
                printf("\x1b[44m ");
            }
        }
        printf("\x1b[0m");
    }
    const char* team_text[] = {
        " team 01 (ZERONE) 팀원들 ",
        "마준서(202617166) : 총괄",
        "백종화(202617139) : 코드",
        "이인욱(202619389) : 코드",
        "이준현(202619549) : 디자인"
    };

    int team_start_x = box_start_x + (box_width - 24) / 2;
    int team_start_y = title_start_y + 5 + 1;

    for (int i = 0; i < 5; i++) {
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
            move_cursor(107, 50);
            printf("\033[1m다음장 (→ , D)\033[0m");

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
            printf("\033[1m이전장 (← , A)\033[0m");

            move_cursor(60, 7);
            printf("\033[1m키 설명\033[0m");
            move_cursor(53, 12);
            printf("W , ↑: 위로 이동");
            move_cursor(53, 13);
            printf("S , ↓: 밑으로 이동");
            move_cursor(53, 14);
            printf("A , ←: 왼쪽 선택");
            move_cursor(53, 15);
            printf("D , →: 오른쪽 선택");
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
        case 'a':
        case 75: // ←
            if (Manual_page > 1)
            {
                Manual_page = Manual_page - 1;
            }
            break;
        case 'd':
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
    system("cls");

    move_cursor(52, 13);
    printf("여기는 게임 시작. 메뉴로 돌아갈때 Backspace 누르기. 일단은 Backspace 누르고 나중에 키 지정하고 게임 여기에 만들기");

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

int Gameover(void)
{
    system("cls");

    move_cursor(52, 13);
    printf("여기에 마무리 되는거 추가로 넣고 꺼지게 하기");

    Sleep(2000);

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