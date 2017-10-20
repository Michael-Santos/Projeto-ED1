 /*==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 01 - Indexação
 *
 * RA: 726572
 * Aluno: Michael dos Santos
 * ========================================================================== */

/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 	11
#define TAM_NOME 			51
#define TAM_DESENVOLVEDORA	51
#define TAM_DATA			11
#define TAM_CLASSIFICACAO 	3
#define TAM_PRECO 			8
#define TAM_DESCONTO 		4
#define TAM_CATEGORIA 		51


#define TAM_REGISTRO 		192
#define MAX_REGISTROS 		1000
#define MAX_CATEGORIAS 		10
#define TAM_ARQUIVO 		(MAX_REGISTROS * TAM_REGISTRO + 1)


/* Saídas do usuário */
#define OPCAO_INVALIDA 			"Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE 	"Memoria insuficiente!"
#define REGISTRO_N_ENCONTRADO 	"Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO 			"Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA 		"ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO 			"Arquivo vazio!"
#define INICIO_BUSCA 		 	"**********************BUSCAR**********************\n"
#define INICIO_LISTAGEM  		"**********************LISTAR**********************\n"
#define INICIO_ALTERACAO 		"**********************ALTERAR*********************\n"
#define INICIO_EXCLUSAO  		"**********************EXCLUIR*********************\n"
#define INICIO_ARQUIVO  		"**********************ARQUIVO*********************\n"
#define SUCESSO  				"OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					"FALHA AO REALIZAR OPERACAO!\n"

/* Registro do jogo */
typedef struct {
	char pk[TAM_PRIMARY_KEY];
	char nome[TAM_NOME];
	char desenvolvedora[TAM_DESENVOLVEDORA];
	char data[TAM_DATA];	/* DD/MM/AAAA */
	char classificacao[TAM_CLASSIFICACAO];
	char preco[TAM_PRECO];
	char desconto[TAM_DESCONTO];
	char categoria[TAM_CATEGORIA];
} Jogo;

/* Registros dos Índices*/

/* Struct para índice Primário */
typedef struct primary_index{
  char pk[TAM_PRIMARY_KEY];
  int rrn;

} Ip;

typedef struct linked_list{
  char pk[TAM_PRIMARY_KEY];
  struct linked_list *prox;
} ll;

typedef struct reverse_index{
  char* chave;
  ll* lista;
} Ir;

/* Variavel global que simula o arquivo de dados */
char ARQUIVO[TAM_ARQUIVO];

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */
 /* Exibe o menu de opções */
void print_menu();

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo();

/* Exibe o jogo, utilize com_desconto = 1 apenas para listagem por preço */
int exibir_registro(int rrn, char com_desconto);

/* Recupera do arquivo o registro com o rrn informado e retorna os dados na
 * struct Jogo */
Jogo recuperar_registro(int rrn);

/*Dada uma certa chave, retorna o rrn dela consultando o ip*/
int recuperar_rrn(Ip* iprimary, const char* pk, int nregistros);

/* (Re)faz o índice primário */
void criar_iprimary(Ip *indice_primario, int nregistros);

/* Salva os dados da entrada para a struct jogo*/
void ler_entrada(char* registro, Jogo *novo);

/* Certifica se o valor de tamanho não ultrapassa o de valor tamanho2*/
int max(int tamanho1, int tamanho2);

/* Gwra chave primária */
void gerarChavePrimaria(Jogo* jogo);

/* Funções que definem grandeza na função qsort */
int compare (const void * a, const void * b);

/* Verifica se já existe chave prímária */
int existe_chavePrimaria(Ip* iprimary, int nregistros, char *chave);

/* Realiza a listagem dos jogos por ordem de código */
int listar_indicePrimario(Ip* iprimary, int nregistros);

/* Realiza a listagem por meio de um index secundário */
int listar_indiceSecundario(Ip* iprimary, Ir *isecundario, int nregistros, int nsecundario);

/* Realiza a listagem por meio de categoria */
int listar_categoria(Ip *iprimary, Ir *icategoria, char *categoria1, int nregistros, int ncat);

/* Realiza a listagem por meio de preco */
int listar_iprice(Ip* iprimary, Ir *iprice, int nregistros, int nsecundario);

/* (Re)cria os index secundários */
void criar_index_secundarios(Ip* iprimary, Ir* ititulo, Ir *idev, Ir *iprice, Ir *icategoria, int nregistros, int* ntitulos, int *ndev, int *nprice, int *ncat);

