/*****************************************/
/* pthread barrier for synchronization   */
/* merge sort using multi thread         */
/* Author: Chengjun Yuan @UVa            */
/* Time:   Oct.15.2015                   */
/*****************************************/
#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#define numThreads 2048
int array[numThreads*2];

typedef struct
{
	int left;
	int right;
}parameters;
parameters *para[numThreads];
int arraySize=0;
pthread_barrier_t barr;				// barrier variable
pthread_mutex_t lock;				// define a mutual exclusion lock for threads.

// read array data from indata.txt .
void readArray(char *fileName){
	FILE *fp;
	int i=0;
	fp=fopen(fileName,"r");
	if(!fp){
		printf("ERROR: Cannot read file %s \n",fileName);
		exit;
	}
	fscanf(fp,"%d",&array[i]); i++;
	while(!feof(fp)){
		fscanf(fp," %d",&array[i]); i++;
	}
	fclose(fp);
	arraySize=i;
	printf("arraySize = %d \n",arraySize);
}

bool checkArraySize(void){
	int n=arraySize;
	int i=0;
	while(n!=1){
		i++;
		n=n>>1;
	}
	n=arraySize;
	if(((n>>i)<<i)==n){
		printf("valid arraySize \n");
		return true;
	}
	else{
		printf("Error: invalid arraySize \n");
		return false;
	}
}

void *mergeSort(void *para){
	int left,right,mid,i,j,k;
	int *leftArray,*rightArray;
	
	parameters *para_=para;
	left=para_->left;
	right=para_->right;
	mid=(left+right)/2;
	leftArray=(int *)calloc(sizeof(int),right-left+1);
	rightArray=(int *)calloc(sizeof(int),right-left+1);
	//printf("inthread: %d %d %d \n",left,right,mid);
	for(i=left;i<=mid;i++){
		leftArray[i-left]=array[i];
	}
	for(j=mid+1;j<=right;j++){
		rightArray[j-mid-1]=array[j];
	}
	i=left; j=mid+1; k=left;
	while(i<=mid&&j<=right){
		if(leftArray[i-left]<=rightArray[j-mid-1]){
			array[k]=leftArray[i-left];
			i++; k++;
		}else{
			array[k]=rightArray[j-mid-1];
			j++; k++;
		}
	}
	while(i<=mid){
		array[k]=leftArray[i-left];
		i++; k++;
	}
	while(j<=right){
		array[k]=rightArray[j-mid-1];
		j++; k++;
	}
	free(leftArray);
	free(rightArray);
	//for(i=left;i<=right;i++)printf("%d ",array[i]);
	//printf("\n****\n");
	//pthread_mutex_lock(&lock);
	pthread_barrier_wait(&barr);
	//pthread_mutex_unlock(&lock);
	//pthread_exit(NULL);
}

int main(int argc,char *argv[]){
	int i,j,k;
	pthread_t tid[numThreads];			// to store the id of each threads.
	pthread_mutex_init(&lock,NULL);		// initiate the mutual exclusion lock.
	readArray("indata.txt");
	if(checkArraySize()){
		i=arraySize/2;					// number of threads in this merge layer
		while(1){
			j=arraySize/i;				// number of elements to be sorted in single thread
			printf("layer %d %d \n",i,j);
			if(pthread_barrier_init(&barr,NULL,i+1)) // initiate barrier with a count of i+1;
			{
				printf("Could not create a barrier\n");
				return -1;
			}
			for(k=0;k<i;k++){
				para[k]=(parameters*)malloc(sizeof(parameters));
				para[k]->left=k*j;
				para[k]->right=(k+1)*j-1;
				if(pthread_create(&tid[k],NULL,mergeSort,para[k])){
					printf("Could not create thread\n");
					return -1;
				}
			}
			pthread_barrier_wait(&barr);// wait for all threads being finished.
			pthread_barrier_destroy(&barr);
			for(k=0;k<i;k++)free(para[k]);
			if(i==1)break;				// merge sort has been finished if i==1
			else i=i/2;					// merge sort for next layer
			
		}
	}
	//printf("sorted array: ");
	for(i=0;i<arraySize;i++)printf("%d ",array[i]); 
	printf("\n");
	pthread_mutex_destroy(&lock);		// destroy the mutual exclusion lock.
}