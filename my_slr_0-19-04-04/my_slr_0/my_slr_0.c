#include "basic_ops.h"
//G:    Dragonbook-4.1.2-(4.1)

//FUNCTIONS:
void initialize_PRODUCTION(PRODUCTION productions[PRODUCTION_COUNT_MAX])
//action_table[][].to will refer to productions[x]
//productions[action[][].to].length   is an int
//productions[action[][].to].left     is a char
//productions[action[][].to].right    is a string
{
    memset(productions,0,sizeof(productions));

    productions[0].left='S'; productions[0].right="E";
    productions[0].length=strlen(productions[0].right);

    productions[1].left='E'; productions[1].right="E+T";
    productions[1].length=strlen(productions[1].right);

    productions[2].left='E'; productions[2].right="T";
    productions[2].length=strlen(productions[2].right);

    productions[3].left='T'; productions[3].right="T*F";
    productions[3].length=strlen(productions[3].right);

    productions[4].left='T'; productions[4].right="F";
    productions[4].length=strlen(productions[4].right);

    productions[5].left='F'; productions[5].right="(E)";
    productions[5].length=strlen(productions[5].right);

    productions[6].left='F'; productions[6].right="i";
    productions[6].length=strlen(productions[6].right);

    return;
}

//  0 S->E
//  1 E->E+T
//  2 E->T
//  3 T->T*F
//  4 T->F
//  5 F->(E)
//  6 F->i

void initialize_ACTION_TABLE (ACTION_TABLE action_table[][V_COUNT_MAX])
{
    memset(action_table,0,sizeof(action_table));
    int i,j;
    for(i=0; i< item_count ;i++)
    {
        for(j=0; j< vt_count ;j++)
        {
            action_table[i][j].type='e';
            action_table[i][j].to=ERR;
            //each item is initialized as "error"
        }
    }
    //unable to construct this table
    //just write in...

    //  action_table
    //  +   *   (   )   i   #
    //  0   1   2   3   4   5
    action_table[0][2].type='s'; action_table[0][2].to=4;
    action_table[0][4].type='s'; action_table[0][4].to=5;

    action_table[1][0].type='s'; action_table[1][0].to=6;
    action_table[1][5].type='a'; action_table[1][5].to=ACC;

    action_table[2][0].type='r'; action_table[2][0].to=2;
    action_table[2][1].type='r'; action_table[2][1].to=2;
    action_table[2][3].type='r'; action_table[2][3].to=2;
    action_table[2][5].type='r'; action_table[2][5].to=2;

    action_table[3][0].type='r'; action_table[3][0].to=4;
    action_table[3][1].type='r'; action_table[3][1].to=4;
    action_table[3][3].type='r'; action_table[3][3].to=4;
    action_table[3][5].type='r'; action_table[3][5].to=4;

    action_table[4][2].type='s'; action_table[4][2].to=4;
    action_table[4][4].type='s'; action_table[4][4].to=5;

    action_table[5][0].type='r'; action_table[5][0].to=6;
    action_table[5][1].type='r'; action_table[5][1].to=6;
    action_table[5][3].type='r'; action_table[5][3].to=6;
    action_table[5][5].type='r'; action_table[5][5].to=6;

    action_table[6][2].type='s'; action_table[6][2].to=4;
    action_table[6][4].type='s'; action_table[6][4].to=5;

    action_table[7][2].type='s'; action_table[7][2].to=4;
    action_table[7][4].type='s'; action_table[7][4].to=5;

    action_table[8][3].type='s'; action_table[8][3].to=11;

    action_table[9][0].type='r'; action_table[9][0].to=1;
    action_table[9][1].type='s'; action_table[9][1].to=7;
    action_table[9][3].type='r'; action_table[9][3].to=1;
    action_table[9][5].type='r'; action_table[9][5].to=1;

    action_table[10][0].type='r'; action_table[10][0].to=3;
    action_table[10][1].type='r'; action_table[10][1].to=3;
    action_table[10][3].type='r'; action_table[10][3].to=3;
    action_table[10][5].type='r'; action_table[10][5].to=3;

    action_table[11][0].type='r'; action_table[11][0].to=5;
    action_table[11][1].type='r'; action_table[11][1].to=5;
    action_table[11][3].type='r'; action_table[11][3].to=5;
    action_table[11][5].type='r'; action_table[11][5].to=5;

    return;
}

