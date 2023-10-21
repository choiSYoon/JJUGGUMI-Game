// 2023-2 고급프로그래밍 과제: 쭈꾸미 게임
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "jjuggumi.h"

#define	DATA_FILE	"jjuggumi.dat"

int jjuggumi_init(void);

// low 이상 high 이하 난수를 발생시키는 함수
int randint(int low, int high) {
	int rnum = rand() % (high - low + 1) + low;
	return rnum;
}

int jjuggumi_init(void) {
	srand((unsigned int)time(NULL));

	printf("플레이어 수: ");
	scanf_s("%d", &n_player);

	n_alive = n_player;
	for (int i = 0; i < n_player; i++) {
		player[i] = true;
	}
	return 0;
}

int intro(void) { /// 인트로 작성, 5초 내로 끝내기
	printf("================쭈꾸미 게임================\n");
	Sleep(300);
	printf("         *   *****        *\n");
	printf("        **  *******        **\n");
	printf("        ** *********        **\n");
	printf("       *** *  **  **        ***\n");
	printf("       *** *********        ***\n");
	printf("        **  *******         ***\n");
	printf("        **   *****          **\n");
	printf("         *    ***           **\n");
	printf("         *******************\n");
	printf("         *****      *******\n");
	Sleep(300);
	printf("===========================================\n");
	Sleep(1000);
	
	system("cls");
	printf("================GAME START================\n");
	for (int i = 0; i < 4; i++) {
		printf("                     *                      \n");
		Sleep(300);
	}
	return 0;
}

int ending(void) {//우승자 출력
	int winner = 0;
	for (int i = 0; i < n_player; i++) {
		if (player[i] == true) { winner++; }
	}
	if (winner >= 2 || winner == 0) {
		printf("우승자를 가리지 못했습니다.\n");
	}
	else {
		for (int i = 0; i < n_player; i++) {
			if (player[i] == true) { printf("우승자는 %d 번 플레이어 입니다.\n", i); }
		}
	}
	
	return 0;
}

int main(void) {
	jjuggumi_init();
	intro();
	mugunghwa();
	//nightgame();
	//juldarigi();
	//jebi();

	ending();
	return 0;
}
