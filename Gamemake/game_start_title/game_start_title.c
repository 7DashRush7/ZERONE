#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int main(void) {
    // ta.txt 파일을 읽기 모드("r")로 열기
    FILE* fp = fopen("ta.txt", "r");
    char buffer[1024]; // 한 줄을 읽어올 임시 저장 공간 (넉넉하게 1024바이트 설정)

    // 파일이 정상적으로 열렸는지 확인
    if (fp == NULL) {
        printf("오류: ta.txt 파일을 찾을 수 없습니다.\n");
        printf("프로젝트 폴더 내에 파일이 있는지 확인해주세요.\n");
        return 1;
    }

    // 파일의 끝(EOF)에 도달할 때까지 한 줄씩 읽어서 출력
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);
    }

    // 파일 닫기
    fclose(fp);

    return 0;
}