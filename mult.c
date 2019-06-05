#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void gera_matriz(int m, int n);
int** aloca_matriz(int m, int n, FILE* f);
void multiplica_matrizes(int** a, int** b, int** mult, int m, int n);

int main (){
	int m = 2000, n = 2000;
	int** a;
	int** b;
	int** mult;
	//gera_matriz(m, n);

	// ALOCAÇÃO DINÂMICA DELAS COLOCANDO OS VALORES DO ARQUIVO NAS MATRIZES a E b
	FILE* f1 = fopen("matriz1.txt", "r");
	a = aloca_matriz(m, n, f1);
	FILE* f2 = fopen("matriz2.txt", "r");
	b = aloca_matriz(m, n, f2);

	fclose(f1);
	fclose(f2);

	// ALOCAÇÃO DINÂMICA DA MATRIZ RESULTANTE
	mult = (int**)malloc(m*sizeof(int*));
	for(int i = 0; i < m; i++){
		mult[i] = (int*)malloc(n*sizeof(int));
	}

  	clock_t tempo = clock();
	// MULTIPLICAÇÃO DAS MATRIZES a E b resultando em mult
	multiplica_matrizes(a, b, mult, m, n);
  	float tempoFinal = (float)((clock() - tempo)*1.0 / CLOCKS_PER_SEC);
  	printf("%f\n", tempoFinal);

	free(a);
	free(b);
	free(mult);

	return 0;
}

void gera_matriz(int m, int n) { 
	//int i, j;
	FILE *f1 = fopen("matriz1.txt", "w");
	FILE *f2 = fopen("matriz2.txt", "w");

	for(int i = 0; i < m; i++){
    	for(int j = 0; j < n; j++){
    		fprintf(f1,"%d ", rand() % 10);
    		fprintf(f2,"%d ", rand() % 10);
    	}
    	//fprintf(f1, "\n");
    	//fprintf(f2, "\n");
 	}

  	fclose(f1);
  	fclose(f2);
}


int** aloca_matriz(int m, int n, FILE* f) {
	//int i, j;
	int** matriz;
	matriz = (int**)malloc(m*sizeof(int*));
	for(int i = 0; i < m; i++){
		matriz[i] = (int*)malloc(n*sizeof(int));
		for(int j = 0; j < n; j++){
			fscanf(f,"%d",&matriz[i][j]);
		}
	}
	return matriz;
}


void multiplica_matrizes(int** a, int** b, int** mult, int m, int n) {
	//int i, j, k;
	int sum = 0;
	for (int i = 0; i < m; i++) {
    	for (int j = 0; j < m; j++) {
        	for (int k = 0; k < n; k++) {
          		sum = sum + a[i][k]*b[k][j];
        	}
 			mult[i][j] = sum;
        	sum = 0;
      	}
    }
}