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

#define MAX_CHARACTERS 10

typedef struct {
	int position;
	int type; // 0: 마동석, 1: 시민, 2: 좀비
	int stamina; // 마동석 전용
	int aggro;
} Character;

int intro(void);
int setup(int* trainLength, int* probability);
int printPattern(int len, int numCharacters);
int printStatus(int len, int m_p_C, int m_p_Z, int turn, int aggro);
int printMadongseok(int len, int m_p_M, int aggro);
int moveCitizen(int p);
int moveZombie(int p);
int moveMadongseok(void);
int z_action(void);
int success(void);
int gameover(void);

Character characters[MAX_CHARACTERS];

int main(void) {
	int game = 0;
	int trainLength = 0;
	int probability = 0;

	int numCitizens = 1;
	int numZombies = 1;

	int p_M = 2, p_Z = 3, p_C = 6;
	int m_p_M, m_p_Z, m_p_C;
	int turn = 0;
	int aggro;

	int m;

	srand((unsigned int)time(NULL));
	//인트로
	intro();

	//설정
	setup(&trainLength, &probability);

	int numCharacters = 1 + numCitizens + numZombies;

	//열차 초기 상태 출력
	printPattern(trainLength, numCharacters);

	
	while (1) {
		//시민 이동
		aggro = characters[1].aggro;
		m_p_C = moveCitizen(probability);

		//좀비 이동
		if(turn % 2 == 0) m_p_Z = moveZombie(probability);

		//열차 상태 출력
		printPattern(trainLength, numCharacters);

		//시민, 좀비 상태 출력
		printStatus(trainLength, m_p_C, m_p_Z, turn, aggro);

		characters[1].position = m_p_C;
		characters[2].position = m_p_Z;

		//마동석 이동
		aggro = characters[0].aggro;
		m_p_M = moveMadongseok();

		//열차 상태 출력
		printPattern(trainLength, numCharacters);

		//마동석 상태 출력
		printMadongseok(trainLength, m_p_M, aggro);

		//좀비 행동
		game = z_action();
		if (game == 2) break;

		m_action(trainLength);

		characters[0].position = m_p_M;


		if (trainLength - characters[1].position == 1) {
			game = 1;
			break;
		}

		if (characters[1].position - characters[2].position <= 1) {
			game = 2;
			break;
		}
		

		turn++;
	}
	/*
	//아웃트로
	if (game == 1) {
		success();
	}
	if (game == 2) {
		gameover();
	}
	*/
	return 0;
}


int intro(void) {
	printf("GMAE START\n");
	return 0;
}

