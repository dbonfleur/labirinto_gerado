#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <string.h>
#include <windows.h>
#include <stdbool.h>
#define FATORALEATORIO 0.2

typedef struct Jogador {
    int posInix;
    int posIniy;
    int posFimx;
    int posFimy;
    int desenhox;
    int desenhoy;
    int movimentos;
} Jogador;

typedef struct Celula {
    int x, y; // coordenadas da c�lula
    int visitado; // flag que indica se a c�lula j� foi visitada
    int paredes[4]; // lista de paredes (norte, leste, sul, oeste)
} Celula;

typedef struct Labirinto {
    Celula **Celulas; // matriz de c�lulas que formam o desenho do labirinto
} Labirinto;

typedef struct {
    int linha;
    int coluna;
    int f_score;
} node;

Jogador jogador;

Labirinto lab;

Celula* vizinhos[4];    //Celulas vizinhas que ser� utilizadas para selecionar recursivamente e aleatoriamente
                        //os vizinhos que ainda n�o foram visitados, at� todas as celulas serem escolhidas.

int LARGURA, ALTURA, LINHAS, COLUNAS;

int **matrizLab;      // matriz de inteiros, representando o labirinto desenhado numericamente
                      // (1) para paredes, (0) espa�os vazios, (2) jogador.

int deslcLinhas[4]  =   {-1, 0, 1,  0};                 //(norte, leste, sul, oeste)
int deslcColunas[4] =   { 0, 1, 0, -1};                 //(norte, leste, sul, oeste)
int dirMatriz[4] =      {4, 5, 6, 7};                   //(norte, leste, sul, oeste)
int randDirecoes[10] =  {0, 0, 0, 1, 1, 1, 1, 1, 2, 3}; //(30% norte, 50% leste, 10% sul, 10% oeste)

char nomeArq[30];
FILE *arq;

// Fun��o que inicializa as c�lulas do labirinto
void init_Celulas(Labirinto* Labirinto) {
    for (int i = 0; i < LINHAS; i++)
        for(int j = 0; j < COLUNAS; j++)
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
    system("cls");
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
    printf("\n\n");
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
void imprimeMatriz() {
    system("cls");
    for(int i=0;i<LINHAS;i++) {
        for(int j=0;j<COLUNAS;j++)
            printf("%d ", matrizLab[i][j]);
        printf("\n");
    }
}

// Fun��o que pede ao usu�rio altura e largura do labirinto que ser� gerado, al�m do nome do arquivo que ser� salvo os testes.
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

    LINHAS = ((ALTURA*2)+2);   //Como a c�lula de labirinto tem 1 casa com 4 paredes, a representa��o disso � 0 rodeado por 1 como parede, que ser�o removidas aleat�riamente.
    COLUNAS = ((LARGURA*2)+2);
}

