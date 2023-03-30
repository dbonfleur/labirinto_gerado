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
    int x, y; // coordenadas da c�lula
    int visitado; // flag que indica se a c�lula j� foi visitada
    int paredes[4]; // lista de paredes (norte, leste, sul, oeste)
} Celula;

Celula* vizinhos[4];    //Celulas vizinhas que ser� utilizadas para selecionar recursivamente e aleatoriamente
                        //os vizinhos que ainda n�o foram visitados, at� todas as celulas serem escolhidas.
typedef struct Labirinto {
    Celula **Celulas; // matriz de c�lulas que formam o desenho do labirinto
} Labirinto;

int **matrizLab;      // matriz de inteiros, representando o labirinto desenhado numericamente
                      // (1) para paredes, (0) espa�os vazios, (2) jogador.

// Fun��o que inicializa as c�lulas do labirinto
void init_Celulas(Labirinto* Labirinto) {
    for (int i = 0; i < LARGMAT; i++)
        for(int j = 0; j < ALTMAT; j++)
            matrizLab[i][j] = 1;                //Inicializa a matriz contendo tudo como parade
    for (int y = 0; y < ALTURA; y++) {
        for (int x = 0; x < LARGURA; x++) {
            Celula Celula = { x, y, 0, {1, 1, 1, 1} };  //Inicia a celula onde cada uma ser� responsavel por 4 sentidos
            Labirinto->Celulas[x][y] = Celula;
            matrizLab[(y*2)+1][(x*2)+1] = 0;            //Como cada celula possui mais 4, a matriz tem q ser o dobro maior mais 1 para conseguir representar em matriz.
        }                                               //Assim cada c�lula, vai receber como vazia, depois ser� realizada o remover das paredes aleatoriamente conforme � gerado o labirinto.
    }
}

// Fun��o que retorna uma c�lula vizinha aleat�ria que ainda n�o foi visitada
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

// Fun��o recursiva que gera o labirinto usando o algoritmo Recursive Backtracking
void gerarLabirinto(Labirinto* Labirinto, Celula* atual) {
    atual->visitado = 1; // marca a c�lula como visitada
    Celula* vizinho = vizinhoAleatorio(Labirinto, atual);
    while (vizinho) {
        removeParede(atual, vizinho); // remove a parede entre as c�lulas
        gerarLabirinto(Labirinto, vizinho); // visita a c�lula vizinha recursivamente
        vizinho = vizinhoAleatorio(Labirinto, atual); // escolhe outra c�lula vizinha aleat�ria
    }
}

// Fun��o que imprime o labirinto na tela
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
    // imprime a �ltima linha de paredes sul
    printf("+");
    for (int x = 0; x < LARGURA; x++) {
        printf("%s", Labirinto->Celulas[x][ALTURA-1].paredes[2] ? "--+" : "  +");
    }
    printf("\n");
}

// Fun��o que retorna uma posi��o aleat�ria das laterais, definindo o inicio e final do labirinto, dependendo do tipo.
int randomInicialFinal(int tipo) {
    int div;
    if(tipo)    //Se for 1, ir� definir a casa inicial.
        div = (LARGURA * 0.4);
    else        //Caso 0, ir� definir a casa final.
        div = (ALTURA * 0.5);
    return rand() % div;
}

// Fun��o teste para imprimir a matriz.
/*void imprimeMatriz() {
    for(int i=0;i<LARGMAT;i++) {
        for(int j=0;j<ALTMAT;j++)
            printf("%d ", matrizLab[i][j]);
        printf("\n");
    }
}*/

// Fun��o que pede ao usu�rio altura e largura do labirinto que ser� gerado.
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

    LARGMAT = ((ALTURA*2)+1);   //Como a c�lula de labirinto tem 1 casa com 4 paredes, a representa��o disso � 0 rodeado por 1 como parede, que ser�o removidas aleat�riamente.
    ALTMAT = ((LARGURA*2)+1);
}

// Fun��o que escolhe algumas casa aleat�riamente, afim de ramificar ainda mais o labirinto e possibilitar v�rios caminhos que conseguem chegar ao final.
void aleatorizaCaminhos(Labirinto* lab) {
    Celula* viz;                            //Celula vizinha que sera randomizada.
    int aletLarg, aletAlt, vizRand;
    int coef = (LARGURA * ALTURA) * 0.15;   //Determina a porcetagem de c�lulas que ser�o escolhidas aleat�riamente para remover paredes aleat�riamente tamb�m.
    int marcado[LARGURA][ALTURA];           //Matriz auxiliar, para evitar escolher c�lulas repetidas.

    for(int i=0; i<LARGURA; i++)
        for(int j=0; j<ALTURA;j++)
            marcado[i][j] = 0;              //Inicializa todas como nao marcado.

    for(int i=0; i<coef; i++) {
        aletLarg = 2 + (rand() % (LARGURA - 2));            //Randomiza uma casa em largura na matriz.
        aletAlt = 2 + (rand() % (ALTURA - 2));              //O mesmo so que em altura.

        Celula* atual = &lab->Celulas[aletLarg][aletAlt];   //atribui a c�lula escolhida aleat�riamente.

        if(marcado[aletLarg][aletAlt] != 1) {               //Se n�o for marcado, ent�o prossegue para remo��o da parede
            int count = 0;
            int paredes[4];

            for(int j=0; j<4; j++) {
                if(atual->paredes[j]) {         //Verifica quais dos 4 pontos da c�lula tem parede.
                    paredes[count] = j;         //Caso um dos lados tem, guarda a informa��o de qual lugar no vetor est� a parede.
                    count++;
                }
            }

            if(count >= 2) {                    //Caso tenha mais de 2 paredes, ele remove uma.
                vizRand = rand() % count;       //Escolhe aleat�riamente um dos 4 lados.
                viz = NULL;                     //Anula vizinho.

                switch(paredes[vizRand]) {      //Escolhe a parede randomizada sendo 0 -> Norte, 1 -> Leste, 2 -> Sul, 3 -> Oeste
                    case 0:                     //Norte
                        if((aletAlt+1) > 0)
                            viz = &lab->Celulas[aletLarg][aletAlt-1];
                        break;
                    case 1:                     //Leste
                        if((aletLarg+1) < LARGURA)
                            viz = &lab->Celulas[aletLarg+1][aletAlt];
                        break;
                    case 2:                     //Sul
                        if((aletAlt+1) < ALTURA)
                            viz = &lab->Celulas[aletLarg][aletAlt+1];
                        break;
                    case 3:                     //Oeste
                        if((aletLarg-1) > 0)
                            viz = &lab->Celulas[aletLarg-1][aletAlt];
                }
                if(viz != NULL)                 //Por ironia do destino, as vezes o vizinho vem nulo mesmo, isso evita bug.
                    removeParede(atual, viz);   //Se por ajuda do universo o vizinho n�o for nulo, entao ele informa pra fun��o as c�lulas para remover a parede.
            }
            marcado[aletLarg][aletAlt] = 1;     //Marca para evitar repetir mes c�lula.
        }
    }

}

//Fun��o crucial para inicializa��o da constru��o  do labirinto gerado autom�ticamente.
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

    Celula* inicio = &lab.Celulas[ini][ALTURA-1]; // escolhe a c�lula inicial (canto superior esquerdo)
    gerarLabirinto(&lab, inicio);
    aleatorizaCaminhos(&lab);
    printLabirinto(&lab);
    //imprimeMatriz();

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

void gotoxy(int x, int y) {
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
