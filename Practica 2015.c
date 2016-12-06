#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>

// Shared resources - All shared resources as a communication way between
// different threads.
char **buffer1;
char **buffer2;
int tamBuffer1;

FILE *firstFile;
FILE *secondFile;
char *readFileName;

sem_t availableSpaceBuffer1;
sem_t availableDataBuffer1;
sem_t availableSpaceBuffer2;
sem_t availableDataBuffer2;

// Words in file counter
int wordsCounter = 0;
// Final number of file words
int numberOfWords;

// Palindrome function
int palindromo (char *palabra) {

 int esPalindromo = 1; // significa que sí lo es
 int i,j;

 j = strlen(palabra)-1;
 for (i = 0; i < strlen (palabra) / 2 && esPalindromo; i++, j--) {
   if (*(palabra + i) != * (palabra + j)) {
     esPalindromo = 0; // No es palíndromo
   }
 }

 return esPalindromo;
}

// Productor thread - Reads words (Strings) from a text file and save them
// in a circular buffer (buffer1).
void *productor(void *arg) {
  // Declare variable which represents next position to fill in the
  // circular buffer1.
  int nextToFillBuffer1 = 0;
  // Productor loop
  while (1) {

    char str[11];
    int endOfFile;
    firstFile = fopen(readFileName, "r");
    // Read the file and save words in Buffer1
    while (1) {
      endOfFile = fscanf(firstFile, "%s", str);
      if (endOfFile != EOF) {
        sem_wait(&availableSpaceBuffer1);
        strcpy(buffer1[nextToFillBuffer1], str);
        nextToFillBuffer1 = (nextToFillBuffer1 + 1) % tamBuffer1;
        sem_post(&availableDataBuffer1);
      } else {
        break;
      }
      wordsCounter = wordsCounter + 1;
    }
    numberOfWords = wordsCounter;
    fclose(firstFile);
    fflush(stdout);
    pthread_exit(NULL);
  }
}

// Consumer thread - Imports words saved in buffer1 and check if they are
// palindromes or not. Then, it saves the answer in buffer2. Answer options:
// 1. "pal si" / "pal no".
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

// Final consumer thread - imports strings from buffer2 and save them in
// another text file.
void *finalConsumer(void *arg) {
  int finalConsumerCounter = 0;
  // Declare nextToEmptyBuffer2. Value = 0.
  int nextToEmptyBuffer2 = 0;
  // Open file
  secondFile = fopen("Results.txt", "w");
  fclose(secondFile);
  // Loop
  while (1) {
    char str[6];
    secondFile = fopen("Results.txt", "a");
    // Wait for available data in buffer2
    sem_wait(&availableDataBuffer2);
    // Save buffer1[nextToEmptyBuffer2] in data var
    strcpy(str, buffer2[nextToEmptyBuffer2]);
    // Signal: available space in buffer2
    sem_post(&availableSpaceBuffer2);
    // Update nextToEmptyBuffer2
    nextToEmptyBuffer2 = (nextToEmptyBuffer2 + 1) % 5;
    finalConsumerCounter = finalConsumerCounter + 1;
    // Add data var in the final text file
    fprintf(secondFile, "%s\n", str);
    fclose(secondFile);

    if (finalConsumerCounter == numberOfWords) {
      break;
    }
  }
  fflush(stdout);
  pthread_exit(NULL);
}

// Main program
int main(int argc, char *argv[]) {
  if (argc == 3) {

    tamBuffer1 = atoi(argv[1]);

    // Create space for circular buffers

    if ((buffer1 = (char**)malloc(tamBuffer1 *  sizeof (char*))) == NULL) {
      perror("Reserva de espacio para Buffer1 malo");
      return 0;
    }
    int i=0;
    for (i = 0; i < tamBuffer1; i++) {
      if((buffer1[i] = (char*)malloc(11*sizeof(char)))== NULL) {
        perror("Reserva de espacio para Buffer1 malo");
        return 0;
      }
    }

    if ((buffer2 = (char**)malloc(5 * sizeof (char*))) == NULL) {
      perror("Reserva de espacio para Buffer1 malo");
      return 0;
    }
    
    for (i = 0; i < 5; i++) {
      if((buffer2[i] = (char*)malloc(20*sizeof(char)))== NULL) {
        perror("Reserva de espacio para Buffer1 malo");
        return 0;
      }
    }

    readFileName = argv[2];

    sem_init(&availableSpaceBuffer1, 0, tamBuffer1);
    sem_init(&availableDataBuffer1, 0, 0);
    sem_init(&availableDataBuffer2, 0, 0);
    sem_init(&availableSpaceBuffer2, 0, 5);

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

  } else {
    printf("Plase, introduce the buffer size and the text file as program arguments in the command line.\n");
  }

  return 0;
}
