// RUN: %llvmgcc %s -emit-llvm -g -O0 -c -o %t.bc
// RUN: rm -rf %t.klee-out
// RUN: %klee -taint=direct --output-dir=%t.klee-out -exit-on-error %t.bc

#include<klee/klee.h>

/**
* In the following C/C++ example, a utility function is used to trim trailing whitespace from a character string. 
* The function copies the input string to a local character string and uses a while statement to remove the trailing whitespace by moving backward through the string and overwriting whitespace with a NUL character.
* However, this function can cause a buffer underwrite if the input character string contains all whitespace. On some systems the while statement will move backwards past the beginning of a character string and will call the isspace() function on an address outside of the bounds of the local buffer.
* From: https://cwe.mitre.org/data/definitions/124.html
*/


char* trimTrailingWhitespace(char *strMessage, int length) {
  char *retMessage;
  char *message = malloc(sizeof(char)*(length+1));
  
  // copy input string to a temporary string
  char message[length+1];
  int index;
  for (index = 0; index < length; index++) {
    message[index] = strMessage[index];
  }
  message[index] = '\0';
  
  // trim trailing whitespace
  int len = index-1;
  while (isspace(message[len])) {
    message[len] = '\0';
    len--;
  }
  
  // return string without trailing whitespace
  retMessage = message;
  klee_assert(klee_get_taint (&retMessage, sizeof (retMessage))==1);

  return retMessage;
}


int 
main (int argc, char *argv[])
{
  char *strMessage = "     ";
  klee_set_taint (1, &strMessage, sizeof (strMessage));

  trimTrailingWhitespace(strMessage, sizeof (strMessage));

  return 0;
}
