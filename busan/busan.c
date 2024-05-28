//20190889
//이윤석

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>

// 파라미터
#define LEN_MIN 15 // 기차 길이
#define LEN_MAX 50
#define STM_MIN 0 // 마동석 체력
#define STM_MAX 5
#define PROB_MIN 10 // 확률
#define PROB_MAX 90
#define AGGRO_MIN 0 // 어그로 범위
#define AGGRO_MAX 5

// 마동석 이동 방향
#define MOVE_LEFT 1
#define MOVE_STAY 0

// 좀비의 공격 대상
#define ATK_NONE 0
#define ATK_CITIZEN 1
#define ATK_DONGSEOK 2

// 마동석 행동
#define ACTION_REST 0
#define ACTION_PROVOKE 1
#define ACTION_PULL 2


int intro(void);
int setup(int* trainLength, int* stm, int* probability);
int printPattern(int len, int p_M, int p_Z, int p_C);
int printStatus(int len, int p_C, int m_p_C, int p_Z, int m_p_Z, int turn, int aggro);
int printMadongseok(int len, int p_M, int m_p_M, int turn, int aggro, int stm);
int moveCitizen(int p, int p_C);
int moveZombie(int p, int p_Z);
int moveMadongseok(int move, int p_M);
int success(void);
int gameover(void);

int main(void) {
	int game = 0;
	int trainLength = 0;
	int stm = 0;
	int probability = 0;
	int p_M = 2, p_Z = 3, p_C = 6;
	int m_p_M, m_p_Z, m_p_C;
	int turn = 0;
	int aggro = 1;
	int move = 0;

	srand((unsigned int)time(NULL));
	//인트로
	intro();

	//설정
	setup(&trainLength, &stm, &probability);

	//열차 초기 상태 출력
	printPattern(trainLength, p_M, p_Z, p_C);

	while (1) {
		//시민 이동
		m_p_C = moveCitizen(probability, p_C);

		//좀비 이동
		if(turn % 2 == 0) m_p_Z = moveZombie(probability, p_Z);

		//열차 상태 출력
		printPattern(trainLength, p_M, m_p_Z, m_p_C);

		//시민, 좀비 상태 출력
		printStatus(trainLength, p_C, m_p_C, p_Z, m_p_Z, turn, aggro);

		printf("madongseok move(0:stay, 1:left)>>");
		scanf_s("%d", &move);

		m_p_M = moveMadongseok(move, p_M);

		printStatus(trainLength, p_C, m_p_C, p_Z, m_p_Z, turn, aggro);


		p_C = m_p_C;
		p_Z = m_p_Z;

		if (trainLength - p_C == 1) {
			game = 1;
			break;
		}

		if (p_C - p_Z <= 1) {
			game = 2;
			break;
		}
		

		turn++;
	}

	//아웃트로
	if (game == 1) {
		success();
	}
	if (game == 2) {
		gameover();
	}

	return 0;
}


int intro(void) {
	printf("GMAE START\n");
	return 0;
}


int setup(int* trainLength, int* stm, int* probability) {
	int num;
	while (1) {
		printf("train length (%d~%d): ", LEN_MIN, LEN_MAX);
		scanf_s("%d", &num);
		if (LEN_MAX >= num && num >= LEN_MIN) {
			*trainLength = num;
			break;
		}
	}
	while (1) {
		printf("madongseok stamina(%d~%d)>>", STM_MIN, STM_MAX);
		scanf_s("%d", &num);
		if (STM_MAX >= num && num >= STM_MIN) {
			*stm = num;
			break;
		}
	}
	while (1) {
		printf("percentile probability \'p\' (%d~%d): ", PROB_MIN, PROB_MAX);
		scanf_s("%d", &num);
		if (PROB_MAX >= num && num >= PROB_MIN) {
			*probability = num;
			break;
		}
	}
	return 0;
}


int printPattern(int len, int p_M, int p_Z, int p_C) {
	// 첫째 줄 출력
	for (int i = 0; i < len; i++) {
		printf("#");
	}
	printf("\n");

	// 둘째 줄 출력
	printf("#");
	for (int i = 1; i < len - 1; i++) {
		if (len - p_M == i) {
			printf("M");
		}
		else if (len - p_Z == i) {
			printf("Z");
		}
		else if (len - p_C == i) {
			printf("C");
		}
		else {
			printf(" ");
		}
	}
	printf("#\n");

	// 셋째 줄 출력
	for (int i = 0; i < len; i++) {
		printf("#");
	}
	printf("\n\n");
	return 0;
}


int printStatus(int len, int p_C, int m_p_C, int p_Z, int m_p_Z, int turn, int aggro) {
	if (p_C == m_p_C) {
		printf("citizen: stay %d (aggro: %d)\n", len - m_p_C, aggro);
	}
	else {
		printf("citizen: %d -> %d (aggro: %d)\n", len - p_C, len - m_p_C, aggro);
	}
	if (turn % 2 == 0) {
		if (p_Z == m_p_Z) {
			printf("zombie: stay %d\n", len - m_p_Z);
		}
		else {
			printf("zombie: %d -> %d\n", len - p_Z, len - m_p_Z);
		}
	}
	else {
		printf("zombie: stay %d (cannot move)\n", len - m_p_Z);
	}
	printf("\n");
	return 0;
}

int printMadongseok(int len, int p_M, int m_p_M, int turn, int aggro, int stm) {
	if (p_M == m_p_M) {
		printf("madongseok: stay %d(aggro: %d, stamina: %d)\n", len- p_M, aggro, stm);
	}
	else {
		printf("madongseok: %d -> %d (aggro: %d, stamina: %d)\n", len - p_M, len - m_p_M, aggro, stm);
	}
}


int moveCitizen(int probability, int p_C) {
	int p = 100 - probability;	//확률
	int move_p_C;				//움직이고난 후 시민 위치
	int num = rand() % 100 + 1;

	if (p >= num) {
		move_p_C = p_C + 1;
	}
	else {
		move_p_C = p_C;
	}

	return move_p_C;
}


int moveZombie(int p, int p_Z) {
	int move_p_Z;
	int num = rand() % 100 + 1;

	if (p >= num) {
		move_p_Z = p_Z + 1;
	}
	else {
		move_p_Z = p_Z;
	}

	return move_p_Z;
}

int moveMadongseok(int move, int p_M) {
	int move_p_M;

	if (move == 1) {
		move_p_M = p_M + 1;
	}
	else {
		move_p_M = p_M;
	}

	return move_p_M;
}


int success(void) {
	printf("SUCCESS!\n");
	printf("citizen(s) escaped to the next train\n");
	return 0;
}


int gameover(void) {
	printf("GAME OVER!\n");
	printf("Citizen(s) has(have) been attacked by a zombie\n");
	return 0;
}