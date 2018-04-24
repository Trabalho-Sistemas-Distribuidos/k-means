#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// numProcessos == numCentroides
//format : argv[0] file numPoints NumIteracoes xMin xMax yMin yMax

typedef struct Ponto
{
	float x;
	float y;
}Ponto;

int centroideMaisPerto(float x,float y,Ponto* centroides,int size)
{
	int i;
	int rankMenorDistancia;
	float menorDistancia;
	rankMenorDistancia=0;
	menorDistancia=sqrt(pow(x-centroides[0].x,2)+pow(y-centroides[0].y,2));
	for(i=1; i<size; i++)
	{
		float distancia=sqrt(pow(x-centroides[i].x,2)+pow(y-centroides[i].y,2));
		if( distancia < menorDistancia)
		{
			menorDistancia=distancia;
			rankMenorDistancia=i;
		}
	}
	return i;
}

Ponto atualizaCentroide(FILE* fp,Ponto* centroides,int size,int rank)
{
	Ponto aux;
	char buffer[100];
	aux.x=0;
	aux.y=0;
	
	int n=0;
	
	while(!feof(fp))
	{
		float xAux,yAux;
		fgets(buffer,1000,fp);
		sscanf(buffer,"%d %d",&xAux,&yAux);
		if(centroideMaisPerto(xAux,yAux,centroides,size)==rank)
		{
			aux.x+=xAux;
			aux.y+=yAux;
			n++;
		}
	}
	rewind(fp);
	aux.x/=n;
	aux.y/=n;
	return aux;
}



int main(int argc, char* argv[])
{
	int size;
	int rank;
	int numPoints;
	int numIteracoes;
	float xMin,xMax,yMin,yMax;
	
	xMin=atof(argv[4]);
	xMax=atof(argv[5]);
	yMin=atof(argv[6]);
	yMax=atof(argv[7]);
	
	FILE* fp;
	
	srand(time(NULL));
	
	MPI_init(argc,argv);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	numPoints=atoi(argv[2]);
	fp=fopen(argv[1],"r");
	numIteracoes=atoi(argv[3]);
	
	float xCentroide, yCentroide;
	xCentroide=(rank/size)*(xMax-xMin)+xMin;
	yCentroide=(rank/size)*(yMax-yMin)+yMin;
	
	Ponto* centroides = (Ponto*) calloc(sizeof(Ponto)*size);
	centroides[rank].x=xCentroide;
	centroides[rank].y=yCentroide;
	
	int i;
	MPI_Request requests[size];
	for(i=0; i<size; i++)
	{
		if(i==rank)
			continue;
		MPI_ISend(&(centroides[rank].x),1,MPI_FLOAT,i,42,MPI_COMM_WORLD,requests+i);
		MPI_ISend(&(centroides[rank].y),1,MPI_FLOAT,i,42,MPI_COMM_WORLD,requests+i);
	}
	
	
	for(i=0; i<size; i++)
	{
		if(i==rank)
			continue;
		MPI_Recv(&(centroides[i].x),1,MPI_FLOAT,i,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		MPI_Recv(&(centroides[i].y),1,MPI_FLOAT,i,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		
	}
	
	for(i=0; i<numIteracoes; i++)
	{
		int j;
		for(j=0; j<size; j++)
		{
			if(j==size) continue;
			MPI_Wait(requests+i,MPI_STATUS_IGNORE);
		}
		centroides[rank]=atualizaCentroide(fp,centroides,size,rank);
		
		for(j=0; j<size; j++)
		{
			if(j==rank)
				continue;
			MPI_ISend(&(centroides[rank].x),1,MPI_FLOAT,i,42,MPI_COMM_WORLD,requests+j);
			MPI_ISend(&(centroides[rank].y),1,MPI_FLOAT,i,42,MPI_COMM_WORLD,requests+j);
		}
		for(j=0; j<size; j++)
		{
			if(j==rank)
				continue;
			MPI_Recv(&(centroides[j].x),1,MPI_FLOAT,i,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			MPI_Recv(&(centroides[j].y),1,MPI_FLOAT,i,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		}
	}
	printf("o centroide de numero %d é (%f,%f)\n", rank,centroides[rank].x,centroides[rank].y);
	free(centroides);
	fclose(fp);
	MPI_Finalize();
	return 0;
	
	
}
