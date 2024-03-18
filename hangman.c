#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdbool.h>

char* scanDynamicString();
void printGuess(char* guess, int size);
bool isLetterInWord(char letter, char* word, char* guess, int size);
bool isWordGuessed(char* guess, int size);
void displayHangman(int attempts);

int main() {
  char *word = scanDynamicString();
  int size = strlen(word);
  char guess[size];
  char letter;
  int numAttempts = 0;

  memset(guess, '_', size);
  printGuess(guess, size);

  while (!isWordGuessed(guess, size) && numAttempts < 6) {
    printf("Entrer une lettre :\t");
    scanf(" %c", &letter);

    if (!isLetterInWord(letter, word, guess, size)) {
      printf("Cette lettre n'appartient pas au mot\n");
      numAttempts++;
      printf("Tentatives restantes : %i\n", 6-numAttempts);
    }

    displayHangman(6-numAttempts);
    printGuess(guess, size);
  }

  if (isWordGuessed(guess, size)) {
    printf("\nBravo, tu as trouvé le mot\n");
  } else {
    printf("\nTu as été pendu\n");
    printf("Le mot était : %s\n", word);
  }

  free(word);
}

void printGuess(char* guess, int size) {
  for (int i = 0; i < size; i++) {
    printf("%c", guess[i]);
  }
  printf("\n");
}

bool isLetterInWord(char letter, char* word, char* guess, int size) {
  bool found_char = false;
  for (int i = 0; i < size; i++) {
    if (word[i] == letter && guess[i] != letter) {
      guess[i] = letter;
      found_char = true;
    }
  }
  return found_char;
}

bool isWordGuessed(char* guess, int size) {
  for (int i = 0; i < size; i++) {
    if (guess[i] == '_') {
      return false;
    }
  }
  return true;
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

// void displayHangman(int attempts) {
//   if (attempts == 5) {
//     printf("O\n");
//   } else if (attempts == 4) {
//     printf("O\n|\n");
//   } else if (attempts == 3) {
//     printf(" O\n-|\n");
//   } else if (attempts == 2) {
//     printf(" O\n-|-\n");
//   } else if (attempts == 1) {
//     printf(" O\n-|-\n/\n");
//   } else if (attempts == 0) {
//     printf(" O\n-|-\n/\\\n");
//   }
// }

void displayHangman(int attempts) {
  const char *hangmanStages[] = {
    "      _____\n      |/    |\n      |\n      |\n      |\n      |\n=======\n",
    "      _____\n      |/    |\n      |     O\n      |\n      |\n      |\n=======\n",
    "      _____\n      |/    |\n      |     O\n      |     /|\\\n      |\n      |\n=======\n",
    "      _____\n      |/    |\n      |     O\n      |     /|\\\n      |     / \n      |\n=======\n",
    "      _____\n      |/    |\n      |     O\n      |     /|\\\n      |     / \\\n      |\n=======\n",
    "      _____\n      |/    |\n      |     O\n      |     /|\\\n      |     / \\\n      |      \n      |\n=======\n"
  };


  printf("%s", hangmanStages[6 - attempts]);
}