/* (Re)cria um index secundário */
void criar_isecundario(Ir* isecundario,  char* chave, char* chave_primaria, int *nregistros, int tamanho);

/* Busca se já existe a chave secundária no index secundário */
int buscar_chave_secundaria(Ir* ititulo, char* titulo, int nregistros);

/* Insere a chave primária no index secundário */
void inserir_chave(ll** lista, char* chave_primaria);

/* Remover indice primario */
int remover_indice_primario(Ip* indice_primario, int* nregistros, char* pk);

/* Libera espaço no arquivo de dados */
int libera_espaco(int nregistros);

/* Limpa listas dos indices secundários */
void limpa_listas(Ir *isecundario, int nregistros);

/* "Meus" métodos de ordenação  */

/*Selection Sort*/
void selection_sort(Ir *isedundario, int nregistros);


void selection_sort_primario(Ip *iprimario, int nregistros);

/* Qsort */
void myswap(void *e1, void *e2, int size);
void myqsort(void* base, size_t n, size_t size, int (*cmp)(const void*, const void*));

/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * ======================================================================= */
int main(){
  /* Arquivo */
	int carregarArquivo = 0, nregistros = 0, ntitulos = 0, ndev = 0, nprice = 0, ncat = 0, descon, contagem = 0;
	Jogo jogo;
	int numCaracter;
	int existe, existe1, i;
	float preconum, descontonum;
	ll *atual, *anterior;
	char registro[TAM_REGISTRO], chave[TAM_PRIMARY_KEY], titulo[TAM_NOME], desenvolvedora[TAM_DESENVOLVEDORA], 
		preco[TAM_PRECO], desconto[TAM_DESCONTO], categoria[TAM_CATEGORIA], categoria1[TAM_CATEGORIA];;
	char *cat;
	scanf("%d%*c", &carregarArquivo); /* 1 (sim) | 0 (nao) */
	
	if (carregarArquivo)
		nregistros = carregar_arquivo();

	/* Índice primário */
	Ip *iprimary = (Ip *) malloc (MAX_REGISTROS * sizeof(Ip));
  	if (!iprimary) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}

	/* Índice de titulos */
	Ir *ititulo = (Ir *) malloc (MAX_REGISTROS * sizeof(Ir));
  	if (!ititulo) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}

	/* Índice de desenvolvedoras */
	Ir *idev = (Ir *) malloc (MAX_REGISTROS * sizeof(Ir));
  	if (!idev) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}

	/* Índice de preços */
	Ir *iprice = (Ir *) malloc (MAX_REGISTROS * sizeof(Ir));
  	if (!idev) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}

	/* Índice de categorias */
	Ir *icategoria = (Ir *) malloc (MAX_REGISTROS * sizeof(Ir));
  	if (!idev) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}

	criar_iprimary(iprimary, nregistros);
	criar_index_secundarios(iprimary, ititulo, idev, iprice, icategoria, nregistros, &ntitulos, &ndev, &nprice, &ncat);
	/* Alocar e criar índices secundários */

    /* Execução do programa */
	int opcao = 0, opcao_menu = 0;
	while(1) {
		//print_menu();
		scanf("%d%*c", &opcao);
		switch(opcao) {
		
		/* Recebe ĩnformações do jogo, gera chave primária e verifica se já existe algum registro com a chave primária. 
		 * Apenas caso já não exista chave primária igual é inserido o novo jogo nos indices e no arquivo de dados */
		case 1:
			/* inserir */
			scanf("%[^\n]\n%[^\n]\n%[^\n]\n%[^\n]\n%[^\n]\n%[^\n]\n%[^\n]", jogo.nome, jogo.desenvolvedora, 
					jogo.data, jogo.classificacao, jogo.preco, jogo.desconto, jogo.categoria);

			getchar();

			gerarChavePrimaria(&jogo);

			if(existe_chavePrimaria(iprimary, nregistros, jogo.pk) == -1){
				if(nregistros == MAX_REGISTROS)
					printf(MEMORIA_INSUFICIENTE);
				else{
					strcpy(iprimary[nregistros].pk, jogo.pk);
					iprimary[nregistros].rrn = nregistros;
					
					/* Coloca o novo registro no arquivo de dados */
					numCaracter = sprintf(ARQUIVO+nregistros*192, "%s@%s@%s@%s@%s@%s@%s@", jogo.nome, jogo.desenvolvedora,
						jogo.data, jogo.classificacao, jogo.preco, jogo.desconto, jogo.categoria);
					
					/* Acrescenta # nos caracteres que não sobraram no novo registro no arquivo de dados */
					for(i = 0; i < 192-numCaracter; i++)
						*(ARQUIVO+nregistros*TAM_REGISTRO+numCaracter+i) = '#';
					*(ARQUIVO+(nregistros+1)*TAM_REGISTRO) = '\0';
					
					nregistros++;
					myqsort (iprimary, nregistros, sizeof(Ip), compare);
					
					/* Adiciona o novo registro nos indices secundarios */
					criar_isecundario(ititulo, jogo.nome, jogo.pk, &ntitulos, TAM_NOME);
					criar_isecundario(idev, jogo.desenvolvedora, jogo.pk, &ndev, TAM_DESENVOLVEDORA);
					
					sscanf(jogo.preco, "%f", &preconum);
					sscanf(jogo.desconto, "%f", &descontonum);
					preconum = preconum* (100 - descontonum);
					preconum = (int)preconum / 100.0;

					sprintf(preco, "%08.3f", preconum);
					preco[7] = '\0';
					criar_isecundario(iprice, preco, jogo.pk, &nprice, TAM_PRECO);

					strcpy(categoria, jogo.categoria);
					for (cat = strtok (categoria, "|"); cat != NULL; cat = strtok (NULL, "|"))
						criar_isecundario(icategoria, cat, jogo.pk, &ncat, TAM_CATEGORIA);

				}
			}else
				printf(ERRO_PK_REPETIDA, jogo.pk);

			break;
		case 2:
			/* alterar */
			printf(INICIO_ALTERACAO);
			scanf("%s", chave);
			getchar();

			if((existe = existe_chavePrimaria(iprimary, nregistros, chave)) != -1){
				jogo = recuperar_registro(iprimary[existe].rrn);

				do{
					scanf("%[^\n]", desconto);
					getchar();
					sscanf(desconto, "%d", &descon);
					
					/* Verifica se a entrada é válida */
					if((strlen(desconto) != 3) || (descon < 0 || descon > 100))
						printf(CAMPO_INVALIDO);
					else
						break;
				}while(1);

				/* Reinsere o registro no arquivo de dados com o campo desconto alterado */
				sprintf(registro, "%s@%s@%s@%s@%s@%s@%s", jogo.nome, jogo.desenvolvedora, 
					jogo.data, jogo.classificacao, jogo.preco, desconto, jogo.categoria);
				strncpy(ARQUIVO+(iprimary[existe].rrn)*TAM_REGISTRO, registro, strlen(registro));

				/* Recriar ou alterar as chaves de iprice */

				/* Fazer contas para encontrar a chave secundária */
				sscanf(jogo.preco, "%f", &preconum);
				sscanf(jogo.desconto, "%f", &descontonum);
				preconum = preconum* (100 - descontonum);
				preconum = (int)preconum / 100.0;

				sprintf(preco, "%08.3f", preconum);
				preco[7] = '\0';

				if((existe1 = buscar_chave_secundaria(iprice, preco, nprice)) != -1){
					anterior = NULL; 
					atual = iprice[existe1].lista;

					while((strcmp(atual->pk, jogo.pk))){
						anterior = atual;
						atual = atual->prox;
					}
					
					if(!anterior){
						iprice[existe1].lista = atual->prox;
						free(atual);
					}else{
						anterior->prox = atual->prox;
						free(atual);
					}
				}	
				jogo = recuperar_registro(iprimary[existe].rrn);
				sscanf(jogo.preco, "%f", &preconum);
				sscanf(jogo.desconto, "%f", &descontonum);
				preconum = preconum* (100 - descontonum);
				preconum = (int)preconum / 100.0;

				sprintf(preco, "%08.3f", preconum);
				preco[7] = '\0';

				criar_isecundario(iprice, preco, jogo.pk, &nprice, TAM_PRECO);

				printf(SUCESSO);
			}else{
				printf(REGISTRO_N_ENCONTRADO);
				printf(FALHA);
			}
			break;
		case 3:
			/* excluir */
			printf(INICIO_EXCLUSAO);
			scanf("%s", chave);
			getchar();
			if(remover_indice_primario(iprimary, &nregistros, chave))
				printf(SUCESSO);
			else{
				printf(REGISTRO_N_ENCONTRADO);
				printf(FALHA);
			}
			break;
		case 4:
			/* buscar */
			printf(INICIO_BUSCA );
			scanf("%d", &opcao_menu);
			getchar();
			switch(opcao_menu){
				case 1:
					scanf("%s", chave);
					getchar();
					if((existe = existe_chavePrimaria(iprimary, nregistros, chave)) != -1)
						exibir_registro(iprimary[existe].rrn, 0);
					else
						printf(REGISTRO_N_ENCONTRADO);
					break;
				case 2:
					/* Verifica se existe o titulo no vetor de indice e caso encontrar percorre a lista exibindo
					*  o registro de cada pk encontrada. Quando for o ultimo elemento da lista não irá exivir o \n */
					scanf("%[^\n]", titulo);
					getchar();
					contagem = 0;
					if((existe = buscar_chave_secundaria(ititulo, titulo, ntitulos)) != -1){
						atual = ititulo[existe].lista;
						while(atual != NULL){
							if((existe = existe_chavePrimaria(iprimary, nregistros, atual->pk)) != -1){
								if(contagem){
									printf("\n");
								}
								contagem++;
								exibir_registro(iprimary[existe].rrn, 0);
							}
							atual = atual->prox;
						}
						if(!contagem)
							printf(REGISTRO_N_ENCONTRADO);
					}
					else
						printf(REGISTRO_N_ENCONTRADO);
					break;
				case 3:
					scanf("%[^\n]", desenvolvedora);
					getchar();
					scanf("%[^\n]", categoria1);
					getchar();
					contagem = 0;
					/* Procura pela desenvolvedora e em seguida pela categoria */
					if((existe = buscar_chave_secundaria(idev, desenvolvedora, ndev)) != -1){
						atual = idev[existe].lista;
						while(atual != NULL){
							if((existe = existe_chavePrimaria(iprimary, nregistros, atual->pk)) != -1){
								jogo = recuperar_registro(iprimary[existe].rrn);

								/* Verfica se um jogo pertence a uma certa categoria e caso pertença o exibe */
								strcpy(categoria, jogo.categoria);
								for (cat = strtok (categoria, "|"); cat != NULL; cat = strtok (NULL, "|")){
									if(!(strcmp(categoria1, cat))){
										if(contagem){
											printf("\n");
										}
										contagem++;
										exibir_registro(iprimary[existe].rrn, 0);
										break;
									}
								}
							}								
							atual = atual->prox;
						}
						if(!contagem)
							printf(REGISTRO_N_ENCONTRADO);
					}else
						printf(REGISTRO_N_ENCONTRADO);

					break;
				default:
					printf("Opção inválida de listagem\n");
					break;
			}			
			break;
		case 5:
			/* listar */
			printf(INICIO_LISTAGEM);
			scanf("%d", &opcao_menu);

			switch(opcao_menu){
				case 1:
					/* Realiza a listagem por odem de chave-primária */
					if(nregistros){
						if(listar_indicePrimario(iprimary, nregistros) == 0){
							printf(REGISTRO_N_ENCONTRADO);
						}
					}else{
						printf(REGISTRO_N_ENCONTRADO);
					}
					break;
				case 2:
					getchar();
					scanf("%[^\n]", categoria1);
					getchar();

					if(nregistros){
						if(!(listar_categoria(iprimary, icategoria, categoria1, nregistros, ncat))){
							printf(REGISTRO_N_ENCONTRADO);
						}
					}else{
						printf(REGISTRO_N_ENCONTRADO);
					}

					break;
				case 3:
					/* Realiza listagem de desenvolvedora (Na verdade deve-se realizar em ordem de título)*/
					if(nregistros){
						if(!(listar_indiceSecundario(iprimary, idev, nregistros, ndev))){
							printf(REGISTRO_N_ENCONTRADO);
						}
					}else{
						printf(REGISTRO_N_ENCONTRADO);
					}
					break;
				case 4:
					if(nregistros){
						if(!(listar_iprice(iprimary, iprice, nregistros, nprice))){
							printf(REGISTRO_N_ENCONTRADO);
						}
					}else{
						printf(REGISTRO_N_ENCONTRADO);
					}
					break;
				default:
					printf("Opção inválida de listagem\n");
					break;
			}

			break;
		case 6:
			/* liberar espaco */
			nregistros = libera_espaco(nregistros);

			/* É preciso desalocar chave dos indices secundários */
			limpa_listas(ititulo, ntitulos);
			limpa_listas(idev, ndev);
			limpa_listas(iprice, nprice);
			limpa_listas(icategoria, ncat);

			/* (Re)criar os indices */
			criar_iprimary(iprimary, nregistros);
			ntitulos = ndev = nprice = ncat = 0;
			criar_index_secundarios(iprimary, ititulo, idev, iprice, icategoria, nregistros, &ntitulos, &ndev, &nprice, &ncat);

		break;
		case 7:
			/* imprimir arquivo de dados*/
			printf(INICIO_ARQUIVO);
			printf("%s\n", ARQUIVO);
			break;
		case 8:
    		/* Liberar memoria e finalizar */
			limpa_listas(ititulo, ntitulos);
			limpa_listas(idev, ndev);
			limpa_listas(iprice, nprice);
			limpa_listas(icategoria, ncat);

			free(iprimary);
			free(ititulo);
			free(idev);
			free(iprice);
			free(icategoria);
			return (0);
			break;
		default:
			printf(OPCAO_INVALIDA);
			break;
		}
	}
	return (0);
}

