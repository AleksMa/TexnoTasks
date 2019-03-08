#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#define EXT_BUFF_SIZE 51                  // Size of input buffer
#define BUFF_SIZE (EXT_BUFF_SIZE - 1)     // Buffer size without \n or \0
#define ALLOC_COUNT 10                    // Buffer of buffer pointers size
#define ERR "[error]"
#define ERROR 0

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

/*
TODO:
[SOLVED] 1.  Для обозначения булевых значений можно использовать дефайны #define TRUE/FALSE 1/0 либо подключить <stdbool.h>
[DONE] 2.  Нет проверки указателей передаваемых через аргументы функций
[DONE for read_strings] 3.  Вместо передачи char*** в виде аргумента функции подумайте о том, что может стоит возвращать char**, что упрощает чтение кода
[DONE] 4.  Функции выделения памяти в куче являются дорогостоящими. Вместо того, чтобы выделять память для каждой новой строчки на каждой итерации лучше выделить память заранее под несколько строк сразу и в процессе чтения уже ресайсить как массив строк, так и сами строки.
[DONE] 5.  стр. 109 - 114 - может стоить воспользоваться strlen?
[DONE] 6.  Обработку результатов стандартных метод лучше выполнять в инверсивной порядке, причем в вашем случае если реаллок не выделил память, и то программа явно должна завершать свое выполнение т.к. дальнейшее ее выполнение невозможно
        char *buff = (char *) realloc(temp, (l * EXT_BUFF_SIZE) * sizeof(char));
        if (buff) {
          temp = buff;
        } else {
          free(temp);
          mem_free(s, i + 1);
        }
        char *buff = (char *) realloc(temp, (l * EXT_BUFF_SIZE) * sizeof(char));
        if (!buff) {
          ALARM
        }
[DONE] 7.  to_lower_case - зачем логика перевыделения памяти (умножение на 2), если есть возможность посчитать размер строки?
[DONE] 8. исправьте сообщения cppcheck'a
*/


//Lower Case : 97..122
//Upper Case : 65.. 90
// \0       : 0
// \n       : 10


// Print n strings from array source
void print_str(char **source, size_t n) {
  if (!source)
    return;
  for (size_t i = 0; i < n; i++) {
    printf("%s", source[i]);
  }
}

// Free source array of n items dynamic memory
void mem_free(char **s, size_t n) {
  if (!s)
    return;
  for (size_t i = 0; i < n; i++) {
    free(s[i]);
  }
  free(s);
}

// Task function - cast each character in each string to lower case
// Return 0 for memory errors  (n > 0)
size_t to_lower_case(char **source, size_t n, char ***dest) {
  if (!source || !dest || !n) {
    return ERROR;
  }

  char **temp = (char **) calloc(sizeof(char *), n);
  if (!temp)
    return ERROR;

  for (size_t i = 0; i < n; i++) {
    size_t len = strlen(source[i]);

    temp[i] = calloc(sizeof(char), len + 1);   // + 1 for '\0'
    if (!temp[i]) {
      mem_free(temp, i);
      return ERROR;
    }

    for (size_t j = 0; j <= len; j++)
      temp[i][j] = (char) (source[i][j] >= 'A' && source[i][j] <= 'Z' ? source[i][j] - 'A' + 'a' : source[i][j]);
  }

  *dest = temp;
  return n;
}

// Read all strings to dest array until get EOF
// Return 0 for no input data and -1 for memory errors
char **read_strings(size_t *n) {
  if (!n)
    return ERROR;

  char **s = (char **) calloc(sizeof(char *), ALLOC_COUNT);
  size_t buff_size = ALLOC_COUNT, i = 0;
  char *flag_buffer;

  if (!s)
    return ERROR;
  for (size_t j = 0; j < ALLOC_COUNT; j++) {
    s[j] = (char *) calloc(sizeof(char), EXT_BUFF_SIZE);
    if (!s[j]) {
      mem_free(s, j);
      return ERROR;
    }
  }

  for (;; i++) {
    if (i >= buff_size) {

      char **t = (char **) realloc(s, 2 * buff_size * sizeof(char *));
      if (!t) {
        mem_free(s, buff_size);
        return ERROR;
      }
      s = t;
      for (size_t j = buff_size; j < 2 * buff_size; j++) {
        s[j] = (char *) calloc(sizeof(char), EXT_BUFF_SIZE);
        if (!s[j]) {
          mem_free(s, buff_size + j);
          return ERROR;
        }
      }
      buff_size *= 2;
    }

    size_t k = 0, l = 1;                  // k - counter of input blocks in s[i], l - coefficient of reallocation

    while (1) {
      flag_buffer = fgets(&s[i][BUFF_SIZE * k], EXT_BUFF_SIZE, stdin);
      if (!flag_buffer) {
        if (ferror(stdin))
          return ERROR;
        break;                                                  // Get EOF
      }

      size_t j = strlen(&s[i][BUFF_SIZE * k]);
      if (s[i][BUFF_SIZE * k + j - 1] == '\n') {                // Get '\n'
        break;
      }
      else {
        l *= 2;
        k++;
        char *buff = (char *) realloc(s[i], (l * EXT_BUFF_SIZE) * sizeof(char));
        if (!buff) {
          mem_free(s, buff_size);
          return ERROR;
        }
        s[i] = buff;
      }
    }
    if (!flag_buffer)   // Get EOF
      break;
  }

  if (!i && s[i][0] == '\0') {          // Empty string - error
    mem_free(s, i + 1);
    return ERROR;
  }

  *n = i + 1;

  if (i + 1 < buff_size) {                             // Delete unused memory
    for (size_t j = i + 1; j < buff_size; j++) {
      free(s[j]);
    }
    char **t = realloc(s, (i + 1) * sizeof(char *));
    if (!t)
      return ERROR;
    s = t;
  }

  return s;
}

int main() {
  char **dest = NULL;
  size_t n = 0;

  char **s = read_strings(&n);

  if (!s) {                                   // No input strings or memory err (0)
    printf(ERR);
    return 0;
  }

  n = to_lower_case(s, n, &dest);

  if (!n) {                                       // Memory err (0)
    printf(ERR);
    return 0;
  }

  print_str(dest, n);

  mem_free(s, n);
  mem_free(dest, n);

  return 0;
}