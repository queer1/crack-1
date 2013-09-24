/*
 * crack.c
 *   Multithreaded brute-force password hash cracker.
 *   usage: crack threads keysize target
 *
 *   Nathan Bossart
 *   Oct 1, 2013
 */

#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
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
  char* str = malloc(10*sizeof(char));
  memset(str, '\0', sizeof(str));
  do {
    double rem = fmod(num, 26);
    str[strlen(str)] = 'a' + (int) rem;
    num = (num-rem)/26;
  } while (num > 0);
  return str;
}

// increments the string
char* incrementString(char* curr_string) {
  int i = strlen(curr_string)-1;
  while (++curr_string[i]=='{') {
    curr_string[i--] = 'a';
    if (i==-1) {
      return strcat(curr_string, "a");
    }
  }
  return curr_string;
}

// 1337 haxor
void* crack(seg* range) {
  double k = range->start;
  char* hash;
  char* guess;
  guess = getNthString(range->start);
  struct crypt_data data;
  data.initialized = 0;
  while (k++<range->end) {
    hash = crypt_r(guess, SALT, &data);
    if (strcmp(hash, TARGET)==0) {
      printf("%s\n", guess);
      free(guess);
      exit(3);
    }
    guess = incrementString(guess);
  }
  printf("no match found\n");
  free(guess);
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

  // prepare thread data
  strcpy(TARGET, argv[3]);
  int threads = atoi(argv[1]);
  double perThread = floor(possibilities/threads);
  pthread_t thread_id[threads];
  int thread = 0;
  seg range[threads];
  while (thread<threads) {
    range[thread].start = perThread * thread;
    if (thread==threads-1) {
      range[thread].end = possibilities;
    } else {
      range[thread].end = (thread+1)*perThread;
    }
    ++thread;
  }

  // start threads
  i = 0;
  while (i<threads) {
    if (pthread_create(&thread_id[i], NULL, crack, &range[i])) {
      perror("Creating thread");
      exit(1);
    }
    ++i;
  }

  // join threads
  i = 0;
  while (i<threads) {
    pthread_join(thread_id[i++], NULL);
  }

}
