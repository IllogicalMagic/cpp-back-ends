#ifndef __CALC_TABLE_H__
#define __CALC_TABLE_H__

namespace SemTable
{

  // Tree token table
  enum Token {ST_SEQ,
	      CLEAR,
	      WHILE,DO,
	      IF,
	      FUNC,
	      ARR_DECL,
	      ASSIGN,
	      NUM,NAME,
	      INC_P,DEC_P,INC,DEC,
	      CMP,LESS,LESS_EQ,GR,GR_EQ,EQUAL,NOT_EQUAL,
	      OR,AND,
	      UN_PLUS,UN_MINUS,NOT,
	      ARR_ACCESS,
	      TERN,
	      PLUS,MINUS,
	      MUL,DIV};
}

#endif
