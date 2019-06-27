#include <immintrin.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define SEC_AS_NANO 1000000000.0

void matriz_transposta (float* matriz, int n);
void multiplica_matrizes(float* a, float* b, float* mult, int n);
void imprime_matriz(float* matriz, int n);

int main () {
	int tam = 8, newtam = tam;
	int n = tam*tam, i, j;

	// Caso o tamanho da matriz não seja multiplo de 8, ele ajustará o tamanho ao próximo multiplo para poder fazer a multiplicação com AVX
	if (tam % 8 != 0) { 
		for(i = 0; newtam%8 != 0; i++) {
			newtam++;
		}
		n = newtam*newtam;
		//printf("Novo tamanho: %d\n", newtam);
	}

	// Alocação dinâmica das matrizes em forma de vetor
	float* a = (float*)aligned_alloc(32, n*sizeof(float));
	float* b = (float*)aligned_alloc(32, n*sizeof(float));
	float* mult = (float*)aligned_alloc(32, n*sizeof(float));

	// Gera as matrizes A e B, sabendo que caso os índices sejam maiores que o tamanho original, recebam 0 para não afetar na multiplicação
	for(i = 0; i < newtam; i++){
		for(j = 0; j < newtam; j++) {
			if (i >= tam || j >= tam) {
				a[i*newtam+j] = 0.0;
			}
			else {
				a[i*newtam+j] = rand() % 10;
			}
		}
	}

	for(i = 0; i < newtam; i++){
		for(j = 0; j < newtam; j++) {
			if (i >= tam || j >= tam) {
				b[i*newtam+j] = 0.0;
			}
			else {
				b[i*newtam+j] = rand() % 10;
			}
		}
	}					

	// Inicialização da matriz resultante com 0
	for(i = 0; i < n; i++){
		mult[i] = 0.0;
	}

	// Imprime as matrizes A e B
	printf("A: \n");
	imprime_matriz(a, newtam);
	printf("\n");

	printf("B: \n");
	imprime_matriz(b, newtam);
	printf("\n");

	// Cálculo do tempo em segundos apenas da função da multiplicação e a transposta
  	//clock_t tempo = clock();
  	struct timespec ini, fim;
    clock_gettime(CLOCK_REALTIME, &ini);
	matriz_transposta(b, newtam); //Passa a matriz B para transposta
	multiplica_matrizes(a, b, mult, newtam); //Multiplica A e B transposta, resultando em mult
  	//float tempoFinal = (float)((clock() - tempo)*1.0 / CLOCKS_PER_SEC);
  	clock_gettime(CLOCK_REALTIME, &fim);
    double iniSec = ini.tv_sec + ini.tv_nsec / SEC_AS_NANO;
    double fimSec = fim.tv_sec + fim.tv_nsec / SEC_AS_NANO;
  	// Imprime a matriz resultante
  	printf("MULTIPLICAÇÃO: \n");
  	imprime_matriz(mult, newtam);
	printf("\n"); 

  	//printf("AVX: %.5f seg\n", tempoFinal);
    printf("AVX = %.5f s\n",(fimSec-iniSec));

	free(a);
	free(b);
	free(mult);

	return 0;
}

