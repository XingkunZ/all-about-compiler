#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

//------DEFINE------
#define MAX_FILE_NAME 50    //最大文件名长 
#define MAX_TABLE 200       //最大表项数
#define MAX_OP 15           //最长操作符长
#define MAX_NAME 80         //最长命名
#define MAX_LINE 80         //一行内最大的字符数
#define MAX_STRING 80       //最大字符串长
#define MAX_IDENTIFY 80     //最大单词长
#define MAX_LABEL 200       //标签数
#define MAX_STACK 20        //栈容量
#define MAX_ASSIGN 100      //assign_table容量
#define MAX_TEMP 100        //临时变量最大数

//------SYMBOL------
enum SYMBOL 
{
    IDEN, INTCON, CHARCON, STRCON, 
    CHARTK, CONSTTK, DOTK, ELSETK, FORTK, IFTK, INTTK,
    MAINTK, PRINTFTK, RETURNTK, SCANFTK, VOIDTK, WHILETK,SWITCHTK,
    PLUS, MINU, MULT, DIV, 
    LSS, LEQ, GRE, GEQ, EQL, NEQ, ASSIGN, 
    SEMICN, COMMA, 
    SQUO, DQUO, 
    LPARENT, RPARENT, LBRACK, RBRACK, LBRACE, RBRACE
};
//------RECORD------
typedef struct
{
    char name[MAX_NAME];
    //0     1      2    3     4      5     6
    //null  const  var  func  array  para  tmp
    int kind;
    //0     1     2     3
    //null  int   char  string
    int type;
    union
    {
        int int_value;
        char char_value;
        char* string_value;
    };
}RECORD;//符号表表项

char *RECORD_TYPE[]={
    "char", "const", "do", "else", "for", "if", "int",
    "main", "printf", "return", "scanf", "void", "while","switch"
};//14 types

typedef struct
{
    char name[MAX_NAME];
    int type;//int:1, char:2
    int offset;
}ASSIGN_ADDR;//地址分配项

//------QUATERNION------
typedef enum quaternion_op
{
    ADD, SUB, MUL, DIVV, NEG, MOV,//赋值
    JZ, JNZ, JL, JLE, JG, JGE, JE,
    JNE, RET, WRITE, READ, PARAV,
    CALL, JMP, NOP, MAINF, EMAINF,
    FUNC, EFUNC, CONST, VAR
}QUATERNION_OP;

typedef struct
{
    QUATERNION_OP op;             //op
    char operand_a[MAX_OP];       //a
    char operand_b[MAX_OP];       //b
    char operand_result[MAX_OP];  //result
    int label;                    //init:-1
}QUATERNION;

const char* quaternion_op_name[]={
    "ADD", "SUB", "MUL", "DIVV", "NEG", "MOV",
    "JZ", "JNZ", "JL", "JLE", "JG", "JGE", "JE",
    "JNE", "RET", "WRITE", "READ", "PARAV",
    "CALL", "JMP", "NOP", "MAINF", "EMAINF",
    "FUNC", "EFUNC", "CONST", "VAR"
};

//------GLOBAL VAR------
RECORD table[MAX_TABLE];//符号表
QUATERNION quaternion_table[MAX_TABLE];//四元式表
char label_table[MAX_LABEL][MAX_LABEL];//标签表
char string_table[200][MAX_STRING];//字符串表
ASSIGN_ADDR assignaddr_table[MAX_ASSIGN];//地址变量分配表
int table_index=0;      //符号表当前指针
int qtable_index=0; //四元式表当前指针
int ltable_index=0; //标签表当前指针
int stable_index=0; //字符串表指针
int at=0;           //变量地址分配指针

int qt=0;           //遍历四元式的指针
int st=0;           //遍历字符串表的指针
int esp=0;          //栈顶指针
int temp_addr=0;    //临时变量分配起始地址

//FILE
FILE *in_txt, *out_txt, *error_txt,*a_asm;
char filename[MAX_FILE_NAME];//读入文件名

