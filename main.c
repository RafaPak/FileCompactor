#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

// Autores: Rafael Pak Bragagnolo - 18206 & Ivan Baraldi Knobel - 18193
// Compactador e Descompactador de Arquivos com a codifica��o de Huffman

// N� da �rvore, que armazena o caracter, sua frequ�ncia e os n�s direito e esquerdo
typedef struct noArvore
{
    int freq;
    unsigned char c;
    struct noArvore *esq;
    struct noArvore *dir;
} noArvore;

// N� da lista, que armazena um n� da �rvore e o pr�ximo n� da lista
typedef struct noLista
{
    noArvore *no;
    struct noLista *prox;
} noLista;

// Lista que tem um ponteiro para o primeiro n� e a quantidade de n�s
typedef struct lista
{
    noLista *head;
    int qtd;
} lista;

// Novo n� da lista, que devolve um ponteiro com esse n� e recebe como par�metro um n� de �rvore
noLista *novoNoL(noArvore *noArv)
{
    noLista *novo;
    if ((novo = malloc(sizeof(*novo))) == NULL) return NULL;

    novo->no = noArv;

    novo->prox = NULL;

    return novo;
}

// Novo n� da �rvore, que devolve um ponteiro para esse n� e recebe como par�metro o caracter,
// sua frequ�ncia e os n�s esquerdo e direito
noArvore *novoNoArv(unsigned char c, int freq, noArvore *esq, noArvore *dir)
{
    noArvore *novo;
    if ((novo = malloc(sizeof(*novo))) == NULL)return NULL;

    novo->c = c;
    novo->freq = freq;
    novo->esq = esq;
    novo->dir = dir;

    return novo;
}

// Obt�m o n�mero de vezes que o mesmo caracter apareceu e armazena esse valor na posi��o respectiva do vetor
void obterFreqByte(FILE *arq, unsigned int *lista)
{
    unsigned char ch;

    while (fread(&ch, 1, 1, arq))
        lista[(unsigned char)ch]++;

    rewind(arq);
}

// Insere um novo n� na lista, que ambos foram passados como par�metros
void insereLista(noLista *n, lista *l)
{
    if (!l->head)
        l->head = n;

    else if (n->no->freq < l->head->no->freq)
    {
        n->prox = l->head;
        l->head = n;
    }
    else
    {
        noLista *aux = l->head->prox;
        noLista *aux2 = l->head;

        while (aux && aux->no->freq <= n->no->freq)
        {
            aux2 = aux;
            aux = aux2->prox;
        }

        aux2->prox = n;
        n->prox = aux;
    }

    l->qtd++;
}

// Remove o primeiro n� da lista e retorna um ponteiro para um n� da �rvore
noArvore *diminuiLista(lista *l)
{
    noLista *noL = l->head;
    noArvore *noArv = noL->no;

    l->head = noL->prox;

    free(noL);
    noL = NULL;

    l->qtd--;

    return noArv;
}

// Gera a �rvore de Huffman, primeiro criando a lista com todos os caracteres, atrav�s da lista dos caracteres, que � passada como par�metro e depois
// formando uma �rvore, juntando os n�s da lista e formando uma �rvore (a de Huffman), usando o m�todo diminuiLista() fazendo com que s� sobre um n�
// na lista, que � a pr�pria �rvore de Huffman
noArvore *construirArv(unsigned *lst)
{
    lista l = {NULL, 0};

    for (int i = 0; i < 256; i++)
        if (lst[i])
            insereLista(novoNoL(novoNoArv(i, lst[i], NULL, NULL)), &l);

    while (l.qtd > 1)
    {
        noArvore *noEsq = diminuiLista(&l);
        noArvore *noDir = diminuiLista(&l);

        noArvore *soma = novoNoArv('#', noEsq->freq + noDir->freq, noEsq, noDir);

        insereLista(novoNoL(soma), &l);
    }

    return diminuiLista(&l);
}

// Come�ando na ra�z, forma-se um caminho de 0s (esquerda), 1s (direita) e/ou nulo de acordo com caminho da raiz da �rvore at� uma folha, que representa
// cada caracter do arquivo; isso feito atrav�s de recurs�o
bool codigoByte(noArvore *no, unsigned char c, char *buffer, int tamanho)
{
    if (!(no->esq || no->dir) && no->c == c)
    {
        buffer[tamanho] = '\0';
        return true;
    }
    else
    {
        bool encontrado = false;

        if (no->esq)
        {
            buffer[tamanho] = '0';
            encontrado = codigoByte(no->esq, c, buffer, tamanho + 1);
        }

        if (!encontrado && no->dir)
        {
            buffer[tamanho] = '1';
            encontrado = codigoByte(no->dir, c, buffer, tamanho + 1);
        }

        if (!encontrado)
            buffer[tamanho] = '\0';

        return encontrado;
    }
}

// Fun��o recursiva que percorre a �rvore e desaloca o endere�o de mem�ria de cada n�
void desalocarArvHuffman(noArvore *no)
{
    if (!no)
        return;
    else
    {
        noArvore *esq = no->esq;
        noArvore *dir = no->dir;
        free(no);

        desalocarArvHuffman(esq);
        desalocarArvHuffman(dir);
    }
}

// M�todo que pega 1 bit de 1 byte
int geraBit(FILE *arqEnt, int pos, unsigned char *aux)
{
    (pos % 8 == 0) ? fread(aux, 1, 1, arqEnt) : NULL == NULL ;

    return !!((*aux) & (1 << (pos % 8)));
}