/* Exibe o jogo, utilize com_desconto = 1 para listagem por preço */
int exibir_registro(int rrn, char com_desconto) {

	if(rrn < 0)
		return (0);

	float preco;
	short int desconto;
	Jogo j = recuperar_registro(rrn);
	char *cat, categorias[TAM_CATEGORIA];
	char precochar[TAM_PRECO];

	printf("%s\n", j.pk);
	printf("%s\n", j.nome);
	printf("%s\n", j.desenvolvedora);
	printf("%s\n", j.data);

	if(com_desconto){
		sscanf(j.preco, "%f", &preco);
		sscanf(j.desconto, "%hd", &desconto);
		preco = preco * (100 - desconto);
		preco = (int)preco / 100.0;
		sprintf(precochar, "%08.3f", preco);
		precochar[7] = '\0';
		printf("%s\n", precochar);
	}
	else{
		printf("%s\n", j.preco);
		printf("%s\n", j.desconto);
	}

	strncpy(categorias, j.categoria, max(strlen(j.categoria)+1, TAM_CATEGORIA));
  	for (cat = strtok (categorias, "|"); cat != NULL; cat = strtok (NULL, "|"))
    	printf("%s ", cat);
	
	printf("\n");
	return (1);
}

/* Imprime o menu para o usuário */
void print_menu(){
		printf("\n 1. Cadastro");
		printf("\n 2. Alteração");
		printf("\n 3. Remoção");
		printf("\n 4. Busca");
		printf("\n 5. Listagem");
		printf("\n 6. Liberar espaço");
		printf("\n 7. Finalizar");
		printf("\n\nEntre com uma das opções acima: ");
}

