#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>



int *buffer1;
char **buffer2;
int tam_buffer;
int Nnumeros=20;


sem_t availableSpaceBuffer1;
sem_t availableDataBuffer1;
sem_t availableSpaceBuffer2;
sem_t availableDataBuffer2;

// Words in file counter
int numbersCounter = 0;
// Final number of file words
int numberOfnumbers;




//------------------------------------------------------------------------------
int esPrimo(int a){
   int b=0;
   int res=1;//Es primo
   int i=2;
   while(i<a && b==0){
         if(a%i==0){
            b=1;
            res=0;//No es primo
         }else i++;
   }
   return res;
}
//------------------------------------------------------------------------------

void *productor(void *arg) {
  // Declare variable which represents next position to fill in the
  // circular buffer1.
  int nextToFillBuffer1 = 0;
  // Productor loop
  while (1) {
      srand(time(NULL));
      int numero = rand()%100000;
      if (numbersCounter != Nnumeros) {
        sem_wait(&availableSpaceBuffer1);
        buffer1[nextToFillBuffer1]=numero;
        nextToFillBuffer1 = (nextToFillBuffer1 + 1) % tam_buffer;
        sem_post(&availableDataBuffer1);
      } else {
        break;
      }
      numbersCounter = numbersCounter + 1;
    }
    fflush(stdout);
    pthread_exit(NULL);
 }
//------------------------------------------------------------------------------

void *consumer(void *arg) {
  // Declare variable nextToEmptyBuffer1
  int nextToEmptyBuffer1 = 0;
  int consumerCounter = 0;
  // Declare variable nextToFillBuffer2
  int nextToFillBuffer2 = 0;
  char resultado[20];
  int numero;
  // Loop
  while (1) {
    // Wait: available data in buffer1
    sem_wait(&availableDataBuffer1);
    // Save read data
    numero=buffer1[nextToEmptyBuffer1];
    // Signal there is available space to write in buffer1
    // Palindromes function. Save result in global var
    if (esPrimo(numero)==1){
      sprintf(resultado, "%d es primo", numero);
    } else {
      sprintf(resultado, "%d no es primo", numero);
    }
    nextToEmptyBuffer1 = (nextToEmptyBuffer1 + 1) % tam_buffer;
    sem_post(&availableSpaceBuffer1);
    // Wait for available space in buffer2
    sem_wait(&availableSpaceBuffer2);
    // Save result in Buffer2
    strcpy(buffer2[nextToFillBuffer2], resultado);
    // Update nextToFillBuffer2 var
    nextToFillBuffer2 = (nextToFillBuffer2 + 1) % tam_buffer;
    sem_post(&availableDataBuffer2);
    consumerCounter = consumerCounter + 1;
    if (consumerCounter == Nnumeros) {
      break;
    }
  }
  fflush(stdout);
  pthread_exit(NULL);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

int main(){
   int i;

   tam_buffer= 10;

   // Create space for circular buffers
   buffer1 = (int*)malloc(tam_buffer *  sizeof (int));
   if (NULL==buffer1) {
     perror("Reserva de espacio para Buffer1 malo");
     return 0;
   }


   if ((buffer2 = (char**)malloc(tam_buffer* sizeof (char*))) == NULL) {
     perror("Reserva de espacio para Buffer1 malo");
     return 0;
   }





   printf("hola");
   return(0);
}