void criaArquivo()  {
    printf("Informe o nome do arquivo para salvar o caminho realizado no labirinto -> ");
    scanf("%s", &nomeArq);
    strcat(nomeArq, ".txt");
    arq = fopen(nomeArq, "a+");
    system("cls");
}
// Fun��o que escolhe algumas casa aleat�riamente, afim de ramificar ainda mais o labirinto e possibilitar v�rios caminhos que conseguem chegar ao final.
void aleatorizaCaminhos(Labirinto* lab) {
    Celula* viz;                            //Celula vizinha que sera randomizada.
    int aletLarg, aletAlt, vizRand;
    int coef = (LARGURA * ALTURA) * FATORALEATORIO;   //Determina a porcetagem de c�lulas que ser�o escolhidas aleat�riamente para remover paredes aleat�riamente tamb�m.
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

//Fun��o para liberar ponteiros, ao terminar de executar c�digo
void liberaPonteiros() {
    int i;
    for(i=0; i<LARGURA; i++)
        free(lab.Celulas[i]);
    free(lab.Celulas);

    for(i=0; i<LARGURA; i++)
        free(matrizLab[i]);
    free(matrizLab);
}

//Fun��o crucial para inicializa��o da constru��o  do labirinto gerado autom�ticamente.
void iniciaLabirinto() {

    srand(time(NULL));
    informaAltLarg();
    criaArquivo();

    lab.Celulas = malloc(LARGURA * sizeof(Celula*));
    for(int i=0; i<LARGURA; i++)
        lab.Celulas[i] = malloc(ALTURA * sizeof(Celula));

    matrizLab = malloc(LINHAS * sizeof(int*));
    for(int i=0; i<LINHAS; i++)
        matrizLab[i] = malloc(COLUNAS * sizeof(int));

    init_Celulas(&lab);
    int ini = randomInicialFinal(1);            //Para 1 � casa inicial.
    int end = randomInicialFinal(0);            //Para 0 � casa final.
    lab.Celulas[ini][ALTURA-1].paredes[2] = 0;
    lab.Celulas[LARGURA-1][end].paredes[1] = 0;

    jogador.posInix = (ini*2)+1;
    jogador.posIniy = (ALTURA*2)+1;
    jogador.posFimx = (LARGURA*2)+1;
    jogador.posFimy = (end*2)+1;
    jogador.desenhox = (3*ini)+1;
    jogador.desenhoy = (ALTURA*2)+1;

    matrizLab[(end*2)+1][LARGURA*2] = 0;
    matrizLab[(end*2)+1][(LARGURA*2)+1] = 3;
    matrizLab[ALTURA*2][(ini*2)+1] = 0;
    matrizLab[(ALTURA*2)+1][(ini*2)+1] = 2;

    Celula* inicio = &lab.Celulas[ini][ALTURA-1]; // escolhe a c�lula inicial (canto superior esquerdo)
    gerarLabirinto(&lab, inicio);
    aleatorizaCaminhos(&lab);

    printf("\n");
}

void imprimeMatrizResolvida(int matResolvida[][COLUNAS]) {
    for(int i=0; i<LINHAS; i++) {
        for(int j=0; j<COLUNAS; j++) {
            switch(matResolvida[i][j]) {
                    case 8:
                    case 0:
                        if(j%2 == 0) {
                            fprintf(arq, " ");
                            printf(" ");
                        } else {
                            fprintf(arq, "  ");
                            printf("  ");
                        }
                        break;
                    case 1:
                        if(i%2 == 0) {
                            if(j%2 == 0) {
                                fprintf(arq, "+");
                                printf("+");
                            } else {
                                fprintf(arq, "--");
                                printf("--");
                            }
                        } else
                            if(j%2 == 0) {
                                fprintf(arq, "|");
                                printf("|");
                            } else {
                                fprintf(arq, "++");
                                printf("++");
                            }
                        break;
                    case 2:
                        fprintf(arq, "SS");
                        printf("SS");
                        break;
                    case 3:
                        jogador.movimentos++;
                        fprintf(arq, "FF");
                        printf("FF");
                        break;
                    case 4:     //Norte
                        jogador.movimentos++;
                        fprintf(arq, "/\\");
                        printf("/\\");
                        break;
                    case 5:     //Leste
                        jogador.movimentos++;
                        if(j%2 == 0) {
                            fprintf(arq, ">");
                            printf(">");
                        } else {
                            fprintf(arq, ">>");
                            printf(">>");
                        }
                        break;
                    case 6:     //Sul
                        jogador.movimentos++;
                        fprintf(arq, "\\/");
                        printf("\\/");
                        break;
                    case 7:     //Oeste
                        jogador.movimentos++;
                        if(j%2 == 0) {
                            fprintf(arq, "<");
                            printf("<");
                        } else {
                            fprintf(arq, "<<");
                            printf("<<");
                        }
            }
        }
        fprintf(arq, "\n");
        printf("\n");
    }
}

bool validacaoProfunda(int labVal[][COLUNAS], int linha, int coluna) {
    if(linha < 0 || linha >= LINHAS || coluna < 0 || coluna >= COLUNAS || labVal[linha][coluna] == 1 || labVal[linha][coluna] >= 4)
        return false;
    return true;
}

bool buscaProfunda(int matBusca[][COLUNAS], int linha, int coluna, int dir) {
    //Caso ele esteja na casa aonde contem 3 (representando o final), termina a busca e retorna como poss�vel finalizar o labirinto
    if(matBusca[linha][coluna] == 3)
        return true;

    //Valida a casa aonde ele est� operando, se for validada como espa�o vazio dentro da matriz e n�o visitada, marca como vistada representado pela variavel dir, aonde ser� a dire��o para onde foi sendo (.
    if(validacaoProfunda(matBusca, linha, coluna))
        matBusca[linha][coluna] = dir;

    // Exploramos as quatro dire��es poss�veis a partir da posi��o atual
    for (int i = 0; i < 4; i++) {

        int proxLinha = linha + deslcLinhas[i];
        int proxColuna = coluna + deslcColunas[i];
        int direcao = dirMatriz[i];

        // Se a pr�xima posi��o � v�lida, fazemos a chamada recursiva
        if (validacaoProfunda(matBusca, proxLinha, proxColuna)) {
            if (buscaProfunda(matBusca, proxLinha, proxColuna, direcao)) {
                return true;
            }
        }
    }

    // Se nenhuma das dire��es levou � sa�da, marcamos a posi��o como visitada como num 8 e retornamos falso
    matBusca[linha][coluna] = 8;
    return false;
}

void opcBuscaCega(int matBusca[][COLUNAS]) {
    system("cls");
    fprintf(arq, "Labirinto %dx%d Busca Profunda:\n\n", ALTURA, LARGURA);
    if(buscaProfunda(matBusca, jogador.posIniy, jogador.posInix, dirMatriz[0])) {
        imprimeMatrizResolvida(matBusca);
        printf("\n\nLabirinto %dx%d resolvido utilizando a busca por profundidade ao custo de %d movimentos!\n\n", ALTURA, LARGURA, jogador.movimentos);
        fprintf(arq, "\n\nLabirinto %dx%d resolvido utilizando a busca por profundidade ao custo de %d movimentos!\n\n", ALTURA, LARGURA, jogador.movimentos);
    }
}

int heuristic(int r1, int c1, int r2, int c2) {
    // Dist�ncia Manhattan
    return abs(r1 - r2) + abs(c1 - c2);
}

bool astar(int ini_linha, int ini_coluna, int fim_linha, int fim_coluna, int matBusca[][COLUNAS]) {
    int coef = LINHAS * COLUNAS;

    node *listaAberta;
    listaAberta = malloc(coef * (sizeof(node)));

    node *listaFechada;
    listaFechada = malloc(coef * (sizeof(node)));

    int listaFechadaTam = 0;
    int listaAbertaTam = 1;
    int guardaOrigem[LINHAS][COLUNAS];
    int guardaPosi[LINHAS][COLUNAS];

    listaAberta[0].linha = ini_linha;
    listaAberta[0].coluna = ini_coluna;
    listaAberta[0].f_score = heuristic(ini_linha, ini_coluna, fim_linha, fim_coluna);

    while (listaAbertaTam > 0) {
        // Encontra o n� com o menor f_score
        int atual_index = 0;
        for (int i = 0; i < listaAbertaTam; i++) {
            if (listaAberta[i].f_score < listaAberta[atual_index].f_score) {
                atual_index = i;
            }
        }

        node atual_node = listaAberta[atual_index];

        if (atual_node.linha == fim_linha && atual_node.coluna == fim_coluna) {
            // Chegou ao destino
            int atual_linha = atual_node.linha;
            int atual_coluna = fim_coluna;
            while (atual_linha != ini_linha || atual_coluna != ini_coluna) {
                int prox_linha = guardaOrigem[atual_linha][atual_coluna] / COLUNAS;
                int prox_coluna = guardaOrigem[atual_linha][atual_coluna] % COLUNAS;
                matBusca[atual_linha][atual_coluna] = guardaPosi[atual_linha][atual_coluna]; // Marca o caminho encontrado
                atual_linha = prox_linha;
                atual_coluna = prox_coluna;
            }
            matBusca[ini_linha][ini_coluna] = guardaPosi[atual_linha][atual_coluna];
            free(listaAberta);
            free(listaFechada);
            return true; // Caminho encontrado
        }

        // Remove o n� da lista aberta
        for (int i = atual_index; i < listaAbertaTam - 1; i++) {
            listaAberta[i] = listaAberta[i+1];
        }
        listaAbertaTam--;

        // Adiciona o n� � lista fechada
        listaFechada[listaFechadaTam] = atual_node;
        listaFechadaTam++;

        // Encontra os n�s adjacentes
        int vizinhos[4][2] = {{-1,0}, {0,1}, {1,0}, {0,-1}};
        for (int i = 0; i < 4; i++) {
            int viz_linha = atual_node.linha + vizinhos[i][0];
            int viz_coluna = atual_node.coluna + vizinhos[i][1];

            // Verifica se a c�lula � v�lida e livre
            if (viz_linha >= 0 && viz_linha < LINHAS && viz_coluna >= 0 && viz_coluna < COLUNAS && matBusca[viz_linha][viz_coluna] == 0) {

                // Verifica se o n� j� est� na lista fechada
                int veriListaFechada = 0;
                for (int j = 0; j < listaFechadaTam; j++) {
                    if (listaFechada[j].linha == viz_linha && listaFechada[j].coluna == viz_coluna) {
                        veriListaFechada = 1;
                        break;
                    }
                }
                if (veriListaFechada) {
                    continue;
                }

                // Calcula o novo f_score do n�
                int g_score = atual_node.f_score - heuristic(atual_node.linha, atual_node.coluna, fim_linha, fim_coluna);
                g_score += 1; // Custo do movimento
                int h_score = heuristic(viz_linha, viz_coluna, fim_linha, fim_coluna);
                int f_score = g_score + h_score;

                // Verifica se o n� j� est� na lista aberta
                int flagListaAberta = 0;
                for (int j = 0; j < listaAbertaTam; j++) {
                    if (listaAberta[j].linha == viz_linha && listaAberta[j].coluna == viz_coluna) {
                        flagListaAberta = 1;
                        if (f_score < listaAberta[j].f_score) {
                            // Atualiza o n� na lista aberta com o novo f_score
                            listaAberta[j].f_score = f_score;
                            guardaOrigem[viz_linha][viz_coluna] = atual_node.linha * COLUNAS + atual_node.coluna;
                            guardaPosi[viz_linha][viz_coluna] = i + 4;
                        }
                        break;
                    }
                }
                if (flagListaAberta) {
                    continue;
                }

                // Adiciona o n� � lista aberta
                node viz_node = {viz_linha, viz_coluna, f_score};
                listaAberta[listaAbertaTam] = viz_node;
                listaAbertaTam++;
                guardaOrigem[viz_linha][viz_coluna] = atual_node.linha * COLUNAS + atual_node.coluna;
                guardaPosi[viz_linha][viz_coluna] = i + 4;
            }
        }
    }
    free(listaAberta);
    free(listaFechada);
    return false; // Caminho n�o encontrado
}

void opcBuscaHeuristica(int matBusca[][COLUNAS]) {
    system("cls");
    printf("\n\tAguarde...\n\n");
    fprintf(arq, "Labirinto %dx%d Busca Astar:\n\n", ALTURA, LARGURA);
    matBusca[jogador.posFimy][jogador.posFimx] = 0;
    if(astar(jogador.posIniy, jogador.posInix, jogador.posFimy, jogador.posFimx, matBusca)) {
        imprimeMatrizResolvida(matBusca);
        printf("\n\nLabirinto %dx%d resolvido utilizando a heur�sitca astar (*A) ao custo de %d movimentos!\n\n", ALTURA, LARGURA, jogador.movimentos);
        fprintf(arq, "\n\nLabirinto %dx%d resolvido utilizando a heur�sitca astar (*A) ao custo de %d movimentos!\n\n", ALTURA, LARGURA, jogador.movimentos);
    }
}

bool buscaProfundaAleatoriaGuiada(int matBusca[][COLUNAS], int linha, int coluna, int dir) {
    //Caso ele esteja na casa aonde contem 3 (representando o final), termina a busca e retorna como poss�vel finalizar o labirinto
    if(matBusca[linha][coluna] == 3)
        return true;

    //Valida a casa aonde ele est� operando, se for validada como espa�o vazio dentro da matriz e n�o visitada, marca como vistada representado pela variavel dir, aonde ser� a dire��o para onde foi sendo (.
    if(validacaoProfunda(matBusca, linha, coluna))
        matBusca[linha][coluna] = dir;

    int randEscolha;
    int listaJaExplorado[4] = {0, 0, 0, 0};

    // Exploramos as quatro dire��es poss�veis a partir da posi��o atual
    for (int i = 0; i < 4; i++) {               //(norte, leste, sul, oeste)
        do {
            randEscolha = rand() % 10;
        }while(listaJaExplorado[randDirecoes[randEscolha]]);

        listaJaExplorado[randDirecoes[randEscolha]] = 1;

        int proxLinha = linha + deslcLinhas[randDirecoes[randEscolha]];
        int proxColuna = coluna + deslcColunas[randDirecoes[randEscolha]];
        int direcao = dirMatriz[randDirecoes[randEscolha]];

        // Se a pr�xima posi��o � v�lida, fazemos a chamada recursiva
        if (validacaoProfunda(matBusca, proxLinha, proxColuna)) {
            if (buscaProfundaAleatoriaGuiada(matBusca, proxLinha, proxColuna, direcao)) {
                return true;
            }
        }
    }

    // Se nenhuma das dire��es levou � sa�da, marcamos a posi��o como visitada como num 8 e retornamos falso
    matBusca[linha][coluna] = 8;
    return false;
}

void opcBuscaPensada(int matBusca[][COLUNAS]) {
    system("cls");
    fprintf(arq, "Labirinto %dx%d Busca Profunda Aleat�ria Guiada:\n\n", ALTURA, LARGURA);
    if(buscaProfundaAleatoriaGuiada(matBusca, jogador.posIniy, jogador.posInix, dirMatriz[0])) {
        imprimeMatrizResolvida(matBusca);
        printf("\n\nLabirinto %dx%d resolvido utilizando a busca profunda aleat�ria guiada ao custo de %d movimentos!\n\n", ALTURA, LARGURA, jogador.movimentos);
        fprintf(arq, "\n\nLabirinto %dx%d resolvido utilizando a busca profunda aleat�ria guiada ao custo de %d movimentos!\n\n", ALTURA, LARGURA, jogador.movimentos);
    }
}

void gotoxy(int x, int y) {
     SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),(COORD){x,y});
}

