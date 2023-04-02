#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <windows.h>
#include <stdbool.h>

typedef struct Jogador {
    int posMatx;
    int posMaty;
    int desenhox;
    int desenhoy;
} Jogador;

typedef struct Celula {
    int x, y; // coordenadas da célula
    int visitado; // flag que indica se a célula já foi visitada
    int paredes[4]; // lista de paredes (norte, leste, sul, oeste)
} Celula;

typedef struct Labirinto {
    Celula **Celulas; // matriz de células que formam o desenho do labirinto
} Labirinto;

Jogador jogador;

Labirinto lab;

Celula* vizinhos[4];    //Celulas vizinhas que será utilizadas para selecionar recursivamente e aleatoriamente
                        //os vizinhos que ainda não foram visitados, até todas as celulas serem escolhidas.

int LARGURA, ALTURA, LINHAS, COLUNAS;

int **matrizLab;      // matriz de inteiros, representando o labirinto desenhado numericamente
                      // (1) para paredes, (0) espaços vazios, (2) jogador.

int deslcLinhas[4]  = {-1, 0, 1,  0};//(norte, leste, sul, oeste)
int deslcColunas[4] = { 0, 1, 0, -1};//(norte, leste, sul, oeste)
int dirMatriz[4] =    {4, 5, 6, 7}; //(norte, leste, sul, oeste)

// Função que inicializa as células do labirinto
void init_Celulas(Labirinto* Labirinto) {
    for (int i = 0; i < LINHAS; i++)
        for(int j = 0; j < COLUNAS; j++)
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

// Função teste para imprimir a matriz.
void imprimeMatriz() {
    for(int i=0;i<LINHAS;i++) {
        for(int j=0;j<COLUNAS;j++)
            printf("%d ", matrizLab[i][j]);
        printf("\n");
    }
}

// Função que pede ao usuário altura e largura do labirinto que será gerado.
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

    LINHAS = ((ALTURA*2)+2);   //Como a célula de labirinto tem 1 casa com 4 paredes, a representação disso é 0 rodeado por 1 como parede, que serão removidas aleatóriamente.
    COLUNAS = ((LARGURA*2)+2);
}

// Função que escolhe algumas casa aleatóriamente, afim de ramificar ainda mais o labirinto e possibilitar vários caminhos que conseguem chegar ao final.
void aleatorizaCaminhos(Labirinto* lab) {
    Celula* viz;                            //Celula vizinha que sera randomizada.
    int aletLarg, aletAlt, vizRand;
    int coef = (LARGURA * ALTURA) * 0.15;   //Determina a porcetagem de células que seráo escolhidas aleatóriamente para remover paredes aleatóriamente também.
    int marcado[LARGURA][ALTURA];           //Matriz auxiliar, para evitar escolher células repetidas.

    for(int i=0; i<LARGURA; i++)
        for(int j=0; j<ALTURA;j++)
            marcado[i][j] = 0;              //Inicializa todas como nao marcado.

    for(int i=0; i<coef; i++) {
        aletLarg = 2 + (rand() % (LARGURA - 2));            //Randomiza uma casa em largura na matriz.
        aletAlt = 2 + (rand() % (ALTURA - 2));              //O mesmo so que em altura.

        Celula* atual = &lab->Celulas[aletLarg][aletAlt];   //atribui a célula escolhida aleatóriamente.

        if(marcado[aletLarg][aletAlt] != 1) {               //Se não for marcado, então prossegue para remoção da parede
            int count = 0;
            int paredes[4];

            for(int j=0; j<4; j++) {
                if(atual->paredes[j]) {         //Verifica quais dos 4 pontos da célula tem parede.
                    paredes[count] = j;         //Caso um dos lados tem, guarda a informação de qual lugar no vetor está a parede.
                    count++;
                }
            }

            if(count >= 2) {                    //Caso tenha mais de 2 paredes, ele remove uma.
                vizRand = rand() % count;       //Escolhe aleatóriamente um dos 4 lados.
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
                    removeParede(atual, viz);   //Se por ajuda do universo o vizinho não for nulo, entao ele informa pra função as células para remover a parede.
            }
            marcado[aletLarg][aletAlt] = 1;     //Marca para evitar repetir mes célula.
        }
    }

}

