#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>




int *buffer1;
char **buffer2;
int tam_buffer;
int Nnumeros=1000;

FILE *archivo;

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
  srand(time(NULL));
  while (1) {

      int numero = rand()%100000;
      if (numbersCounter != Nnumeros) {
        sem_wait(&availableSpaceBuffer1);
        //printf("nextToFillBuffer1 --> %d\n",nextToFillBuffer1 );
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
void *finalConsumer(void *arg) {
  int finalConsumerCounter = 0;
  // Declare nextToEmptyBuffer2. Value = 0.
  int nextToEmptyBuffer2 = 0;
  // Open file
  archivo= fopen("Results.txt", "w");
  fclose(archivo);
  // Loop
  while (1) {
    char str[6];
    archivo = fopen("Results.txt", "a");
    // Wait for available data in buffer2
    sem_wait(&availableDataBuffer2);
    // Save buffer1[nextToEmptyBuffer2] in data var
    strcpy(str, buffer2[nextToEmptyBuffer2]);
    // Signal: available space in buffer2
    sem_post(&availableSpaceBuffer2);
    // Update nextToEmptyBuffer2
    //printf("nextToEmptyBuffer2 --> %d\n",nextToEmptyBuffer2 );
    nextToEmptyBuffer2 = (nextToEmptyBuffer2 + 1) % tam_buffer;
    finalConsumerCounter = finalConsumerCounter + 1;
    // Add data var in the final text file
    fprintf(archivo, "%s\n", str);
    fclose(archivo);

    if (finalConsumerCounter == Nnumeros) {
      break;
    }
  }
  fflush(stdout);
  pthread_exit(NULL);
}




//------------------------------------------------------------------------------

int main(){
   int i;

   tam_buffer= 500;

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

   for (i=0; i<tam_buffer; i++){
      buffer2[i] = (int *)malloc (25*sizeof(int));
   }

   sem_init(&availableSpaceBuffer1, 0, tam_buffer);
   sem_init(&availableDataBuffer1, 0, 0);
   sem_init(&availableDataBuffer2, 0, 0);
   sem_init(&availableSpaceBuffer2, 0, tam_buffer);

   pthread_t productorThread;
   pthread_t consumerThread;
   pthread_t finalConsumerThread;

   pthread_create(&productorThread, NULL, productor, (void *) NULL);
   pthread_create(&consumerThread, NULL, consumer, (void *) NULL);
   pthread_create(&finalConsumerThread, NULL, finalConsumer, (void *) NULL);

   fflush(stdout);
   pthread_join(productorThread, NULL);
   pthread_join(consumerThread, NULL);
   pthread_join(finalConsumerThread, NULL);

   sem_destroy(&availableSpaceBuffer1);
   sem_destroy(&availableDataBuffer1);
   sem_destroy(&availableDataBuffer2);
   sem_destroy(&availableSpaceBuffer2);







   printf("hola");
   return(0);
}
