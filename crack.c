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

// reverses a string
void reverse_string(char* str) {
  int i = 0;
  int len = strlen(str);
  while (i<len/2) {
    char tmp = str[len-1-i];
    str[len-i-1] = str[i];
    str[i++] = tmp;
  }
}

// gets the nth possible string
char* getNthString(double num) {
  char* str = malloc(10*sizeof(char));
  memset(str, '\0', sizeof(str));
  int i = 0;
  do {
    double rem = fmod(num, 26);
    str[i++] = 'a' + (int) rem;
    num = (num-rem)/26;
  } while (num > 0);
  reverse_string(str);
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

// 1337 haxor
void* crack(seg* range) {
  double k = range->start;
  char* hash;
  char* guess;
  guess = getNthString(k);
  struct crypt_data data;
  data.initialized = 0;
  while (k++<range->end) {
    hash = crypt_r(guess, SALT, &data);
    if (strcmp(hash, TARGET)==0) {
      printf("%s\n", guess);
      free(guess);
      exit(3);
    }
    incrementString(guess);
  }
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
    double p = pow(26, i);
    possibilities += p;
  }

  // prepare thread data
  strcpy(TARGET, argv[3]);
  int    thread    = 0;
  int    threads   = atoi(argv[1]);
  double perThread = floor(possibilities/threads);
  seg    range[threads];
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
  pthread_t thread_id[threads];
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

  printf("No match found\n");

}
