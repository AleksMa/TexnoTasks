#include <stdio.h>
#include <stdlib.h>

#define EXT_BUFF_SIZE 51                  // Size of input buffer
#define BUFF_SIZE (EXT_BUFF_SIZE - 1)     // Buffer size without \n or \0

//Lower Case : 97..122
//Upper Case : 65.. 90
// \0       : 0
// \n       : 10

// Task function - cast each character in each string to lower case
size_t to_lower_case(char **source, size_t n, char ***test) {
  char **dest = (char **) calloc(sizeof(char *), n);

  for (int j = 0; j < n; j++) {
    size_t l = EXT_BUFF_SIZE;
    dest[j] = calloc(sizeof(char), EXT_BUFF_SIZE);
    char *str = source[j];
    size_t i = 0;

    while (str[i] != '\n' && str[i] != '\0') {
      if (i + 2 >= l) {
        l *= 2;
        char *t = (char *) realloc(dest[j], l * sizeof(char));
        dest[j] = t;
      }
      dest[j][i] = (char) (str[i] >= 'A' && str[i] <= 'Z' ? str[i] - 'A' + 'a' : str[i]);
      i++;
    }
    dest[j][i] = str[i];
  }

  *test = dest;
  return n;
}

// Read all strings to dest array until get EOF
size_t input(char ***dest){
  char **s = NULL;
  char *fl2;
  size_t k = 0, l = 0, i = 0;
  int fl = 0;
  for (; ; i++) {
    fl = 0;
    k = 0;
    l = 1;
    char **t = (char **) realloc(s, (i + 1) * sizeof(char *));
    s = t;
    char *temp = (char *) calloc(sizeof(char), EXT_BUFF_SIZE);
    while (1) {
      fl2 = fgets(&temp[BUFF_SIZE * k], EXT_BUFF_SIZE, stdin);
      if (!fl2)  // If get EOF
        break;
      for (int j = 0; j < BUFF_SIZE; j++) {
        if (temp[BUFF_SIZE * k + j] == '\n') {
          fl = 1;
          break;
        }
      }
      if (fl == 1) {
        break;
      } else if (!fl) {
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
  *dest = s;
  return i + 1;
}

// Print n strings from array source
void print_str(char **source, size_t n){
  for (size_t i = 0; i < n; i++) {
    printf("%s", source[i]);
  }
}

// Free source array of n items dynamic memory
void memfree(char **s, size_t n){
  for (size_t i = 0; i < n; i++) {
    free(s[i]);
  }
  free(s);
}

int main() {
  char **s = NULL, **dest = NULL;

  size_t n = input(&s);

  size_t K = to_lower_case(s, n, &dest);

  // No input strings
  if (!K) {
    printf("[error]");
    return 0;
  }

  print_str(dest, n);

  memfree(s, n);
  memfree(dest, n);

  return 0;
}