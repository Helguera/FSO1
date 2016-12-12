#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <stdlib.h>

int *buffer1;
char **buffer2;
int tam_buffer;
int Nnumeros=1000;
int PalabrasDelArchivo = 0;
int PalabrasFinales;

sem_t espacioDisponibleBuffer1;
sem_t espacioDisponibleBuffer2;
sem_t DatosDisponiblesBuffer1;
sem_t DatosDisponiblesBuffer2;

FILE *archivo;

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
  int sigPosicion_Buffer1 = 0;
  // Productor loop
  srand(time(NULL));
  while (1) {
      int numero = rand()%100000;
      if (PalabrasDelArchivo != Nnumeros) {
        sem_wait(&espacioDisponibleBuffer1);
        //printf("sigPosicion_Buffer1 --> %d\n",sigPosicion_Buffer1 );
        buffer1[sigPosicion_Buffer1]=numero;
        sigPosicion_Buffer1 = (sigPosicion_Buffer1 + 1) % tam_buffer;
        sem_post(&DatosDisponiblesBuffer1);
      } else {
        break;
      }
      PalabrasDelArchivo = PalabrasDelArchivo + 1;
    }
    fflush(stdout);
    pthread_exit(NULL);
 }
//------------------------------------------------------------------------------

void *consumidor(void *arg) {

  int sigPosicionParaVaciar_Buffer1 = 0;
  int ContadorDelConsumidor = 0;
  int sigPosicion_Buffer2 = 0;
  char resultado[20];
  int numero;
  
  while (1) {

    sem_wait(&DatosDisponiblesBuffer1); // Espera a que haya espacio disponible en el B.C. 1

    numero=buffer1[sigPosicionParaVaciar_Buffer1]; // Guarda el dato que ha leido
    // Signal there is available space to write in buffer1
    // Palindromes function. Save result in global var
    if (esPrimo(numero)==1){
      sprintf(resultado, "%d es primo.\n", numero);
    } else {
      sprintf(resultado, "%d no es primo.\n", numero);
    }
    sigPosicionParaVaciar_Buffer1 = (sigPosicionParaVaciar_Buffer1 + 1) % tam_buffer;
    sem_post(&espacioDisponibleBuffer1); // Libera el semaforo

    sem_wait(&espacioDisponibleBuffer2); // Espera a que haya espacio disponible en el B.C. 2

    strcpy(buffer2[sigPosicion_Buffer2], resultado); // Cuando hay espacio disponible lo guarda en el BC2

    sigPosicion_Buffer2 = (sigPosicion_Buffer2 + 1) % tam_buffer; // Cambia la posicion del BC2
    sem_post(&DatosDisponiblesBuffer2); // Libera el semaforo
    ContadorDelConsumidor = ContadorDelConsumidor + 1;
    if (ContadorDelConsumidor == Nnumeros) {
      break; 
    }
  }
  fflush(stdout);
  pthread_exit(NULL);
}
//------------------------------------------------------------------------------
void *consumidorFinal(void *arg) {
  int finalContadorDelConsumidor = 0;
  // Declare sigPosicionParaVaciar_Buffer1. Value = 0.
  int sigPosicionParaVaciar_Buffer1 = 0;
  // Open file
  archivo= fopen("Salida.txt", "w");
  fclose(archivo);
  // Loop
  while (1) {
    char str[6];
    archivo = fopen("Salida.txt", "a");
    // Wait for available data in buffer2
    sem_wait(&DatosDisponiblesBuffer2);
    // Save buffer1[sigPosicionParaVaciar_Buffer1] in data var
    strcpy(str, buffer2[sigPosicionParaVaciar_Buffer1]);
    // Signal: available space in buffer2
    sem_post(&espacioDisponibleBuffer2);
    // Update sigPosicionParaVaciar_Buffer1
    //printf("sigPosicionParaVaciar_Buffer1 --> %d\n",sigPosicionParaVaciar_Buffer1 );
    sigPosicionParaVaciar_Buffer1 = (sigPosicionParaVaciar_Buffer1 + 1) % tam_buffer;
    finalContadorDelConsumidor = finalContadorDelConsumidor + 1;
    // Add data var in the final text file
    fprintf(archivo, "%s\n", str);
    fclose(archivo);

    if (finalContadorDelConsumidor == Nnumeros) {
      break;
    }
  }
  fflush(stdout);
  pthread_exit(NULL);
}


//------------------------------------------------------------------------------

int main(){
   int i;

   tam_buffer= 500; // Elegimos un tamanio para el buffer

   buffer1 = (int*)malloc(tam_buffer *  sizeof (int)); // Asignamos el tamanio del buffer1
   if (NULL==buffer1) {
     perror("Se ha reservado mal el espacio para el buffer1");
     return 0;
   }


   if ((buffer2 = (char**)malloc(tam_buffer* sizeof (char*))) == NULL) {
     perror("Se ha reservado mal el espacio para el buffer1");
     return 0;
   }

   for (i=0; i<tam_buffer; i++){
      buffer2[i] = (int *)malloc (25*sizeof(int));
   }

   sem_init(&espacioDisponibleBuffer1, 0, tam_buffer);  // Iniciamos los semaforos
   sem_init(&DatosDisponiblesBuffer1, 0, 0);
   sem_init(&DatosDisponiblesBuffer2, 0, 0);
   sem_init(&espacioDisponibleBuffer2, 0, tam_buffer);

   pthread_t Hilo_Productor;
   pthread_t Hilo_Consumidor;
   pthread_t Hilo_consumidorFinal;

   pthread_create(&Hilo_Productor, NULL, productor, (void *) NULL);
   pthread_create(&Hilo_Consumidor, NULL, consumidor, (void *) NULL);
   pthread_create(&Hilo_consumidorFinal, NULL, consumidorFinal, (void *) NULL);

   fflush(stdout);
   pthread_join(Hilo_Productor, NULL);  // Cambiamos la referencia de los hilos a nada
   pthread_join(Hilo_Consumidor, NULL);
   pthread_join(Hilo_consumidorFinal, NULL);

   sem_destroy(&espacioDisponibleBuffer1);  // Destruimos los semaforos
   sem_destroy(&DatosDisponiblesBuffer1);
   sem_destroy(&DatosDisponiblesBuffer2);
   sem_destroy(&espacioDisponibleBuffer2);


   printf("hola"); // EL programa acaba
   return(0);
}
