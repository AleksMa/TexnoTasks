#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>

#define EXT_BUFF_SIZE 51                  // Size of input buffer
#define BUFF_SIZE (EXT_BUFF_SIZE - 1)     // Buffer size without \n or \0
#define ERR "[error]"
#define ERR_ACTION  { printf(ERR); return 0; }
#define ERROR 0
#define OK    1


/*
 * Мамаев Алексей АПО-12
 *
 * ИЗ2
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
 * Grammar

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

// Classical top-down parsing with accumulator string

char *expr();
char *inner_expr();
char *term();
int num_term(); //
char *line_term(); //
char *line();  //

/*
 *  TODO: ВОЗВРАЩАТЬ В ФУНКЦИЯХ СТРОКИ И ДАЛЕЕ ИХ КОНКАТЕНИРОВАТЬ.
 */

char **lexemes, **instruction_pointer;
char *accumulator, *buff;

char *expr() {
  char *res = term();
  char *next = inner_expr();
  if (strlen(next) == 0)
    return res;
  size_t r = strlen(res), n = strlen(next);
  res = realloc(res, (r + n + 1) * sizeof(char));
  strcat(res, next);
  free(next);
  return res;
}

char *inner_expr() {
  if (**instruction_pointer != '+' && **instruction_pointer != 0 && **instruction_pointer != ')'
      && **instruction_pointer != '\n')
    return 0;
  if (**instruction_pointer == '+') {
    instruction_pointer++;
    char *res = term();
    char *next = inner_expr();
    if (strlen(next) == 0)
      return res;
    size_t r = strlen(res), n = strlen(next);
    res = realloc(res, (r + n + 1) * sizeof(char));
    strcat(res, next);
    free(next);
    return res;
  }
  return "";
}

char *term() {
  if (**instruction_pointer == '\"' || **instruction_pointer == '(') {
    char *res = line();
    char buff[strlen(res) + 1];
    strcpy(buff, res);
    //buff[strlen(res)] = 0;
    int k = num_term();
    res = realloc(res, k * strlen(res) * sizeof(char) + 1);
    for (int i = 1; i < k; i++) {
      strcat(res, buff);
    }
    return res;
  } else if (isdigit(**instruction_pointer)) {
    int k = strtol(*instruction_pointer++, instruction_pointer, 10);
    char *res = line_term();
    char buff[strlen(res) + 1];
    strcpy(buff, res);
    //buff[strlen(res)] = 0;
    res = realloc(res, k * strlen(res) * sizeof(char) + 1);
    for (int i = 1; i < k; i++) {
      strcat(res, buff);
    }
    return res;
  } else
    return 0;
}

int num_term() {
  if (**instruction_pointer == '*') {
    instruction_pointer++;
    if (!isdigit(**instruction_pointer))
      return ERROR;
    int res = strtol(*instruction_pointer++, instruction_pointer, 10);
    res *= num_term();
    return res;
  } else if (**instruction_pointer != ')' && **instruction_pointer != '+' && **instruction_pointer != 0
      && **instruction_pointer != '\n')
    return ERROR;
  else
    return 1;
}

char *line_term() {
  if (**instruction_pointer == '*') {
    instruction_pointer++;
    char *res = line();
    char buff[strlen(res) + 1];
    strcpy(buff, res);
    int k = num_term();
    res = realloc(res, k * strlen(res) * sizeof(char) + 1);
    for (int i = 1; i < k; i++) {
      strcat(res, buff);
    }
    return res;
  }
  return ERROR;
}

char *line() {
  char *res = NULL;
  if (**instruction_pointer == '\"') {
    size_t len = (*(instruction_pointer + 1) - *instruction_pointer);
    res = (char *) calloc(sizeof(char), len - 1);
    memcpy(res, *instruction_pointer++ + 1, len - 2);
    return res;
  } else if (**instruction_pointer == '(') {
    instruction_pointer++;
    res = expr();
    if (**instruction_pointer != ')') {
      return 0;
    }
    instruction_pointer++;
    return res;
  }
  return ERROR;
}

char *read_string() {
  size_t k = 0, l = 1, len = 0;
  char *input_buff = (char *) calloc(sizeof(char), EXT_BUFF_SIZE), *flag_buff = NULL;
  if (!input_buff)
    return ERROR;

  while (1) {
    flag_buff = fgets(&input_buff[BUFF_SIZE * k], EXT_BUFF_SIZE, stdin);

    if (!flag_buff) {
      if (ferror(stdin))
        return ERROR;
      break;                                                  // Get EOF
    }

    l *= 2;
    k++;
    char *buff = (char *) realloc(input_buff, (BUFF_SIZE * k + EXT_BUFF_SIZE) * sizeof(char));
    if (!buff) {
      free(input_buff);
      return ERROR;
    }
    input_buff = buff;
    break;
  }

  len = strlen(input_buff);

  input_buff = realloc(input_buff, len + 1);

  return input_buff;

}

int remove_outer_spaces(char *source) {
  if (!source) {
    return ERROR;
  }
  int fl = 0;

  char *left = source, *right = source;

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

char **lexer(char *source, size_t *n) {
  if (!source || !n)
    return ERROR;
  size_t k = EXT_BUFF_SIZE;
  char **lexemes = (char **) calloc(sizeof(char *), EXT_BUFF_SIZE);
  if (!lexemes)
    return ERROR;

  char *buff = source;
  size_t len = strlen(source), i = 0;

  while (*buff != 0 && *buff != '\n') {
    if (i >= k - 1) {
      k *= 2;
      char **temp = (char **) realloc(lexemes, sizeof(char *) * k);
      if (!temp) {
        free(lexemes);
        return ERROR;
      }
      lexemes = temp;
    }

    if (isdigit(*buff)) {
      lexemes[i] = buff++;
      while (isdigit(*buff)) { buff++; }
      lexemes[++i] = buff;
    } else if (*buff == '\"') {
      lexemes[i] = buff++;
      while (*buff != '\"') { buff++; }
      lexemes[++i] = ++buff;
    } else if (*buff == '+' || *buff == '*' || *buff == '(' || *buff == ')') {
      lexemes[i] = buff++;
      lexemes[++i] = buff;
    } else
      return ERROR;
  }

  char *b = lexemes[0];
  for (size_t j = 0; j < i; j++) {
    while (*b != 0 && *b != '\n' && b < lexemes[j + 1])
      printf("%c", *b++);
    printf("\n");
  }
  *n = i;
  return lexemes;
}

int main() {
  char *dest = NULL;
  size_t n = 0;

  char *input_buff = read_string();
  if (!input_buff) {
    ERR_ACTION
  }

  if (input_buff[strlen(input_buff) - 1] == '\n')
    input_buff[strlen(input_buff) - 1] = 0;

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

  instruction_pointer = lexemes;
  accumulator = (char *) calloc(sizeof(char), EXT_BUFF_SIZE);

  dest = expr();

  printf("\"%s\"\n", dest);

  printf("%s", input_buff);

  free(input_buff);
  free(lexemes);
  free(dest);

  return 0;
}