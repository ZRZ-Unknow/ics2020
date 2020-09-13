#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_NEQ, TK_NUM, TK_NEG, TK_AND, TK_POINT, TK_REG, TK_SNUM,
  /* TODO: Add more token types */
};

int op_priority[14];

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"0x[0-9a-fA-F]+", TK_SNUM}, //16进制
  {"[0-9]+",TK_NUM},   // number
  {"\\$[a-z]+", TK_REG},   // register
  {"--",TK_NEG},        // divide a negative num
  {"\\(",'('},          // left parentheses 
  {"\\)",')'},          // right parentheses
  {"\\*", '*'},         // multiply or point
  {"/",'/'},            // divide
  {"\\+", '+'},         // plus
  {"-",'-'},            // sub
  {"==", TK_EQ},        // equal
  {"!=", TK_NEQ},       // not equal
  {"&&", TK_AND},       // && 
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

typedef struct maintoken{
  int type;
  int len;
  char str[4];
} MainToken;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;
static int pare_check = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;
        
        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE : continue;
          case '(' : {pare_check++;break;}
          case ')' : {pare_check--;break;}
          case TK_NEG : {
            rules[i].token_type='+';
          }
        }
        if(pare_check<0){
          Log("invalid parenthese!\n");
          return false;
        }
        tokens[nr_token].type=rules[i].token_type;
        strncpy(tokens[nr_token].str,substr_start,substr_len);
        nr_token++;
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  if(pare_check!=0){
    Log("invalid parenthese!\n");
    pare_check=0;
    return false;
  }
  return true;
}

static bool check_parentheses(int p,int q){
  if(tokens[p].type=='(' && tokens[q].type==')') return true;
  return false;
}

static int find_main_operator(int p,int q){
  int point=-1;
  for(int i=p;i<=q;i++){
    if(tokens[i].type==TK_NUM) continue;
    if(tokens[i].type=='('){
      int tmp=i+1;
      if(tokens[tmp].type==')') return -1;
      while(tokens[tmp].type!=')') tmp++;
      i=tmp;
      continue;
    }
    if(tokens[i].type=='+' || tokens[i].type=='-') point=i;
    if(tokens[i].type=='*' || tokens[i].type=='/'){
      if(point==-1 || (tokens[point].type!='+' && tokens[point].type!='-')){
        point=i;
      }
    }
    if(tokens[i].type==TK_EQ){
      if(point==-1 || (tokens[point].type==TK_EQ)){
        point=i;
      }
    }
  }
  return point;
}

static int string2num(const char *arg){
  //convert a num_string to a real num: (char*)'76'->(int)76
  char *args=(char *)arg;
  int n=strlen(args);
  int num=0;
  for(int i=n;i>0;i--){
    if(*args<'0'||*args>'9') return -1;
    num+= (i==1)?(*(args++)-'0'):(*(args++)-'0')*10*(i-1);
  }
  return num;
}

static int eval(int p,int q,bool *success){
  if (p>q){
    *success=false;
    return -1;
  }
  else if (p==q){
    return string2num(&tokens[p].str[0]);
  }
  else if (check_parentheses(p,q)==true){
    return eval(p+1,q-1,success);
  }
  else{
    int point=find_main_operator(p,q);
    if(point==-1){
      *success=false;
      return -1;
    }
    switch (tokens[point].type)
    {
    case '+': return eval(p,point-1,success)+eval(point+1,q,success);
    case '-': return eval(p,point-1,success)-eval(point+1,q,success);
    case '*': return eval(p,point-1,success)*eval(point+1,q,success);
    case '/': {
      int res=eval(point+1,q,success);
      if(res==0){
        *success=false;
        return -1;
      }
      return eval(p,point-1,success)/res;
    }
    case TK_EQ: return eval(p,point-1,success)==eval(point+1,q,success);
    default: Assert(0,"eval failed");
    }
  }
  return 0;
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  pare_check=0;
  /* TODO: Insert codes to evaluate the expression. */
  //TODO();
  *success=true;
  return eval(0,nr_token-1,success);
}
