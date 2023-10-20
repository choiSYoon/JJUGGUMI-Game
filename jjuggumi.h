#ifndef _JJUGGUMI_H_
#define _JJUGGUMI_H_

#include <Windows.h>
#include <stdbool.h>

#define PLAYER_MAX		7

bool player[PLAYER_MAX];  // 기본값 true, 탈락하면 false
bool player_clear[PLAYER_MAX]; // 기본값 true, 탈락하면 false
bool player_pause; // 기본값 true, 미니게임이 정지되지 않으면 false

int n_player, n_alive;
int tick;  // 시계

// 미니게임
//void sample(void);
//void mugunghwa(void);
//void mugunghwa(void);
//void nightgame(void);
//void juldarigi(void);
//void jebi(void);

int randint(int low, int high);

#endif
