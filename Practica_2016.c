#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>



int **buffer1;
int **buffer2;


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
    while (1) {
      int numero = rand () % (0-99999+1) + 99999;
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
}
//------------------------------------------------------------------------------

void *consumer(void *arg) {
  // Declare variable nextToEmptyBuffer1
  int nextToEmptyBuffer1 = 0;
  int consumerCounter = 0;
  // Declare variable nextToFillBuffer2
  int nextToFillBuffer2 = 0;
  int resultado;
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
    buffer2[nextToFillBuffer2]=resultado;
    // Update nextToFillBuffer2 var
    nextToFillBuffer2 = (nextToFillBuffer2 + 1) % 5;
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

int main(){



   printf("hola");
   return(0);
}
