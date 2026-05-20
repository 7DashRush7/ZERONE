#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <stdlib.h> // system() 함수를 사용하기 위한 헤더

void gotoxy(int x, int y) {
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

int main(void) {
    // ★ 핵심 추가: 콘솔 창의 가로 글자 수를 210칸, 세로를 60칸으로 넉넉하게 강제 설정합니다.
    // 이렇게 하면 텍스트가 오른쪽 끝에 부딪혀서 깨지는 현상을 막을 수 있습니다.
    system("mode con cols=210 lines=60");

    FILE* fp = fopen("ta.txt", "r");
    char buffer[1024];

    // 시작 좌표 (X: 40, Y: 10)
    int start_x = 40;
    int current_y = 10;

    if (fp == NULL) {
        printf("오류: ta.txt 파일을 찾을 수 없습니다.\n");
        return 1;
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        // 엔터(\n, \r) 제거
        buffer[strcspn(buffer, "\r\n")] = 0;

        gotoxy(start_x, current_y);
        printf("%s", buffer);
        current_y++;
    }

    fclose(fp);

    // 그림 밑으로 커서 이동시켜서 종료 메시지와 겹치지 않게 함
    gotoxy(0, current_y + 2);

    return 0;
}