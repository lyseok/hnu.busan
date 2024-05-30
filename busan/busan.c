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
	int stamina; // 마동석 전용
	int aggro;
	int pull; // 좀비 전용
} Character;

int intro(void);
int firstsetup(int*, int*);
int setup(int);

int printPattern(int ,int ,int);
int printCitizen(int , int , int);
int printVillain(int, int);
int printZombie(int ,int , int);
int printMadongseok(int , int , int );

int moveCitizen(int);
int moveVillain(int);
int moveZombie(int);
int moveMadongseok(void);

int z_action(void);
int v_action(void);
int m_action(int, int);

int rest(int);
int provoke(int);
int pull(int, int);
int success(int);
int gameover(void);

Character maDongSeok;
Character villain;
Character zombie[MAX_CHARACTERS];
Character citizen[MAX_CHARACTERS];
Character characters[MAX_CHARACTERS];

int main(void) {
	int game = 0;
	int trainLength = 0;
	int probability = 0;

	int numCitizens = 1;
	int numZombies = 1;

	int m_p_M, m_p_Z, m_p_C, m_p_V;
	int turn = 0;
	int aggro;

	int stage = 1;

	srand((unsigned int)time(NULL));
	//인트로
	intro();

	//설정
	firstsetup(&trainLength, &probability);

	while (stage <= 4) {
		if (stage >= 2) {
			turn = 0;
			printf("**********************************************\n");
			printf("stage: %d start\n\n", stage);
		}

		setup(stage);

		int numCharacters = 1 + numCitizens + numZombies;

		//열차 초기 상태 출력
		printPattern(trainLength, numCharacters, stage);

		while (1) {
			//시민 이동
			aggro = citizen[0].aggro;
			m_p_C = moveCitizen(probability);
			if (stage == 2) m_p_V = moveVillain(m_p_C);

			//좀비 이동
			if (turn % 2 == 0) m_p_Z = moveZombie(probability);

			//열차 상태 출력
			printPattern(trainLength, numCharacters, stage);

			//시민, 빌런, 좀비 상태 출력
			printCitizen(trainLength, m_p_C, aggro);
			if (stage == 2) printVillain(trainLength, m_p_V);
			printZombie(trainLength, m_p_Z, turn);

			//마동석 이동
			aggro = maDongSeok.aggro;
			m_p_M = moveMadongseok();

			//열차 상태 출력
			printPattern(trainLength, numCharacters, stage);

			//마동석 상태 출력
			printMadongseok(trainLength, m_p_M, aggro);
			if (stage == 2) {
				//좀비 행동
				v_action();

				game = z_action();
				if (game == 2) break;

				// 마동석 행동
				m_action(trainLength, probability);
			}
			else {
				//좀비 행동
				game = z_action();
				if (game == 2) break;

				// 마동석 행동
				m_action(trainLength, probability);
			}
			if (trainLength - citizen[0].position == 1) {
				game = 1;
				break;
			}
			if (citizen[0].position - zombie[0].position <= 1) {
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
			maDongSeok.stamina = num;
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

int setup(int stage) {
	maDongSeok.position = 2; // 마동석
	maDongSeok.aggro = 1;

	citizen[0].position = 6; // 시민
	citizen[0].aggro = 1;

	zombie[0].position = 3; // 좀비
	zombie[0].pull = 0;

	if (stage == 2) { // 빌런
		villain.position = 5;
		villain.aggro = 1;
	}

	return 0;
}

int printPattern(int len, int numCharacters, int stage) {
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
		if (len - maDongSeok.position == i) c = 'M';
		else if (len - citizen[0].position == i) c = 'C';
		else if (len - zombie[0].position == i) c = 'Z';
		if(stage == 2) if (len - villain.position == i) c = 'V';
		/*
		else if (len - villain.position == i) c = 'V';
		/*
		for (int j = 0; j < MAX_CHARACTERS; j++) {
			if (len - characters[j].position == i) {
				if (characters[j].type == 0) c = 'M';
				else if (characters[j].type == 1) c = 'C';
				else if (characters[j].type == 2) c = 'Z';
				break;
			}
		}
		*/
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

int printCitizen(int len, int m_p_C, int aggro) {
	if (citizen[0].position == m_p_C) {
		printf("citizen: stay %d (aggro: %d -> %d)\n", len - m_p_C, aggro, citizen[0].aggro);
	}
	else {
		printf("citizen: %d -> %d (aggro: %d -> %d)\n", len - citizen[0].position, len - m_p_C, aggro, citizen[0].aggro);
	}

	citizen[0].position = m_p_C;

	return 0;
}

int printVillain(int len, int m_p_V) {
	if (villain.position == m_p_V) {
		printf("villain: %d (aggro: %d)\n", len - m_p_V, villain.aggro);
	}
	else {
		printf("villain: %d -> %d (aggro: %d)\n", len - villain.position, len - m_p_V, villain.aggro);
	}

	villain.position = m_p_V;

	return 0;
}

int printZombie(int len, int m_p_Z, int turn) {
	if (turn % 2 == 0) {
		if (zombie[0].position == m_p_Z) {
			if (zombie[0].pull == 1) {
				printf("zombie: stay %d (madongseok pulled)\n", len - m_p_Z);
				zombie[0].pull = 0;
			}
			else {
				printf("zombie: stay %d\n", len - m_p_Z);
			}
		}
		else {
			printf("zombie: %d -> %d\n", len - zombie[0].position, len - m_p_Z);
		}
	}
	else {
		printf("zombie: stay %d (cannot move)\n", len - m_p_Z);
	}
	printf("\n");

	zombie[0].position = m_p_Z;

	return 0;
}

int printMadongseok(int len, int m_p_M, int aggro) {
	if (maDongSeok.position == m_p_M) {
		printf("madongseok: stay %d(aggro: %d -> %d, stamina: %d)\n",
			len - maDongSeok.position, aggro, maDongSeok.aggro, maDongSeok.stamina);
	}
	else {
		printf("madongseok: %d -> %d (aggro: %d -> %d, stamina: %d)\n",
			len - maDongSeok.position, len - m_p_M, aggro, maDongSeok.aggro, maDongSeok.stamina);
	}
	// 마동석 이동한 위치로 위치이동 적용
	maDongSeok.position = m_p_M;
	printf("\n");

	return 0;
}

int moveCitizen(int probability) {
	int p = 100 - probability;	//확률
	int move_p_C;				//움직이고난 후 시민 위치
	int num = rand() % 100 + 1;

	if (p <= num) {
		move_p_C = citizen[0].position + 1;
		citizen[0].aggro++;
		if (citizen[0].aggro > AGGRO_MAX) citizen[0].aggro = AGGRO_MAX;
	}
	else {
		move_p_C = citizen[0].position;
		citizen[0].aggro--;
		if (citizen[0].aggro < AGGRO_MIN) citizen[0].aggro = AGGRO_MIN;
	}

	return move_p_C;
}

int moveVillain(int m_p_C) {
	int move_p_V;

	if (citizen[0].position != m_p_C) {
		move_p_V = villain.position + 1;
		villain.aggro++;
		if (villain.aggro > AGGRO_MAX) villain.aggro = AGGRO_MAX;
	}
	else {
		move_p_V = villain.position;
		villain.aggro--;
		if (villain.aggro < AGGRO_MIN) villain.aggro = AGGRO_MIN;
	}

	return move_p_V;
}

int moveZombie(int p) {
	int move_p_Z;
	if (zombie[0].pull == 1) {
		move_p_Z = zombie[0].position;
		return move_p_Z;
	}
	if (maDongSeok.aggro > citizen[0].aggro) {
		if (maDongSeok.position < zombie[0].position) {
			move_p_Z = zombie[0].position - 1;
			if (maDongSeok.position == move_p_Z) move_p_Z = zombie[0].position;
		}
		else {
			move_p_Z = zombie[0].position + 1;
			if (maDongSeok.position == move_p_Z) move_p_Z = zombie[0].position;
		}
	}
	else {
		if (citizen[0].position < zombie[0].position) {
			move_p_Z = zombie[0].position - 1;
			if (citizen[0].position == move_p_Z) move_p_Z = zombie[0].position;
		}
		else {
			move_p_Z = zombie[0].position + 1;
			if (citizen[0].position == move_p_Z) move_p_Z = zombie[0].position;
		}
	}
	return move_p_Z;
}

int moveMadongseok(void) {
	int move_p_M;
	int move;

	if (zombie[0].position - maDongSeok.position == 1) {
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
		move_p_M = maDongSeok.position + 1;
		maDongSeok.aggro++;
		if (maDongSeok.aggro > AGGRO_MAX) maDongSeok.aggro = AGGRO_MAX;
	}
	else {
		move_p_M = maDongSeok.position;
		maDongSeok.aggro--;
		if (maDongSeok.aggro < AGGRO_MIN) maDongSeok.aggro = AGGRO_MIN;
	}

	return move_p_M;
}

int z_action(void) {
	int m_stm;

	if (zombie[0].position - maDongSeok.position <= 1 && citizen[0].position - zombie[0].position <= 1) {
		if (maDongSeok.aggro >= zombie[0].aggro) {
			m_stm = maDongSeok.stamina--;
			printf("cityzen does nothing.\n");
			if (maDongSeok.stamina == 0) {
				printf("GAME OVER! madongseok dead...\n");
				return 2;
			}
			printf("Zombie attacked madongseok (aggro: %d vs %d, madongseok stamina: %d -> %d)\n",
				zombie[0].aggro, maDongSeok.aggro, m_stm, maDongSeok.stamina);
		}
		else {
			printf("citizen does nothing.\n");
			printf("GAME OVER! citizen dead...\n");
			return 2;
		}
	}
	else if (zombie[0].position - maDongSeok.position <= 1) {
		m_stm = maDongSeok.stamina--;
		printf("cityzen does nothing.\n");
		if (maDongSeok.stamina == 0) {
			printf("GAME OVER! madongseok dead...\n");
			return 2;
		}
		printf("Zombie attacked madongseok (madongseok stamina: %d -> %d)\n", m_stm, maDongSeok.stamina);
	}
	else if (citizen[0].position - zombie[0].position <= 1) {
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
		if (zombie[0].position - maDongSeok.position == 1) {
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
	aggro = maDongSeok.aggro--;
	stm = maDongSeok.stamina++;

	if (maDongSeok.aggro < AGGRO_MIN) maDongSeok.aggro = AGGRO_MIN;
	if (maDongSeok.stamina > STM_MAX) maDongSeok.stamina = STM_MAX;

	printf("\nmadongseok rests...\n");
	printf("madongseok: %d (aggro: %d -> %d, stamina: %d -> %d)\n\n",
		len - maDongSeok.position, aggro, maDongSeok.aggro, stm, maDongSeok.stamina);
}

int provoke(int len) {
	int aggro;
	aggro = maDongSeok.aggro;
	maDongSeok.aggro = AGGRO_MAX;

	printf("\nmadongseok provoked zombie...\n");
	printf("madongseok: %d (aggro: %d -> %d, stamina: %d)\n\n",
		len - maDongSeok.position, aggro, maDongSeok.aggro, maDongSeok.stamina);
}

int v_action() {
	int p = 100 - 30;
	int num = rand() % 100 + 1;
	if (citizen[0].position - villain.position == 1 || villain.position - citizen[0].position == 1) {
		if (p >= num) {
			int temp;
			temp = citizen[0].position;
			citizen[0].position = villain.position;
			villain.position = temp;
		}
	}
	return 0;
}

int pull(int len, int probability) {
	int aggro, stm;
	int p = 100 - probability;
	int num = rand() % 100 + 1;
	aggro = maDongSeok.aggro;
	stm = maDongSeok.stamina--;
	maDongSeok.aggro += 2;

	if (maDongSeok.aggro > AGGRO_MAX) maDongSeok.aggro = AGGRO_MAX;
	if (maDongSeok.stamina < STM_MIN) maDongSeok.stamina = STM_MIN;

	if (p >= num) {
		zombie[0].pull = 1;
		printf("\nmadongseok pulled zombie...Next turn, it can't move\n");
		printf("madongseok: %d (aggro: %d -> %d, stamina: %d -> %d)\n\n",
			len - maDongSeok.position, aggro, maDongSeok.aggro, stm, maDongSeok.stamina);
	}
	else {
		printf("\nmadongseok failed to pull zombie\n");
		printf("madongseok: %d (aggro: %d -> %d, stamina: %d -> %d)\n\n",
			len - maDongSeok.position, aggro, maDongSeok.aggro, stm, maDongSeok.stamina);
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