/* Carrega o arquivo de dados */
int carregar_arquivo() {
	scanf("%[^\n]", ARQUIVO);
	getchar();
	return (strlen(ARQUIVO) / TAM_REGISTRO);
}

/* Gerar a struct de chave primária na memória. Lê cada registro do "ARQUIVO" e acrescenta no vetor de 
chaves-primárias e realiza ordenação das chaves para permitir buscabinária*/
void criar_iprimary(Ip *indice_primario, int nregistros){
	int  i;
	Jogo jogo;

	for(i = 0; i < nregistros; i++){
		sscanf(ARQUIVO+i*TAM_REGISTRO, "%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@%[^@]", jogo.nome, jogo.desenvolvedora, 
		jogo.data, jogo.classificacao, jogo.preco, jogo.desconto, jogo.categoria);
		
		/* Gerar a chave primária */
		gerarChavePrimaria(&jogo);
		strcpy(indice_primario[i].pk, jogo.pk);
		indice_primario[i].rrn = i;
	}
	myqsort (indice_primario, nregistros, sizeof(Ip), compare);
}

/* Certifica se o valor de tamanho não ultrapassa o de valor tamanho2*/
int max(int tamanho1, int tamanho2){
	return((tamanho1 > tamanho2 ? tamanho2 : tamanho1));
}

