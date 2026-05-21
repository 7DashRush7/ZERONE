#pragma execution_character_set("utf-8") // [추가됨] 코드 내 한글 깨짐 완벽 방지!
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>

// 1. 개별 팀원 아스키아트 및 설명 출력 함수
void print_member_page(const char* filename, const char* description) {
    printf("\x1b[2J\x1b[H");

    FILE* file = fopen(filename, "r");
    if (file != NULL) {
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), file)) {
            printf("%s", buffer);
        }
        fclose(file);
    }
    else {
        printf("\n[오류] '%s' 파일을 찾을 수 없습니다.\n", filename);
        printf("프로젝트 폴더 안에 파일 이름이 정확히 %s 인지 확인해주세요.\n\n", filename);
    }

    printf("\n\n==================================================\n");
    printf("  %s\n", description);
    printf("==================================================\n");
    printf("\n[ <- 이전 페이지 ]                     [ 다음 페이지 -> ]\n");
}

// 2. 마지막 ZERONE 로고 화면 출력 함수
void draw_final_screen() {
    printf("\x1b[2J\x1b[H");

    int box_width = 50;
    int box_height = 30;

    int box_start_x = 40;
    int box_start_y = 5;

    // 배경 그리기
    for (int i = 0; i < box_height; i++) {
        printf("\x1b[%d;%dH", box_start_y + i, box_start_x);
        printf("\x1b[44m");
        for (int j = 0; j < box_width; j++) {
            printf(" ");
        }
        printf("\x1b[0m");
    }

    // 픽셀 아트 ZERONE 로고
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

    // 팀원 명단
    const char* team_text[] = {
        " team 01 (ZERONE) 팀원들",
        "마준서(202619549) : 총괄",
        "백종화(202617139) : 코드",
        "이인욱(202619389) : 코드",
        "이준현(202619549) : 디자인"
    };

    int team_start_x = box_start_x + (box_width - 24) / 2;
    int team_start_y = title_start_y + 5 + 1;

    for (int i = 0; i < 5; i++) {
        printf("\x1b[%d;%dH\x1b[30m\x1b[44m%s\x1b[0m", team_start_y + i, team_start_x, team_text[i]);
    }

    // 마무리 및 네비게이션 안내
    printf("\x1b[%d;1H\n", box_start_y + box_height + 1);
    printf("[ <- 이전 페이지 ]                               [ ESC 종료 ]\n");
}

int main() {
    // 콘솔 출력을 UTF-8 모드로 강제 변경 (특수 문자 깨짐 방지)
    SetConsoleOutputCP(CP_UTF8);

    const char* filenames[] = {
        "1.txt",
        "2.txt",
        "3.txt",
        "4.txt"
    };

    const char* descriptions[] = {
        "마준서(202619549) : 총괄",
        "백종화(202617139) : 코드",
        "이인욱(202619389) : 코드",
        "이준현(202619549) : 디자인"
    };

    int current_page = 0;
    int total_pages = 5;

    while (1) {
        if (current_page < 4) {
            print_member_page(filenames[current_page], descriptions[current_page]);
        }
        else if (current_page == 4) {
            draw_final_screen();
        }

        int ch = _getch();
        if (ch == 224 || ch == 0) {
            ch = _getch();

            if (ch == 75) {         // 왼쪽 화살표
                if (current_page > 0) current_page--;
            }
            else if (ch == 77) {  // 오른쪽 화살표
                if (current_page < total_pages - 1) current_page++;
            }
        }
        else if (ch == 27) {      // ESC 키
            break;
        }
    }

    printf("\x1b[2J\x1b[H");
    return 0;
}