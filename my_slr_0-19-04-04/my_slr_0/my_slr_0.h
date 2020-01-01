#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//G:    Dragonbook-4.1.2-(4.1)
#define TRUE 1
#define FALSE 0
#define ACC 1
#define ERR 1
#define ITEM_STACK_MAX 30
#define SYMBOL_STACK_MAX 100
#define V_COUNT_MAX 100
#define ITEM_COUNT_MAX 50
#define PRODUCTION_COUNT_MAX 20
#define STRING_MAX 100

char a_string[STRING_MAX];
//try ctrl+shift+l
int vt_count=6;     //useless soon
int vn_count=3;     //useless soon
int item_count=12;  //useless soon
char vt[6]={'+','*','(',')','i','#'};
char vn[3]={'E','T','F'};

typedef int ERROR;

typedef struct
{
    char base[SYMBOL_STACK_MAX];
    int top;
}SYMBOL_STACK;

typedef struct
{
    int base[ITEM_STACK_MAX];
    int top;
}ITEM_STACK;

typedef struct
{
    char left;
    char *right;
    int length;
}PRODUCTION;

typedef struct 
{
    char owner;//the VN
    char *conclude;
    int number;
}SET;

//define an item of the action table
typedef struct
{
    char type;  //hint: a,e,s,r
    int to;     //default:ERROR=0; ACC=1
}ACTION_TABLE;
//end of the defintion of ACTION_TABLE

//PRODUCTIONS
PRODUCTION productions[PRODUCTION_COUNT_MAX];

//TABLES:       action_table[][],goto_table[][]
ACTION_TABLE action_table[ITEM_COUNT_MAX][V_COUNT_MAX];
int goto_table[ITEM_COUNT_MAX][V_COUNT_MAX];

//STACKS:       analysis_stack,item_stack,input_stack
ITEM_STACK item_stack;
SYMBOL_STACK input_stack;
SYMBOL_STACK analysis_stack;

//SETS:         first,follow
SET first[V_COUNT_MAX],follow[V_COUNT_MAX];
