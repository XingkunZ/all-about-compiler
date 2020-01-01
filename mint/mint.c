#include "mint.h"
#include "error.h"
#include "lexer.h"
#include "parser.h"
#include "generator.h"
#include "optimizer.h"

void init_mint()
{
    int i=0;
    for(i=0;i<MAX_TABLE;i++)
        quaternion_table[i].label=-1;
        //每个四元式的标签初始化为-1
}

void process_files()
{
    printf("====================== MINT COMPILER ======================\n");
    printf("# Input a source file name [eg.>a.txt]\n >");
    gets(filename);
    in_txt=fopen(filename,"r");
    out_txt=fopen("quaternions.txt","w");
    error_txt=fopen("error.txt","w");
    a_asm=fopen("a.asm","w");
}

void parser()
{
    get_symbol();
    program();
    if(error_flag)
        printf("# Error happens!Check [error.txt] for more details.\n");
    else 
    {
        printf("# [%s] is successfully compiled.\n",filename);
    }
}

void print_quaternions()
{
    int i=0;
    for(i=0;i<qtable_index;i++)
    {
        if(quaternion_table[i].label != -1)
        {
            fprintf(out_txt,"%s:\n",label_table[quaternion_table[i].label]);
        }
        fprintf(out_txt,"%10s %10s %10s %10s\n",
        quaternion_op_name[quaternion_table[i].op],quaternion_table[i].operand_a,quaternion_table[i].operand_b,quaternion_table[i].operand_result);
    }
}

void generate_optimize()
{
    make_asm();
    // peephole_opt();
}

void close_files()
{
    fclose(in_txt);
	fclose(out_txt);
	fclose(error_txt);
	fclose(a_asm);
}

void execute()
{
    char do_exe;
    if(!error_flag)
    {
        //system(".\\masm32\\bin\\ml.exe /c /coff a.asm");
        // system(".\\masm32\\bin\\link.exe /subsystem:console a.obj");
        system(".\\ml.exe /c /coff a.asm");
        printf("# Has assembled ASM file.\n");
        system(".\\link.exe /subsystem:console a.obj");
        printf("# Has linked OBJ file.\n");
        
        printf("# Quaternions are in [quaternions.txt].\n");
        printf("# Machine instructions are in [a.asm].\n");
    
        printf("# Do you need to execute it?[y/n]\n >");
        while(1)
        {
            do_exe=getchar();
            getchar();//\0
            if(do_exe=='y' || do_exe=='n') break;
            else
            {
                printf("# Pleace input again.[y/n]\n >");
            }
        }
        if(do_exe=='y')
        {
            printf("--------------------EXECUTION-------------------\n");
            system("a.exe");
        }
    }
}

void end_mint()
{
    if(!error_flag)
    {
        //程序运行结束
        printf("\n");
        printf("-----------------END OF EXECUTION---------------\n");
    }
    printf("===========================================================\n");
}

int main()
{
    init_mint();
    process_files();
    parser();
    print_quaternions();
    generate_optimize();
    close_files();
    execute();
    end_mint();
    return 0;
}