/* Recupera do arquivo o registro com o rrn informado e retorna os dados na
 * struct Jogo */
Jogo recuperar_registro(int rrn){
	Jogo jogo;

	sscanf(ARQUIVO+rrn*TAM_REGISTRO, "%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@%[^@]", jogo.nome, jogo.desenvolvedora, 
	jogo.data, jogo.classificacao, jogo.preco, jogo.desconto, jogo.categoria);

	/* Gerar chave-primária. É preciso liberar depois, pois o conteúdo foi 
	 * alocado dinamicamente na função geraChavePrimária */
	gerarChavePrimaria(&jogo);

	return(jogo);
}

/* Função de compararação */
int compare(const void * a, const void * b){
  return( strcmp( ((Ip*)a)->pk, ((Ip*)b)->pk ) );
}

int compare_secundario(const void * a, const void * b){
  return( strcmp( ((Ir*)a)->chave, ((Ir*)b)->chave ) );
}

/* Gera chave-primária */
void gerarChavePrimaria(Jogo* jogo){
	jogo->pk[0] = toupper(jogo->nome[0]);
	jogo->pk[1] = toupper(jogo->nome[1]);
	jogo->pk[2] = jogo->desenvolvedora[0];
	jogo->pk[3] = jogo->desenvolvedora[1];
	jogo->pk[4] = jogo->data[0];
	jogo->pk[5] = jogo->data[1];
	jogo->pk[6] = jogo->data[3];
	jogo->pk[7] = jogo->data[4];
	jogo->pk[8] = jogo->classificacao[0];
	jogo->pk[9] = jogo->classificacao[1];
	jogo->pk[10] = '\0';
}

