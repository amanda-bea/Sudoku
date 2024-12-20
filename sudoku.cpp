/* Trabalho Sudoku, AlgProg2
   por Amanda Beatriz C. de Jesus e Ana Clara B. Ottoni*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h> //nova biblioteca adicionada

/* Constantes */
#define ERROR_FILE_MSG	"Nao foi possivel abrir o arquivo!\n"
#define INVALID_OPTION	"Opcao invalida! Tente novamente!"

/* Definicoes de tipo */
enum boolean {
	FALSO=0, VERDADEIRO=1
};

/* Prototipos */
FILE* carregue(char quadro[9][9]);
FILE* carregue_continue_jogo (char quadro[9][9], char *nome_arquivo);
void carregue_novo_jogo(char quadro[9][9], char *nome_arquivo);
FILE* crie_arquivo_binario(char quadro[9][9]);
int determine_quadrante(int x, int y);
int eh_valido(const char quadro[9][9], int x, int y, int valor);
int eh_valido_na_coluna(const char quadro[9][9], int y, int valor);
int eh_valido_no_quadrante3x3(const char quadro[9][9], int x, int y, int valor);
int eh_valido_na_linha(const char quadro[9][9], int x, int valor);
int existe_posicao_vazia(const char quadro[9][9]);
void imprima(const char quadro[9][9]);
void jogue();
void resolve_completo(FILE*, char quadro[9][9]);
void resolve_um_passo(char quadro[9][9]);
void salve_jogada_bin(FILE* fb, char quadro[9][9]);

/* Funcoes auxiliares */
int fim_x(int quadr);
int fim_y(int quadr);
int leia_opcao();
void gen_random(char *s, const int len);
int ini_x(int quadr);
int ini_y(int quadr);
void menu();
void menu_arquivo();


/* -----------------------------------------------------------------------------
 * -----------------------------------------------------------------------------
 * MAIN
 * /////////////////////////////////////////////////////////////////////////////
 */
int main() {

	// inicia o jogo
	jogue();

	return 0;
}

