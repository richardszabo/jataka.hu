#include <stdio.h>

#include "parse.h"
#include "defs.h"

#undef PARSE_WARNS

char *start;
char *ptr;
char term;

char *tok_sval;
int tok_ival;
float tok_fval;

position_spec poss;
position pos;
player_position *player_posp = (player_position *)&pos;
ball_position *ball_posp = (ball_position *)&pos;
goal_position *goal_posp = (goal_position *)&pos;

#define PARSE_ERR_SIZE 200
char parse_err_buf[PARSE_ERR_SIZE];

#define ERR_RET(str) \
    do { \
      if(parse_errstr==NULL) { \
        snprintf(parse_err_buf, PARSE_ERR_SIZE, "error:parse: %s at %d\n", str, ptr-start+1); \
        parse_errstr=parse_err_buf; \
      } \
      return 0; \
    } while(0);

#define IS_SEPARATOR() (*ptr==' ' || *ptr=='(' || *ptr==')' || *ptr=='\0')
#define EXPECT_SEPARATOR() if(!IS_SEPARATOR()) ERR_RET("separator expected")
#define SKIP_WS() while(*ptr == ' ') ++ptr
#define IS_TOK_OPEN() (*ptr=='(')
#define IS_TOK_CLOSE() (*ptr==')')
#define IS_TOK_ID() ((*ptr>='a' && *ptr<='z') || (*ptr>='A' && *ptr<='Z') || *ptr=='_')
#define IS_TOK_FLOAT() ((*ptr>='0' && *ptr<='9') || *ptr=='-')

int break_sexp(int lev)
{
  while(lev>0) {
    switch(*ptr) {
    case '(': ++lev; break;
    case ')': --lev; break;
    case '\0': ERR_RET("cannot break from expression");
    }
    ++ptr;
  }
  return 1;
}

int get_tok_id()
{
  tok_sval = ptr;
  while(IS_TOK_ID()) ++ptr;
  if(!IS_SEPARATOR()) {
#ifdef PARSE_WARNS
    fprintf(stderr, "warning: non-separator non-id character\n");
#endif
    while(!IS_SEPARATOR()) ++ptr;
  }
  if(ptr!=tok_sval) {
    term = *ptr;
    *ptr = '\0';
    return 1;
  } else {
    ERR_RET("identifier expected");
  }
}

int get_tok_open()
{
  if(*ptr=='(') {
    ++ptr;
    return 1;
  } else {
    ERR_RET("opening parenthesis expected");
  }
}

int get_tok_close()
{
  if(*ptr==')') {
    ++ptr;
    return 1;
  } else {
    ERR_RET("error: closing parenthesis expected");
  }
}

int get_tok_int()
{
  char *p0 = ptr;
  int sign = 1;
  tok_ival = 0;
  if(*ptr=='-') {
    sign = -1;
    ++ptr;
  }
  while(*ptr>='0' && *ptr<='9') {
    tok_ival *= 10;
    tok_ival += *ptr-'0';
    ++ptr;
  }
  EXPECT_SEPARATOR();
  tok_ival *= sign;
  if(ptr==p0) ERR_RET("could not parse integer");
  return 1;
}

int get_tok_float()
{
  char *p0 = ptr;
  int sign = 1;
  int frac = 1;
  tok_fval = 0;
  if(*ptr=='-') {
    sign = -1;
    ++ptr;
  }
  while(*ptr>='0' && *ptr<='9') {
    tok_fval *= 10;
    tok_fval += *ptr-'0';
    ++ptr;
  }
  if(*ptr=='.') {
    ++ptr;
    while(*ptr>='0' && *ptr<='9') {
      tok_fval *= 10;
      tok_fval += *ptr-'0';
      frac *= 10;
      ++ptr;
    }
  }
  EXPECT_SEPARATOR();
  tok_fval *= sign;
  tok_fval /= frac;
  if(ptr==p0) ERR_RET("could not parse float");
  return 1;
}

int get_floats()
{
  poss = POS_NONE;
  SKIP_WS();
  if(IS_TOK_FLOAT()) {
    if(!get_tok_float()) return 0;
    pos.distance = tok_fval;
    poss = POS_DIR;
    SKIP_WS();
  } else {
    return get_tok_close();
  }
  if(IS_TOK_FLOAT()) {
    if(!get_tok_float()) return 0;
    pos.direction = tok_fval;
    poss = POS_DISTDIR;
    SKIP_WS();
  } else {
    pos.direction = pos.distance;
    pos.distance = DIST_UNKNOWN;
    return get_tok_close();
  }
  if(IS_TOK_FLOAT()) {
    if(!get_tok_float()) return 0;
    pos.dist_change = tok_fval;
    poss = POS_DIST_CHANGE;
    SKIP_WS();
  } else {
    return get_tok_close();
  }
  if(IS_TOK_FLOAT()) {
    if(!get_tok_float()) return 0;
    pos.dir_change = tok_fval;
    poss = POS_DIR_CHANGE;
    SKIP_WS();
  } else {
    return get_tok_close();
  }
  if(IS_TOK_FLOAT()) {
    if(!get_tok_float()) return 0;
    pos.body_facing_dir = tok_fval;
    poss = POS_BODY_FACING;
    SKIP_WS();
  } else {
    return get_tok_close();
  }
  if(IS_TOK_FLOAT()) {
    if(!get_tok_float()) return 0;
    pos.head_facing_dir = tok_fval;
    poss = POS_HEAD_FACING;
    SKIP_WS();
  } else {
    return get_tok_close();
  }
  return get_tok_close();
}