// M�todo Main que cont�m um cabe�alho e puxa a fun��o iniciar() e retorna 0
int main()
{
    printf("=-=-=-=-=-=-=-=-=-=-=\n");
    printf("     Compactador     \n");
    printf("=-=-=-=-=-=-=-=-=-=-=\n\n");
    iniciar();
    return 0;
}

// M�todo que inicia com um menu de opera��es para o usu�rio com switch, que vai compactar um arquivo, descompat�-lo, sair do programa ou ent�o enviar�
// um aviso de erro
int iniciar()
{
    printf("Operacao: \n\n");
    printf("1. Compactar Arquivo\n");
    printf("2. Descompactar Arquivo\n");
    printf("3. Sair\n\n");

    char escolha;
    scanf("%c", &escolha);
    printf("\n");

    switch (escolha)
    {
        case '1':
            compactar();
            break;

        case '2':
            descompactar();
            break;

        case '3':
            break;

        default:
            printf("Valor de operacao invalido\n");
            break;
    }

    return 0;
}

// M�todo que compacta um arquivo em outro, do formato .dale
int compactar()
{
    FILE *arqLer;
    FILE *arqEsc;
    char nomeArqLer[30];
    char nomeArqEsc[30];
    unsigned lBytes[256] = {0};

    printf("Digite o nome do arquivo que sera compactado: ");
    scanf("%s", nomeArqLer);

    arqLer = fopen(nomeArqLer, "rb");

    if (arqLer == NULL)
        erroArq();

    printf("\nDigite o nome do arquivo em que o arquivo original sera compactado: ");
    scanf("%s", nomeArqEsc);
    printf("\n");

    arqEsc = fopen(nomeArqEsc, "wb");

    if (arqEsc == NULL)
        erroArq();

    obterFreqByte(arqLer, lBytes);

    noArvore *raiz = construirArv(lBytes);

    fwrite(lBytes, 256, sizeof(lBytes[0]), arqEsc);
    fseek(arqEsc, sizeof(unsigned int), SEEK_CUR);

    unsigned char c;
    unsigned tamanho = 0;
    unsigned char aux = 0;

    while (fread(&c, 1, 1, arqLer) >= 1)
    {
        char buffer[1024] = {0};
        codigoByte(raiz, c, buffer, 0);

        for (char *i = buffer; *i; i++)
        {
            if (*i == '1')
                aux = aux | (1 << (tamanho % 8));

            tamanho++;

            if (tamanho % 8 == 0)
            {
                fwrite(&aux, 1, 1, arqEsc);
                aux = 0;
            }
        }
    }

    fwrite(&aux, 1, 1, arqEsc);
    fseek(arqEsc, 256 * sizeof(unsigned int), SEEK_SET);
    fwrite(&tamanho, 1, sizeof(unsigned), arqEsc);

    fseek(arqLer, 0L, SEEK_END);
    double tamanhoLer = ftell(arqLer);

    fseek(arqEsc, 0L, SEEK_END);
    double tamanhoEsc = ftell(arqEsc);

    printf("\nArquivo de entrada: %s (%g bytes)\nArquivo de saida: %s (%g bytes)", nomeArqLer, tamanhoLer / 1000, nomeArqEsc, tamanhoEsc / 1000);

    desalocarArvHuffman(raiz);

    fclose(arqEsc);
    fclose(arqLer);
}

// M�todo que desompacta um arquivo no formato .dale, para determinado formato de arquivo
int descompactar()
{
    FILE *arqLer;
    FILE *arqEsc;
    char nomeArqLer[30];
    char nomeArqEsc[30];
    unsigned lBytes[256] = {0};

    printf("Digite o nome do arquivo que esta compactado: ");
    scanf("%s", nomeArqLer);

    arqLer = fopen(nomeArqLer, "rb");

    if (arqLer == NULL)
        erroArq();

    printf("\nDigite o nome do arquivo em que o arquivo original sera descompactado: ");
    scanf("%s", nomeArqEsc);
    printf("\n");

    arqEsc = fopen(nomeArqEsc, "wb");

    if (arqEsc == NULL)
        erroArq();

    fread(lBytes, 256, sizeof(lBytes[0]), arqLer);

    noArvore *raiz = construirArv(lBytes);

    unsigned tamanho;
    fread(&tamanho, 1, sizeof(tamanho), arqLer);

    unsigned posicao = 0;
    unsigned char aux = 0;

    while (posicao < tamanho)
    {
        noArvore *noAtual = raiz;

        while (noAtual->esq || noAtual->dir)
            noAtual = geraBit(arqLer, posicao++, &aux) ? noAtual->dir : noAtual->esq;

        fwrite(&(noAtual->c), 1, 1, arqEsc);
    }

    fseek(arqLer, 0L, SEEK_END);
    double tamanhoLer = ftell(arqLer);

    fseek(arqEsc, 0L, SEEK_END);
    double tamanhoEsc = ftell(arqEsc);

    printf("Arquivo de entrada: %s (%g bytes)\nArquivo de saida: %s (%g bytes)", nomeArqLer, tamanhoLer / 1000, nomeArqEsc, tamanhoEsc / 1000);

    desalocarArvHuffman(raiz);

    fclose(arqEsc);
    fclose(arqLer);
}

// Erro na abertura de arquivo, caso passe um nome de arquivo inv�lido ou d� algum problema para abrir o arquivo
void erroArq()
{
    printf("Problemas com a abertura do arquivo\n");
    exit(0); // Interrompe a execu��o do programa e fecha todos os arquivos que o programa abriu
}
