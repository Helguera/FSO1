#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>

int **buffer1;
int **buffer2;
int tamBuffer1;

sem_t availableSpaceBuffer1;
sem_t availableDataBuffer1;
sem_t availableSpaceBuffer2;
sem_t availableDataBuffer2;

// Words in file counter
int wordsCounter = 0;
// Final number of file words
int numberOfWords;


//------------------------------------------------------------------------------

void *productor(void *arg) {
  // Declare variable which represents next position to fill in the
  // circular buffer1.
  int nextToFillBuffer1 = 0;
  // Productor loop
  while (1) {

    int numeros=0;

    while (1) {
      numero = rand () % (0-99999+1) + 99999;
      //endOfFile = fscanf(firstFile, "%s", str);
      if (numeros != Nnumeros) {
        sem_wait(&availableSpaceBuffer1);
        strcpy(buffer1[nextToFillBuffer1], numero);
        nextToFillBuffer1 = (nextToFillBuffer1 + 1) % tamBuffer1;
        sem_post(&availableDataBuffer1);
        numeros++;
      } else {
        break;
      }
      numeros = numeros + 1;
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
  char result[15];
  char word[11];
  // Loop
  while (1) {
    // Wait: available data in buffer1
    sem_wait(&availableDataBuffer1);
    // Save read data
    strcpy(word, buffer1[nextToEmptyBuffer1]);
    // Signal there is available space to write in buffer1
    // Palindromes function. Save result in global var
    if (palindromo(word)){
      sprintf(result, "%s es palindromo", word);
    } else {
      sprintf(result, "%s no es palindromo", word);
    }
    nextToEmptyBuffer1 = (nextToEmptyBuffer1 + 1) % tamBuffer1;
    sem_post(&availableSpaceBuffer1);
    // Wait for available space in buffer2
    sem_wait(&availableSpaceBuffer2);
    // Save result in Buffer2
    strcpy(buffer2[nextToFillBuffer2], result);
    // Update nextToFillBuffer2 var
    nextToFillBuffer2 = (nextToFillBuffer2 + 1) % 5;
    sem_post(&availableDataBuffer2);
    consumerCounter = consumerCounter + 1;
    if (consumerCounter == numberOfWords) {
      break;
    }
  }
  fflush(stdout);
  pthread_exit(NULL);
}



int main(){
   printf("hola");
   return(0);
}
