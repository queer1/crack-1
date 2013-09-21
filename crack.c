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
#include <time.h>

// gets the num-th possible string
char* getNthString(double num) {
  char* str = malloc(10*sizeof(char));
  while (num>=0) {
    double num2 = num;
    int i = 0;
    while (((int) num2)>25) {
      num2/=26;
      ++i;
    }
    char toCat = 'a' + (int) num2;
    strcat(str, &toCat);
    double toSub = 26;
    int j=1;
    while (j++<i) {
      toSub*=26;
    }
    if ((int) num2 == 0) {
      break;
    } else {
      num -= (double) (((int) num2)*toSub);
    }
  }
  char n = '\0';
  strcat(str, &n);
  return str;
}

// increments the string
char* getNextString(char* curr_string) {
  int i = strlen(curr_string)-1;
  ++curr_string[i];
  while (curr_string[i]=='{') {
    if (i==0) {
      int newLen = strlen(curr_string)+1;
      free(curr_string);
      int j = 0;
      char* guess = malloc(10*sizeof(char));
      while (j<newLen) {
	guess[j++] = 'a';
      }
      guess[j] = '\0';
      return guess;
    }
    curr_string[i--] = 'a';
    ++curr_string[i];
  }
  return curr_string;
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

  // find number of possible passwords
  int keysize = atoi(argv[2]);
  int i = keysize+1;
  double possibilities = 0;
  while (--i>0) {
    double p = 1;
    int j = 0;
    while (j++<i) {
      p*=26;
    }
    possibilities += p;
  }

  // crack it!
  double k = 0;
  char* hash;
  char* guess;
  guess = getNthString(0);
  while (k++<possibilities) {
    hash = crypt(guess, salt);
    if (strcmp(hash, argv[3])==0) {
      printf("%s\n", guess);
      free(guess);
      exit(0);
    }
    guess = getNextString(guess);
  }
  printf("no match found\n");
  free(guess);
}
