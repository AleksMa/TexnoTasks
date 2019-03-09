#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>

#define EXT_BUFF_SIZE 51                  // Size of input buffer
#define BUFF_SIZE (EXT_BUFF_SIZE - 1)     // Buffer size without \n or \0
#define ERR "[error]"
#define ERR_ACTION  { printf(ERR); return 0; }
#define ERROR      0
#define INT_ERROR -1
#define OK         1
#define TRUE       1
#define FALSE      0

typedef int bool;


/*
 * Мамаев Алексей АПО-12
 *
 * Run ID = 888
 *
 * ИЗ-2
 *
 * Разработайте программу-калькулятор, умеющую вычислять арифметические выражения над строками.
 * На стандартный поток ввода программы подается входное выражение, а результат вычислений программа должна вывести на стандартный поток вывода.

 * Строка - это заключенный в двойные кавычки произвольный набор символов.
 * Например,
 * "1", "123", "zz!@#111' ad x" - строки,
 * 'asd', "asddas - не строки.
 * "" - пустая строка, допустимо.
 *
 * Поддерживаемые операции: '+' - конкатенация, '*' - умножение, '()' - задание приоритета вычислений.
 * Конкатенировать (складывать) можно только строки! Иначе необходимо вывести "[error]".
 */



/*
 * Grammar (BNF)

 * Expr         ::=   Term InnerExpr .
 * InnerExpr    ::= + Term InnerExpr  | .
 * Term         ::=   Line NumTerm    | Num LineTerm .
 * NumTerm      ::= * Num  NumTerm    | .
 * LineTerm     ::= * Line NumTerm   .
 * Num          ::=   Digit DigitTail .
 * Digit        ::=   0 | 1  | ... | 9 .
 * DigitTail    ::=   Digit  | .
 * Line         ::=   String | '(' Expr ')' .
 * String       ::=   " StringTail " .
 * StringTail   ::=   <any_char> StringTail | .
 *
 */

// Classic top-down parsing with accumulator strings
// Each function for each expression in BNF (except number, digits, strings)

char *parser();
char *expr();
char *inner_expr();
char *term();
long num_term();
char *line_term();
char *line();

char **lexemes, **instruction_pointer;      // Array of lexemes and pointer for this array

char *parser() {
  instruction_pointer = lexemes;
  char *result = expr();
  if (!result)
    return ERROR;

  size_t len = strlen(result);
  char *parsed = calloc(sizeof(char), len + 3);

  if (!parsed) {
    free(result);
    return ERROR;
  }

  parsed[0] = '\"';
  strcpy(parsed + 1, result);
  parsed[len + 1] = '\"';

  free(result);
  return parsed;
}

char *expr() {
  char *res = term();
  char *next = inner_expr();

  if (!res || !next) {
    if (res)
      free(res);
    if (next)
      free(next);
    return ERROR;
  }

  if (strlen(next) == 0) {
    free(next);
    return res;
  }

  size_t res_len = strlen(res), next_len = strlen(next);
  char *temp = realloc(res, (res_len + next_len + 1) * sizeof(char));
  if (!temp) {
    free(res);
    free(next);
    return ERROR;
  }
  res = temp;
  strcat(res, next);
  free(next);

  return res;
}

char *inner_expr() {
  if (**instruction_pointer != '+' && **instruction_pointer != 0 && **instruction_pointer != ')')
    return 0;
  if (**instruction_pointer == '+') {
    instruction_pointer++;

    char *res = term();
    char *next = inner_expr();

    if (!res || !next) {
      if (res)
        free(res);
      if (next)
        free(next);
      return ERROR;
    }

    if (strlen(next) == 0) {
      free(next);
      return res;
    }

    size_t res_len = strlen(res), next_len = strlen(next);
    char *temp = realloc(res, (res_len + next_len + 1) * sizeof(char));
    if (!temp) {
      free(res);
      free(next);
      return ERROR;
    }
    res = temp;
    strcat(res, next);
    free(next);

    return res;
  }
  char *empty = calloc(sizeof(char), 1);
  if (!empty)
    return ERROR;
  empty[0] = 0;
  return empty;

}

char *term() {
  if (**instruction_pointer == '\"' || **instruction_pointer == '(') {
    char *res = line();
    if (!res)
      return ERROR;

    char buff[strlen(res) + 1];
    strcpy(buff, res);
    long k = num_term();
    if (k == INT_ERROR) {
      free(res);
      return ERROR;
    }
    if (k == 0) {
      free(res);
      char *empty = calloc(sizeof(char), 1);
      if (!empty)
        return ERROR;
      empty[0] = 0;
      return empty;
    }
    char *temp = realloc(res, (k * strlen(res) + 1) * sizeof(char));
    if (!temp) {
      free(res);
      return ERROR;
    }
    res = temp;

    for (long i = 1; i < k; i++) {
      strcat(res, buff);
    }
    return res;

  } else if (isdigit(**instruction_pointer)) {
    long k = strtol(*instruction_pointer, instruction_pointer + 1, 10);
    instruction_pointer++;

    char *res = line_term();
    if (!res)
      return ERROR;

    if (k == 0) {
      free(res);
      char *empty = calloc(sizeof(char), 1);
      if (!empty)
        return ERROR;
      empty[0] = 0;
      return empty;
    }

    char buff[strlen(res) + 1];
    strcpy(buff, res);

    char *temp = realloc(res, (k * strlen(res) + 1) * sizeof(char));
    if (!temp) {
      free(res);
      return ERROR;
    }
    res = temp;

    for (int i = 1; i < k; i++) {
      strcat(res, buff);
    }
    return res;
  } else
    return 0;
}

