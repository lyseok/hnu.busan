//20190889
//������

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>

// �Ķ����
#define LEN_MIN 15 // ���� ����
#define LEN_MAX 50
#define STM_MIN 0 // ������ ü��
#define STM_MAX 5
#define PROB_MIN 10 // Ȯ��
#define PROB_MAX 90
#define AGGRO_MIN 0 // ��׷� ����
#define AGGRO_MAX 5

// ������ �̵� ����
#define MOVE_LEFT 1
#define MOVE_STAY 0

// ������ ���� ���
#define ATK_NONE 0
#define ATK_CITIZEN 1
#define ATK_DONGSEOK 2

// ������ �ൿ
#define MADONGSEOK 0
#define ACTION_REST 0
#define ACTION_PROVOKE 1
#define ACTION_PULL 2

#define MAX_CHARACTERS 10

typedef struct {
	int position;
	int type; // 0: ������, 1: �ù�, 2: ����
	int stamina; // ������ ����
	int aggro;
	int pull;
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
int m_action(int, int);
int rest(int);
int provoke(int);
int pull(int, int);
int success(void);
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

	int m;

	srand((unsigned int)time(NULL));
	//��Ʈ��
	intro();

	//����
	setup(&trainLength, &probability);

	int numCharacters = 1 + numCitizens + numZombies;

	//���� �ʱ� ���� ���
	printPattern(trainLength, numCharacters);
	
	while (1) {
		//�ù� �̵�
		aggro = characters[1].aggro;
		m_p_C = moveCitizen(probability);

		//���� �̵�
		if(turn % 2 == 0) m_p_Z = moveZombie(probability);

		//���� ���� ���
		printPattern(trainLength, numCharacters);

		//�ù�, ���� ���� ���
		printStatus(trainLength, m_p_C, m_p_Z, turn, aggro);

		characters[1].position = m_p_C;
		characters[2].position = m_p_Z;

		//������ �̵�
		aggro = characters[MADONGSEOK].aggro;
		m_p_M = moveMadongseok();

		//���� ���� ���
		printPattern(trainLength, numCharacters);

		//������ ���� ���
		printMadongseok(trainLength, m_p_M, aggro);

		characters[MADONGSEOK].position = m_p_M;

		//���� �ൿ
		game = z_action();
		if (game == 2) break;

		// ������ �ൿ
		m_action(trainLength, probability);



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

	//�ƿ�Ʈ��
	if (game == 1) {
		success();
	}
	/*
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

	characters[MADONGSEOK].position = 2; // ������
	characters[MADONGSEOK].type = 0;
	characters[MADONGSEOK].stamina = stm;
	characters[MADONGSEOK].aggro = 1;
	characters[1].position = 6; // �ù�
	characters[1].type = 1;
	characters[1].aggro = 1;
	characters[2].position = 3; // ����
	characters[2].type = 2;
	characters[2].pull = 0;

	return 0;
}

int printPattern(int len, int numCharacters) {
	// ù° �� ���
	for (int i = 0; i < len; i++) {
		printf("#");
	}
	printf("\n");

	// ��° �� ���
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

	// ��° �� ���
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
			if (characters[2].pull == 1) {
				printf("zombie: stay %d (madongseok pulled)\n", len - m_p_Z);
				characters[2].pull = 0;
			}
			else {
				printf("zombie: stay %d\n", len - m_p_Z);
			}
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
	if (characters[MADONGSEOK].position == m_p_M) {
		printf("madongseok: stay %d(aggro: %d -> %d, stamina: %d)\n",
			len - characters[MADONGSEOK].position, aggro, characters[MADONGSEOK].aggro, characters[MADONGSEOK].stamina);
	}
	else {
		printf("madongseok: %d -> %d (aggro: %d -> %d, stamina: %d)\n",
			len - characters[MADONGSEOK].position, len - m_p_M, aggro, characters[MADONGSEOK].aggro, characters[MADONGSEOK].stamina);
	}
	printf("\n");
}

int moveCitizen(int probability) {
	int p = 100 - probability;	//Ȯ��
	int move_p_C;				//�����̰� �� �ù� ��ġ
	int num = rand() % 100 + 1;

	if (p <= num) {
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
	if (characters[2].pull == 1) {
		move_p_Z = characters[2].position;
		return move_p_Z;
	}
	if (characters[MADONGSEOK].aggro > characters[1].aggro) {
		if (characters[MADONGSEOK].position < characters[2].position) {
			move_p_Z = characters[2].position - 1;
			if (characters[MADONGSEOK].position == move_p_Z) move_p_Z = characters[2].position;
		}
		else {
			move_p_Z = characters[2].position + 1;
			if (characters[MADONGSEOK].position == move_p_Z) move_p_Z = characters[2].position;
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

	if (characters[2].position - characters[MADONGSEOK].position == 1) {
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

	if (characters[2].position - characters[MADONGSEOK].position <= 1 && characters[1].position - characters[2].position <= 1) {
		if (characters[MADONGSEOK].aggro >= characters[2].aggro) {
			m_stm = characters[MADONGSEOK].stamina--;
			printf("cityzen does nothing.\n");
			if (characters[MADONGSEOK].stamina == 0) {
				printf("GAME OVER! madongseok dead...\n");
				return 2;
			}
			printf("Zombie attacked madongseok (aggro: %d vs %d, madongseok stamina: %d -> %d)\n",
				characters[2].aggro, characters[MADONGSEOK].aggro, m_stm, characters[MADONGSEOK].stamina);
		}
		else {
			printf("citizen does nothing.\n");
			printf("GAME OVER! citizen dead...\n");
			return 2;
		}
	}
	else if (characters[2].position - characters[MADONGSEOK].position <= 1) {
		m_stm = characters[MADONGSEOK].stamina--;
		printf("cityzen does nothing.\n");
		if (characters[MADONGSEOK].stamina == 0) {
			printf("GAME OVER! madongseok dead...\n");
			return 2;
		}
		printf("Zombie attacked madongseok (madongseok stamina: %d -> %d)\n", m_stm, characters[MADONGSEOK].stamina);
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

int m_action(int len, int probability) {
	int m;

	while (1) {
		if (characters[2].position - characters[MADONGSEOK].position == 1) {
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
		characters[2].pull = 1;
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

int success(void) {
	printf("YOU WIN!\n");
	return 0;
}

int gameover(void) {
	printf("GAME OVER!\n");
	printf("Citizen(s) has(have) been attacked by a zombie\n");
	return 0;
}
