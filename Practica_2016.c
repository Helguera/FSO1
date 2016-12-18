#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <stdlib.h>

int *buffer1;
char **buffer2;
int tam_buffer1;
int tam_buffer2;
int Nnumeros;
int PalabrasDelArchivo = 0;


int sigPosicionParaVaciar_Buffer1 = 0;
int ContadorDelConsumidor = 0;
int sigPosicion_Buffer2 = 0;

sem_t espacioDisponibleBuffer1;     //Semaforos que tendra el programa
sem_t espacioDisponibleBuffer2;
sem_t DatosDisponiblesBuffer1;
sem_t DatosDisponiblesBuffer2;
pthread_mutex_t mutex;

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
  int sigPosicion_Buffer1 = 0;
  srand(time(NULL));
  // Bucle del productor
  while (1) {
      int numero = rand()%100000;
      if (PalabrasDelArchivo != Nnumeros) {
        sem_wait(&espacioDisponibleBuffer1);
        buffer1[sigPosicion_Buffer1]=numero;
        sigPosicion_Buffer1 = (sigPosicion_Buffer1 + 1) % tam_buffer1;
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

void *consumidor(void *arg) {       //Consumidor intermedio

  char resultado[20];
  int numero;

  while (1) {
    pthread_mutex_lock(&mutex);
    ContadorDelConsumidor = ContadorDelConsumidor + 1;
    if (ContadorDelConsumidor > Nnumeros) {
      pthread_mutex_unlock(&mutex);
      break;
    }
    pthread_mutex_unlock(&mutex);

    sem_wait(&DatosDisponiblesBuffer1); // Espera a que haya espacio disponible en el B.C. 1

    pthread_mutex_lock(&mutex);
    numero=buffer1[sigPosicionParaVaciar_Buffer1]; // Guarda el dato que ha leido
    sigPosicionParaVaciar_Buffer1 = ContadorDelConsumidor % tam_buffer1;
    pthread_mutex_unlock(&mutex);

    sem_post(&espacioDisponibleBuffer1); // Libera el semaforo

    if (esPrimo(numero)==1){
      sprintf(resultado, "%d es primo", numero);
    } else {
      sprintf(resultado, "%d no es primo", numero);
    }

    sem_wait(&espacioDisponibleBuffer2); // Espera a que haya espacio disponible en el B.C. 2

    pthread_mutex_lock(&mutex);
    strcpy(buffer2[sigPosicion_Buffer2], resultado); // Cuando hay espacio disponible lo guarda en el BC2
    sigPosicion_Buffer2 = (sigPosicion_Buffer2 + 1) % tam_buffer2; // Cambia la posicion del BC2
    pthread_mutex_unlock(&mutex);

    sem_post(&DatosDisponiblesBuffer2); // Libera el semaforo

  }
  fflush(stdout);
  pthread_exit(NULL);
}
//------------------------------------------------------------------------------
void *consumidorFinal(void *arg) {
  int finalContadorDelConsumidor = 0;
  int sigPosicionParaVaciar_Buffer2 = 0;
  // Abre el archivo
  archivo= fopen("Salida.txt", "w");
  fclose(archivo);
  // Bucle
  while (1) {
    char str[6];
    archivo = fopen("Salida.txt", "a");
    // Espera a que haya espacio en el buffer 2
    sem_wait(&DatosDisponiblesBuffer2);
    strcpy(str, buffer2[sigPosicionParaVaciar_Buffer2]);
    sem_post(&espacioDisponibleBuffer2);
    sigPosicionParaVaciar_Buffer2 = (sigPosicionParaVaciar_Buffer2 + 1) % tam_buffer2;
    finalContadorDelConsumidor = finalContadorDelConsumidor + 1;
    // Escribe el resultado al final del fichero
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

int main(int argc, char *argv[]){
   if (argc!=4){
      printf("\nTiene que introducir tres argumentos\n");
      return 0;
   }

   int i;
   Nnumeros = atoi(argv[1]);
   tam_buffer1 = atoi(argv[2]);
   tam_buffer2 = atoi(argv[3]);

   if (tam_buffer1>Nnumeros/2 || tam_buffer2>Nnumeros/2){// Elegimos un tamanio para el buffer
      printf("\nEl tamaño de los buffers no puede ser mayor que la mitad de los numeros\n");
      return 0;
   }

   if (tam_buffer1<1 || tam_buffer2<1 || Nnumeros<1){
      printf("\nEl tamaño de los buffers no puede ser negativo o 0\n" );
      return 0;
   }

   buffer1 = (int*)malloc(tam_buffer1 *  sizeof (int)); // Asignamos el tamanio del buffer1
   if (NULL==buffer1) {
     perror("Se ha reservado mal el espacio para el buffer1");
     return 0;
   }


   if ((buffer2 = (char**)malloc(tam_buffer2* sizeof (char*))) == NULL) {
     perror("Se ha reservado mal el espacio para el buffer2");
     return 0;
   }

   for (i=0; i<tam_buffer2; i++){
      buffer2[i] = (char *)malloc (25*sizeof(char));
   }

   pthread_mutex_init(&mutex,NULL);
   sem_init(&espacioDisponibleBuffer1, 0, tam_buffer1);  // Iniciamos los semaforos
   sem_init(&DatosDisponiblesBuffer1, 0, 0);
   sem_init(&DatosDisponiblesBuffer2, 0, 0);
   sem_init(&espacioDisponibleBuffer2, 0, tam_buffer2);

   pthread_t Hilo_Productor;
   pthread_t Hilo_Consumidor1;
   pthread_t Hilo_Consumidor2;
   pthread_t Hilo_Consumidor3;
   pthread_t Hilo_Consumidor4;
   pthread_t Hilo_consumidorFinal;

   pthread_create(&Hilo_Productor, NULL, productor, (void *) NULL);
   pthread_create(&Hilo_Consumidor1, NULL, consumidor, (void *) NULL);
   pthread_create(&Hilo_Consumidor2, NULL, consumidor, (void *) NULL);
   pthread_create(&Hilo_Consumidor3, NULL, consumidor, (void *) NULL);
   pthread_create(&Hilo_Consumidor4, NULL, consumidor, (void *) NULL);
   pthread_create(&Hilo_consumidorFinal, NULL, consumidorFinal, (void *) NULL);

   fflush(stdout);
   pthread_join(Hilo_Productor, NULL);
   pthread_join(Hilo_Consumidor1, NULL);
   pthread_join(Hilo_Consumidor2, NULL);
   pthread_join(Hilo_Consumidor3, NULL);
   pthread_join(Hilo_Consumidor4, NULL);
   pthread_join(Hilo_consumidorFinal, NULL);

   sem_destroy(&espacioDisponibleBuffer1);  // Destruimos los semaforos
   sem_destroy(&DatosDisponiblesBuffer1);
   sem_destroy(&DatosDisponiblesBuffer2);
   sem_destroy(&espacioDisponibleBuffer2);


   printf("\nEl programa ha finalizado\n"); // El programa acaba
   return(0);

}
