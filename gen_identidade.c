#include <stdio.h>
#include <stdlib.h>


int get_identidade(int** mat, int l, int c);
char* is_identidade(int ret);
void gera_matrizrand(int m, int n);
void gera_identidade(int m, int n);

int main (){
	int m = 10000, n = 10000;
	int** matriz;
	gera_identidade(m,n);
	//gera_matrizrand(m,n);
	FILE* f = fopen("teste.txt", "r");
	matriz = (int**)malloc(m*sizeof(int*));
	for(int i = 0; i < m; i++){
		matriz[i] = (int*)malloc(n*sizeof(int));
		for(int j = 0; j < n; j++){
			fscanf(f,"%d",&matriz[i][j]);
		}
	}
	fclose(f);
	printf("%s",is_identidade(get_identidade(matriz,m,n)));
	free(matriz);
	return 0;
}

void gera_identidade(int m, int n){
	FILE *f= fopen("teste.txt", "w");
  	for(int i = 0; i < m; i++){
    	for(int j = 0; j < n; j++){
    	 	if(i==j){
        		fprintf(f,"%d ",1);
      		}
      		else{
        		fprintf(f,"%d ",0);
      		}
    	}
  	}
  fclose(f);
}

void gera_matrizrand(int m, int n){
	FILE *f= fopen("teste.txt", "w");
	for(int i = 0; i < m; i++){
    	for(int j = 0; j < n; j++){
    		fprintf(f,"%d ",rand() % 2);
    	}
 	}
  	fclose(f);
}

int get_identidade(int** matriz, int m, int n){
	for(int i=0; i<m; i++){
		for(int j=0; j<n; j++){
			if(matriz[i][j] != 1 && i == j){
				return 0;
			}
			if(matriz[i][j] != 0 && i != j){
				return 0;
			}
		}
	}
	return 1;
}

char* is_identidade (int ret){
	if(ret == 1){
		return "É identidade\n";
	}
	if(ret == 0){
		return "Não é identidade\n";
	}
}