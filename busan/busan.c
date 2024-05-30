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
#define MADONGSEOK 0
#define ACTION_REST 0
#define ACTION_PROVOKE 1
#define ACTION_PULL 2

#define MAX_CHARACTERS 10

typedef struct {
	int position;
	int type; // 0: 마동석, 1: 시민, 2: 좀비, 3: 빌런
	int stamina; // 마동석 전용
	int aggro;
	int pull;
} Character;

int intro(void);
int firstsetup(int* trainLength, int* probability);
int setup(void);
int printPattern(int len, int numCharacters);
int printStatus(int len, int m_p_C, int m_p_Z, int turn, int aggro);
int printMadongseok(int len, int m_p_M, int aggro);
int moveCitizen(int p);
int moveZombie(int p);
int moveMadongseok(void);
int z_action(void);
int m_action(int, int);
int rest(int);
int provoke(int);
int pull(int, int);
int success(int);
int gameover(void);

Character characters[MAX_CHARACTERS];

int main(void) {
	int game = 0;
	int trainLength = 0;
	int probability = 0;

	int numCitizens = 1;
	int numZombies = 1;

	int m_p_M, m_p_Z, m_p_C;
	int turn = 0;
	int aggro;

	int stage = 1;

	srand((unsigned int)time(NULL));
	//인트로
	intro();

	//설정
	firstsetup(&trainLength, &probability);

	while (stage <= 4) {
		if (stage >= 2) printf("stage: %d start\n\n", stage);

		setup();

		int numCharacters = 1 + numCitizens + numZombies;

		//열차 초기 상태 출력
		printPattern(trainLength, numCharacters);

		while (1) {
			//시민 이동
			aggro = characters[3].aggro;
			m_p_C = moveCitizen(probability);

			//좀비 이동
			if (turn % 2 == 0) m_p_Z = moveZombie(probability);

			//열차 상태 출력
			printPattern(trainLength, numCharacters);

			//시민, 좀비 상태 출력
			printStatus(trainLength, m_p_C, m_p_Z, turn, aggro);

			//characters[3].position = m_p_C;
			//characters[1].position = m_p_Z;

			//마동석 이동
			aggro = characters[MADONGSEOK].aggro;
			m_p_M = moveMadongseok();

			//열차 상태 출력
			printPattern(trainLength, numCharacters);

			//마동석 상태 출력
			printMadongseok(trainLength, m_p_M, aggro);

			//characters[MADONGSEOK].position = m_p_M;

			//좀비 행동
			game = z_action();
			if (game == 2) break;

			// 마동석 행동
			m_action(trainLength, probability);



			if (trainLength - characters[3].position == 1) {
				game = 1;
				break;
			}

			if (characters[3].position - characters[1].position <= 1) {
				game = 2;
				break;
			}


			turn++;
		}

		//아웃트로
		if (game == 1) {
			stage = success(stage);
			game = 0;
		}
		/*
		if (game == 2) {
			gameover();
		}
		*/
	}
	return 0;
}


int intro(void) {
	printf("GMAE START\n");
	return 0;
}

