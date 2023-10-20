#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"
#include <stdio.h>

#define DIR_UP		0
#define DIR_DOWN	1
#define DIR_LEFT	2
#define DIR_RIGHT	3

void gamemap_init(void);
void move_manual(key_t key);
void move_random(int i, int dir);
void move_tail(int i, int nx, int ny);

int px[PLAYER_MAX], py[PLAYER_MAX], period[PLAYER_MAX], prex[PLAYER_MAX], prey[PLAYER_MAX];  // 각 플레이어 위치, 이동 주기
int str_intro = 0, tagger_front = 1;


void gamemap_init(void) {
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
void move_tail(int player2, int nx, int ny) {
	int p = player2;  // 이름이 길어서...
	if (player_clear[p] == false && player[p] == true && player_pause == false) {
		back_buf[nx][ny] = back_buf[px[p]][py[p]];
		back_buf[px[p]][py[p]] = ' ';
		px[p] = nx;
		py[p] = ny;
	}
}

void tagger(int n) { // 술래 배치
	if (n == 1) {
		back_buf[3][1] = '@';
		back_buf[4][1] = '@';
		back_buf[5][1] = '@';
		
	}
	else {
		back_buf[3][1] = '#';
		back_buf[4][1] = '#';
		back_buf[5][1] = '#';
	}

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
				player_clear[i] = true;
				back_buf[px[i]][py[i]] = ' ';
			}
		}
	}
}

void reload(void) { //맵 다시로드
	system("cls");
	map_init(9, 35);
	tagger(0);
	for (int i = 0; i < n_player; i++) {
		if (player[i] == true && player_clear[i] == false) {
			back_buf[px[i]][py[i]] = '0' + i;
		}
	}
}

void comment(void) {
	char intro[] = "무궁화 꽃이 피었습니다";
	if (tick % (str_intro < 11 ? 1000 : 200) == 0 && tagger_front == 1) {
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

void player_move_check(void) {
	if (tagger_front == 1) {
		for (int i = 0; i < n_player; i++) {
			prex[i] = px[i];
			prey[i] = py[i];
		}
	}
	else {
		for (int i = 0; i < n_player; i++) {
			if (px[i] != prex[i] || py[i] != prey[i]) {
				player[i] = false;
				back_buf[px[i]][py[i]] = ' ';
			}
		}
	}
}

void start_game(void) { //모든 플레이어를 출발선으로 배치, 술래 배치
	gamemap_init();
	system("cls");
	display();
	tagger(0);
	for (int i = 0; i < n_player; i++) {
		if (n_player >= 7) {
			move_tail(i, 1 + i, 33);
		}
		else if (n_player >= 5) {
			move_tail(i, 2 + i, 33);
		}
		else if (n_player >= 3) {
			move_tail(i, 3 + i, 33);
		}
		else {
			move_tail(i, 4 + i, 33);
		}
	}
	dialog("곧 게임이 시작됩니다.");
	reload(); // dialog가 비정상적 작동을 해서 맵 다시불러오는 임시 코드 (추후 삭제바람)
}

void mugunghwa(void) {
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
			if (tick % period[i] == 0 && tagger_front == 1) { // 뒤에 볼 때
				move_random(i, random_move());
			}
			else if (tick % period[i] == 0 && tagger_front == 0) { // 앞에 볼 때
				int rand_move;
				rand_move = rand() % 99; //0~99 랜덤한 숫자
				if (rand_move <= 9) { // 10% 확률로 움직임
					move_random(i, random_move());
				}
			}
		}
		finish_line();
		player_move_check();
		comment();
		display();
		Sleep(10);
		tick += 10;
		if (tick % 3000 == 0 && tagger_front == 0) {
			tagger_front = 1;
			reload();
		}
		if (str_intro == 22) {
			str_intro = 0;
			tagger_front = 0;
			tagger(1);
			tick = 0;
		}
	}
}