/* Verifica se já existe chave prímária */
int existe_chavePrimaria(Ip* iprimary, int nregistros, char *chave){
	int esq = 0;
    int dir = nregistros-1; 
    int meio;
    while (esq <= dir){
        meio = (esq + dir)/2;
        if (!(strcmp(chave, iprimary[meio].pk))){
            if(iprimary[meio].rrn != -1)
            	return(meio);
        	else
        		return(-1);
        }if ((strcmp(chave, iprimary[meio].pk) < 0))
            dir = meio-1;
        else
            esq = meio+1;
    }
    return(-1);
}
 	
/* Realiza a listagem dos jogos por ordem de código */
int listar_indicePrimario(Ip* iprimary, int nregistros){
	int i, contagem = 0;;

	for(i = 0; i < nregistros; i++){
		if(iprimary[i].rrn != -1){
			if(contagem){
				printf("\n");
			}
			exibir_registro(iprimary[i].rrn, 0);
			contagem++;
		}
	}
	return(contagem);
}

/* Realiza a listagem por meio de um index secundário */
int listar_indiceSecundario(Ip* iprimary, Ir *isecundario, int nregistros, int nsecundario){
	int i, existe, contagem = 0;
	ll *atual;

	/* Percorre o indice secundario e a lista de cada posicao do vetor. 
	*  Quando for o ultimo elemento da lista não irá exibir o \n*/
	for(i=0; i < nsecundario; i++){
		atual = isecundario[i].lista;
		
		while(atual != NULL){
			if((existe = existe_chavePrimaria(iprimary, nregistros, atual->pk)) != -1){
				if(contagem){
					printf("\n");
				}
				contagem++;
				exibir_registro(iprimary[existe].rrn, 0);
			}
			atual = atual->prox;
		}
	}
	return(contagem);
}

/* Realiza a listagem por meio de um index secundário */
int listar_iprice(Ip* iprimary, Ir *iprice, int nregistros, int nsecundario){
	int i, existe, contagem = 0;
	ll *atual;

	/* Percorre o indice secundario e a lista de cada posicao do vetor. 
	*  Quando for o ultimo elemento da lista não irá exibir o \n*/
	for(i=0; i < nsecundario; i++){
		atual = iprice[i].lista;
		
		while(atual != NULL){
			if((existe = existe_chavePrimaria(iprimary, nregistros, atual->pk)) != -1){
				if(contagem){
					printf("\n");
				}
				contagem++;
				exibir_registro(iprimary[existe].rrn, 1);
			}
			atual = atual->prox;
		}
	}
	return(contagem);
}

/* Realiza a listagem por meio de desenvolvedora e categoria */
int listar_categoria(Ip *iprimary, Ir *icategoria, char *categoria1, int nregistros, int ncat){
	int existe, contagem = 0;
	char categoria[TAM_CATEGORIA], *cat;
	ll *atual;
	Jogo jogo;

	if((existe = buscar_chave_secundaria(icategoria, categoria1, ncat)) != -1){
		atual = icategoria[existe].lista;
		while(atual != NULL){
			if((existe = existe_chavePrimaria(iprimary, nregistros, atual->pk)) != -1){
				jogo = recuperar_registro(iprimary[existe].rrn);

				/* Verfica se um jogo pertence a uma certa categoria e caso pertença o exibe */
				strcpy(categoria, jogo.categoria);
				for (cat = strtok (categoria, "|"); cat != NULL; cat = strtok (NULL, "|")){
					if(!(strcmp(categoria, cat))){
						if(contagem){
							printf("\n");
						}
						contagem++;
						exibir_registro(iprimary[existe].rrn, 0);
					}
				}
			}								
			atual = atual->prox;
		}
	}
	return(contagem);
}

