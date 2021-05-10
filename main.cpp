#include <iostream>
#include <pthread.h>
#include <math.h>
#include <time.h>

pthread_mutex_t mutex;

using namespace std;

int **vetor,
    THREAD = 5, // VALOR PREDEFINIDO, PARA CASO O USUÁRIO DIGITAR ERRADO.
    TAM = 5000,
    quantidade_primos = 0,// CONTADOR PARA OS PRIMOS.
    sequencial = 0;

typedef struct
{
    int inicial_l;
    int final_l;
    int inicial_col;
    int final_col;

}  Parametro;

// IDENTIFICA OS NUMEROS PRIMOS.
bool eh_primo(int num)
{
    int raiz_primo = sqrt(num);

    for (int i=2; i< raiz_primo;  i++)
    {
        if( num % i == 0)
        {
            return false;
        }
    }
    return true;
}

// CONTA OS NUMEROS PRIMOS.
void *conta_primos(void *parametro)
{
    Parametro *vetor_p = (Parametro*)parametro;

    for(int i=vetor_p->inicial_l; i <  vetor_p->final_l; i++ )
    {
        for(int j=vetor_p->inicial_col; j< vetor_p->final_col; j++)
        {
            if(eh_primo(vetor[i][j]))
            {
                // CONTA OS NUMEROS PRIMOS.
                if(sequencial == 0){
                quantidade_primos+=1;
                }else{
                    pthread_mutex_lock (&mutex);

                    quantidade_primos+=1;

                    pthread_mutex_unlock (&mutex);
                }
            }
        }
    }

    printf("Terminado thread.\n");
    sequencial = 1;
    // FINALIZA AS THREADS.
    pthread_exit((void*) NULL);

}

int main ()
{
    int quantidade_primos_1, quantidade_primos_2, sair = 0;
    
    while(sair != 1){

    printf("\n ------ O TAMANHO DA MATRIZ DEVE SER DIVISIVEL PELO NUMERO DE THREADS! ------ \n");
    printf("\n1. Definir o tamanho da matriz (p. ex. tamanho = 5000, matriz 5000x5000): \n");
    scanf("%d", &TAM);
    printf("\n2. Definir o numero de Threads (Threads > 1): \n");
    scanf("%d", &THREAD);

    time_t inicial, final;
    double t_1, t_2;

    // inicia mutex
     pthread_mutex_init(&mutex, NULL);


    // ID DA THREAD.
    pthread_t tid[THREAD+1];

    // NOME DA THREAD.
    pthread_attr_t attr[THREAD+1];

    // PARAMETROS PARA THREAD.
    Parametro parametro[THREAD+1];

    // SEMENTE.
    srand(time(NULL));

    // ALOCAÇÃO DINAMICA DE MEMÓRIA.
    vetor = new int*[TAM];

    for(int i=0; i<TAM; i++)
    {
        vetor[i] = new int[TAM];
    }

    for(int i=0; i <  TAM; i++ )
    {
        for(int j=0; j< TAM; j++)
        {
            vetor[i][j] = rand()%(9999999);
        }
    }

    printf("\n-----------------------------------------------------------------------------\n");
    printf("\nVerificando numeros primos...\n\n");

    // INICIA AS THREADS.
    for(int i=0; i<= THREAD; i++)
    {
        pthread_attr_init(&attr[i]);
    }

    // INICIO DO TIME SEQUENCIAL.
    inicial = clock();

    // MODO SEQUENCIAL.
    parametro[THREAD].inicial_l = 0;
    parametro[THREAD].final_l = TAM;
    parametro[THREAD].inicial_col = 0;
    parametro[THREAD].final_col = TAM;
    pthread_create(&tid[THREAD], &attr[THREAD], conta_primos, &parametro[THREAD]);

    pthread_join(tid[THREAD], NULL);

    final = clock();
    t_1 = ((double)(final-inicial))/(CLOCKS_PER_SEC);
    quantidade_primos_1 = quantidade_primos;
    quantidade_primos=0;

    // INICIO DO TIME.
    inicial = clock();


    // DELIMITA O INICIO E O FIM DE CADA THREAD.
    for(int i=1; i <= THREAD;  i++)
    {
        parametro[i-1].inicial_l = ((i-1)*(TAM/THREAD));
        parametro[i-1].final_l = (i-1)*(TAM/THREAD)+(TAM/THREAD);
        parametro[i-1].inicial_col = 0;
        parametro[i-1].final_col = TAM;

        pthread_create(&tid[i-1], &attr[i-1], conta_primos, &parametro[i-1]);
    }

    // AGUARDA AS THREADS.
    for(int i=0; i<THREAD; i++)
    {
        pthread_join(tid[i], NULL);
    }

    final = clock();
    t_2 = ((double)(final-inicial))/(CLOCKS_PER_SEC);
    quantidade_primos_2 = quantidade_primos;

    printf("\n(Sequencial)\nQuantidade de numeros primos = %d", quantidade_primos_1);
    printf("\nTempo de execucao: %.4lf s\n\n", t_1);

    printf("\n(Paralelo)\nQuantidade de numeros primos = %d", quantidade_primos_2);
    printf("\nTempo de execucao: %.4lf s\n\n-----------------------------------------------------------------------------\n", t_2);

    printf("\nSair?(1 - Sim, 0 - Nao) ");
    scanf("%d", &sair);
        
    // DESALOCA MEMORIA.
    for(int i=0; i<TAM; i++)
    {
        delete vetor[i];
    }

    pthread_mutex_destroy(&mutex);
    delete vetor;
    }
}