long num_term() {
  if (**instruction_pointer == '*') {
    instruction_pointer++;

    if (!isdigit(**instruction_pointer))
      return INT_ERROR;

    int res = (int) strtol(*instruction_pointer, instruction_pointer + 1, 10);
    instruction_pointer++;

    long num = num_term();
    if (num == INT_ERROR)
      return INT_ERROR;
    res *= num;
    return res;
  } else if (**instruction_pointer != ')' && **instruction_pointer != '+' && **instruction_pointer != 0)
    return INT_ERROR;
  else
    return 1;
}

char *line_term() {
  if (**instruction_pointer == '*') {
    instruction_pointer++;

    char *res = line();
    if (!res)
      return ERROR;

    char buff[strlen(res) + 1];
    strcpy(buff, res);
    long k = num_term();
    if (k == INT_ERROR)
      return ERROR;

    if (k == 0) {
      free(res);
      char *empty = calloc(sizeof(char), 1);
      if (!empty)
        return ERROR;
      empty[0] = 0;
      return empty;
    }

    char *temp = realloc(res, (k * strlen(res) + 1) * sizeof(char));
    if (!temp) {
      free(res);
      return ERROR;
    }
    res = temp;

    for (int i = 1; i < k; i++) {
      strcat(res, buff);
    }
    return res;
  }
  return ERROR;
}

char *line() {
  if (**instruction_pointer == '\"') {
    size_t len = (*(instruction_pointer + 1) - *instruction_pointer);
    char *res = (char *) calloc(sizeof(char), len - 1);
    if (!res)
      return ERROR;

    memcpy(res, *instruction_pointer++ + 1, len - 2);
    return res;
  } else if (**instruction_pointer == '(') {
    instruction_pointer++;

    char *res = expr();
    if (!res)
      return ERROR;

    if (**instruction_pointer != ')') {
      return ERROR;
    }

    instruction_pointer++;
    return res;
  }
  return ERROR;
}


// Make array of lexemes (pointers to input string)
char **lexer(char *source, size_t *n) {
  if (!source || !n)
    return ERROR;

  size_t k = EXT_BUFF_SIZE;
  char **lexemes = (char **) calloc(sizeof(char *), EXT_BUFF_SIZE);
  if (!lexemes)
    return ERROR;

  char *pointer = source;
  size_t i = 0;

  while (*pointer != 0) {
    if (i >= k - 1) {
      k *= 2;
      char **temp = (char **) realloc(lexemes, sizeof(char *) * k);
      if (!temp) {
        free(lexemes);
        return ERROR;
      }
      lexemes = temp;
    }

    if (isdigit(*pointer)) {
      lexemes[i] = pointer++;
      while (isdigit(*pointer)) { pointer++; }
      lexemes[++i] = pointer;
    } else if (*pointer == '\"') {
      lexemes[i] = pointer++;
      while (*pointer != '\"') {
        if (*pointer == 0) {
          free(lexemes);
          return ERROR;
        }
        pointer++;
      }
      lexemes[++i] = ++pointer;
    } else if (*pointer == '+' || *pointer == '*' || *pointer == '(' || *pointer == ')') {
      lexemes[i] = pointer++;
      lexemes[++i] = pointer;
    } else {
      free(lexemes);
      return ERROR;
    }
  }

  /*
  char *b = lexemes[0];
  for (size_t j = 0; j < i; j++) {
    while (*b != 0 && *b != '\n' && b < lexemes[j + 1])
      printf("%c", *b++);
    printf("\n");
  }
  */
  *n = i;
  return lexemes;
}

// Read input expression
char *read_string() {
  size_t k = 0, l = 1, len = 0;
  char *input_buff = (char *) calloc(sizeof(char), EXT_BUFF_SIZE), *flag_buff = NULL;
  if (!input_buff)
    return ERROR;

  while (1) {
    flag_buff = fgets(&input_buff[BUFF_SIZE * k], EXT_BUFF_SIZE, stdin);

    if (!flag_buff) {
      if (ferror(stdin)) {
        free(input_buff);
        return ERROR;
      }
      break;                                                  // Get EOF
    }

    k++;
    if (k >= l - 1) {
      l *= 2;
      char *buff = (char *) realloc(input_buff, (BUFF_SIZE * l + 1) * sizeof(char));
      if (!buff) {
        free(input_buff);
        return ERROR;
      }
      input_buff = buff;
    }
  }

  len = strlen(input_buff);

  if (input_buff[len - 1] == '\n')
    input_buff[len-- - 1] = 0;

  input_buff = realloc(input_buff, len + 1);

  return input_buff;

}

// Remove all spaces in expression (except inside strings)
int remove_outer_spaces(char *input_buff) {
  if (!input_buff) {
    return ERROR;
  }

  bool fl = FALSE;
  char *left = input_buff, *right = input_buff;

  while (*right != 0) {
    *left = *right++;

    if (*left == '\"')
      fl = !fl;

    if (*left != ' ' || fl) {
      left++;
    }
  }
  *left = 0;

  return OK;
}

int main() {
  char *result = NULL;
  size_t n = 0;

  char *input_buff = read_string();
  if (!input_buff) {
    ERR_ACTION
  }

  int res = remove_outer_spaces(input_buff);
  if (!res) {
    free(input_buff);
    ERR_ACTION
  }

  lexemes = lexer(input_buff, &n);
  if (!lexemes) {
    free(input_buff);
    ERR_ACTION
  }

  result = parser();
  if (!result) {
    free(input_buff);
    free(lexemes);
    ERR_ACTION
  }

  printf("%s", result);

  //printf("%s", input_buff);

  free(input_buff);
  free(lexemes);
  free(result);

  return 0;
}