/*
 * crack.c
 *   Multithreaded brute-force password hash cracker.
 *   usage: crack threads keysize target
 *
 *   Nathan Bossart
 *   Sept 25, 2013
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <crypt.h>
#include <math.h>

char SALT[3];
char TARGET[20];

typedef struct seg seg;
struct seg {
  double start, end;
};

// gets the nth possible string
char* getNthString(double num) {

  // calculate the string
  char* str = malloc(10*sizeof(char));
  memset(str, '\0', sizeof(str));
  int i = 0;
  do {
    double rem = fmod(num, 26);
    str[i++] = 'a' + (int) rem;
    num = (num-rem)/26;
  } while (num > 0);

  // reverse the string
  i = 0;
  int len = strlen(str);
  while (i<len/2) {
    str[len-1-i] = str[len-1-i] ^ str[i];
    str[i]       = str[len-1-i] ^ str[i];
    str[len-1-i] = str[len-1-i] ^ str[i];
    ++i;
  }
  return str;
}

// increments the string
void incrementString(char* curr_string) {
  int i = strlen(curr_string)-1;
  while (++curr_string[i]=='{') {
    curr_string[i--] = 'a';
    if (i==-1) {
      strcat(curr_string, "a");
      break;
    }
  }
}

// each thread tries a range of strings
void* crack(seg* range) {
  double i = range->start;
  char* hash;
  char* guess = getNthString(i);
  struct crypt_data data;
  data.initialized = 0;
  while (i++<range->end) {
    hash = crypt_r(guess, SALT, &data);
    if (strcmp(hash, TARGET)==0) {
      printf("%s\n", guess);
      exit(3);
    }
    incrementString(guess);
  }
}

int main(int argc, char* argv[]) {

  // check command line format
  if (argc != 4) {
    fprintf(stderr, "usage: %s threads keysize target\n", argv[0]);
    exit(1);
  }

  // get salt
  strncpy(SALT, argv[3], 2);

  // find number of possible passwords
  int i = atoi(argv[2]);
  double possibilities = 0;
  while (i>0) {
    possibilities += pow(26, i--);
  }

  // prepare thread data and start threads
  strcpy(TARGET, argv[3]);
  int       threads   = atoi(argv[1]);
  double    perThread = floor(possibilities/threads);
  seg       range[threads];
  pthread_t thread_id[threads];
  while (i<threads) {
    range[i].start = perThread * i;
    if (i==threads-1) {
      range[i].end = possibilities;
    } else {
      range[i].end = range[i].start + perThread;
    }
    if (pthread_create(&thread_id[i], NULL, crack, &range[i])) {
      perror("Creating thread");
      exit(1);
    }
    ++i;
  }

  // join threads
  i = 0;
  do {
    pthread_join(thread_id[i++], NULL);
  } while (i<threads);

}
