#include <stdio.h>
#include <windows.h>
#include <conio.h>

int main() 
{
    // ==========================================
    // 1. 프로그램 시작 즉시 콘솔 창을 전체 화면(최대화)으로 강제 전환
    // ==========================================
    
    
    // 창이 최대화되고 버퍼 크기가 갱신될 때까지 아주 잠깐(0.1초) 대기합니다.
    Sleep(100); 

    // ==========================================
    // 2. 최대화된 터미널의 전체 화면 크기 가져오기
    // ==========================================
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    int columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    // ==========================================
    // 3. 전체 화면 파란색(44)으로 초기화
    // ==========================================
    printf("\x1b[44m"); // 배경색 파란색 설정
    printf("\x1b[2J");  // 화면 전체 지우기 (전체화면이 파란색으로 꽉 채워짐)

    // ==========================================
    // 4. 미니멀 3D + ZAE/ONE 두 줄 픽셀 아트 
    // ==========================================
    const char* logo[] = {
        "                                                    ",
        "  WWWWWW                                            ",
        " WWW   WWWBB                                        ",
        " WWW   WWWBB                                        ",
        " WWW   WWWBB   XXXXXX  XXXXX  XXXXX                 ",
        "  WWWWWWWBB        X   X   X  X                     ",
        "    BBBBB         X    XXXXX  XXXXX                 ",
        "                 X     X   X  X                     ",
        "                XXXXXX X   X  XXXXX                 ",
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

    // 넓어진 전체화면 기준으로 정중앙 좌표 동적 계산
    int logo_start_x = (columns - logo_width) / 2;
    int logo_start_y = (rows - logo_height) / 2;

    // 창이 너무 작을 때를 대비한 최소 좌표 방어코드
    if (logo_start_x < 1) logo_start_x = 1;
    if (logo_start_y < 1) logo_start_y = 1;

    // 색상 적용 출력 반복문
    for (int i = 0; i < logo_height; i++) {
        printf("\x1b[%d;%dH", logo_start_y + i, logo_start_x);
        for (int j = 0; j < logo_width; j++) {
            if (logo[i][j] == 'W') {
                printf("\x1b[47m "); // 흰색 입체 면
            }
            else if (logo[i][j] == 'B' || logo[i][j] == 'X') {
                printf("\x1b[40m "); // 검정색 그림자 및 픽셀 글자
            }
            else {
                printf("\x1b[44m "); // 파란색 배경 유지
            }
        }
        printf("\x1b[0m\x1b[44m"); // 텍스트 속성 초기화 후 파란색 배경 유지
    }

    // ==========================================
    // 5. 화면 밀림(스크롤) 방지 및 대기
    // ==========================================
    printf("\x1b[%d;1H", rows); 
    
    // 키 입력 대기 (이 상태에서 완벽한 풀화면 로고를 감상할 수 있습니다)
    _getch(); 

    // 프로그램 종료 전 터미널 속성 완벽 복구
    printf("\x1b[0m"); 
    printf("\x1b[2J"); 
    printf("\x1b[1;1H");

    return 0;
}