void initialize_GOTO_TABLE(int goto_table[][V_COUNT_MAX])
{
    memset(goto_table,-1,sizeof(goto_table));
    //just write in...
    goto_table[0][0]=1,goto_table[0][1]=2,goto_table[0][2]=3;
    goto_table[4][0]=8,goto_table[4][1]=2,goto_table[4][2]=3;
    goto_table[6][1]=9,goto_table[6][2]=3;
    goto_table[7][2]=10;

    return;
}

void initialize_SET(SET sets[V_COUNT_MAX])
{
    memset(sets,0,sizeof(sets));
    int i;
    for(i=0;i<vn_count;i++)
    {
        sets[i].owner=vn[i];
        sets[i].conclude="";
        sets[i].number=0;
    }
}

void initialize()
{
    initialize_PRODUCTION(productions);

    initialize_ITEM_STACK(item_stack);
    initialize_SYMBOL_STACK(input_stack);
    initialize_SYMBOL_STACK(analysis_stack);

    initialize_SET(first);
    initialize_SET(follow);

    initialize_ACTION_TABLE(action_table);
    initialize_GOTO_TABLE(goto_table);

    item_stack=push_item(item_stack,0);
    input_stack=push_symbol(input_stack,'#');
    analysis_stack=push_symbol(analysis_stack,'#');

    return;
}

void input_string()
{
    printf("Input a string here:\n> ");
    scanf("%s",a_string);

    int i,len=strlen(a_string);
    for(i=len-1;i>=0;i--)
    {
        input_stack=push_symbol(input_stack,a_string[i]);
        //don't know why push_symbol doesn't work
    }
    return;
}

void reduction()
//  position is input_stack.top
//-------------------------------------
//  action_table          | goto_table
//-------------------------------------
//  +   *   (   )   i   # | E   T   F
//  0   1   2   3   4   5 | 0   1   2
//-------------------------------------
{
    int i,_item,_to,counter=0;
    char _new,_type,_jmp;
    PRODUCTION _production;
    ///_new:    the top symbol from input_stack
    ///_type:   the type of action from action_table
    ///_to:     the content of action from action_table
    ///_production: the production in the current reduction step

    while(TRUE)
    {
        //check the action table
        _new =input_stack.base[input_stack.top-1];
        _type = action_table[item_stack.base[item_stack.top-1]][get_num(_new)].type;
        _to = action_table[item_stack.base[item_stack.top-1]][get_num(_new)].to;
        //printf("item=%d,new symbol=%c,action=%c%d\n",item_stack.base[item_stack.top-1],_new,_type,_to);
        
        if(_type=='s')//shift
        {
            item_stack=push_item(
                item_stack,
                _to
            );
            analysis_stack=push_symbol(
                analysis_stack,
                _new
            );
            input_stack=pop_symbol(input_stack);
        }
        else if(_type=='r')//reduction
        {
            _production=productions[_to];
            for(i=0;i<_production.length;i++)
            {
                analysis_stack=pop_symbol(analysis_stack);
                item_stack=pop_item(item_stack);
            }
            analysis_stack=push_symbol(analysis_stack,_production.left);

            counter++;
            printf("Reduction %d: ",counter);
            print_p(_production);

            _jmp=analysis_stack.base[analysis_stack.top-1];
            _item=goto_table[item_stack.base[item_stack.top-1]][get_num(_jmp)];
            item_stack=push_item(item_stack,_item);
        }
        else if(_type=='a')//accept
        {
            counter++;
            printf("Reduction %d: ",counter);
            print_p(productions[0]);

            printf("End of reduction!\n");
            return;
        }
        else if(_type=='e')//error
        {
            printf("Error happens! ");
            printf("This string is illegal!\n");
            return;
        }
    }
}

int main()
{
    memset(a_string,0,sizeof(a_string));
    initialize();
    input_string();
    reduction();
    return 0;
}