void multiplica_matrizes(float* a, float* b, float* mult, int n) {
	/* Instruções vetoriais utilizadas e suas funcionalidades
	__m256 _mm256_load_ps (float const * mem_addr)
	Load 256-bits (composed of 8 packed single-precision (32-bit) floating-point elements) from memory into dst. 
	mem_addr must be aligned on a 32-byte boundary or a general-protection exception may be generated.

	__m256 _mm256_mul_ps (__m256 a, __m256 b)
	Multiply packed single-precision (32-bit) floating-point elements in a and b, and store the results in dst. 
	
	__m256 _mm256_add_ps (__m256 a, __m256 b)
	Add packed single-precision (32-bit) floating-point elements in a and b, and store the results in dst.
	*/
	__m256 reglinhaA, reglinhaB, resultado; // Declaração de registradores
	int i, j, k, aux = 0, tam = n*n;
	float soma = 0.0;
	//#pragma omp parallel private(resultado)
	for (i = 0; i < tam; i += n) { 
		for (k = 0; k < tam; k += n) { 
			for (j = 0; j < n; j += 8) {
				reglinhaA = _mm256_load_ps(&a[i+j]);
				//printf("a%d: %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f e %.2f\n", i*n+j, reglinhaA[0], reglinhaA[1], reglinhaA[2], reglinhaA[3], reglinhaA[4], reglinhaA[5], reglinhaA[6], reglinhaA[7]);
				reglinhaB = _mm256_load_ps(&b[j+k]);
				//printf("b%d: %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f e %.2f\n", j*n+k, reglinhaB[0], reglinhaB[1], reglinhaB[2], reglinhaB[3], reglinhaB[4], reglinhaB[5], reglinhaB[6], reglinhaB[7]);
				resultado = _mm256_mul_ps(reglinhaA, reglinhaB);
				resultado = _mm256_hadd_ps(resultado, resultado);
				resultado = _mm256_hadd_ps(resultado, resultado);
				soma += resultado[0] + resultado[7];
			}
			mult[aux] = soma;
			soma = 0.0;
			aux++;
		}
		//printf("\n");
	}
}


	/*
	#pragma omp parallel firstprivate(mult) private(reglinhaA, reglinhaB, resultado, soma)
	for (i = 0; i < n; i++) { 
		for (j = 0; j < n; j++) { 
			for (k = 0; k < n; k += 8) {
				reglinhaA = _mm256_load_ps(&a[i*n+k]);
				//printf("i = %d   j = %d   k = %d\n", i, j, k);
				//printf("a%d: %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f e %.2f\n", i*n+j, reglinhaA[0], reglinhaA[1], reglinhaA[2], reglinhaA[3], reglinhaA[4], reglinhaA[5], reglinhaA[6], reglinhaA[7]);
				reglinhaB = _mm256_load_ps(&b[j*n+k]);
				//printf("b%d: %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f e %.2f\n", j*n+k, reglinhaB[0], reglinhaB[1], reglinhaB[2], reglinhaB[3], reglinhaB[4], reglinhaB[5], reglinhaB[6], reglinhaB[7]);
				resultado = _mm256_mul_ps(reglinhaA, reglinhaB);
				resultado = _mm256_hadd_ps(resultado, resultado);
				resultado = _mm256_hadd_ps(resultado, resultado);
				soma += resultado[0] + resultado[7];
			}
			mult[aux] = soma;
			soma = 0.0;
			aux++;
		}
		//printf("\n");
	}

	for (i = 0; i < tam; i += n) { 
		for (k = 0; k < tam; k += n) { 
			for (j = 0; j < n; j += 8) {
				reglinhaA = _mm256_load_ps(&a[i+j]);
				reglinhaB = _mm256_load_ps(&b[j+k]);
				resultado = _mm256_mul_ps(reglinhaA, reglinhaB);
				resultado = _mm256_hadd_ps(resultado, resultado);
				resultado = _mm256_hadd_ps(resultado, resultado);
				soma += resultado[0] + resultado[7];
			}
			mult[aux] = soma;
			soma = 0.0;
			aux++;
		}
	}
}
for(i = 0; i < n; i++) {
	for(j = 0; j < n; j++) {
		reglinhaA = _mm256_set1_ps(a[i*n + j]);
		for(k = 0; k < n; k += 8){ 
			reglinhaB = _mm256_load_ps(&b[j*n + k]);
			resultado = _mm256_load_ps(&mult[i*n + k]);
			resultado = _mm256_add_ps(resultado, _mm256_mul_ps(reglinhaA, reglinhaB));
			_mm256_store_ps(&mult[i*n + k], resultado);
		}
	}
}*/


void matriz_transposta (float* matriz, int n) {
	int aux, i, j; 
  	for (i = 0; i < n; i++) {
    	for (j = i+1; j < n; j++) {
    		if(i != j) {
	    		aux = matriz[i*n + j];
	   			matriz[i*n + j] = matriz[j*n + i];
	   			matriz[j*n + i] = aux;
    		}
    	}
  	}	
 }

void imprime_matriz(float* matriz, int n) {
	int i;
 	for (i = 0; i < n*n; i++) {
 		if (i%n == 0 && i != 0) { 
 			printf("\n"); 
 		}
    	printf("%.2f\t", matriz[i]);
    }
    printf("\n");
 }