/* -----------------------------------------------------------------------------
 * CARREGAR
 * Inicializa o SUDOKU a partir de um novo jogo ou estado de jogo anterior
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
FILE* carregue(char quadro[9][9]) {
	int opcao;
	char nome[50];
	menu_arquivo();
	opcao = leia_opcao();

	switch(opcao) {

		// carregar novo sudoku
		case 1:{
			//conferir de arquivo é válido
			FILE *f;
			
			printf("Qual é o nome do seu jogo.txt? ");
			scanf("%s", &nome);

			f = fopen(nome, "r");
			if (f == NULL) {
				printf(ERROR_FILE_MSG);
				break;
			}

			else {
				carregue_novo_jogo(quadro, nome);
			}

			break;
		}
		// continuar jogo
		case 2:{
			//conferir de arquivo é válido
			printf("Insira o nome do jogo que deseja continuar: ");
			scanf("%s", &nome);
			
			FILE *fb = fopen(nome, "rb");
			if (fb == NULL) {
				printf(ERROR_FILE_MSG);
				break;
			}
			else {
				carregue_continue_jogo(quadro, nome);
			}

			return fb;
			break;
		}
		// retornar ao menu anterior
		case 9: {
			menu();
			break;
		}
		default:
		break;
	}

}

/* -----------------------------------------------------------------------------
 * CARREGAR CONTINUAR JOGO
 * Carrega um estado de jogo a partir de um arquivo binario
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
FILE* carregue_continue_jogo (char quadro[9][9], char *nome_arquivo) {
    // Abrir o arquivo binário para leitura e escrita sem sobreescrever
    FILE *fb = fopen(nome_arquivo, "rb+");

    // Mover o ponteiro do arquivo para 81 caracteres antes do final
    fseek(fb, -81, SEEK_END);

    // Ler os últimos 81 caracteres do arquivo binário e armazenar na matriz quadro
    fread(quadro, sizeof(char), 9 * 9, fb);
    
    return fb;
}

/* -----------------------------------------------------------------------------
 * CARREGAR NOVO JOGO
 * Carrega um novo jogo do Sudoku
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void carregue_novo_jogo(char quadro[9][9], char *nome_arquivo) {
	FILE *f; //txt

	//abrir o arquivo somente para ler o quadro
	f = fopen(nome_arquivo, "r");
	
	//salvar quadro do txt no quadro atual do programa
	for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            fscanf(f, "%1d", (int*)&quadro[i][j]);
        }
    }

	//arquivo fechado pois não será mais usado
    fclose(f);
}

/* -----------------------------------------------------------------------------
 * CRIAR ARQUIVO BINARIO
 * Criar arquivo binario
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
FILE* crie_arquivo_binario(char quadro[9][9]) {
	//variaveis para criação do arquivo
	FILE *fb;
	int jogadas = 0;
	char nome[20];
	
	//geração de nome aleatório com 10 caracteres
	gen_random(nome, 10);
	strcat(nome, ".bin");

	//criação de arquivo no modo de leitura e escrita que será usado futuramente no salve_jogada_bin
	fb = fopen(nome, "wb+");

	//iniciar arquivo com 1 jogada e 1 quadro, como solicitado
	fwrite(&jogadas, sizeof(int), 1, fb);
	fwrite(quadro, sizeof(char), 81, fb);

	return fb;
}

/* -----------------------------------------------------------------------------
 * DETERMINAR QUADRANTE
 * Dado as posicoes x e y, determina o quadrante do quadro
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int determine_quadrante(int x, int y) {
	if (x < 3 && y < 3)
		return 1;
	else if (x < 3 && y < 6)
		return 2;
	else if (x < 3 && y < 9)
		return 3;
	else if (x < 6 && y < 3)
		return 4;
	else if (x < 6 && y < 6)
		return 5;
	else if (x < 6 && y < 9)
		return 6;
	else if (x < 9 && y < 3)
		return 7;
	else if (x < 9 && y < 6)
		return 8;
	else
		return 9;
}

/* -----------------------------------------------------------------------------
 * E VALIDO
 * Determina se um valor na posicao x e y eh valido
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int eh_valido(const char quadro[9][9], int x, int y, int valor) {

	// verifica as tres condicoes
	if (!eh_valido_na_coluna(quadro, y, valor))
		return FALSO;
	if (!eh_valido_na_linha(quadro, x, valor))
		return FALSO;
	if (!eh_valido_no_quadrante3x3(quadro, x, y, valor))
		return FALSO;

	return VERDADEIRO;
}

/* -----------------------------------------------------------------------------
 * E VALIDO NA COLUNA
 * Verifica se um valor na coluna y e valido
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int eh_valido_na_coluna(const char quadro[9][9], int y, int valor) {
	for (int i = 0; i < 9; i++) // percorrendo a coluna y
	{
		if (valor == quadro[i][y]) // se já houver o referido valor em alguma posição da coluna y
			return 0; // entao retornar 0
	}
	return 1; // caso não haja o valor na coluna y, retornar 1
}

/* -----------------------------------------------------------------------------
 * E VALIDO NA LINHA
 * Verifica se um valor na linha x e valido
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int eh_valido_na_linha(const char quadro[9][9], int x, int valor) {
		for (int i = 0; i < 9; i++) // percorrendo a linha x
	{
		if (valor == quadro[x][i])  // se já houver o referido valor em alguma posição da linha x
			return 0; // entao retornar 0
	}
	return 1; // caso não haja o valor na linha x, retornar 1
}

/* -----------------------------------------------------------------------------
 * E VALIDO NO QUADRO 3X3
 * Verifica se um valor e valido no quadrante da posicao x, y
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int eh_valido_no_quadrante3x3(const char quadro[9][9], int x, int y, int valor) {
	int q = determine_quadrante(x,y); // chama a função determine_quadrante, 
	// que retorna o número do quadrante, e atribui tal valor a variavel q

	// percorrendo o quadrante q
	for (int i = ini_x(q); i < fim_x(q); i++)
	{
		for (int j = ini_y(q); j < fim_y(q); j++)
		{
			if(valor == quadro[i][j]) // se já houver o referido valor em alguma posição do quadrante q
				return 0; // entao retornar 0
		}
	}

	return 1; // caso não haja o valor no quadrante q, retornar 1
	
}

/* -----------------------------------------------------------------------------
 * EXISTE CAMPO VAZIO
 * Verifica se existe um campo nao preenchido
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int existe_posicao_vazia(const char quadro[9][9]) {
	int i, j;

	for(i = 0; i < 9; i++) {
		for(j = 0; j < 9; j++) {
			if (quadro[i][j] == 0)
				return VERDADEIRO;
		}
	}

	return FALSO;
}

/* -----------------------------------------------------------------------------
 * IMPRIMIR
 * Imprime o quadro recebido do sudoku
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void imprima(const char quadro[9][9]) {
	int i, j;

//	puts("~~~~~~~~ SUDOKU ~~~~~~~~");
	puts("    0 1 2   3 4 5   6 7 8");
	for (i = 0; i < 9; i++) {
		if (i % 3 == 0)
			puts("  -------------------------");
		for (j = 0; j < 9; j++) {

			if (j == 0)
				printf("%d | ", i);
			else if (j % 3 == 0)
				printf("| ");

			if (quadro[i][j] == 0)
				printf("- ");
			else
				printf("%d ", quadro[i][j]);
		}
		puts("|");
	}
	puts("  -------------------------");
}

/* -----------------------------------------------------------------------------
 * JOGAR
 * Realiza toda a logica do jogo
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void jogue() {
	int opcao;
	char quadro[9][9] = { {0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0}
	};
	FILE *fb = NULL;

	opcao = 0;

	while (opcao != 9) {
		// imprima na tela o quadro atual
		imprima(quadro);

		// apresente um menu com as opcoes
		menu();
		opcao = leia_opcao();

		switch (opcao) {

		// carregue sudoku
		case 1:
			fb = carregue(quadro);

			if (fb == NULL) {
				fb = crie_arquivo_binario(quadro);
			}
			break;

		// preencha quadro com um valor
		case 2: {
			int x, y, valor;

			printf("Entre com a posicao e o valor (linha, coluna, valor): ");
			scanf("%d %d %d", &x, &y, &valor);


			if (eh_valido(quadro, x, y, valor)) {
				quadro[x][y] = valor;
				salve_jogada_bin(fb, quadro);
			}
			else
				puts("Valor ou posicao incorreta! Tente novamente!");
		}
			break;

		// resolva 1 passo
		case 3:
			resolve_um_passo(quadro);
			salve_jogada_bin(fb, quadro);
			puts("Um passo resolvido!");
			break;

		// resolva o sudoku completo
		case 4:
			resolve_completo(fb, quadro);
			salve_jogada_bin(fb, quadro);
			break;

		case 9:
			puts("Programa finalizado ..");
			fclose(fb);
			break;

		default:
			puts(INVALID_OPTION);
			break;
		}
	}
}

/* -----------------------------------------------------------------------------
 * RESOLVER
 * Resolve o sudoku
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void resolve_completo(FILE *fb, char quadro[9][9]) {
	while(existe_posicao_vazia(quadro)) {
		resolve_um_passo(quadro);
		salve_jogada_bin(fb, quadro);
	}
}

/* -----------------------------------------------------------------------------
 * RESOLVER UM PASSO
 * Preenche apenas um campo vazio
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void resolve_um_passo(char quadro[9][9]) {
	// percorre a matriz quadro até encontrar um elemento igual a 0
	for(int i = 0; i < 9; i++){
		for (int j = 0; j < 9; j++)
		{
			int cont = 0; // variável que conta quantas vezes existe um valor válido para a posição
			int aux; // variável que guarda o valor válido

			if(quadro[i][j] == 0){
				for(int l = 1; l < 10; l++){
					if(eh_valido(quadro, i, j, l)){ // a cada iteração, é verificado se o valor de l é válido para tal posição
						cont += 1; // se i valor l é valido para a posição, incrementar 1 a variavel cont 
						aux = l; // e atribuir o valor de l a variável aux
					}
				}
				
				// se só houver um valor possível para a referida posição
				if (cont == 1){
					quadro[i][j] = aux; // então atribuir o valor de aux a matriz do jogo
					return;
				}
			}
		}
		
	}
}

/* -----------------------------------------------------------------------------
 * SALVAR JOGADA BINARIO
 * Salva o estado atual do quadro no arquivo binario
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void salve_jogada_bin (FILE *fb, char quadro[9][9]) {
    int jogadas;

	//ponteiro sempre alocado no começo para ler o n de jogadas atual
	fseek(fb, 0, SEEK_SET);
    fread(&jogadas, sizeof(int), 1, fb);
    jogadas = jogadas + 1; //adicionar uma jogada

	//alocar novamente o ponteiro e reescrever o n de jogadas
    fseek(fb, 0, SEEK_SET);
    fwrite(&jogadas, sizeof(int), 1, fb);

	//alocar ponteiro no final e escrever quadro atual
    fseek(fb, 0, SEEK_END);
    fwrite(quadro, sizeof(char), 9 * 9, fb);
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 							FUNCOES AUXILIARES
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

/* -----------------------------------------------------------------------------
 * FIM X
 * Indice final da linha para o quadrante recebido como parametro
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int fim_x(int quadr) {
	switch(quadr) {
		case 1:
		case 2:
		case 3:
			return 2;

		case 4:
		case 5:
		case 6:
			return 5;

		default:
			return 8;
	}
}

/* -----------------------------------------------------------------------------
 * FIM Y
 * Indice final da coluna para o quadrante recebido como parametro
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int fim_y(int quadr) {
	switch(quadr) {
		case 1:
		case 4:
		case 7:
			return 2;

		case 2:
		case 5:
		case 8:
			return 5;

		default:
			return 8;
	}
}

/* -----------------------------------------------------------------------------
 * GEN_RANDOM
 * Gera uma cadeia de caracteres randomica de tamanho len
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void gen_random(char *s, const int len) {
	srand(time(NULL));
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
	int i;

    for (i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;
}

/* -----------------------------------------------------------------------------
 * INI X
 * Indice inicial da linha para o quadrante recebido como parametro
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int ini_x(int quadr) {
	switch(quadr) {
		case 1:
		case 2:
		case 3:
			return 0;

		case 4:
		case 5:
		case 6:
			return 3;

		default:
			return 6;
	}
}

/* -----------------------------------------------------------------------------
 * INI Y
 * Indice inicial da coluna para o quadrante recebido como parametro
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int ini_y(int quadr) {
	switch(quadr) {
		case 1:
		case 4:
		case 7:
			return 0;

		case 2:
		case 5:
		case 8:
			return 3;

		default:
			return 6;
	}
}

/* -----------------------------------------------------------------------------
 * LE OPCAO
 * Imprime a mensagem a faz a leitura da opcao
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int leia_opcao () {
	int opcao;

	printf("Opcao: ");
	scanf("%d", &opcao);

	return opcao;
}

/* -----------------------------------------------------------------------------
 * MENU
 * Imprime o menu de opcoes
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void menu() {
	puts("\n~~~~~~~~ SUDOKU ~~~~~~~~");
	puts("[1] - Carregar jogo");
	puts("[2] - Jogar");
	puts("[3] - Resolver um passo");
	puts("[4] - Resolver");
	puts("[9] - Finalizar");
	puts("--------");
}

/* -----------------------------------------------------------------------------
 * MENU ARQUIVO
 * Imprime o menu de opcoes do arquivo
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void menu_arquivo() {
	puts("\n~~~~~ MENU ARQUIVO ~~~~~");
	puts("[1] - Novo jogo");
	puts("[2] - Continuar jogo");
	puts("[9] - Retornar ao menu anterior");
	puts("--------");
}
