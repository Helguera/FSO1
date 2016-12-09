#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>



int * buffer1;
char **buffer2;
int tamBuffer1;

sem_t availableSpaceBuffer1;
sem_t availableDataBuffer1;
sem_t availableSpaceBuffer2;
sem_t availableDataBuffer2;

// Words in file counter
int numbersCounter = 0;
// Final number of file words
int numberOfnumbers;

int main(){
   int i;

   tamBuffer1 = 10;

   // Create space for circular buffers
   buffer1 = (int*)malloc(tamBuffer1 *  sizeof (int));
   if (NULL==buffer1) {
     perror("Reserva de espacio para Buffer1 malo");
     return 0;
   }


   if ((buffer2 = (char**)malloc(tamBuffer1 * sizeof (char*))) == NULL) {
     perror("Reserva de espacio para Buffer1 malo");
     return 0;
   }

   char resultado[20];
   int numero=345;
   sprintf(resultado, "%d es primo", numero);
   printf("%s",resultado);

   srand(time(NULL));
   numero = rand()%100000;
   buffer1[0]=numero;
   printf("%d hola",buffer1[0]);




   printf("hola, funciona");
   return(0);
}
