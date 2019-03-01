#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DIFF 32
#define L_UPPER 65
#define R_UPPER 90
#define EXT_BUFF_SIZE 105
#define BUFF_SIZE (EXT_BUFF_SIZE - 1)

//LowerCase : 97..122
//UpperCase : 65.. 90
// \0       : 0
// \n       : 10

int toLowerCase(char **source, size_t n, char **dest) {
  for (int j = 0; j < n; j++) {
    char *str = source[j];
    size_t i = 0;

    while (str[i] != '\n' && str[i] != '\0') {
      /*dest[j][i]*/ char c = (char) (str[i] >= L_UPPER && str[i] <= R_UPPER ? str[i] + DIFF : str[i]);
      printf("%c ", c);
      i++;
    }
    printf("%d\n", str[i]);
  }
  return n;
}

int main() {
  size_t n = 0, k = 0, l = 1, K = 1;
  char *fl2;
  int fl = 0;
  char **s = NULL;
  size_t i = 0;
  for (; ; i++) {
    fl = 0;
    k = 0;
    l = 1;
    char **t = (char **) realloc(s, (i + 1) * sizeof(char*));
    s = t;
    char *temp = (char *) calloc(sizeof(char), EXT_BUFF_SIZE);
    while (1) {
      fl2 = fgets(&temp[BUFF_SIZE * k], EXT_BUFF_SIZE, stdin);
      if(!fl2)
        break;
      for (int j = 0; j < BUFF_SIZE; j++) {
        if (temp[BUFF_SIZE * k + j] == '\n') {
          fl = 1;
          break;
        }
      }
      if (fl == 1){
        break;
      }
      else if (!fl) {
        l *= 2;
        k++;
        char *buff = (char *) realloc(temp, (l * EXT_BUFF_SIZE) * sizeof(char));
        temp = buff;
      }
    }
    s[i] = temp;
    if (!fl2)
      break;
  }
  n = i;
  char **dest;





  toLowerCase(s, n, dest);

  /*
  for (int i = 0; i < n; i++) {
    printf("%s\n", dest[i]);
  }
   */

  printf("%ld\n", n);


  for (i = 0; i <= n; i++) {
    free(s[i]);
  }

  free(s);


  return 0;
}