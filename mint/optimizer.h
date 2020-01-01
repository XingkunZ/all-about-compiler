
void peephole_opt()
{
    char asm_line1[MAX_LINE],asm_line2[MAX_LINE];
    FILE *in_asm;

    fclose(a_asm);
    system("rename a.asm ~a.asm");
    //生成临时文件
    in_asm =fopen("~a.asm","r");
    a_asm = fopen("a.asm","w");
}