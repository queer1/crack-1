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

char* getNextString(char* curr_string, int* curr_iteration, int* max_iteration) {
  if (curr_iteration++==max_iteration) {
	return NULL;
  } else {
	int i = strlen(curr_string)-1;
	while (i-->0) {
	  curr_st
	}
  }
}

int main(int argc, char* argv[]) {

  // check command line format
  if (argc != 4) {
    fprintf(stderr, "usage: %s threads keysize target\n", argv[0]);
    exit(1);
  }

  int i = 0;
  int m = 10;
  char* curr_string = "aaaaaaaa";
  while (curr_string!=NULL) {
	printf("%s\n", curr_string);
	curr_string = getNextString(curr_string, &i, &m);
	++i;
  }

}