//GLOBAL
int cur_char_count=0;   //当前行内的读入字符数
int cur_word_len=0;     //当前单词长
int line_len=0;         //行长
int line_count=0;       //读入行数

int type;       //char:2,int:1
int cur_int=0;  //最后读入的整数
int is_signed;  //最后读入的整数类型：signed:1, unsigned:0;
char cur_char=' ';      //当前读入的字符
char cur_const_char=0;  //当前读入的字符常量
char const_string[MAX_STRING];  //最后读入的字符串常量
int is_global=0;                //当前变量是：全局1 / 局部0
char cur_line[MAX_LINE]={0};        //当前行
char cur_identify[MAX_IDENTIFY]={0};//最后读入的标识符

int para_in_table=0;    //参数在符号表中的起始位置
int localvar_in_table;  //局部变量在符号表中的起始位置
int tempvar_count=0;    //临时变量分配counter

int end_of_file=0;  //是否读到文件尾
int error_flag=0;   //是否出错
int has_return=0;   //是否已经合法返回

enum SYMBOL symbol_tag;//标识符类型

//------FUCTIONS------
//mint.c
//初始化
void init_mint();
//处理输入输出文件
void process_files();
//关闭打开的文件
void close_files();
//自顶向下的语法分析器
void parser();
//编译结束的格式化输出
void end_mint();
//打印四元式表
void print_quaternions();
//优化与生成汇编文件
void generate_optimize();
//汇编、链接、执行
void execute();

//error.h
void error(int index); //输出错误信息

//lexer.h
void get_char();    //获取下一个字符
void get_symbol();  //获取下一个单词

//parser.h
void program();
void const_describe();
void const_define();
void variable_describe();
void variable_define();
void int_number();
void with_return_function_define();
void no_return_function_define();
void main_function_define();
void compound_statement();
void statement_list();
void statement();
void if_statement();
void for_statement();
void dowhile_statement();
void scanf_statement();
void printf_statement();
void return_statement();
void switch_statement();
void assign_statement(int pos);
int function_call(int pos);
void parameters();
void parameter_value_list(int pos);
int expression(int *flag);
int term(int *flag,int *index,int *index_type);
int factor(int *flag,int *index,int *index_type);
int condition(int res);
int step_length();

int table_position(char *name);
void put_in_table(char *name,int kind,int type,int int_value,char char_value,char *string_value);
void put_in_quaternion_table(QUATERNION_OP op,char *a,char *b,char *r);
int make_quaternion_operand(char *q_op,int flag,int index, int index_type,int pos);
int allocate_tempvar(char *name,int type);
int put_in_label_table();
int put_in_string_table();

//generator.h
void make_asm();    //生成a.asm文件
void asm_head();    //填写.head文件头
void asm_data();    //填写.data部分
void asm_code();    //填写.code部分
//放入分配地址表/分配地址；
void put_in_assignaddr_table(char *name,int length,int type);
//找到地址分配表中的（局部/全局）变量的偏移量并返回；-1:not var;
int get_var_offset(char *xname);
//找到地址分配表中的数组的偏移量并返回；
//return:name_offset:-2--非数组/立即数/全局变量;-1--other situation;
//index_offset：-1--非数组/立即数/全局变量; 
//index_type: 0--local var; -1--not array; 1--not local var
int get_array_offset(const char *xname,int *xoffset,int *xtype);
//获取四元式操作符的类型；
int type_of(char *name);
//执行函数
    void _add();
    void _sub();
    void _mul();
    void _divv();
    void _neg();
    void _mov();
    void _ret();
    void _write();
    void _read();
    void _parav();
    void _call();
    void _jmp();
    void _j();
    void _nop();
    //处理：全局/局部的name[全局/局部的index];
    void _array(char *name,int name_offset,int index_offset,int index_type);

//optimizer.h
//窥孔优化
void peephole_opt();