/*
  Parses objectinfo data

  Returns:
    boolean indicating the success of parsing.
*/
int get_objinfo()
{
  if(!get_tok_open()) {
    return 0;
  }
  if(!get_tok_open()) return 0;
  if(!get_tok_id()) return 0;
  if(strcmp(tok_sval, "player")==0
     || strcmp(tok_sval, "Player")==0) {
    bool_type really_see = *tok_sval=='p';
    player_spec ps;
    bool_type is_my_player;
    *ptr = term;
    SKIP_WS();
    if(IS_TOK_ID()) {
      if(!get_tok_id()) return 0;
      is_my_player = pscb.is_my_name(tok_sval);
      *ptr = term;
      SKIP_WS();
      if(IS_TOK_CLOSE()) {
	if(!get_tok_close()) return 0;
	SKIP_WS();
      } else {
#ifdef PARSE_WARNS
	fprintf(stderr, "warning: unprocessed player attributes\n");
#endif
	break_sexp(1);
      }
      ps = PL_TEAM;
    } else {
      if(!get_tok_close()) return 0;
      SKIP_WS();
      ps = PL_NONE;
    }
    SKIP_WS();
    if(!get_floats()) return 0;
    pscb.see_player(ps, really_see, is_my_player, poss, player_posp);
  } else if(strcmp(tok_sval, "ball")==0
     || strcmp(tok_sval, "Ball")==0) {
    bool_type really_see = *tok_sval=='b';
    float dist, ang;
    *ptr = term;
    SKIP_WS();
    if(!get_tok_close()) return 0;
    SKIP_WS();
    if(!get_floats()) return 0;
    pscb.see_ball(really_see, poss, ball_posp);
  } else if(strcmp(tok_sval, "goal")==0) {
    float dist, ang;
    char side;
    *ptr = term;
    SKIP_WS();
    if(!get_tok_id()) return 0;
    if(strcmp(tok_sval, "r")==0 || strcmp(tok_sval, "l")==0) {
      side = *tok_sval;
      *ptr = term;
      SKIP_WS();
      if(!get_tok_close()) return 0;
      SKIP_WS();
      if(!get_floats()) return 0;
      pscb.see_goal(side, poss, goal_posp);
    } else {
#ifdef PARSE_WARNS
      fprintf(stderr, "warning: unknown goal attribute: %s\n", tok_sval);
#endif
      *ptr = term;
      break_sexp(2);
    }
  } else {
#ifdef PARSE_WARNS
    fprintf(stderr, "warning: unknown object: %s\n", tok_sval);
#endif
    *ptr = term;
    break_sexp(2);
  }
  return 1;
}

/*  Parses soccerserver message
  
  Params:
    char *s - the message string
  Returns:
    1 on success (boolean true)
    On error, the return value is 0. The parse_errstr string
      contains the error description.
    The parser tries to ignore non-fatal errors but it emits
      a warning message on stderr.

  Notes:
    To actual extraction of parse data one must define
    parse_see_cb pscb;
    in another module

 */
int parse(char *s)
{
  start = s;
  ptr = start;

  parse_errstr = NULL;

  if(!get_tok_open()) return CMD_ERR;
  if(!get_tok_id()) return CMD_ERR;
  if(strcmp(tok_sval, "see")==0) {
    *ptr = term;
    SKIP_WS();
    if(!get_tok_int()) return CMD_SEE_ERR;
    pscb.see_new(tok_ival);
    SKIP_WS();
    while(IS_TOK_OPEN()) {
      if(!get_objinfo()) return CMD_SEE_ERR;
      SKIP_WS();
    };
    if(!get_tok_close()) return CMD_SEE_ERR;
    return CMD_SEE;
  } else if(strcmp(tok_sval, "init")==0) {
    *ptr = term;
    SKIP_WS();
    if(!get_tok_id()) return CMD_INIT_ERR;
    if(strcmp(tok_sval, "r")==0 || strcmp(tok_sval, "l")==0) {
      char side = *tok_sval;
      *ptr = term;
      SKIP_WS();
      if(!get_tok_int()) return CMD_INIT_ERR;
      picb.cmd_init(*tok_sval, tok_ival);
#ifdef PARSE_WARNS
      fprintf(stderr, "warning: skipping init params\n");
#endif
      *ptr = term;
      break_sexp(1);
    } else {
#ifdef PARSE_WARNS
      fprintf(stderr, "warning: unknown init attribute: %s\n", tok_sval);
#endif
      *ptr = term;
      break_sexp(1);
    }
  } else {
#ifdef PARSE_WARNS
    fprintf(stderr, "parse: warning: unknown command: %s\n", tok_sval);
#endif
    *ptr = term;
    return break_sexp(1) ? CMD_UNK : CMD_UNK_ERR;
  }
  
}
