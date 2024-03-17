#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

char* scanDynamicString();

int main() {
  char *word = scanDynamicString();

  size_t size = strlen(word);

  char guess[size];
  char letter;
  int found_word = 0, found_char, numAttempts = 0;
  for (int i = 0; i < size; i++) {
    guess[i] = '_';
    printf("%c", guess[i]);
  }
  printf("\n");

  do
  {
    printf("Entrer une lettre :\t");
    scanf(" %c", &letter);
    found_char = 0;

    for (int i = 0; i < size; i++) {
      if (word[i] == letter && guess[i] != letter) {
        guess[i] = letter;
        found_char = 1;
      }
    }

    if (found_char == 0) {
      printf("Cette lettre n'appartient pas au mot\n");
      numAttempts++;
      printf("Tentatives restantes : %i\n", 6-numAttempts);
    }

    found_word = 1;
    for (int j = 0; j < size; j++) {
      if (guess[j] == '_') {
        found_word = 0;
      }
      printf("%c", guess[j]);
    }
    printf("\n");
  } while (found_word != 1 && numAttempts < 6);

  if (found_word == 1) {
    printf("\nBravo, tu as trouvé le mot\n");
  } else {
    printf("\nTu as été pendu\n");
    printf("Le mot était : %s\n", word);
  }

  free(word);
}

char* scanDynamicString() {
  printf("Quel mot voulez-vous faire deviner ?\n");

  struct termios old_termios, new_termios;
  char* word = NULL;
  int capacity = 0; // Initial capacity of the word
  int size = 0;     // Current size of the word

  tcgetattr(STDIN_FILENO, &old_termios);
  new_termios = old_termios;
  new_termios.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL | ICANON);
  tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);

  while(1) {
    char c = getc(stdin);

    // Check if last character
    if (c == EOF || c == '\n') {
      if (word != NULL) {
        word[size] = '\0';
      }

      break;
    }

    // Reallocate memory to add c
    if (size >= capacity) {
      capacity = (capacity == 0) ? 1 : capacity * 2;
      char *temp = realloc(word, capacity * sizeof(char));

      if (temp == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        free(word);

        return NULL;
      }

      word = temp;
    }

    // Increment size of word to add c
    word[size++] = c;
  }

  tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);

  return word;
}
