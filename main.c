#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

int LARGURA, ALTURA, LARGMAT, ALTMAT;

typedef struct Jogador {
    int posx;
    int posy;
    int desx;
    int desy;
} Jogador;

Jogador jogador;

typedef struct Celula {
    int x, y; // coordenadas da célula
    int visitado; // flag que indica se a célula já foi visitada
    int paredes[4]; // lista de paredes (norte, leste, sul, oeste)
} Celula;

Celula* vizinhos[4];    //Celulas vizinhas que será utilizadas para selecionar recursivamente e aleatoriamente
                        //os vizinhos que ainda não foram visitados, até todas as celulas serem escolhidas.
typedef struct Labirinto {
    Celula **Celulas; // matriz de células que formam o desenho do labirinto
} Labirinto;

int **matrizLab;      // matriz de inteiros, representando o labirinto desenhado numericamente
                      // (1) para paredes, (0) espaços vazios, (2) jogador.

// Função que inicializa as células do labirinto
void init_Celulas(Labirinto* Labirinto) {
    for (int i = 0; i < LARGMAT; i++)
        for(int j = 0; j < ALTMAT; j++)
            matrizLab[i][j] = 1;                //Inicializa a matriz contendo tudo como parade
    for (int y = 0; y < ALTURA; y++) {
        for (int x = 0; x < LARGURA; x++) {
            Celula Celula = { x, y, 0, {1, 1, 1, 1} };  //Inicia a celula onde cada uma será responsavel por 4 sentidos
            Labirinto->Celulas[x][y] = Celula;
            matrizLab[(y*2)+1][(x*2)+1] = 0;            //Como cada celula possui mais 4, a matriz tem q ser o dobro maior mais 1 para conseguir representar em matriz.
        }                                               //Assim cada célula, vai receber como vazia, depois será realizada o remover das paredes aleatoriamente conforme é gerado o labirinto.
    }
}

// Função que retorna uma célula vizinha aleatória que ainda não foi visitada
Celula* vizinhoAleatorio(Labirinto* labAtual, Celula* viz) {
    int x = viz->x;
    int y = viz->y;
    for(int i=0;i<4;i++)
        vizinhos[i] = NULL;
    int count = 0;
    if (y > 0 && !labAtual->Celulas[x][y-1].visitado)
        vizinhos[count++] = &labAtual->Celulas[x][y-1]; // norte
    if (x < LARGURA-1 && !labAtual->Celulas[x+1][y].visitado)
        vizinhos[count++] = &labAtual->Celulas[x+1][y]; // leste
    if (y < ALTURA-1 && !labAtual->Celulas[x][y+1].visitado)
        vizinhos[count++] = &labAtual->Celulas[x][y+1]; // sul
    if (x > 0 && !labAtual->Celulas[x-1][y].visitado)
        vizinhos[count++] = &labAtual->Celulas[x-1][y]; // oeste
    if (count == 0)
        return NULL;
    int index = rand() % count;
    return vizinhos[index];
}

void removeParede(Celula* atual, Celula* vizinho) {
    if (atual->x < vizinho->x) {                        //Esquerdo
        atual->paredes[1] = 0; // leste do atual
        vizinho->paredes[3] = 0; // oeste do vizinho
        matrizLab[(atual->y*2)+1][(atual->x*2)+2] = 0;
    } else if (atual->x > vizinho->x) {                 //Direito
        atual->paredes[3] = 0; // oeste do atual
        vizinho->paredes[1] = 0; // leste do vizinho
        matrizLab[(atual->y*2)+1][(atual->x*2)] = 0;
    } else if (atual->y < vizinho->y) {                 //Acima
        atual->paredes[2] = 0; // sul do atual
        vizinho->paredes[0] = 0; // norte do vizinho
        matrizLab[(atual->y*2)+2][(atual->x*2)+1] = 0;
    } else if (atual->y > vizinho->y) {                 //Abaixo
        atual->paredes[0] = 0; // norte do atual
        vizinho->paredes[2] = 0; // sul do vizinho
        matrizLab[(atual->y*2)][(atual->x*2)+1] = 0;
    }
}

// Função recursiva que gera o labirinto usando o algoritmo Recursive Backtracking
void gerarLabirinto(Labirinto* Labirinto, Celula* atual) {
    atual->visitado = 1; // marca a célula como visitada
    Celula* vizinho = vizinhoAleatorio(Labirinto, atual);
    while (vizinho) {
        removeParede(atual, vizinho); // remove a parede entre as células
        gerarLabirinto(Labirinto, vizinho); // visita a célula vizinha recursivamente
        vizinho = vizinhoAleatorio(Labirinto, atual); // escolhe outra célula vizinha aleatória
    }
}

