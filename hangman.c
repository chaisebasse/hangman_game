#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  char *word = malloc(sizeof(char));
  if (word == NULL) {
    printf("Memory allocation failed.\n");
    return EXIT_FAILURE;
  }
  printf("Quel mot voulez-vous faire deviner ?\n");
  scanf("%s", word);

  size_t size = strlen(word);

  char guess[size];
  char letter;
  int found_word = 0, found_char, numAttempts = 0;
  for (int i = 0; i < size; i++) {
    guess[i] = '_';
  }

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
  } while (found_word != 1 && numAttempts < 6);

  if (found_word == 1) {
    printf("\nBravo, tu as trouvé le mot\n");
  } else {
    printf("\nTu as été pendu\n");
    printf("Le mot était : %s\n", word);
  }

  free(word);
}
