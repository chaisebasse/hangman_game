#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <curses.h>

typedef struct score {
    char* medal;
    int rating;
} score;

char* scanDynamicString();
void printGuess(char* guess, int size);
bool isLetterInWord(char letter, char* word, char* guess, int size);
bool isWordGuessed(char* guess, int size);
void displayHangman(int attempts);
int selectDifficulty();
score calculateScore(double elapsedTime, int numFails, int difficulty, int numAttempts);

int main() {
  char *word = scanDynamicString();
  int size = strlen(word);
  char guess[size];
  char letter;
  const int difficulty = selectDifficulty(); // Actual difficulty value
  int numAttempts = 0;
  int numFails = 0; // Track the number of wrong guesses
  time_t startTime, endTime; // Variables to track time
  double elapsedTime;

  memset(guess, '_', size);
  printGuess(guess, size);

  startTime = time(NULL);

  while (!isWordGuessed(guess, size) && difficulty > 0) {
    printf("Entrer une lettre :\t");
    scanf(" %c", &letter);
    numAttempts++;

    if (!isLetterInWord(letter, word, guess, size)) {
      printf("Cette lettre n'appartient pas au mot\n");
      numFails++; // Increment numFails when a wrong letter is guessed
      printf("tentatives restantes : %i\n", difficulty - numFails);
    }

    displayHangman(difficulty);
    printGuess(guess, size);
  }

  endTime = time(NULL);
  elapsedTime = difftime(endTime, startTime);

  score playerScore = calculateScore(elapsedTime, numFails, difficulty, numAttempts);

  if (isWordGuessed(guess, size)) {
    printf("\nBravo, tu as trouvé le mot en %i tentatives\n", numAttempts);
    printf("%d%% jusqu'à la médaille de %s", playerScore.rating, playerScore.medal);
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

// Use termios or noecho() ?
char* scanDynamicString() {
  printf("What word do you want to guess?\n");

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

void displayHangman(int fails) {
  // For each number of wrong attempts there is a specific version of the hangman
  const char *hangmanStages[] = {
    "=======\n",
    "   |\n   |\n   |\n   |\n   |\n=======\n",
    "   _____\n   |\n   |\n   |\n   |\n   |\n=======\n",
    "   _____\n   |/  \n   |\n   |\n   |\n   |\n=======\n",
    "   _____\n   |/  |\n   |\n   |\n   |\n   |\n=======\n",
    "   _____\n   |/  |\n   |   O\n   |\n   |\n   |\n=======\n",
    "   _____\n   |/  |\n   |   O\n   |   |\n   |\n   |\n=======\n",
    "   _____\n   |/  |\n   |   O\n   |  /|\n   |\n   |\n=======\n",
    "   _____\n   |/  |\n   |   O\n   |  /|\\\n   |\n   |\n=======\n",
    "   _____\n   |/  |\n   |   O\n   |  /|\\\n   |  /\n   |\n=======\n",
    "   _____\n   |/  |\n   |   O\n   |  /|\\\n   |  / \\\n   |\n=======\n"
  };

  printf("%s", hangmanStages[10 - fails]);
}

int selectDifficulty() {
  int selected = 0;

  // Initialize curses
  initscr();
  raw();
  keypad(stdscr, TRUE);
  noecho();

  // Difficulty options
  printw("Difficulty:\n");
  printw("  Easy\n");
  printw("  Normal\n");
  printw("  Hard\n");

  move(selected + 1, 4);

  int ch;
  while (1) {
    ch = getch();

    // UP
    if (ch == KEY_UP) {
      selected--;
      if (selected < 0) {
        selected = 2;
      }
      move(selected + 1, 4);
    }

    // Down
    if (ch == KEY_DOWN) {
      selected++;
      if (selected > 2) {
        selected = 0;
      }
      move(selected + 1, 4);
    }

    // Enter
    if (ch == 10) {
      break;
    }

  }

  int difficulty;
  // Set difficulty based on selection
  switch (selected) {
    case 0:
      difficulty = 10; // Easy
      break;
    case 1:
      difficulty = 6; // Normal
      break;
    case 2:
      difficulty = 3; // Hard
      break;
  }

  endwin();
  echo();

  // Print selected difficulty
  printf("Number of attempts allowed: %d\n", difficulty);

  return difficulty;
}

score calculateScore(double elapsedTime, int numFails, int difficulty, int numAttempts) {
  score myScore;
  myScore.rating = 100;

  // Determine medal according to time spent
  if (elapsedTime <= 20) {
    myScore.medal = "Or";
  } else if (elapsedTime <= 50) {
    myScore.medal = "Argent";
  } else {
    myScore.medal = "Bronze";
  }

  if (numFails > 0) {
    double deduction_per_fail = 100.0 / (difficulty); // Calculate the deduction per fail based on the difficulty
    myScore.rating -= deduction_per_fail * numFails; // Deduct from the rating based on the number of fails and the deduction per fail
  }

  return myScore;
}