//Função crucial para inicialização da construção  do labirinto gerado automáticamente.
void iniciaLabirinto() {

    srand(time(NULL));
    informaAltLarg();

    lab.Celulas = malloc(LARGURA * sizeof(Celula*));
    for(int i=0; i<LARGURA; i++)
        lab.Celulas[i] = malloc(ALTURA * sizeof(Celula));

    matrizLab = malloc(LINHAS * sizeof(int*));
    for(int i=0; i<LINHAS; i++)
        matrizLab[i] = malloc(COLUNAS * sizeof(int));

    init_Celulas(&lab);
    int ini = randomInicialFinal(1);            //Para 1 é casa inicial.
    int end = randomInicialFinal(0);            //Para 0 é casa final.
    lab.Celulas[ini][ALTURA-1].paredes[2] = 0;
    lab.Celulas[LARGURA-1][end].paredes[1] = 0;

    jogador.posMatx = (ini*2)+1;
    jogador.posMaty = ALTURA*2;
    jogador.desenhox = (3*ini)+1;
    jogador.desenhoy =  ALTURA*2;

    matrizLab[(end*2)+1][LARGURA*2] = 0;
    matrizLab[(end*2)+1][(LARGURA*2)+1] = 3;
    matrizLab[ALTURA*2][(ini*2)+1] = 0;
    matrizLab[(ALTURA*2)+1][(ini*2)+1] = 2;

    Celula* inicio = &lab.Celulas[ini][ALTURA-1]; // escolhe a célula inicial (canto superior esquerdo)
    gerarLabirinto(&lab, inicio);
    aleatorizaCaminhos(&lab);
    //printLabirinto(&lab);
    //imprimeMatriz();

    /*for(int i=0; i<LARGURA; i++)
        free(lab.Celulas[i]);
    free(lab.Celulas);*/

    printf("\n");
}
///Inicio das funções que terão a funcionalidade de realizar teste em si no labirinto.
void gotoxy(int x, int y) {
     SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),(COORD){x,y});
}

int testVencer() {
    if(jogador.posMatx >= (LARGURA*2)+1)
        return 0;
    return 1;
}

int jogo() {
    int flagMov = 1, flagDesenho = 1;
    char opc;
    do {
        if(flagDesenho) {
            gotoxy(jogador.desenhox, jogador.desenhoy);
            printf("##");
            flagDesenho = 0;
        }
        opc = getch();
        switch(opc) {
            case 'w':       //Acima
                if(jogador.posMaty-2 >= 0) {
                    if(matrizLab[jogador.posMaty-1][jogador.posMatx] == 0) {
                        gotoxy(jogador.desenhox, jogador.desenhoy);
                        printf("/\\");
                        flagDesenho = 1;
                        if(flagMov) {
                            matrizLab[jogador.posMaty][jogador.posMatx] = 0;
                            jogador.posMaty--;
                            matrizLab[jogador.posMaty][jogador.posMatx] = 2;
                            jogador.desenhoy--;
                            flagMov = 0;
                        } else {
                            matrizLab[jogador.posMaty][jogador.posMatx] = 0;
                            jogador.posMaty-=2;
                            matrizLab[jogador.posMaty][jogador.posMatx] = 2;
                            jogador.desenhoy-=2;
                        }
                    }
                }
                break;
            case 'a':       //Esquerdo
                if(jogador.posMatx-2 >= 0) {
                    if(matrizLab[jogador.posMaty][jogador.posMatx-1] == 0) {
                        gotoxy(jogador.desenhox, jogador.desenhoy);
                        printf("<-");
                        flagDesenho = 1;
                        matrizLab[jogador.posMaty][jogador.posMatx] = 0;
                        jogador.posMatx-=2;
                        matrizLab[jogador.posMaty][jogador.posMatx] = 2;
                        jogador.desenhox-=3;
                    }
                }
                break;
            case 's':       //Abaixo
                if(jogador.posMaty+2 <= (ALTURA*2)) {
                    if(matrizLab[jogador.posMaty+1][jogador.posMatx] == 0) {
                        gotoxy(jogador.desenhox, jogador.desenhoy);
                        printf("\\/");
                        flagDesenho = 1;
                        matrizLab[jogador.posMaty][jogador.posMatx] = 0;
                        jogador.posMaty+=2;
                        matrizLab[jogador.posMaty][jogador.posMatx] = 2;
                        jogador.desenhoy+=2;
                    }
                }
                break;
            case 'd':       //Direita
                if(jogador.posMatx+2 <= (LARGURA*2)+1) {
                    if(matrizLab[jogador.posMaty][jogador.posMatx+1] == 0) {
                        gotoxy(jogador.desenhox, jogador.desenhoy);
                        printf("->");
                        flagDesenho = 1;
                        matrizLab[jogador.posMaty][jogador.posMatx] = 0;
                        jogador.posMatx+=2;
                        matrizLab[jogador.posMaty][jogador.posMatx] = 2;
                        jogador.desenhox+=3;
                    }
                }
        }
    }while(testVencer());

    gotoxy(0, ALTURA*2+3);

    for(int i=0; i<LINHAS; i++)
        free(matrizLab[i]);
    free(matrizLab);

    return 1;
}

