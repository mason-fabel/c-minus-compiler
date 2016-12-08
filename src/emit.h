#ifndef EMIT_CODE_H__
#define EMIT_CODE_H__

#include <stdio.h>

#define NONE -1
#define GP    0
#define FP    1
#define RT    2
#define AC    3
#define AC1   4
#define AC2   5
#define AC3   6
#define PC    7

#define NO_COMMENT (char *)""

#define TraceCode   1

void emitSetFile(FILE* f);
void emitBackup(int loc);
void emitComment(char *c);
void emitComment(char *c, char *cc);
void emitGoto(int d, int s, char *c);
void emitGoto(int d, int s, char *c, char *cc);
void emitGotoAbs(int a, char *c);
void emitGotoAbs(int a, char *c, char *cc);
void emitRM(char *op, int r, int d, int s, char *c);
void emitRM(char *op, int r, int d, int s, char *c, char *cc);
void emitRMAbs(char *op, int r, int a, char *c);
void emitRMAbs(char *op, int r, int a, char *c, char *cc);
void emitRO(char *op, int r, int s, int t, char *c);
void emitRO(char *op, int r, int s, int t, char *c, char *cc);
void backPatchAJumpToHere(int addr, char *comment);
void backPatchAJumpToHere(char *cmd, int reg, int addr, char *comment);
void emitLit(char *s);
int emitSkip(int howMany);

#endif
