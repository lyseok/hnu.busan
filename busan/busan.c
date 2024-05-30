#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>

/*
 �ڵ�� 3-3 ��������3:�ù�'��'���� �Ϸ��Ͽ����ϴ�.
 ����ִ� ��� �ùε��� ���� �ձ��� �̵��Ͽ������� �Լ��� �ű��� ���ϰ� �����Լ��� �ξ� �˼��մϴ�.
 �ڵ���� �ۼ��ϴٺ��� �������� ������������ �������� ���� �������� �� �����ϴ�.
 �׷��� �̹���ȸ�� �����ϴµ� ���� ������ �� �� �����ϴ�. 
 �����մϴ�.
*/


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
#define ACTION_REST 0
#define ACTION_PROVOKE 1
#define ACTION_PULL 2

#define MAX_CHARACTERS 25

typedef struct {
    int position;
    int stamina; // ������ ����
    int aggro;
    int pull; // ���� ����
    int life; // �ù� ����
} Character;

int intro(void);
int firstsetup(int*, int*);
int setup(int, int, int*);

int printPattern(int, int, int);
int printCitizen(int, int, int, int);
int printVillain(int, int);
int printZombie(int, int, int);
int printMadongseok(int, int, int);

int moveCitizen(int, int, int);
int moveVillain(int);
int moveZombie(int);
int moveMadongseok(void);

int z_action(int*);
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

int main(void) {
    int game = 0;
    int trainLength = 0;
    int probability = 0;

    int numCitizens = 1;
    int numZombies = 1;

    int m_p_C[MAX_CHARACTERS];
    int aggro[MAX_CHARACTERS];

    int m_p_M, m_p_Z, m_p_V;
    int turn = 0;
    int all;
    int stage = 1;

    srand((unsigned int)time(NULL));
    //��Ʈ��
    intro();

    //����
    firstsetup(&trainLength, &probability);

    while (stage <= 4) {
        if (stage >= 2) {
            turn = 0;
            printf("**********************************************\n");
            printf("stage: %d start\n\n", stage);
        }

        setup(stage, trainLength, &numCitizens);

        int numCharacters = 1 + numCitizens + numZombies;

        //���� �ʱ� ���� ���
        printPattern(trainLength, numCharacters, stage);

        while (1) {
            //�ù� �̵�
            for (int i = 0; i < numCitizens; i++) {
                if (citizen[i].life == 1) {
                    aggro[i] = citizen[i].aggro;
                    m_p_C[i] = moveCitizen(probability, i, trainLength);
                }
            }
            if (stage == 2) m_p_V = moveVillain(m_p_C[0]);

            //���� �̵�
            if (turn % 2 == 0) m_p_Z = moveZombie(probability);

            //���� ���� ���
            printPattern(trainLength, numCharacters, stage);

            //�ù�, ����, ���� ���� ���
            for (int i = 0; i < numCitizens; i++) {
                if (citizen[i].life == 1) {
                    printCitizen(trainLength, m_p_C[i], aggro[i], i);
                }
            }
            printZombie(trainLength, m_p_Z, turn);
            if (stage == 3) {
                all = 0;
                for (int i = 0; i < numCitizens; i++) {
                    if (citizen[i].life == 1) {
                        all++;
                    }
                }
                printf("%d citizen(s) alive(s).", all);
                if (all == 0) {
                    game = 2;
                    break;
                }
            }
            if (stage == 2) printVillain(trainLength, m_p_V);
            printZombie(trainLength, m_p_Z, turn);

            //������ �̵�
            aggro[0] = maDongSeok.aggro;
            m_p_M = moveMadongseok();

            //���� ���� ���
            printPattern(trainLength, numCharacters, stage);

            //������ ���� ���
            printMadongseok(trainLength, m_p_M, aggro[0]);

            if (stage == 2) {
                // ���� �ൿ
                v_action();
            }

            //���� �ൿ
            game = z_action(&numCitizens);
            if (game == 2) break;

            // ������ �ൿ
            m_action(trainLength, probability);


            if (stage == 1 || stage == 2) {
                // �������� 1, 2: �� ���� �ù��� ��ġ�� trainLength-1�̸� ���� ���������� �̵�
                if (citizen[0].life == 1 && citizen[0].position >= trainLength - 1) {
                    game = 1;
                    break;
                }
            }
            else if (stage == 3) {
                // �������� 3: ��� �ù��� ��ġ�� trainLength-1�̾�� ���� ���������� �̵�
                int allEscaped = 1;
                for (int i = 0; i < numCitizens; i++) {
                    if (citizen[i].life == 1 && citizen[i].position < trainLength - 1) {
                        allEscaped = 0;
                        break;
                    }
                }
                if (allEscaped) {
                    game = 1;
                    break;
                }
            }
            else {
                // ��� ����ִ� �ù��� ������ ���� ���� �����ߴ��� Ȯ��
                int c_life = 0;
                for (int i = 0; i < numCitizens; i++) {
                    if (citizen[i].life == 1) c_life = 1;
                }
                if (c_life == 0) break;

                int allEscaped = 1;
                for (int i = 0; i < numCitizens; i++) {
                    if (citizen[i].life == 1 && citizen[i].position != 0) {
                        allEscaped = 0;
                        break;
                    }
                }
                if (allEscaped) {
                    game = 1;
                    break;
                }
            }

            turn++;
        }

        //�ƿ�Ʈ��
        if (game == 1) {
            stage = success(stage);
            game = 0;
        }
        if (game == 2) {
            gameover();
            break;
        }
    }
    return 0;
}

