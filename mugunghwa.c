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

int px[PLAYER_MAX], py[PLAYER_MAX], period[PLAYER_MAX];  // �� �÷��̾� ��ġ, �̵� �ֱ�
int str_intro = 0;

void game_map_init(void) {
	map_init(9, 35);
	int x, y;
	for (int i = 0; i < n_player; i++) {
		// ���� �ڸ��� ������ �ٽ� ����
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
	// �� �������� ������ �� x, y�� delta
	static int dx[4] = { -1, 1, 0, 0 };
	static int dy[4] = { 0, 0, -1, 1 };

	int dir;  // ������ ����(0~3)
	switch (key) {
	case K_UP: dir = DIR_UP; break;
	case K_DOWN: dir = DIR_DOWN; break;
	case K_LEFT: dir = DIR_LEFT; break;
	case K_RIGHT: dir = DIR_RIGHT; break;	
	default: return;
	}

	// �������� ���� �ڸ�
	int nx, ny;
	nx = px[0] + dx[dir];
	ny = py[0] + dy[dir];
	if (!placable(nx, ny)) {
		return;
	}

	move_tail(0, nx, ny);
}

// 0 <= dir <= 4�� �ƴϸ� ����
void move_random(int player, int dir) {
	int p = player;  // �̸��� ��...
	int nx, ny;  // �������� ������ ���� �ڸ�

	// ������ ������ ���� ���� ���ٰ� ����(���� ������ ����)	

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

		if (!placable(nx, ny)) { // ������ ������ ������ �������� �ٽ� ���� �� �ڷδ� ����
			nx = px[p] + randint(-1, 1);
			ny = py[p] + randint(-1, 0);
		}
	} while (!placable(nx, ny));
	move_tail(p, nx, ny);
}

// back_buf[][]�� ���
void move_tail(int player, int nx, int ny) {
	int p = player;  // �̸��� ��...
	back_buf[nx][ny] = back_buf[px[p]][py[p]];
	back_buf[px[p]][py[p]] = ' ';
	px[p] = nx;
	py[p] = ny;
}

void tagger(void) { // ���� ��ġ
	back_buf[3][1] = '#';
	back_buf[4][1] = '#';
	back_buf[5][1] = '#';
}

int random_move(void) {
	int rand_move, last_move;
	rand_move = rand() % 99; //0~99 ������ ����
	if (rand_move <= 69) { // 70% Ȯ���� �������� �̵�
		return 2;
	}
	else if (70 <= rand_move && rand_move <= 79) { // 10% Ȯ���� �������� �̵�
		return 0;
	}
	else if (80 <= rand_move && rand_move <= 89) { // 10% Ȯ���� �Ʒ������� �̵�
		return 1;
	}
	else { // 10% Ȯ���� ���ڸ�
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

void reload(void) { //�� �ٽ÷ε�
	system("cls");
	map_init(9, 35);
	tagger();
	for (int i = 0; i < n_player; i++) {
		back_buf[px[i]][py[i]] = '0' + i;
	}
}

void comment(void) {
	char intro[] = "����ȭ ���� �Ǿ����ϴ�";
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

void start_game(void) { //��� �÷��̾ ��߼����� ��ġ, ���� ��ġ
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
		// player 0�� ������ ������(4����)
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