#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <float.h>

// numProcessos == numCentroides
//format : argv[0] file NumIteracoes

typedef struct Ponto
{
	float x;
	float y;
}Ponto;

void descreverOArquivo(FILE* fp, float *xMin, float *xMax, float *yMin, float *yMax, int *numPoints){
	*numPoints=0;
	*xMax=0;
	*yMax=0;
	*xMin=FLT_MAX;
	*yMin=FLT_MAX;
	char buffer[1000];
	int n=0;
	while(!feof(fp))
	{	
		float xAux,yAux;
		fgets(buffer,1000,fp);
		sscanf(buffer,"%f %f",&xAux,&yAux);
		if(xAux<*xMin)
			*xMin=xAux;
		if(yAux<*yMin)
			*yMin=yAux;
		if(xAux>*xMax)
			*xMax=xAux;
		if(yAux>*yMax)
			*yMax=yAux;
		n++;
	}
	*numPoints=n;
}

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
	return rankMenorDistancia;
}

Ponto atualizaCentroide(FILE* fp,Ponto* centroides,int size,int rank)
{
	Ponto aux;
	char buffer[1000];
	aux.x=0;
	aux.y=0;
	
	int n=0;
	
	while(!feof(fp))
	{
		float xAux,yAux;
		fgets(buffer,1000,fp);
		sscanf(buffer,"%f %f",&xAux,&yAux);
		int centroide = centroideMaisPerto(xAux,yAux,centroides,size);
		printf("%d centroide mais proximo %d\n", rank,centroide);
		if(centroide==rank)
		{
			aux.x+=xAux;
			aux.y+=yAux;
			n++;
		}
	}
	printf("Esse é o valor de n : %d\n", n);
	rewind(fp);
	if(n)
	{
		aux.x/=n;
		aux.y/=n;
	}
	else
	{
		aux.x=centroides[rank].x;
		aux.y=centroides[rank].y;
	}
	return aux;
}


// argv[0] file numIteracoes
int main(int argc, char* argv[])
{
	int size;
	int rank;
	int numPoints;
	int numIteracoes;
	float xMin,xMax,yMin,yMax;
	
	//xMin=atof(argv[4]);
	//xMax=atof(argv[5]);
	//yMin=atof(argv[6]);
	//yMax=atof(argv[7]);
	
	FILE* fp;
	
	srand(time(NULL));
	
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	
	//numPoints=atoi(argv[2]);
	fp=fopen(argv[1],"r");
	numIteracoes=atoi(argv[2]);
	
	descreverOArquivo(fp,&xMin,&xMax,&yMin,&yMax,&numPoints);

	float xCentroide, yCentroide;
	xCentroide=(((xMax-xMin)/(size+1))*(rank+1))+xMin;
	yCentroide=(((yMax-yMin)/(size+1))*(rank+1))+yMin;
	
	Ponto* centroides = (Ponto*) calloc(sizeof(Ponto)*size,0);
	centroides[rank].x=xCentroide;
	centroides[rank].y=yCentroide;
	
	int i;
	MPI_Request requests[size];
	for(i=0; i<size; i++)
	{
		if(i==rank)
			continue;
		MPI_Send(&(centroides[rank].x),1,MPI_FLOAT,i,42,MPI_COMM_WORLD);//,requests+i);
		MPI_Send(&(centroides[rank].y),1,MPI_FLOAT,i,42,MPI_COMM_WORLD);//,requests+i);
	}
	
	for(i=0; i<size; i++)
	{
		if(i==rank)
			continue;
		MPI_Recv(&(centroides[i].x),1,MPI_FLOAT,i,42,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		MPI_Recv(&(centroides[i].y),1,MPI_FLOAT,i,42,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		
	}
	for(i=0; i<numIteracoes; i++)
	{
		int j;
		centroides[rank]=atualizaCentroide(fp,centroides,size,rank);
		for(j=0; j<size; j++)
		{
			if(j==rank)
				continue;
			MPI_Send(&(centroides[rank].x),1,MPI_FLOAT,j,42,MPI_COMM_WORLD);
			MPI_Send(&(centroides[rank].y),1,MPI_FLOAT,j,42,MPI_COMM_WORLD);
		}
		for(j=0; j<size; j++)
		{
			if(j==rank)
				continue;
			MPI_Recv(&(centroides[j].x),1,MPI_FLOAT,j,42,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			MPI_Recv(&(centroides[j].y),1,MPI_FLOAT,j,42,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		}
	}
	printf("o centroide de numero %d é (%f,%f)\n", rank,centroides[rank].x,centroides[rank].y);
	free(centroides);
	fclose(fp);
	MPI_Finalize();
	return 0;
	
	
}
