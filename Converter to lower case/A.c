#include <stdio.h>
#include <stdlib.h>

#define EXT_BUFF_SIZE 51                  // Size of input buffer
#define BUFF_SIZE (EXT_BUFF_SIZE - 1)     // Buffer size without \n or \0
#define ERR "[error]"

/*
 * Мамаев Алексей АПО-12
 *
 * ИЗ1
 *
 * Составить программу построчной обработки текста. Суть обработки - приведение каждого символа в каждой строке к нижнему регистру.
 * Программа считывает входные данные со стандартного ввода, и печатает результат в стандартный вывод.
 * Процедура приведения к нижнему регистру должна быть оформлена в виде отдельной функции, которой на вход подается массив строк (который необходимо обработать), количество переданных строк, а также указатель на переменную, в которой необходимо разместить результат - массив уже обработанных строк.
 * В качестве возвращаемого значения функция должна возвращать количество строк, содержащихся в результирующем массиве.
 * Программа должна уметь обрабатывать ошибки - такие как неверные входные данные(отсутствие входных строк) или ошибки выделения памяти и т.п.
 * В случае возникновения ошибки нужно выводить об этом сообщение "[error]" и завершать выполнение программы.
 */




//Lower Case : 97..122
//Upper Case : 65.. 90
// \0       : 0
// \n       : 10


// Print n strings from array source
void print_str(char **source, size_t n) {
  for (size_t i = 0; i < n; i++) {
    printf("%s", source[i]);
  }
}

// Free source array of n items dynamic memory
void mem_free(char **s, size_t n) {
  for (size_t i = 0; i < n; i++) {
    free(s[i]);
  }
  free(s);
}

// Task function - cast each character in each string to lower case
// Return 0 for memory errors  (n > 0)
size_t to_lower_case(char **source, size_t n, char ***test) {
  char **dest = (char **) calloc(sizeof(char *), n);
  if (!dest)
    return 0;

  for (size_t j = 0; j < n; j++) {
    size_t l = EXT_BUFF_SIZE;
    dest[j] = calloc(sizeof(char), EXT_BUFF_SIZE);
    if (!dest[j]) {
      mem_free(dest, j);
      return 0;
    }
    char *str = source[j];
    size_t i = 0;

    while (str[i] != '\n' && str[i] != '\0') {
      if (i + 2 >= l) {
        l *= 2;
        char *t = (char *) realloc(dest[j], l * sizeof(char));
        if (!t) {
          mem_free(dest, j + 1);
          return 0;
        }
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
// Return 0 for no input data and -1 for memory errors
long read_strings(char ***dest) {
  char **s = NULL;
  char *fl2;
  size_t k = 0, l = 0, i = 0;
  int fl = 0;
  for (;; i++) {
    fl = 0;
    k = 0;
    l = 1;
    char **t = (char **) realloc(s, (i + 1) * sizeof(char *));
    if (!t) {
      if (i > 0)
        mem_free(s, i);
      return -1;
    }
    s = t;
    char *temp = (char *) calloc(sizeof(char), EXT_BUFF_SIZE);
    if (!temp) {
      mem_free(s, i + 1);
      return -1;
    }
    while (1) {
      fl2 = fgets(&temp[BUFF_SIZE * k], EXT_BUFF_SIZE, stdin);
      if (!fl2)                                                 // Get EOF
        break;
      for (int j = 0; j < BUFF_SIZE; j++) {
        if (temp[BUFF_SIZE * k + j] == '\n') {
          fl = 1;
          break;
        }
      }
      if (fl) {                                                 // Get '\n'
        break;
      } else {
        l *= 2;
        k++;
        char *buff = (char *) realloc(temp, (l * EXT_BUFF_SIZE) * sizeof(char));
        if (buff) {
          temp = buff;
        } else {
          free(temp);
          mem_free(s, i + 1);
        }
      }
    }
    s[i] = temp;
    if (!fl2)
      break;
  }
  if (!i && s[i][0] == '\0') {
    mem_free(s, i + 1);
    return 0;
  }
  *dest = s;
  return i + 1;
}

int main() {
  char **s = NULL, **dest = NULL;

  long k = read_strings(&s);

  // No input strings (0) or memory err (-1)
  if (k <= 0) {
    printf(ERR);
    return 0;
  }

  size_t n = (size_t) k;

  n = to_lower_case(s, n, &dest);

  // Memory err (0)
  if (!n) {
    printf(ERR);
    return 0;
  }

  print_str(dest, n);

  mem_free(s, n);
  mem_free(dest, n);

  return 0;
}