int firstsetup(int* trainLength, int* probability) {
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
			characters[MADONGSEOK].stamina = num;
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

int setup() {
	characters[MADONGSEOK].position = 2; // 마동석
	characters[MADONGSEOK].type = 0;
	characters[MADONGSEOK].aggro = 1;
	characters[3].position = 6; // 시민
	characters[3].type = 1;
	characters[3].aggro = 1;
	characters[1].position = 3; // 좀비
	characters[1].type = 2;
	characters[1].pull = 0;

	return 0;
}

int printPattern(int len, int numCharacters) {
	// 첫째 줄 출력
	for (int i = 0; i < len; i++) {
		printf("#");
	}
	printf("\n");

	// 둘째 줄 출력
	printf("#");
	for (int i = 1; i < len - 1; i++) {
		char c = ' ';
		//for (int j = 0; j < numCharacters; j++) {
		for (int j = 0; j < MAX_CHARACTERS; j++) {
			if (len - characters[j].position == i) {
				if (characters[j].type == 0) c = 'M';
				else if (characters[j].type == 1) c = 'C';
				else if (characters[j].type == 2) c = 'Z';
				break;
			}
		}
		printf("%c", c);
	}
	printf("#\n");

	// 셋째 줄 출력
	for (int i = 0; i < len; i++) {
		printf("#");
	}
	printf("\n\n");
	return 0;
}

int printStatus(int len, int m_p_C, int m_p_Z, int turn, int aggro) {
	if (characters[3].position == m_p_C) {
		printf("citizen: stay %d (aggro: %d -> %d)\n", len - m_p_C, aggro, characters[3].aggro);
	}
	else {
		printf("citizen: %d -> %d (aggro: %d -> %d)\n", len - characters[3].position, len - m_p_C, aggro, characters[3].aggro);
	}
	if (turn % 2 == 0) {
		if (characters[1].position == m_p_Z) {
			if (characters[1].pull == 1) {
				printf("zombie: stay %d (madongseok pulled)\n", len - m_p_Z);
				characters[1].pull = 0;
			}
			else {
				printf("zombie: stay %d\n", len - m_p_Z);
			}
		}
		else {
			printf("zombie: %d -> %d\n", len - characters[1].position, len - m_p_Z);
		}
	}
	else {
		printf("zombie: stay %d (cannot move)\n", len - m_p_Z);
	}
	printf("\n");

	characters[3].position = m_p_C;
	characters[1].position = m_p_Z;

	return 0;
}

int printMadongseok(int len, int m_p_M, int aggro) {
	if (characters[MADONGSEOK].position == m_p_M) {
		printf("madongseok: stay %d(aggro: %d -> %d, stamina: %d)\n",
			len - characters[MADONGSEOK].position, aggro, characters[MADONGSEOK].aggro, characters[MADONGSEOK].stamina);
	}
	else {
		printf("madongseok: %d -> %d (aggro: %d -> %d, stamina: %d)\n",
			len - characters[MADONGSEOK].position, len - m_p_M, aggro, characters[MADONGSEOK].aggro, characters[MADONGSEOK].stamina);
	}
	// 마동석 이동한 위치로 위치이동 적용
	characters[MADONGSEOK].position = m_p_M;
	printf("\n");
}

int moveCitizen(int probability) {
	int p = 100 - probability;	//확률
	int move_p_C;				//움직이고난 후 시민 위치
	int num = rand() % 100 + 1;

	if (p <= num) {
		move_p_C = characters[3].position + 1;
		characters[3].aggro++;
		if (characters[3].aggro > AGGRO_MAX) characters[3].aggro = AGGRO_MAX;
	}
	else {
		move_p_C = characters[3].position;
		characters[3].aggro--;
		if (characters[3].aggro < AGGRO_MIN) characters[3].aggro = AGGRO_MIN;
	}

	return move_p_C;
}

int moveZombie(int p) {
	int move_p_Z;
	if (characters[1].pull == 1) {
		move_p_Z = characters[1].position;
		return move_p_Z;
	}
	if (characters[MADONGSEOK].aggro > characters[3].aggro) {
		if (characters[MADONGSEOK].position < characters[1].position) {
			move_p_Z = characters[1].position - 1;
			if (characters[MADONGSEOK].position == move_p_Z) move_p_Z = characters[1].position;
		}
		else {
			move_p_Z = characters[1].position + 1;
			if (characters[MADONGSEOK].position == move_p_Z) move_p_Z = characters[1].position;
		}
	}
	else {
		if (characters[3].position < characters[1].position) {
			move_p_Z = characters[1].position - 1;
			if (characters[3].position == move_p_Z) move_p_Z = characters[1].position;
		}
		else {
			move_p_Z = characters[1].position + 1;
			if (characters[3].position == move_p_Z) move_p_Z = characters[1].position;
		}
	}
	return move_p_Z;
}

int moveMadongseok(void) {
	int move_p_M;
	int move;

	if (characters[1].position - characters[MADONGSEOK].position == 1) {
		while (1) {
			printf("madongseok move(0:stay)>>");
			scanf_s("%d", &move);
			if (move == MOVE_STAY) break;
		}
	}
	else {
		while (1) {
			printf("madongseok move(0:stay, 1:left)>>");
			scanf_s("%d", &move);
			if (move == MOVE_STAY || move == MOVE_LEFT) break;
		}
	}


	if (move == MOVE_LEFT) {
		move_p_M = characters[MADONGSEOK].position + 1;
		characters[MADONGSEOK].aggro++;
		if (characters[MADONGSEOK].aggro > AGGRO_MAX) characters[MADONGSEOK].aggro = AGGRO_MAX;
	}
	else {
		move_p_M = characters[MADONGSEOK].position;
		characters[MADONGSEOK].aggro--;
		if (characters[MADONGSEOK].aggro < AGGRO_MIN) characters[MADONGSEOK].aggro = AGGRO_MIN;
	}

	return move_p_M;
}

int z_action(void) {
	int m_stm;

	if (characters[1].position - characters[MADONGSEOK].position <= 1 && characters[3].position - characters[1].position <= 1) {
		if (characters[MADONGSEOK].aggro >= characters[1].aggro) {
			m_stm = characters[MADONGSEOK].stamina--;
			printf("cityzen does nothing.\n");
			if (characters[MADONGSEOK].stamina == 0) {
				printf("GAME OVER! madongseok dead...\n");
				return 2;
			}
			printf("Zombie attacked madongseok (aggro: %d vs %d, madongseok stamina: %d -> %d)\n",
				characters[1].aggro, characters[MADONGSEOK].aggro, m_stm, characters[MADONGSEOK].stamina);
		}
		else {
			printf("citizen does nothing.\n");
			printf("GAME OVER! citizen dead...\n");
			return 2;
		}
	}
	else if (characters[1].position - characters[MADONGSEOK].position <= 1) {
		m_stm = characters[MADONGSEOK].stamina--;
		printf("cityzen does nothing.\n");
		if (characters[MADONGSEOK].stamina == 0) {
			printf("GAME OVER! madongseok dead...\n");
			return 2;
		}
		printf("Zombie attacked madongseok (madongseok stamina: %d -> %d)\n", m_stm, characters[MADONGSEOK].stamina);
	}
	else if (characters[3].position - characters[1].position <= 1) {
		printf("citizen does nothing.\n");
		printf("GAME OVER! citizen dead...\n");
		return 2;
	}
	else {
		printf("cityzen does nothing.\n");
		printf("zombie attcked nobody\n");
	}
	return 0;
}

int m_action(int len, int probability) {
	int m;

	while (1) {
		if (characters[1].position - characters[MADONGSEOK].position == 1) {
			printf("madongseok action(0.rest, 1.provoke, 2.pull)>>");
			scanf_s("%d", &m);
			if (m == ACTION_REST) {
				rest(len);
				break;
			}
			else if (m == ACTION_PROVOKE) {
				provoke(len);
				break;
			}
			else if (m == ACTION_PULL) {
				pull(len, probability);
				break;
			}
		}
		else {
			printf("madongseok action(0.rest, 1.provoke)>>");
			scanf_s("%d", &m);
			if (m == ACTION_REST) {
				rest(len);
				break;
			}
			else if (m == ACTION_PROVOKE) {
				provoke(len);
				break;
			}
		}
	}

	return 0;
}

int rest(int len) {
	int aggro, stm;
	aggro = characters[MADONGSEOK].aggro--;
	stm = characters[MADONGSEOK].stamina++;

	if (characters[MADONGSEOK].aggro < AGGRO_MIN) characters[MADONGSEOK].aggro = AGGRO_MIN;
	if (characters[MADONGSEOK].stamina > STM_MAX) characters[MADONGSEOK].stamina = STM_MAX;

	printf("\nmadongseok rests...\n");
	printf("madongseok: %d (aggro: %d -> %d, stamina: %d -> %d)\n\n",
		len - characters[MADONGSEOK].position, aggro, characters[MADONGSEOK].aggro, stm, characters[MADONGSEOK].stamina);
}

int provoke(int len) {
	int aggro;
	aggro = characters[MADONGSEOK].aggro;
	characters[MADONGSEOK].aggro = AGGRO_MAX;

	printf("\nmadongseok provoked zombie...\n");
	printf("madongseok: %d (aggro: %d -> %d, stamina: %d)\n\n",
		len - characters[MADONGSEOK].position, aggro, characters[MADONGSEOK].aggro, characters[MADONGSEOK].stamina);
}

int pull(int len, int probability) {
	int aggro, stm;
	int p = 100 - probability;
	int num = rand() % 100 + 1;
	aggro = characters[MADONGSEOK].aggro;
	stm = characters[MADONGSEOK].stamina--;
	characters[MADONGSEOK].aggro += 2;

	if (characters[MADONGSEOK].aggro > AGGRO_MAX) characters[MADONGSEOK].aggro = AGGRO_MAX;
	if (characters[MADONGSEOK].stamina < STM_MIN) characters[MADONGSEOK].stamina = STM_MIN;

	if (p >= num) {
		characters[1].pull = 1;
		printf("\nmadongseok pulled zombie...Next turn, it can't move\n");
		printf("madongseok: %d (aggro: %d -> %d, stamina: %d -> %d)\n\n",
			len - characters[MADONGSEOK].position, aggro, characters[MADONGSEOK].aggro, stm, characters[MADONGSEOK].stamina);
	}
	else {
		printf("\nmadongseok failed to pull zombie\n");
		printf("madongseok: %d (aggro: %d -> %d, stamina: %d -> %d)\n\n",
			len - characters[MADONGSEOK].position, aggro, characters[MADONGSEOK].aggro, stm, characters[MADONGSEOK].stamina);
	}
}

int success(int stage) {
	if (stage == 4) printf("YOU WIN!\n");
	return ++stage;
}

int gameover(void) {
	printf("GAME OVER!\n");
	printf("Citizen(s) has(have) been attacked by a zombie\n");
	return 0;
}