int testVencer(int posix) {
    if(posix >= (LARGURA*2)+1)
        return 1;
    return 0;
}

int jogarLabirinto(int matBusca[][COLUNAS]) {
    int flagMov = 1, flagDesenho = 1;
    char opc;
    jogador.movimentos = 0;
    int posix = jogador.posInix;
    int posiy = jogador.posIniy;
    printLabirinto(&lab);
    do {
        if(flagDesenho) {
            gotoxy(jogador.desenhox, jogador.desenhoy);
            printf("##");
            flagDesenho = 0;
        }
        opc = getch();
        switch(opc) {
            case 'w':       //Acima
                if(jogador.posIniy-2 >= 0) {
                    if(matBusca[jogador.posIniy-1][jogador.posInix] == 0) {
                        gotoxy(jogador.desenhox, jogador.desenhoy);
                        printf("/\\");
                        flagDesenho = 1;
                        matBusca[jogador.posIniy][jogador.posInix] = 0;
                        jogador.posIniy-=2;
                        matBusca[jogador.posIniy][jogador.posInix] = 2;
                        jogador.desenhoy-=2;
                        jogador.movimentos+=2;
                    }
                }
                break;
            case 'a':       //Esquerdo
                if(jogador.posInix-2 >= 0) {
                    if(matBusca[jogador.posIniy][jogador.posInix-1] == 0) {
                        gotoxy(jogador.desenhox, jogador.desenhoy);
                        printf("<-");
                        flagDesenho = 1;
                        matBusca[jogador.posIniy][jogador.posInix] = 0;
                        jogador.posInix-=2;
                        matBusca[jogador.posIniy][jogador.posInix] = 2;
                        jogador.desenhox-=3;
                        jogador.movimentos+=2;
                    }
                }
                break;
            case 's':       //Abaixo
                if(jogador.posIniy+2 <= (ALTURA*2)) {
                    if(matBusca[jogador.posIniy+1][jogador.posInix] == 0) {
                        gotoxy(jogador.desenhox, jogador.desenhoy);
                        printf("\\/");
                        flagDesenho = 1;
                        matBusca[jogador.posIniy][jogador.posInix] = 0;
                        jogador.posIniy+=2;
                        matBusca[jogador.posIniy][jogador.posInix] = 2;
                        jogador.desenhoy+=2;
                        jogador.movimentos+=2;
                    }
                }
                break;
            case 'd':       //Direita
                if(jogador.posInix+2 <= (LARGURA*2)+1) {
                    if(matBusca[jogador.posIniy][jogador.posInix+1] == 0) {
                        gotoxy(jogador.desenhox, jogador.desenhoy);
                        printf("->");
                        flagDesenho = 1;
                        matBusca[jogador.posIniy][jogador.posInix] = 0;
                        jogador.posInix+=2;
                        matBusca[jogador.posIniy][jogador.posInix] = 2;
                        jogador.desenhox+=3;
                        jogador.movimentos+=2;
                    }
                }
        }
    }while(!testVencer(jogador.posInix) && opc != 'x');
    system("cls");
    printf("Voc� finalizou o labirinto com %d movimentos!\n\n", jogador.movimentos);
    jogador.movimentos = 0;
    jogador.posInix = posix;
    jogador.posIniy = posiy;
}

 bool menu() {
    int opc, matBusca[LINHAS][COLUNAS];
    do {
        do{
            printf("#-=-=-=-=-| Labirinto %dx%d |-=-=-=-=-=-#\n\t(1)Realizar Busca Cega. (Profundidade)\n\t(2)Realizar Busca Heur�stica. (*A)\n\t(3)Realizar Busca Pensada (Profunda Aleat�ria Guiada)\n\t(4)Imprimir Labirinto.\n\t(5)Jogar Labirinto\n\t(0)Sair.\n#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#\n\n\tOp��o: ", ALTURA, LARGURA);
            scanf("%d", &opc);
            if(opc < 0 || opc > 5) {
                system("cls");
                printf("Op��o inv�lida!\n\n");
            }
        }while(opc < 0 || opc > 5);
        jogador.movimentos = 0;
        if(opc != 0)
            for(int i=0; i<LINHAS; i++)
                for(int j=0; j<COLUNAS; j++)
                    matBusca[i][j] = matrizLab[i][j];
        switch(opc) {
                case 1:
                    opcBuscaCega(matBusca);
                    break;
                case 2:
                    opcBuscaHeuristica(matBusca);
                    break;
                case 3:
                    opcBuscaPensada(matBusca);
                    break;
                case 4:
                    printLabirinto(&lab);
                    break;
                case 5:
                    jogarLabirinto(matBusca);
                    break;
                case 0:
                    fclose(arq);
                    liberaPonteiros();
                    return true;
        }
    }while(1);
}

int main() {
    setlocale(LC_ALL, "");
    iniciaLabirinto();
    return menu();
}
