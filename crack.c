/*
 * crack.c
 *   Multithreaded brute-force password hash cracker.
 *   usage: crack threads keysize target
 *
 *   Nathan Bossart
 *   Oct 1, 2013
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <crypt.h>

char* getNextString(char* curr_string, int* curr_iteration, int* max_iteration, int keysize) {
  if ((*curr_iteration)++==*max_iteration) {
    return NULL;
  } else {
    int i = keysize-1;
    ++curr_string[i];
    while (curr_string[i]=='{') {
      curr_string[i--] = 'a';
      ++curr_string[i];
    }
    return curr_string;
  }
}

int main(int argc, char* argv[]) {

  // check command line format
  if (argc != 4) {
    fprintf(stderr, "usage: %s threads keysize target\n", argv[0]);
    exit(1);
  }

  // get salt
  char salt[3];
  strncpy(salt, argv[3], 2);

  // calc num of possible strings
  int i = 0;
  int possibilities = 1;
  while (i++<atoi(argv[2])) {
    possibilities *= 26;
  }
  --possibilities;

  // init password guess
  i = 0;
  char* guess = malloc((atoi(argv[3])+1)*sizeof(char));
  while (i<atoi(argv[2])) {
    guess[i++] = 'a';
  }

  // try all possibilities
  i = 0;
  char* hash;
  while (guess!=NULL) {
    hash = crypt(guess, salt);
    if (strcmp(hash, argv[3])==0) {
      printf("%s\n", guess);
      free(guess);
      exit(0);
    }
    curr_string = getNextString(guess, &i, &possibilities, atoi(argv[2]));
  }
  free(guess);
  printf("no match found\n");

}