// Função que imprime o labirinto na tela
void printLabirinto(Labirinto* Labirinto) {
    // imprime a primeira linha de paredes norte
    printf("+");
    for (int x = 0; x < LARGURA; x++)
        printf("%s", Labirinto->Celulas[x][0].paredes[0] ? "--+" : "  +");
    printf("\n");
    // imprime as linhas do meio com paredes oeste e leste
    for (int y = 0; y < ALTURA; y++) {
        printf("%s", Labirinto->Celulas[0][y].paredes[3] ? "|" : " ");
        for (int x = 0; x < LARGURA; x++)
            printf("  %s", Labirinto->Celulas[x][y].paredes[1] ? "|" : " ");
        printf("\n");
        // imprime as paredes sul da linha atual, se existirem
        if (y < ALTURA-1) {
            printf("+");
            for (int x = 0; x < LARGURA-1; x++)
                printf("%s+", Labirinto->Celulas[x][y].paredes[2] ? "--" : "  ");
            printf("%s\n", Labirinto->Celulas[LARGURA-1][y].paredes[2] ? "--+" : "  +");
        }
    }
    // imprime a última linha de paredes sul
    printf("+");
    for (int x = 0; x < LARGURA; x++) {
        printf("%s", Labirinto->Celulas[x][ALTURA-1].paredes[2] ? "--+" : "  +");
    }
    printf("\n");
}

// Função que retorna uma posição aleatória das laterais, definindo o inicio e final do labirinto, dependendo do tipo.
int randomInicialFinal(int tipo) {
    int div;
    if(tipo)    //Se for 1, irá definir a casa inicial.
        div = (LARGURA * 0.4);
    else        //Caso 0, irá definir a casa final.
        div = (ALTURA * 0.5);
    return rand() % div;
}

void imprimeMatriz() {
    for(int i=0;i<LARGMAT;i++) {
        for(int j=0;j<ALTMAT;j++)
            printf("%d ", matrizLab[i][j]);
        printf("\n");
    }
}

void informaAltLarg() {
    do {
        printf("Informe a Altura do labirinto (minimo 5) -> ");
        scanf("%d", &ALTURA);
        if(ALTURA < 5) {
            system("cls");
            printf("Altura invalida!\n\n");
        }
    }while(ALTURA < 5);
    do {
        printf("Informe a Largura do labirinto (minimo 5) -> ");
        scanf("%d", &LARGURA);
        if(LARGURA < 5) {
            system("cls");
            printf("Largura invalida!\n\n");
        }
    }while(LARGURA < 5);
    system("cls");

    LARGMAT = ((ALTURA*2)+1);
    ALTMAT = ((LARGURA*2)+1);
}

void aleatorizaCaminhos(Labirinto* lab) {
    int aletLarg, aletAlt;
    int coef = (LARGURA * ALTURA) * 0.25;
    int marcado[LARGURA * ALTURA];
    for(int i=0; i<(LARGURA*ALTURA); i++)
        marcado[i] = 0;
    system("pause");
    for(int i=0; i<coef; i++) {
        aletLarg = 1 + (rand() % LARGURA - 1);
        aletAlt = 1 + (rand() % ALTURA - 1);
        if(marcado[aletLarg * aletAlt] != 1) {
            int count = 0;
            int paredes[4];
            for(int j=0; j<4; j++)
                if(&lab->Celulas[aletLarg][aletAlt].paredes[j])
                    paredes[count++] = j;
            int viz = rand() % count;
            if(count >= 2) {
                switch(paredes[viz]) {
                    case 0:     //Norte

                        removeParede(&lab->Celulas[aletLarg][aletAlt], &lab->Celulas[aletLarg][aletAlt-1]);
                        break;
                    case 1:     //Leste
                        removeParede(&lab->Celulas[aletLarg][aletAlt], &lab->Celulas[aletLarg+1][aletAlt]);
                        break;
                    case 2:     //Sul
                        removeParede(&lab->Celulas[aletLarg][aletAlt], &lab->Celulas[aletLarg][aletAlt+1]);
                        break;
                    case 3:     //Oeste
                        removeParede(&lab->Celulas[aletLarg][aletAlt], &lab->Celulas[aletLarg-1][aletAlt]);
                }
                /*if(count == 2) {
                    for(int j=0; j<2; j++)
                        if(paredes[j]=!paredes[viz])
                            int viz2 = paredes[j];
                    int flag = rand % 2;
                    switch(paredes[viz]) {
                        case 0:     //Norte
                            switch(viz2) {
                                case 1:     //Leste
                                    if(flag)

                                    else

                                    break;
                                case 2:     //Sul
                                    if(flag)

                                    else

                                    break;
                                case 3:     //Oeste
                                    if(flag)

                                    else

                            }
                            break;
                        case 1:     //Leste
                            switch(viz2) {
                                case 0:     //Norte
                                    if(flag)

                                    else

                                    break;
                                case 2:     //Sul

                                    break;
                                case 3:     //Oeste
                            }
                            break;
                        case 2:     //Sul
                            switch(viz2) {
                                case 0:     //Norte

                                    break;
                                case 1:     //Leste

                                    break;
                                case 3:     //Oeste
                            }
                            break;
                        case 3:     //Oeste
                            switch(viz2) {
                                case 0:     //Norte

                                    break;
                                case 1:     //Leste

                                    break;
                                case 2:     //Sul
                            }
                    }
                }*/
            }
            marcado[aletLarg * aletAlt] = 1;
        }
    }
}