bool validacaoProfunda(int labVal[][COLUNAS], int linha, int coluna) {

    if(linha < 0 || linha >= LINHAS || coluna < 0 || coluna >= COLUNAS || labVal[linha][coluna] == 1 || labVal[linha][coluna] >= 4)
        return false;

    return true;
}

bool buscaProfunda(int matBusca[][COLUNAS], int linha, int coluna, int dir) {

    //Caso ele esteja na casa aonde contem 3 (representando o final), termina a busca e retorna como possível finalizar o labirinto
    if(matBusca[linha][coluna] == 3)
        return true;

    //Valida a casa aonde ele está operando, se for validada como espaço vazio dentro da matriz e não visitada, marca como vistada representado pela variavel dir, aonde será a direção para onde foi sendo (.
    if(validacaoProfunda(matBusca, linha, coluna))
        matBusca[linha][coluna] = dir;

    // Exploramos as quatro direções possíveis a partir da posição atual
    for (int i = 0; i < 4; i++) {
        int proxLinha = linha + deslcLinhas[i];
        int proxColuna = coluna + deslcColunas[i];
        int direcao = dirMatriz[i];

        // Se a próxima posição é válida, fazemos a chamada recursiva
        if (validacaoProfunda(matBusca, proxLinha, proxColuna)) {
            if (buscaProfunda(matBusca, proxLinha, proxColuna, direcao)) {
                return true;
            }
        }
    }

    // Se nenhuma das direções levou à saída, marcamos a posição como vazia novamente e retornamos falso
    matBusca[linha][coluna] = 0;
    return false;
}

void opcBuscaCega(int matBusca[][COLUNAS]) {
    system("cls");
    if(buscaProfunda(matBusca, jogador.posMaty, jogador.posMatx, dirMatriz[0])) {
        for(int i=0; i<LINHAS; i++) {
            for(int j=0; j<COLUNAS; j++) {
                switch(matBusca[i][j]) {
                        case 0:
                            if(j%2 == 0)
                                printf(" ");
                            else
                                printf("  ");
                            break;
                        case 1:
                            if(i%2 == 0) {
                                if(j%2 == 0)
                                    printf("+");
                                else
                                    printf("--");
                            } else
                                if(j%2 == 0)
                                    printf("|");
                                else
                                    printf("++");
                            break;
                        case 2:
                            printf("SS");
                            break;
                        case 3:
                            printf("FF");
                            break;
                        case 4:     //Norte
                            printf("/\\");
                            break;
                        case 5:     //Leste
                            if(j%2 == 0)
                                printf(">");
                            else
                                printf(">>");
                            break;
                        case 6:     //Sul
                            printf("\\/");
                            break;
                        case 7:     //Oeste
                            if(j%2 == 0)
                                printf("<");
                            else
                                printf("<<");
                }
            }
            printf("\n");
        }
        for(int i=0; i<LINHAS; i++) {
            for(int j=0; j<COLUNAS; j++) {
                printf("%d ", matBusca[i][j]);
            }
            printf("\n");
        }
    }
    printf("\n");
}

bool menu() {
    int opc, matBusca[LINHAS][COLUNAS];

    do {
        do{
            printf("#-=-=-=-=-| Labirinto %dx%d |-=-=-=-=-=-#\n\t(1)Realizar Busca Cega. (Profundidade)\n\t(2)Imprimir Labirinto.\n\t(0)Sair.\n#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#\n\n\tOpção: ", ALTURA, LARGURA);
            scanf("%d", &opc);
            if(opc < 0 || opc > 2) {
                system("cls");
                printf("Opção inválida!\n\n");
            }
        }while(opc < 0 || opc > 2);
        if(opc != 0)
            for(int i=0; i<LINHAS; i++)
                for(int j=0; j<COLUNAS; j++)
                    matBusca[i][j] = matrizLab[i][j];
        switch(opc) {
                case 1:
                    opcBuscaCega(matBusca);
                    break;
                case 2:
                    system("cls");
                    //printf("posx - %d | posy - %d\n", jogador.posMatx, jogador.posMaty);
                    imprimeMatriz();
                    printLabirinto(&lab);
                    break;
                case 0:
                    return true;
        }
    }while(1);
}


int main() {
    setlocale(LC_ALL, "");
    iniciaLabirinto();

    return menu();
    //return jogo();
}
