#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define EXT_BUFF_SIZE 51                  // Размер буффера, в который читает gets
#define BUFF_SIZE (EXT_BUFF_SIZE - 1)     // Размер буффера без \n или \0

//LowerCase : 97..122
//UpperCase : 65.. 90
// \0       : 0
// \n       : 10

size_t toLowerCase(char **source, size_t n, char ***test) {
  char **dest = (char **) calloc(sizeof(char *), n);
  for (int j = 0; j < n; j++) {
    size_t l = EXT_BUFF_SIZE;
    dest[j] = calloc(sizeof(char), EXT_BUFF_SIZE);
    char *str = source[j];
    size_t i = 0;

    while (str[i] != '\n' && str[i] != '\0') {
      if(i + 2 >= l){
        l *= 2;
        dest[j] = (char*) realloc(dest[j], l * sizeof(char));
      }

      dest[j][i] = (char) (str[i] >= 'A' && str[i] <= 'Z' ? str[i] - 'A' + 'a' : str[i]);
      i++;
    }
    dest[j][i] = str[i];
  }
  *test = dest;
  return n;
}

int main() {
  size_t n = 0, k = 0, l = 0;
  char *fl2;
  int fl = 0, err = 0;
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
  n = i + 1;

  char **dest = NULL;




  size_t K = toLowerCase(s, n, &dest);
  if(!K)
    err = 1;

  if(err){
    printf("[error]");
    return 0;
  }

  for (i = 0; i < n; i++) {
    printf("%s", dest[i]);
  }


  for (i = 0; i < n; i++) {
    free(s[i]);
    free(dest[i]);
  }
  free(s);
  free(dest);


  return 0;
}