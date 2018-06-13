#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <float.h>

//format : argv[0] file NumCentroides NumIteracoes tolerancia
//O arquivo deve estar em formato binario, cada ponto sendo dois doubles, o x e o y

typedef struct Ponto
{
	double x;
	double y;
}Ponto;


// argv[0] file numIteracoes
int main(int argc, char* argv[])
{
	FILE* fp;
	int size;
	int rank;
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	
	fp=fopen(argv[1],"r");
	
	if(rank==0) //root -- master
	{
		
	}
	else //ordinal user -- slave
	{
		int howMuch = howMuchBytes(fp)/2/size;
		int initialPosition = 
	}
	
	MPI_Finalize();
	return 0;
	
	
}
