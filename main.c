#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <float.h>

//format : argv[0] file NumCentroides NumIteracoes tolerancia
//O arquivo deve estar em formato binario, cada ponto sendo dois doubles, o x e o y

typedef struct Point
{
	double x;
	double y;
	int centroid;
}Point;




// argv[0] file numIteracoes numCentroids
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
		Point* initialCentroid = generateCentroid(fp,numCentroids);
	}
	else //ordinal user -- slave
	{
		//NumberOfPairs/numberOfProcessor
		posicione(fp);
		Ponto* pontos = 
	}
	
	MPI_Finalize();
	return 0;
	
	
}