void iniciaLabirinto() {
    srand(time(NULL));
    informaAltLarg();

    Labirinto lab;
    lab.Celulas = malloc(LARGURA * sizeof(Celula*));
    for(int i=0; i<LARGURA; i++)
        lab.Celulas[i] = malloc(ALTURA * sizeof(Celula));

    matrizLab = malloc(LARGMAT * sizeof(int*));
    for(int i=0; i<LARGMAT; i++)
        matrizLab[i] = malloc(ALTMAT * sizeof(int));

    init_Celulas(&lab);
    int ini = randomInicialFinal(1);
    int end = randomInicialFinal(0);
    lab.Celulas[ini][ALTURA-1].paredes[2] = 0;
    lab.Celulas[LARGURA-1][end].paredes[1] = 0;

    jogador.posx = (ini*2)+1;
    jogador.posy = ALTURA*2;
    jogador.desx = (3*ini)+1;
    jogador.desy =  ALTURA*2;
    matrizLab[(end*2)+1][LARGURA*2] = 0;
    matrizLab[ALTURA*2][(ini*2)+1] = 2;

    Celula* inicio = &lab.Celulas[ini][ALTURA-1]; // escolhe a célula inicial (canto superior esquerdo)
    gerarLabirinto(&lab, inicio);
    aleatorizaCaminhos(&lab);
    printLabirinto(&lab);
    /*for(int i=0;i<LARGMAT; i++) {
        for(int j=0; j<ALTMAT; j++)
            printf("%d ", matrizLab[i][j]);
        printf("\n");
    }
    system("pause");*/
    for(int i=0; i<LARGURA; i++)
        free(lab.Celulas[i]);
    free(lab.Celulas);

    printf("\n");
}

int testVencer() {
    if(jogador.posx >= (LARGURA*2)+1)
        return 0;
    return 1;
}

void gotoxy(int x, int y){
     SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),(COORD){x,y});
}

int jogo() {
    int flagMov = 1, flagDesenho = 1;
    char opc;
    do {
        if(flagDesenho) {
            gotoxy(jogador.desx, jogador.desy);
            printf("##");
            flagDesenho = 0;
        }
        opc = getch();
        switch(opc) {
            case 'w':       //Acima
                if(jogador.posy-2 >= 0) {
                    if(matrizLab[jogador.posy-1][jogador.posx] == 0) {
                        gotoxy(jogador.desx, jogador.desy);
                        printf("/\\");
                        flagDesenho = 1;
                        if(flagMov) {
                            matrizLab[jogador.posy][jogador.posx] = 0;
                            jogador.posy--;
                            matrizLab[jogador.posy][jogador.posx] = 2;
                            jogador.desy--;
                            flagMov = 0;
                        } else {
                            matrizLab[jogador.posy][jogador.posx] = 0;
                            jogador.posy-=2;
                            matrizLab[jogador.posy][jogador.posx] = 2;
                            jogador.desy-=2;
                        }
                    }
                }
                break;
            case 'a':       //Esquerdo
                if(jogador.posx-2 >= 0) {
                    if(matrizLab[jogador.posy][jogador.posx-1] == 0) {
                        gotoxy(jogador.desx, jogador.desy);
                        printf("<-");
                        flagDesenho = 1;
                        matrizLab[jogador.posy][jogador.posx] = 0;
                        jogador.posx-=2;
                        matrizLab[jogador.posy][jogador.posx] = 2;
                        jogador.desx-=3;
                    }
                }
                break;
            case 's':       //Abaixo
                if(jogador.posy+2 <= (ALTURA*2)) {
                    if(matrizLab[jogador.posy+1][jogador.posx] == 0) {
                        gotoxy(jogador.desx, jogador.desy);
                        printf("\\/");
                        flagDesenho = 1;
                        matrizLab[jogador.posy][jogador.posx] = 0;
                        jogador.posy+=2;
                        matrizLab[jogador.posy][jogador.posx] = 2;
                        jogador.desy+=2;
                    }
                }
                break;
            case 'd':       //Direita
                if(jogador.posx+2 <= (LARGURA*2)+1) {
                    if(matrizLab[jogador.posy][jogador.posx+1] == 0) {
                        gotoxy(jogador.desx, jogador.desy);
                        printf("->");
                        flagDesenho = 1;
                        matrizLab[jogador.posy][jogador.posx] = 0;
                        jogador.posx+=2;
                        matrizLab[jogador.posy][jogador.posx] = 2;
                        jogador.desx+=3;
                    }
                }
        }
    }while(testVencer());
    gotoxy(0, ALTURA*2+3);

    for(int i=0; i<LARGMAT; i++)
        free(matrizLab[i]);
    free(matrizLab);

    return 1;
}

int main() {
    iniciaLabirinto();
    return jogo();
}
