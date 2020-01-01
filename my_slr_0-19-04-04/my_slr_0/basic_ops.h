#include "my_slr_0.h"
//FUNCTIONS DECLARE:
void initialize();
void initialize_ACTION_TABLE(ACTION_TABLE action_table[][V_COUNT_MAX]);
void initialize_GOTO_TABLE(int goto_table[][V_COUNT_MAX]);
void initialize_ITEM_STACK(ITEM_STACK item_stack);
void initialize_SYMBOL_STACK(SYMBOL_STACK input_stack);
void initialize_PRODUCTION(PRODUCTION productions[PRODUCTION_COUNT_MAX]);
void initialize_SET(SET sets[V_COUNT_MAX]);
ITEM_STACK push_item(ITEM_STACK this_stack,int i);
ITEM_STACK pop_item(ITEM_STACK this_stack);
SYMBOL_STACK push_symbol(SYMBOL_STACK this_stack,char x);
SYMBOL_STACK pop_symbol(SYMBOL_STACK this_stack);
void reduction();
void input_string();
int get_num(char x);
void print_p(PRODUCTION production);

//FUNCTIONS:
//stacks things:
ITEM_STACK push_item(ITEM_STACK this_stack,int i)
{
    this_stack.base[this_stack.top++]=i;
    return this_stack;
}

ITEM_STACK pop_item(ITEM_STACK this_stack)
{
    this_stack.base[--this_stack.top]=0;
    return this_stack;
}

SYMBOL_STACK push_symbol(SYMBOL_STACK this_stack,char x)
{
    this_stack.base[this_stack.top++]=x;
    return this_stack;
}

SYMBOL_STACK pop_symbol(SYMBOL_STACK this_stack)
{
    this_stack.top--;
    this_stack.base[this_stack.top]='\0';
    return this_stack;
}
//end of ADT of stack

void initialize_ITEM_STACK(ITEM_STACK this_stack)
{
    memset(this_stack.base,0,sizeof(this_stack.base));
    this_stack.top=0;
}

void initialize_SYMBOL_STACK(SYMBOL_STACK this_stack)
{
    memset(this_stack.base,0,sizeof(this_stack.base));
    this_stack.top=0;
}

int get_num(char x)
{
    int i=0,j=0;
    for(i=0;i<vt_count;i++)
    {
        if(vt[i]==x)
            return i;
    }
    for(j=0;j<vn_count;j++)
    {
        if(vn[j]==x)
            return j;
    }
    return -1;
}

void print_p(PRODUCTION production)
{
    printf("%c->%s\n",production.left,production.right);
    return;
}