int setup(int* trainLength, int* probability) {
	int num;
	int stm;

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
			stm = num;
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

	characters[0].position = 2; // 마동석
	characters[0].type = 0;
	characters[0].stamina = stm;
	characters[0].aggro = 1;
	characters[1].position = 6; // 시민
	characters[1].type = 1;
	characters[1].aggro = 1;
	characters[2].position = 3; // 좀비
	characters[2].type = 2;

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
		for (int j = 0; j < 3; j++) {
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
	if (characters[1].position == m_p_C) {
		printf("citizen: stay %d (aggro: %d -> %d)\n", len - m_p_C, aggro, characters[1].aggro);
	}
	else {
		printf("citizen: %d -> %d (aggro: %d -> %d)\n", len - characters[1].position, len - m_p_C, aggro, characters[1].aggro);
	}
	if (turn % 2 == 0) {
		if (characters[2].position == m_p_Z) {
			printf("zombie: stay %d\n", len - m_p_Z);
		}
		else {
			printf("zombie: %d -> %d\n", len - characters[2].position, len - m_p_Z);
		}
	}
	else {
		printf("zombie: stay %d (cannot move)\n", len - m_p_Z);
	}
	printf("\n");
	return 0;
}

int printMadongseok(int len, int m_p_M, int aggro) {
	if (characters[0].position == m_p_M) {
		printf("madongseok: stay %d(aggro: %d -> %d, stamina: %d)\n", 
			len - characters[0].position, aggro, characters[0].aggro, characters[0].stamina);
	}
	else {
		printf("madongseok: %d -> %d (aggro: %d -> %d, stamina: %d)\n", 
			len - characters[0].position, len - m_p_M, aggro, characters[0].aggro, characters[0].stamina);
	}
	printf("\n");
}

int moveCitizen(int probability) {
	int p = 100 - probability;	//확률
	int move_p_C;				//움직이고난 후 시민 위치
	int num = rand() % 100 + 1;

	if (p >= num) {
		move_p_C = characters[1].position + 1;
		characters[1].aggro++;
		if (characters[1].aggro > AGGRO_MAX) characters[1].aggro = AGGRO_MAX;
	}
	else {
		move_p_C = characters[1].position;
		characters[1].aggro--;
		if (characters[1].aggro < AGGRO_MIN) characters[1].aggro = AGGRO_MIN;
	}

	return move_p_C;
}

int moveZombie(int p) {
	int move_p_Z;

	if (characters[0].aggro > characters[1].aggro) {
		if (characters[0].position < characters[2].position) {
			move_p_Z = characters[2].position - 1;
			if (characters[0].position == move_p_Z) move_p_Z = characters[2].position;
		}
		else {
			move_p_Z = characters[2].position + 1;
			if (characters[0].position == move_p_Z) move_p_Z = characters[2].position;
		}
	}
	else {
		if (characters[1].position < characters[2].position) {
			move_p_Z = characters[2].position - 1;
			if (characters[1].position == move_p_Z) move_p_Z = characters[2].position;
		}
		else {
			move_p_Z = characters[2].position + 1;
			if (characters[1].position == move_p_Z) move_p_Z = characters[2].position;
		}
	}
	return move_p_Z;
}

int moveMadongseok(void) {
	int move_p_M;
	int move;

	if (characters[2].position - characters[0].position == 1) {
		while (1) {
			printf("madongseok move(0:stay)>>");
			scanf_s("%d", &move);
			if (move == 0) break;
		}
	}
	else {
		while (1) {
			printf("madongseok move(0:stay, 1:left)>>");
			scanf_s("%d", &move);
			if (move == 0 || move == 1) break;
		}
	}
	

	if (move == 1) {
		move_p_M = characters[0].position + 1;
		characters[0].aggro++;
		if(characters[0].aggro > AGGRO_MAX) characters[0].aggro = AGGRO_MAX;
	}
	else {
		move_p_M = characters[0].position;
		characters[0].aggro--;
		if (characters[0].aggro < AGGRO_MIN) characters[0].aggro = AGGRO_MIN;
	}

	return move_p_M;
}

int z_action(void) {
	int m_stm;

	if (characters[2].position - characters[0].position <= 1 && characters[1].position - characters[2].position <= 1) {
		if (characters[0].aggro >= characters[2].aggro) {
			m_stm = characters[0].stamina--;
			printf("cityzen does nothing.\n");
			if (characters[0].stamina == 0) {
				printf("GAME OVER! madongseok dead...\n");
				return 2;
			}
			printf("Zombie attacked madongseok (aggro: %d vs %d, madongseok stamina: %d -> %d)\n",
				characters[2].aggro, characters[0].aggro, m_stm, characters[0].stamina);
		}
		else {
			printf("citizen does nothing.\n");
			printf("GAME OVER! citizen dead...\n");
			return 2;
		}
	}
	else if (characters[2].position - characters[0].position <= 1) {
		m_stm = characters[0].stamina--;
		printf("cityzen does nothing.\n");
		if (characters[0].stamina == 0) {
			printf("GAME OVER! madongseok dead...\n");
			return 2;
		}
		printf("Zombie attacked madongseok (madongseok stamina: %d -> %d)\n", m_stm, characters[0].stamina);
	}
	else if (characters[1].position - characters[2].position <= 1) {
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

int m_action(int len) {
	int aggro, stm;
	int m;
	while (1) {
		printf("madongseok action(0.rest, 1.provoke)>>");
		scanf_s("%d", &m);
		if (m == 0 || m == 1) break;
	}


	if (m == 0) {
		aggro = characters[0].aggro--;
		stm = characters[0].stamina++;

		if (characters[0].aggro < AGGRO_MIN) characters[0].aggro = AGGRO_MIN;
		if (characters[0].stamina > STM_MAX) characters[0].stamina = STM_MAX;

		printf("\nmadongseok rests...\n");
		printf("madongseok: %d (aggro: %d -> %d, stamina: %d -> %d)\n\n",
			len - characters[0].position, aggro, characters[0].aggro, stm, characters[0].stamina);
	}
	else if (m == 1) {
		aggro = characters[0].aggro;
		characters[0].aggro = AGGRO_MAX;

		printf("madongseok provoked zombie...\n");
		printf("madongseok: %d (aggro: %d -> %d, stamina: %d)\n", 
			len - characters[0].position, aggro, characters[0].aggro, characters[0].stamina);
	}

	return 0;
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