/* Remover indice primario */
int remover_indice_primario(Ip* indice_primario, int* nregistros, char* pk){
	int existe;

	if((existe  = existe_chavePrimaria(indice_primario, *nregistros, pk)) == -1){
		return(0);
	}else{
		ARQUIVO[TAM_REGISTRO*(indice_primario[existe].rrn)]     = '*';
		ARQUIVO[TAM_REGISTRO*(indice_primario[existe].rrn) + 1] = '|';
		indice_primario[existe].rrn = -1;
		return(1);
	}
}

/* (Re)cria os indeces secundários*/
void criar_index_secundarios(Ip* iprimary, Ir* ititulo, Ir *idev, Ir *iprice, Ir *icategoria, int nregistros, int* ntitulos, int *ndev, int *nprice, int *ncat){
	int  i;
	Jogo jogo;
	float preco, desconto;
	char *cat, categoria[TAM_CATEGORIA];

	for(i = 0; i < nregistros; i++){
		if(iprimary[i].rrn != -1){
			jogo = recuperar_registro(iprimary[i].rrn);
			criar_isecundario(ititulo, jogo.nome, jogo.pk, ntitulos, TAM_NOME);

			criar_isecundario(idev, jogo.desenvolvedora, jogo.pk, ndev, TAM_DESENVOLVEDORA);

			sscanf(jogo.preco, "%f", &preco);
			sscanf(jogo.desconto, "%f", &desconto);
			preco = preco* (100 - desconto);
			preco = (int)preco / 100.0;

			sprintf(jogo.preco, "%08.3f", preco);
			jogo.preco[7] = '\0';
			criar_isecundario(iprice, jogo.preco, jogo.pk, nprice, TAM_PRECO);

			strcpy(categoria, jogo.categoria);
			for (cat = strtok (categoria, "|"); cat != NULL; cat = strtok (NULL, "|"))
				criar_isecundario(icategoria, cat, jogo.pk, ncat, TAM_CATEGORIA);
		}
	}
}


/* Insere indece secundário */
/* Recebe o vetor de indece secundário, chave primária e o numero de chaves. 
*  Caso não exista ainda chaves cadastradas ela é inserida na ultima posiçaõ e é criada sua lista de pks,
*  mas caso contrário a pk é apenas inserida na chave solicitada */

void criar_isecundario(Ir* isecundario,  char* chave, char* chave_primaria, int *nregistros, int tamanho){
	int lista;

	if((lista = buscar_chave_secundaria(isecundario, chave, *nregistros)) == -1){		

		/* Aloca e insere a nova chave secundária */
		isecundario[*nregistros].chave = (char*)malloc(sizeof(char)*tamanho);
		strcpy(isecundario[*nregistros].chave, chave);

		/* Cria a nova lista de chaves primárias */
		isecundario[*nregistros].lista = NULL;
		inserir_chave(&(isecundario[*nregistros].lista), chave_primaria);

		/* Realizar qsort */
		myqsort (isecundario, (*nregistros)+1 , sizeof(Ir), compare_secundario);
		(*nregistros)++;
	}else{
		/* Insere a chave primária na lista da chave secundária já existente */
		inserir_chave(&(isecundario[lista].lista), chave_primaria);
	}
}

/* Realiza busca binária para achar uma chave secundaria */
int buscar_chave_secundaria(Ir* isecundario, char* chave, int nregistros){
	int esq = 0;    
    int dir = nregistros-1; 
    int meio;
    while (esq <= dir){
        meio = (esq + dir)/2;
        if (!(strcmp(chave, isecundario[meio].chave)))
            return(meio);
        if ((strcmp(chave, isecundario[meio].chave) < 0))
            dir = meio-1;
        else
            esq = meio+1;
    }
    return(-1);
}