int intro(void) {
    printf("GAME START\n");
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

int setup(int stage, int trainLength, int* numCitizens) {
    maDongSeok.position = 2; // ������
    maDongSeok.aggro = 1;

    if (stage >= 3) {
        *numCitizens = trainLength / 4 + rand() % (trainLength / 2 - trainLength / 4);
    }
    for (int i = 0; i < *numCitizens; i++) {
        int position;
        do {
            position = 6 + rand() % (trainLength - 6);
            int unique = 1;
            for (int j = 0; j < i; j++) {
                if (citizen[j].position == position) {
                    unique = 0;
                    break;
                }
            }
            if (unique) {
                citizen[i].position = position;
                break;
            }
        } while (1);
        citizen[0].position = 6;
        citizen[i].aggro = 1;
        citizen[i].life = 1;
    }

    zombie[0].position = 3; // ����
    zombie[0].pull = 0;

    if (stage == 2) { // ����
        villain.position = 5;
        villain.aggro = 1;
    }

    return 0;
}

int printPattern(int len, int numCharacters, int stage) {
    // ù° �� ���
    for (int i = 0; i < len; i++) {
        printf("#");
    }
    printf("\n");

    // ��° �� ���
    printf("#");
    for (int i = 1; i < len - 1; i++) {
        char c = ' ';
        if (len - maDongSeok.position == i) c = 'M';
        for (int j = 0; j < numCharacters; j++) {
            if (len - citizen[j].position == i && citizen[j].life == 1) c = 'C';
            else if (len - zombie[0].position == i) c = 'Z';
            if (stage == 2) if (len - villain.position == i) c = 'V';
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

int printCitizen(int len, int m_p_C, int aggro, int i) {
    if (citizen[i].position == m_p_C) {
        printf("citizen %d: stay %d (aggro: %d -> %d)\n", i, len - m_p_C, aggro, citizen[i].aggro);
    }
    else {
        printf("citizen %d: %d -> %d (aggro: %d -> %d)\n", i, len - citizen[i].position, len - m_p_C, aggro, citizen[i].aggro);
    }

    citizen[i].position = m_p_C;

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
    // ������ �̵��� ��ġ�� ��ġ�̵� ����
    maDongSeok.position = m_p_M;
    printf("\n");

    return 0;
}

int moveCitizen(int probability, int i, int trainLength) {
    int p = 100 - probability; //Ȯ��
    int move_p_C; //�����̰� �� �ù� ��ġ
    int num = rand() % 100 + 1;

    if (p <= num) {
        move_p_C = citizen[i].position + 1;
        if (citizen[i].position >= trainLength) move_p_C = trainLength;
        citizen[i].aggro++;
        if (citizen[i].aggro > AGGRO_MAX) citizen[i].aggro = AGGRO_MAX;
    }
    else {
        move_p_C = citizen[i].position;
        citizen[i].aggro--;
        if (citizen[i].aggro < AGGRO_MIN) citizen[i].aggro = AGGRO_MIN;
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
    int aggro = 0;
    for (int i = 0; i < MAX_CHARACTERS; i++) {
        if (citizen[i].life == 1) {
            if (aggro < citizen[i].aggro) aggro = citizen[i].aggro;
        }
    }
    if (maDongSeok.aggro > aggro) {
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
        move_p_Z = zombie[0].position + 1;

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

int z_action(int* numCitizens) {
    int m_stm;

    for (int i = 0; i < *numCitizens; i++) {
        if (citizen[i].life == 1 && citizen[i].position - zombie[0].position == 1) {
            citizen[i].life = 0;
        }
    }

    if (zombie[0].position - maDongSeok.position <= 1) {
        m_stm = maDongSeok.stamina--;
        printf("Zombie attacked madongseok (madongseok stamina: %d -> %d)\n", m_stm, maDongSeok.stamina);
        if (maDongSeok.stamina == 0) {
            printf("GAME OVER! madongseok dead...\n");
            return 2;
        }
    }

    int liveCitizens = 0;
    for (int i = 0; i < *numCitizens; i++) {
        if (citizen[i].life == 1) {
            liveCitizens++;
        }
    }

    if (liveCitizens == 0) {
        printf("All citizens are dead.\n");
        return 2;
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