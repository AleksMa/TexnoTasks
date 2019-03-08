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

int expr();
int inner_expr();
int term();
int num_term();
int line_term();
int line();

char **lexemes, **instruction_pointer;
char *accumulator, *buff;

int expr() {
  term();
  inner_expr();
}

int inner_expr() {
  if (**instruction_pointer != '+' && **instruction_pointer != 0 && **instruction_pointer != ')' && **instruction_pointer != 0)
    return 0;
  if  (**instruction_pointer == '+'){
    term();
    inner_expr();
  }
}

int term() {
  if (**instruction_pointer == '\"' || **instruction_pointer == '('){
    line();
    num_term();
  }
  else if (isdigit(**instruction_pointer)){
    //num();
    line_term();
  }
  else
    return 0;
}

int num_term() {
  if (**instruction_pointer == '*'){
    //num();
    num_term();
  }
  else if(**instruction_pointer != ')' && **instruction_pointer != '+' && **instruction_pointer != 0 && **instruction_pointer != '\n')
    return 0;
}

int line_term() {
  line();
  num_term();
}

int line() {
  if(**instruction_pointer == '\"'){
    //string
  }
  else if (**instruction_pointer == '('){
    expr();
    if (**instruction_pointer != ')'){
      return 0;
    }
    instruction_pointer++;
  }
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
  }

  len = strlen(input_buff);

  input_buff = realloc(input_buff, len + 1);

  return input_buff;

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

int remove_spaces(char *source) {
  if (!source) {
    return ERROR;
  }

  char *left = source, *right = source;

  while (*right != 0) {
    *left = *right++;
    if (*left != ' ') {
      left++;
    }
  }
  *left = 0;

  return OK;
}

int main() {
  char *dest = NULL;
  size_t n = 0;

  char *input_buff = read_string();
  if (!input_buff) {
    ERR_ACTION
  }

  int res = remove_spaces(input_buff);
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

  res = expr();

  printf("%s", input_buff);

  free(input_buff);
  free(lexemes);

  return 0;
}