/* A chave primária é inserido na lista invertida em ordem crescente */
void inserir_chave(ll** lista, char* chave_primaria){
	ll *atual = *lista;
	ll *anterior = NULL;
	/* Cria e inicializa os valores do novo nó */
	ll *novo = (ll*)malloc(sizeof(ll));
	strcpy(novo->pk, chave_primaria);

	/* Se o a lista estiver vazia o valor apenas é inserido. Caso a lista não esteja vazia é inserido o novo nó
	*  ao achar um nó de menor de maior procedência que o novo ou ao chegar ao final da lista */
	if(!(*lista)){
		*lista = novo;
		(*lista)->prox = NULL; 
	}else{
		while((atual) && (strcmp(chave_primaria, atual->pk) > 0)){
			anterior = atual;
			atual = atual->prox;
		}
		if(!anterior)
			*lista = novo;
		else
			anterior->prox = novo;
		novo->prox = atual;

	}
}


/* Apaga todos os registros marcados */
int libera_espaco(int nregistros){
	int livre = 0, atual = 0;

	/* Caso o arquivo de dados esteja vazio */
	if(!nregistros)
		return(0);

	/* Percorre o ARQUIVO até achar um espaço livre */
	while((livre < nregistros) && !((*(ARQUIVO+(livre*TAM_REGISTRO)) == '*') && (*(ARQUIVO+(livre*TAM_REGISTRO+1)) == '|')))
		livre++;

	/* Procura pelo próximo registro não pagado */
	atual = livre+1;
	while(atual < nregistros){
		if(!((*(ARQUIVO+(atual*TAM_REGISTRO)) == '*') && (*(ARQUIVO+(atual*TAM_REGISTRO+1)) == '|'))){
			strncpy(ARQUIVO+(livre*TAM_REGISTRO), ARQUIVO+(atual*TAM_REGISTRO), TAM_REGISTRO);
			livre++;
		}
		atual++;
	}
	*(ARQUIVO+(livre*TAM_REGISTRO)) = '\0';

	return(livre);
}

/* Limpa listas dos indices secundários */
void limpa_listas(Ir *isecundario, int nregistros){
	int i;
	ll *atual, *aux;

	for(i = 0; i < nregistros; i++){
		free(isecundario[i].chave);
		atual = isecundario[i].lista;

		while(atual){
			aux = atual;
			atual = atual->prox;
			free(aux);
		}
	}
}



/* Métodos de ordenação */

/* Selection Sort */
void selection_sort(Ir *isedundario, int nregistros) { 
  int i, j, min;
  Ir aux;

  for (i = 0; i < (nregistros-1); i++) {
     min = i;
     for (j = (i+1); j < nregistros; j++) {
       if((strcmp(isedundario[j].chave, isedundario[min].chave)) < 0)
         min = j;
     }
     if((strcmp(isedundario[i].chave, isedundario[min].chave))){
       aux = isedundario[i];
       isedundario[i] = isedundario[min];
       isedundario[min] = aux; 
     }
  }
}

/* Selection Sort */
void selection_sort_primario(Ip *iprimario, int nregistros) { 
  int i, j, min;
  Ip aux;

  for (i = 0; i < (nregistros-1); i++) {
     min = i;
     for (j = (i+1); j < nregistros; j++) {
       if((strcmp(iprimario[j].pk, iprimario[min].pk)) < 0)
         min = j;
     }
     if((strcmp(iprimario[i].pk, iprimario[min].pk))){
       aux = iprimario[i];
       iprimario[i] = iprimario[min];
       iprimario[min] = aux;
     }
  }
}


/* Método Qsort foi obtido no site: 
*  http://www.roman10.net/2012/02/23/quick-sort-and-a-sample-implementation-for-c-standard-library-qsort-function/ ] */
/* Qsort */
void myswap(void *e1, void *e2, int size) {

    void* sp = (void*) malloc(size);
    memcpy(sp, e1, size);
    memcpy(e1, e2, size);
    memcpy(e2, sp, size);
    free(sp);

}

void myqsort(void* base, size_t n, size_t size, int (*cmp)(const void*, const void*)) {

    int i;
    int rv;
    void* mp;

    if (n <= 1) {return;}
    	mp = base;

    for (i = 1; i < n; ++i) {
        rv = cmp(base, base + size*i);
        if (rv > 0) {
            mp += size;
            if (mp != base+size*i) {
       	 		myswap(base+size*i, mp, size);
            }
        }
    }
    myswap(base, mp, size);
    myqsort(base, (mp - base)/size, size, cmp);
    myqsort(mp + size, n - 1 - (mp-base)/size, size, cmp);
}