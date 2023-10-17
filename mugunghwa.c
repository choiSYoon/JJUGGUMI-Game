#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"
#include <stdio.h>

#define DIR_UP		0
#define DIR_DOWN	1
#define DIR_LEFT	2
#define DIR_RIGHT	3

void game_map_init(void);
void move_manual(key_t key);
void move_random(int i, int dir);
void move_tail(int i, int nx, int ny);

int px[PLAYER_MAX], py[PLAYER_MAX], period[PLAYER_MAX];  // 각 플레이어 위치, 이동 주기
int str_intro = 0;

void game_map_init(void) {
	map_init(9, 35);
	int x, y;
	for (int i = 0; i < n_player; i++) {
		// 같은 자리가 나오면 다시 생성
		do {
			x = randint(1, N_ROW - 2);
			y = randint(1, N_COL - 2);
		} while (!placable(x, y));
		px[i] = x;
		py[i] = y;
		period[i] = randint(100, 500);

		back_buf[px[i]][py[i]] = '0' + i;  // (0 .. n_player-1)
	}

	tick = 0;
}

void move_manual(key_t key) {
	// 각 방향으로 움직일 때 x, y값 delta
	static int dx[4] = { -1, 1, 0, 0 };
	static int dy[4] = { 0, 0, -1, 1 };

	int dir;  // 움직일 방향(0~3)
	switch (key) {
	case K_UP: dir = DIR_UP; break;
	case K_DOWN: dir = DIR_DOWN; break;
	case K_LEFT: dir = DIR_LEFT; break;
	case K_RIGHT: dir = DIR_RIGHT; break;	
	default: return;
	}

	// 움직여서 놓일 자리
	int nx, ny;
	nx = px[0] + dx[dir];
	ny = py[0] + dy[dir];
	if (!placable(nx, ny)) {
		return;
	}

	move_tail(0, nx, ny);
}

// 0 <= dir <= 4가 아니면 랜덤
void move_random(int player, int dir) {
	int p = player;  // 이름이 길어서...
	int nx, ny;  // 움직여서 다음에 놓일 자리

	// 움직일 공간이 없는 경우는 없다고 가정(무한 루프에 빠짐)	

	do {
		if (dir == 0) {
			nx = px[p] + 1;
			ny = py[p];
		}
		else if (dir == 1) {
			nx = px[p] - 1;
			ny = py[p];
		}
		else if (dir == 2) {
			nx = px[p];
			ny = py[p] - 1;
		}
		else if (dir == 3) {
			nx = px[p];
			ny = py[p] + 1;
		}
		else if (dir == 4) {
			nx = px[p];
			ny = py[p];
		}
		else {
			nx = px[p] + randint(-1, 1);
			ny = py[p] + randint(-1, 0);
		}

		if (!placable(nx, ny)) { // 가야할 공간이 없으면 랜덤으로 다시 지정 단 뒤로는 없음
			nx = px[p] + randint(-1, 1);
			ny = py[p] + randint(-1, 0);
		}
	} while (!placable(nx, ny));
	move_tail(p, nx, ny);
}

// back_buf[][]에 기록
void move_tail(int player, int nx, int ny) {
	int p = player;  // 이름이 길어서...
	back_buf[nx][ny] = back_buf[px[p]][py[p]];
	back_buf[px[p]][py[p]] = ' ';
	px[p] = nx;
	py[p] = ny;
}

void tagger(void) { // 술래 배치
	back_buf[3][1] = '#';
	back_buf[4][1] = '#';
	back_buf[5][1] = '#';
}

int random_move(void) {
	int rand_move, last_move;
	rand_move = rand() % 99; //0~99 랜덤한 숫자
	if (rand_move <= 69) { // 70% 확률로 왼쪽으로 이동
		return 2;
	}
	else if (70 <= rand_move && rand_move <= 79) { // 10% 확률로 위쪽으로 이동
		return 0;
	}
	else if (80 <= rand_move && rand_move <= 89) { // 10% 확률로 아래쪽으로 이동
		return 1;
	}
	else { // 10% 확률로 제자리
		return 4;
	}
}

void finish_line(void) {
	for (int i = 0; i < n_player; i++) {
		if (1 < px[i] && px[i] <= 6 && py[i] <= 2) {
			if (!(px[i] == 2 && py[i] == 2 || px[i] == 6 && py[i] == 2)) {
				back_buf[1][1] = 'F';
			}
		}
	}
}

void reload(void) { //맵 다시로드
	system("cls");
	map_init(9, 35);
	tagger();
	for (int i = 0; i < n_player; i++) {
		back_buf[px[i]][py[i]] = '0' + i;
	}
}

void comment(void) {
	char intro[] = "무궁화 꽃이 피었습니다";
	if (tick % (str_intro < 11 ? 1000 : 200) == 0) {
		gotoxy(N_ROW, str_intro);
		if (intro[str_intro] < 0) {
			printf("%c%c", intro[str_intro], intro[str_intro + 1]);
			str_intro += 2;
		}
		else {
			printf("%c", intro[str_intro]);
			str_intro++;
		}
	}
}

void start_game(void) { //모든 플레이어를 출발선으로 배치, 술래 배치
	tagger();
	for (int i = 0; i < n_player; i++) {
		move_tail(i, 3 + i, 33);
	}
}

void mugunghwa(void) {
	game_map_init();
	system("cls");
	display();
	start_game();
	while (1) {
		// player 0만 손으로 움직임(4방향)
		key_t key = get_key();
		if (key == K_QUIT) {
			break;
		}
		else if (key != K_UNDEFINED) {
			move_manual(key);
		}

		for (int i = 1; i < n_player; i++) {
			if (tick % period[i] == 0) {
				move_random(i, random_move());
			}
		}
		finish_line();
		comment();
		display();
		Sleep(10);
		tick += 10;
		if (str_intro == 22) {
			str_intro = 0;
			reload();
		}
	}
}