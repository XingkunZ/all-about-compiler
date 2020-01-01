//REF.
//https://www.cnblogs.com/yangmingyu/p/6928143.html
//https://blog.csdn.net/sinat_27382047/article/details/79306604
//https://blog.csdn.net/bjbz_cxy/article/details/79467688
//DISCRIPTION: make a_asm

void make_asm()
{
    asm_head();
    asm_data();
    asm_code();
}

void asm_head()
{
    fprintf(a_asm,".386\n");
    fprintf(a_asm,".model flat, stdcall\n");
    fprintf(a_asm,"option casemap :none\n");
    fprintf(a_asm, "\n");
    fprintf(a_asm,"includelib msvcrt.lib\n");
    fprintf(a_asm, "printf  PROTO C:ptr sbyte,:vararg\n");
    fprintf(a_asm, "scanf  PROTO C:ptr sbyte,:vararg\n");
    fprintf(a_asm,"\n");
}

void asm_data()
{
    fprintf(a_asm,".data\n");
    while(quaternion_table[qt].op==CONST)
    {
        fprintf(a_asm,"_%s\tDD\t%s\n",
        quaternion_table[qt].operand_b,quaternion_table[qt].operand_result);
        qt++;
    }
    while(quaternion_table[qt].op==VAR)
    {
        if(!strcmp(quaternion_table[qt].operand_result,""))
        {
            fprintf(a_asm,"_%s\tDD\t?\n",quaternion_table[qt].operand_b);
        }//未初始化
        else
        {
            fprintf(a_asm,"_%s\tDD\t%s dup(?)\n",
            quaternion_table[qt].operand_b,quaternion_table[qt].operand_result);
        }
        qt++;
    }
    int i;
    for(i=0;i<stable_index;i++)//string_table[]
    {
        fprintf(a_asm,"__str%d\tDB\t\"%s\",0\n",i,string_table[i]);
    }//字符串+'0'
    fprintf(a_asm,"_CHAR\tDB\t\"%%c\",0\n");
    fprintf(a_asm,"_INT\tDB\t\"%%d\",0\n");
    fprintf(a_asm,"\n");
}

void asm_code()
{
    fprintf(a_asm,".code\n");
    int i,j,counter;//temp
    int not_main;
    int len;
    char temp[MAX_NAME];
    char temp_table[MAX_TEMP][MAX_OP];//临时变量表
    int localx;
    while(quaternion_table[qt].op==FUNC||quaternion_table[qt].op==MAINF)
    {
        not_main=(quaternion_table[qt].op==MAINF)?0:1;
        if(not_main)
        {
            fprintf(a_asm,"_%s\tproc\n",quaternion_table[qt].operand_a);
        }
        else
        {
            fprintf(a_asm,"main\tproc\n");
        }
        fprintf(a_asm,"\tpush\tebp\n");
        fprintf(a_asm,"\tmov\tebp,\tesp\n");
        qt++;
        esp=4;//一个函数帧压入
        at=0;
        temp_addr=4;
        while(quaternion_table[qt].op==CONST)
        {
            sprintf(temp,"%%%d",at);
            if(!strcmp(quaternion_table[qt].operand_a,"INT"))
            {
                put_in_assignaddr_table(temp,1,1);
            }
            else//not int
            {
                put_in_assignaddr_table(temp,1,2);
            }
            fprintf(a_asm,"\tsub\tesp,\t4\n");
            fprintf(a_asm,"\tmov\tdword ptr [esp],\t%s\n",quaternion_table[qt].operand_result);
            qt++;
        }
        localx=esp;
        while(quaternion_table[qt].op==VAR)
        {
            sprintf(temp,"%%%d",at);
            if(!strcmp(quaternion_table[qt].operand_a,"INT"))
            {   //int 
                //array:len>=1
                len=strcmp(quaternion_table[qt].operand_result,"")?
                    atoi(quaternion_table[qt].operand_result):1;
                put_in_assignaddr_table(temp,len,1);
            }
            else //char
            {
                //array:len>=1
                len=strcmp(quaternion_table[qt].operand_result,"")?
                    atoi(quaternion_table[qt].operand_result):1;
                put_in_assignaddr_table(temp,len,2);
            }
            qt++;
        }
        //预留给函数的临时变量空间
        fprintf(a_asm,"\tsub\tesp,\t%d\n",esp-localx);
        if(not_main)
        {
            fprintf(a_asm,"\tpush\tebx\n");
            fprintf(a_asm,"\tpush\tedi\n");
            fprintf(a_asm,"\tpush\tesi\n");
            esp=esp+3*4;
        }
        temp_addr=esp;//开始分配临时变量
        counter=0;
        for(i=qt;quaternion_table[i].op !=EFUNC && quaternion_table[i].op!=EMAINF;i++)
        {
            if(quaternion_table[i].operand_result[0]!='$')
                continue;//not temp
            for(j=0;j<counter;j++)
            {
                if(!strcmp(quaternion_table[i].operand_result,temp_table[j]))
                    break;
            }
            if(j==counter)
            {
                strcpy(temp_table[counter],quaternion_table[i].operand_result);
                counter++;
            }
        }
        fprintf(a_asm,"\tsub\tesp,\t%d\n",counter*4);
        //execute_func();
        while(quaternion_table[qt].op!=EFUNC && quaternion_table[qt].op!=EMAINF)
        {
            switch(quaternion_table[qt].op)
            {
                case ADD: _add();break;
                case SUB: _sub();break;
                case MUL: _mul();break;
                case DIVV: _divv();break;
                case NEG: _neg();break;
                case MOV: _mov();break;
                case RET: _ret();break;
                case WRITE: _write();break;
                case READ: _read();break;
                case PARAV: _parav();break;
                case CALL: _call();break;
                case JMP: _jmp();break;
                //process multiple types of jumps
                case JZ:
                case JNZ:
                case JL:
                case JLE:
                case JG:
                case JGE:
                case JE:
                case JNE: _j(quaternion_table[qt].op);break;
                case NOP: _nop();break;
                // these are processed below
                default:;
            }
            qt++;
        }
        if(quaternion_table[qt].label!=-1)
        {
            fprintf(a_asm,"%s:\n",label_table[quaternion_table[qt].label]);
        }
        if(not_main)
        {
            fprintf(a_asm, "\tmov\tesp,\tebp\n");
            fprintf(a_asm, "\tsub\tesp,\t%d\n", temp_addr-4);
            fprintf(a_asm, "\tpop\tesi\n");
            fprintf(a_asm, "\tpop\tedi\n");
            fprintf(a_asm, "\tpop\tebx\n");

            fprintf(a_asm, "\tmov\tesp,\tebp\n");
            fprintf(a_asm, "\tpop\tebp\n");
            fprintf(a_asm, "\tret\n");

            fprintf(a_asm,"_%s\tendp\n",quaternion_table[qt].operand_a);
            qt++;
        }
        else //main
        {
            fprintf(a_asm, "\tmov\tesp,\tebp\n");
            fprintf(a_asm, "\tpop\tebp\n");
            fprintf(a_asm, "\tret\n");
            
            fprintf(a_asm, "main\tendp\n");
            fprintf(a_asm, "end main\n");
            return;//循环唯一出口
        }
    }
}

//每个运算前都添加标签
void _add()
{
    if(quaternion_table[qt].label != -1)
    {
        fprintf(a_asm,"%s:\n",label_table[quaternion_table[qt].label]);
    }
    //temp register
    if(quaternion_table[qt].operand_result[0]=='$')
    {
        put_in_assignaddr_table(quaternion_table[qt].operand_result,1,1);
    }
    int index_offset,index_type;//to be assigned
    //var?
    //name_offset1
    int name_offset1 = get_var_offset(quaternion_table[qt].operand_a);
    if(name_offset1==-1)//not var
    {
        //array?
        name_offset1=get_array_offset(quaternion_table[qt].operand_a,&index_offset,&index_type);
        if(name_offset1==-2)//立即数/全局变量
        {
            if(isalpha(quaternion_table[qt].operand_a[0]))//全局变量
                fprintf(a_asm,"\tmov\teax,\t_%s\n",quaternion_table[qt].operand_a);
            else//立即数
                fprintf(a_asm,"\tmov\teax,\t%s\n",quaternion_table[qt].operand_a);
        }
        else
        {
            _array(quaternion_table[qt].operand_a,name_offset1,index_offset,index_type);
            fprintf(a_asm,"\tmov\teax,\t[esi]\n");
        }
    }
    else if(quaternion_table[qt].operand_a[0]=='@')
    {
        fprintf(a_asm,"\tmov\teax,\tdword ptr [ebp+%d]\n",name_offset1);
    }
    else
    {
        fprintf(a_asm,"\tmov\teax,\tdword ptr [ebp-%d]\n",name_offset1);
    }
    //name_offset2
    int name_offset2 = get_var_offset(quaternion_table[qt].operand_b);
    if(name_offset2==-1)//not var
    {
        //array?
        name_offset2=get_array_offset(quaternion_table[qt].operand_b,&index_offset,&index_type);
        if(name_offset2==-2)//立即数/全局变量
        {
            if(isalpha(quaternion_table[qt].operand_b[0]))//全局变量
                fprintf(a_asm,"\tadd\teax,\t_%s\n",quaternion_table[qt].operand_b);
            else//立即数
                fprintf(a_asm,"\tadd\teax,\t%s\n",quaternion_table[qt].operand_b);
        }
        else
        {
            _array(quaternion_table[qt].operand_b,name_offset2,index_offset,index_type);
            fprintf(a_asm,"\tadd\teax,\t[esi]\n");
        }
    }
    else if(quaternion_table[qt].operand_b[0]=='@')
    {
        fprintf(a_asm,"\tadd\teax,\tdword ptr [ebp+%d]\n",name_offset2);
    }
    else
    {
        fprintf(a_asm,"\tadd\teax,\tdword ptr [ebp-%d]\n",name_offset2);
    }
    //name_offset3
    int name_offset3 = get_var_offset(quaternion_table[qt].operand_result);
    if(name_offset3==-1)//not var
    {
        //array?
        name_offset3=get_array_offset(quaternion_table[qt].operand_result,&index_offset,&index_type);
        if(name_offset3==-2)//立即数/全局变量
        {
            if(isalpha(quaternion_table[qt].operand_result[0]))//全局变量
                fprintf(a_asm,"\tmov\t_%s,\teax\n",quaternion_table[qt].operand_result);
            else//立即数
                fprintf(a_asm,"\tmov\t%s,\teax\n",quaternion_table[qt].operand_result);
        }
        else
        {
            _array(quaternion_table[qt].operand_result,name_offset3,index_offset,index_type);
            fprintf(a_asm,"\tmov\t[esi],\teax\n");
        }
    }
    else if(quaternion_table[qt].operand_result[0]=='@')
    {
        fprintf(a_asm,"\tmov\tdword ptr [ebp+%d],\teax\n",name_offset3);
    }
    else
    {
        fprintf(a_asm,"\tmov\tdword ptr [ebp-%d],\teax\n",name_offset3);
    }
}

void _sub()
{
    int index_offset,index_type;
    if(quaternion_table[qt].label != -1)
    {
        fprintf(a_asm,"%s:\n",label_table[quaternion_table[qt].label]);
    }
    if(quaternion_table[qt].operand_result[0]=='$')
    {
        put_in_assignaddr_table(quaternion_table[qt].operand_result,1,1);
    }
    //name_offset1
    int name_offset1 = get_var_offset(quaternion_table[qt].operand_a);
    if(name_offset1==-1)//not var
    {
        //array?
        name_offset1=get_array_offset(quaternion_table[qt].operand_a,&index_offset,&index_type);
        if(name_offset1==-2)//立即数/全局变量
        {
            if(isalpha(quaternion_table[qt].operand_a[0]))//全局变量
                fprintf(a_asm,"\tmov\teax,\t_%s\n",quaternion_table[qt].operand_a);
            else//立即数
                fprintf(a_asm,"\tmov\teax,\t%s\n",quaternion_table[qt].operand_a);
        }
        else
        {
            _array(quaternion_table[qt].operand_a,name_offset1,index_offset,index_type);
            fprintf(a_asm,"\tmov\teax,\t[esi]\n");
        }
    }
    else if(quaternion_table[qt].operand_a[0]=='@')
    {
        fprintf(a_asm,"\tmov\teax,\tdword ptr [ebp+%d]\n",name_offset1);
    }
    else
    {
        fprintf(a_asm,"\tmov\teax,\tdword ptr [ebp-%d]\n",name_offset1);
    }
    //name_offset2
    int name_offset2 = get_var_offset(quaternion_table[qt].operand_b);
    if(name_offset2==-1)//not var
    {
        //array?
        name_offset2=get_array_offset(quaternion_table[qt].operand_b,&index_offset,&index_type);
        if(name_offset2==-2)//立即数/全局变量
        {
            if(isalpha(quaternion_table[qt].operand_b[0]))//全局变量
                fprintf(a_asm,"\tsub\teax,\t_%s\n",quaternion_table[qt].operand_b);
            else//立即数
                fprintf(a_asm,"\tsub\teax,\t%s\n",quaternion_table[qt].operand_b);
        }
        else
        {
            _array(quaternion_table[qt].operand_b,name_offset2,index_offset,index_type);
            fprintf(a_asm,"\tsub\teax,\t[esi]\n");
        }
    }
    else if(quaternion_table[qt].operand_b[0]=='@')
    {
        fprintf(a_asm,"\tsub\teax,\tdword ptr [ebp+%d]\n",name_offset2);
    }
    else
    {
        fprintf(a_asm,"\tsub\teax,\tdword ptr [ebp-%d]\n",name_offset2);
    }
    //name_offset3
    int name_offset3 = get_var_offset(quaternion_table[qt].operand_result);
    if(name_offset3==-1)//not var
    {
        //array?
        name_offset3=get_array_offset(quaternion_table[qt].operand_result,&index_offset,&index_type);
        if(name_offset3==-2)//立即数/全局变量
        {
            if(isalpha(quaternion_table[qt].operand_result[0]))//全局变量
                fprintf(a_asm,"\tmov\t_%s,\teax\n",quaternion_table[qt].operand_result);
            else//立即数
                fprintf(a_asm,"\tmov\t%s,\teax\n",quaternion_table[qt].operand_result);
        }
        else
        {
            _array(quaternion_table[qt].operand_result,name_offset3,index_offset,index_type);
            fprintf(a_asm,"\tmov\t[esi],\teax\n");
        }
    }
    else if(quaternion_table[qt].operand_result[0]=='@')
    {
        fprintf(a_asm,"\tmov\tdword ptr [ebp+%d],\teax\n",name_offset3);
    }
    else
    {
        fprintf(a_asm,"\tmov\tdword ptr [ebp-%d],\teax\n",name_offset3);
    }
}

void _mul()
{
    int index_offset,index_type;
    if(quaternion_table[qt].label != -1)
    {
        fprintf(a_asm,"%s:\n",label_table[quaternion_table[qt].label]);
    }
    //temp register
    if(quaternion_table[qt].operand_result[0]=='$')
    {
        put_in_assignaddr_table(quaternion_table[qt].operand_result,1,1);
    }
    //name_offset1
    int name_offset1=get_var_offset(quaternion_table[qt].operand_a);
    if(name_offset1==-1)
    {
        name_offset1=get_array_offset(quaternion_table[qt].operand_a,&index_offset,&index_type);
        if(name_offset1==-2)//立即数/全局变量
        {
            if(isalpha(quaternion_table[qt].operand_a[0]))
            {
                fprintf(a_asm,"\tmov\teax,\t_%s\n",quaternion_table[qt].operand_a);
            }
            else
            {
                fprintf(a_asm,"\tmov\teax,\t%s\n",quaternion_table[qt].operand_a);
            }
        }
        else
        {
            _array(quaternion_table[qt].operand_a,name_offset1,index_offset,index_type);
            fprintf(a_asm,"\tmov\teax,\t%s\n",quaternion_table[qt].operand_a);
        }
    }
    else if(quaternion_table[qt].operand_a[0]=='@')
    {
        fprintf(a_asm,"\tmov\teax,\tdword ptr [ebp+%d]\n",name_offset1);
    }
    else
    {
        fprintf(a_asm,"\tmov\teax,\tdword ptr [ebp-%d]\n",name_offset1);
    }
    fprintf(a_asm,"\tmov\tedx,\teax\n");
    fprintf(a_asm,"\tsar\tedx,\t31\n");
    //name_offset2
    int name_offset2=get_var_offset(quaternion_table[qt].operand_b);
    if(name_offset2==-1)
    {
        name_offset2=get_array_offset(quaternion_table[qt].operand_b,&index_offset,&index_type);
        if(name_offset2==-2)//全局变量/立即数
        {
            if(isalpha(quaternion_table[qt].operand_b[0]))
            {
                fprintf(a_asm,"\timul\t_%s\n",quaternion_table[qt].operand_b);
            }
            else
            {
                fprintf(a_asm,"\tmov\tecx,\t%s\n",quaternion_table[qt].operand_b);
                fprintf(a_asm,"\timul\tecx\n");
            }
            
        }
        else
        {
            _array(quaternion_table[qt].operand_b,name_offset2,index_offset,index_type);
            fprintf(a_asm,"\timul\t[esi]\n");
        }
    }
    else if(quaternion_table[qt].operand_b[0]=='@')
    {
        fprintf(a_asm,"\timul\tdword ptr [ebp+%d]\n",name_offset2);
    }
    else
    {
        fprintf(a_asm,"\timul\tdword ptr [ebp-%d]\n",name_offset2);
    }
    //name_offset3
    int name_offset3=get_var_offset(quaternion_table[qt].operand_result);
    if(name_offset3==-1)
    {
        name_offset3=get_array_offset(quaternion_table[qt].operand_result,&index_offset,&index_type);
        if(name_offset3==-2)//全局变量/立即数
        {
            if(isalpha(quaternion_table[qt].operand_result[0]))
            {
                fprintf(a_asm,"\tmov\t_%s,\teax\n",quaternion_table[qt].operand_result);
            }
            else
            {
                fprintf(a_asm,"\tmov\t%s,\teax\n",quaternion_table[qt].operand_result);
            }
            
        }
        else
        {
            _array(quaternion_table[qt].operand_result,name_offset3,index_offset,index_type);
            fprintf(a_asm,"\tmov\t[esi],\teax\n");
        }
    }
    else if(quaternion_table[qt].operand_result[0]=='@')
    {
        fprintf(a_asm,"\tmov\tdword ptr [ebp+%d],\teax\n",name_offset3);
    }
    else
    {
        fprintf(a_asm,"\tmov\tdword ptr [ebp-%d],\teax\n",name_offset3);
    }
}

void _divv()
{
    int index_offset,index_type;
    if(quaternion_table[qt].label != -1)
    {
        fprintf(a_asm,"%s:\n",label_table[quaternion_table[qt].label]);
    }
    //temp register
    if(quaternion_table[qt].operand_result[0] == '$')
    {
        put_in_assignaddr_table(quaternion_table[qt].operand_result,1,1);
    }
    //name_offset1
    int name_offset1=get_var_offset(quaternion_table[qt].operand_a);
    if(name_offset1==-1)
    {
        name_offset1=get_array_offset(quaternion_table[qt].operand_a,&index_offset,&index_type);
        if(name_offset1==-2)//立即数/全局变量
        {
            if(isalpha(quaternion_table[qt].operand_a[0]))
            {
                fprintf(a_asm,"\tmov\teax,\t_%s\n",quaternion_table[qt].operand_a);
            }
            else
            {
                fprintf(a_asm,"\tmov\teax,\t%s\n",quaternion_table[qt].operand_a);
            }
        }
        else
        {
            _array(quaternion_table[qt].operand_a,name_offset1,index_offset,index_type);
            fprintf(a_asm,"\tmov\teax,\t[esi]\n",quaternion_table[qt].operand_a);
        }
    }
    else if(quaternion_table[qt].operand_a[0]=='@')
    {
        fprintf(a_asm,"\tmov\teax,\tdword ptr [ebp+%d]\n",name_offset1);
    }
    else
    {
        fprintf(a_asm,"\tmov\teax,\tdword ptr [ebp-%d]\n",name_offset1);
    }
    fprintf(a_asm,"\tmov\tedx,\teax\n");
    fprintf(a_asm,"\tsar\tedx,\t31\n");
    //name_offset2
    int name_offset2=get_var_offset(quaternion_table[qt].operand_b);
    if(name_offset2==-1)
    {
        name_offset2=get_array_offset(quaternion_table[qt].operand_b,&index_offset,&index_type);
        if(name_offset2==-2)//全局变量/立即数
        {
            if(isalpha(quaternion_table[qt].operand_b[0]))
            {
                fprintf(a_asm,"\timul\t_%s\n",quaternion_table[qt].operand_b);
            }
            else
            {
                fprintf(a_asm,"\tmov\tecx,\t%s\n",quaternion_table[qt].operand_b);
                fprintf(a_asm,"\tidiv\tecx\n");
            }
        }
        else
        {
            _array(quaternion_table[qt].operand_b,name_offset2,index_offset,index_type);
            fprintf(a_asm,"\tidiv\t[esi]\n");
        }
    }
    else if(quaternion_table[qt].operand_b[0]=='@')
    {
        fprintf(a_asm,"\tidiv\tdword ptr [ebp+%d]\n",name_offset2);
    }
    else
    {
        fprintf(a_asm,"\tidiv\tdword ptr [ebp-%d]\n",name_offset2);
    }
    //name_offset3
    int name_offset3=get_var_offset(quaternion_table[qt].operand_result);
    if(name_offset3==-1)
    {
        name_offset3=get_array_offset(quaternion_table[qt].operand_result,&index_offset,&index_type);
        if(name_offset3==-2)//全局变量/立即数
        {
            if(isalpha(quaternion_table[qt].operand_result[0]))
            {
                fprintf(a_asm,"\tmov\t_%s,\teax\n",quaternion_table[qt].operand_result);
            }
            else
            {
                fprintf(a_asm,"\tmov\t%s,\teax\n",quaternion_table[qt].operand_result);
            }
        }
        else
        {
            _array(quaternion_table[qt].operand_result,name_offset3,index_offset,index_type);
            fprintf(a_asm,"\tmov\t[esi],\teax\n");
        }
    }
    else if(quaternion_table[qt].operand_result[0]=='@')
    {
        fprintf(a_asm,"\tmov\tdword ptr [ebp+%d],\teax\n",name_offset3);
    }
    else
    {
        fprintf(a_asm,"\tmov\tdword ptr [ebp-%d],\teax\n",name_offset3);
    }
}
void _neg()
{
    int index_offset,index_type;
    if(quaternion_table[qt].label != -1)
    {
        fprintf(a_asm,"%s:\n",label_table[quaternion_table[qt].label]);
    }
    //name_offset1 (only one)
    int name_offset1=get_var_offset(quaternion_table[qt].operand_a);
    if(name_offset1==-1)
    {
        name_offset1=get_array_offset(quaternion_table[qt].operand_a,&index_offset,&index_type);
        if(name_offset1==-2)//立即数/全局变量
        {
            if(isalpha(quaternion_table[qt].operand_a[0]))
            {
                fprintf(a_asm,"\tneg\t_%s\n",quaternion_table[qt].operand_a);
            }
            else
            {
                fprintf(a_asm,"\tneg\t%s\n",quaternion_table[qt].operand_a);
            }
        }
        else
        {
            _array(quaternion_table[qt].operand_a,name_offset1,index_offset,index_type);
            fprintf(a_asm,"\tneg\t[esi]\n");
        }
    }
    else if(quaternion_table[qt].operand_a[0]=='@')
    {
        fprintf(a_asm,"\tneg\teax,\tdword ptr [ebp+%d]\n",name_offset1);
    }
    else
    {
        fprintf(a_asm,"\tneg\teax,\tdword ptr [ebp-%d]\n",name_offset1);
    }
}
void _mov()
{
    int index_offset,index_type;
    if(quaternion_table[qt].label != -1)
    {
        fprintf(a_asm,"%s:\n",label_table[quaternion_table[qt].label]);
    }
    //temp register
    if(quaternion_table[qt].operand_result[0] == '$')
    {
        put_in_assignaddr_table(quaternion_table[qt].operand_result,1,1);
    }
    //name_offset1
    int name_offset1=get_var_offset(quaternion_table[qt].operand_a);
    if(quaternion_table[qt].operand_a[0]=='~')//~eax
        ;
    else if(name_offset1==-1)
    {
        name_offset1=get_array_offset(quaternion_table[qt].operand_a,&index_offset,&index_type);
        if(name_offset1==-2)//立即数/全局变量
        {
            if(isalpha(quaternion_table[qt].operand_a[0]))
            {
                fprintf(a_asm,"\tmov\teax,\t_%s\n",quaternion_table[qt].operand_a);
            }
            else
            {
                fprintf(a_asm,"\tmov\teax,\t%s\n",quaternion_table[qt].operand_a);
            }
        }
        else
        {
            _array(quaternion_table[qt].operand_a,name_offset1,index_offset,index_type);
            fprintf(a_asm,"\tmov\teax,\t[esi]\n",quaternion_table[qt].operand_a);
        }
    }
    else if(quaternion_table[qt].operand_a[0]=='@')
    {
        fprintf(a_asm,"\tmov\teax,\tdword ptr [ebp+%d]\n",name_offset1);
    }
    else
    {
        fprintf(a_asm,"\tmov\teax,\tdword ptr [ebp-%d]\n",name_offset1);
    }
    //name_offset2
    int name_offset2=get_var_offset(quaternion_table[qt].operand_result);
    if(name_offset2==-1)
    {
        name_offset2=get_array_offset(quaternion_table[qt].operand_result,&index_offset,&index_type);
        if(name_offset2==-2)//全局变量/立即数
        {
            if(isalpha(quaternion_table[qt].operand_result[0]))
            {
                fprintf(a_asm,"\tmov\t_%s,\teax\n",quaternion_table[qt].operand_result);
            }
            else
            {
                fprintf(a_asm,"\tmov\t%s,\teax\n",quaternion_table[qt].operand_result);
            }
        }
        else
        {
            _array(quaternion_table[qt].operand_result,name_offset2,index_offset,index_type);
            fprintf(a_asm,"\tmov\t[esi],\teax\n");
        }
    }
    else if(quaternion_table[qt].operand_result[0]=='@')
    {
        fprintf(a_asm,"\tmov\tdword ptr [ebp+%d],\teax\n",name_offset2);
    }
    else
    {
        fprintf(a_asm,"\tmov\tdword ptr [ebp-%d],\teax\n",name_offset2);
    }
}
void _ret()
{
    int index_name,index_type;
    int name_offset1;
    if(quaternion_table[qt].label!=-1)
    {
        fprintf(a_asm,"%s:\n",label_table[quaternion_table[qt].label]);
    }
    //with return
    if(strcmp(quaternion_table[qt].operand_a,""))
    {
        name_offset1=get_var_offset(quaternion_table[qt].operand_a);
        if(name_offset1 == -1)
        {
            name_offset1=get_array_offset(quaternion_table[qt].operand_a,&index_name,&index_type);
            if(name_offset1 == -2)
            {
                if(isalpha(quaternion_table[qt].operand_a[0]))
                {
                    fprintf(a_asm,"\tmov\teax,\t_%s\n",quaternion_table[qt].operand_a);
                }
                else
                {
                    fprintf(a_asm,"\tmov\teax,\t%s\n",quaternion_table[qt].operand_a);                    
                }
            }
            else
            {
                _array(quaternion_table[qt].operand_a,name_offset1,index_name,index_type);
                fprintf(a_asm,"\tmov\teax,\t[esi]\n");
            }
        }
        else if(quaternion_table[qt].operand_a[0]=='@')
        {
            fprintf(a_asm,"\tmov\teax,\tdword ptr [ebp+%d]\n",name_offset1);
        }
        else
        {
            fprintf(a_asm,"\tmov\teax,\tdword ptr [ebp-%d]\n",name_offset1);
        }
    }
    fprintf(a_asm, "\tmov\tesp,\tebp\n");
    fprintf(a_asm, "\tsub\tesp,\t%d\n", temp_addr);
    fprintf(a_asm, "\tpop\tesi\n");
    fprintf(a_asm, "\tpop\tedi\n");
    fprintf(a_asm, "\tpop\tebx\n");
    fprintf(a_asm, "\tmov\tesp,\tebp\n");
    fprintf(a_asm, "\tpop\tebp\n");
    fprintf(a_asm, "\tret\n");
}
void _write()
{
    int index_name,index_type;
    int name_offset;
    if(quaternion_table[qt].label != -1)
    {
        fprintf(a_asm,"%s:\n",label_table[quaternion_table[qt].label]);
    }
    if(strcmp(quaternion_table[qt].operand_a,""))
    {
        fprintf(a_asm, "\tlea\teax,\t%s\n", quaternion_table[qt].operand_a);
        fprintf(a_asm, "\tpush\teax\n");
        fprintf(a_asm, "\tcall\tprintf\n");
        fprintf(a_asm, "\tadd\tesp,\t4\n");
    }
    if(strcmp(quaternion_table[qt].operand_b,""))
    {
        name_offset=get_var_offset(quaternion_table[qt].operand_b);
        if(name_offset==-1)
        {
            name_offset=get_array_offset(quaternion_table[qt].operand_b,&index_name,&index_type);
            if(name_offset==-2)
            {
                if(isalpha(quaternion_table[qt].operand_b[0]))
                {
                    fprintf(a_asm,"\tmov\teax,\t_%s\n",quaternion_table[qt].operand_b);
                }
                else
                {
                    fprintf(a_asm,"\tmov\teax,\t%s\n",quaternion_table[qt].operand_b);
                    
                }
            }
            else
            {
                _array(quaternion_table[qt].operand_b,name_offset,index_name,index_type);
                fprintf(a_asm,"\tmov\teax,\t[esi]\n");
            }
        }
        else if(quaternion_table[qt].operand_b[0]=='@')
        {
            fprintf(a_asm,"\tmov\teax,\tdword ptr [ebp+%d]\n",name_offset);
        }
        else
        {
            fprintf(a_asm,"\tmov\teax,\tdword ptr [ebp-%d]\n",name_offset);
        }
        fprintf(a_asm,"\tpush\teax\n");
        if(type_of(quaternion_table[qt].operand_b)==2)
        {
            fprintf(a_asm,"\tlea\teax,\t_CHAR\n");
        }
        else
        {
            fprintf(a_asm,"\tlea\teax,\t_INT\n");
        }
        fprintf(a_asm, "\tpush\teax\n");
        fprintf(a_asm, "\tcall\tprintf\n");
        fprintf(a_asm, "\tadd\tesp,\t8\n");
    }
}

void _read()
{
    int index_name,index_type;
    if(quaternion_table[qt].label != -1)
    {
        fprintf(a_asm,"%s:\n",label_table[quaternion_table[qt].label]);
    }
    int name_offset=get_var_offset(quaternion_table[qt].operand_result);
    if(name_offset==-1)
    {
        name_offset=get_array_offset(quaternion_table[qt].operand_result,&index_name,&index_type);
        if(name_offset==-2)//立即数/全局变量
        {
            fprintf(a_asm,"\tlea\teax,\t_%s\n",quaternion_table[qt].operand_result);
        }
        else
        {
            _array(quaternion_table[qt].operand_result,name_offset,index_name,index_type);
            fprintf(a_asm,"\tlea\teax,\t[esi]\n");
        }
    }
    else if(quaternion_table[qt].operand_a[0]=='@')
    {
        fprintf(a_asm,"\tlea\teax,\tdword ptr [ebp+%d]\n",name_offset);
    }
    else
    {
        fprintf(a_asm,"\tlea\teax,\tdword ptr [ebp-%d]\n",name_offset);
    }
    fprintf(a_asm,"\tpush\teax\n");
    if(type_of(quaternion_table[qt].operand_result)==2)
    {
        fprintf(a_asm,"\tlea\teax,\t_CHAR\n");
    }
    else
    {
        fprintf(a_asm,"\tlea\teax,\t_INT\n");
    }
    fprintf(a_asm, "\tpush\teax\n");
    fprintf(a_asm, "\tcall\tscanf\n");
    fprintf(a_asm, "\tadd\tesp,\t8\n");
}
void _parav()
{
    int index_name,index_type;
    int name_offset;
    if(quaternion_table[qt].label != -1)
    {
        fprintf(a_asm,"%s:\n",label_table[quaternion_table[qt].label]);
    }
    do
    {
        name_offset=get_var_offset(quaternion_table[qt].operand_a);
        if(name_offset==-1)
        {
            name_offset=get_array_offset(quaternion_table[qt].operand_a,&index_name,&index_type);
            if(name_offset==-2)//立即数/全局变量
            {
                if(isalpha(quaternion_table[qt].operand_a[0]))
                {
                    fprintf(a_asm,"\tpush\t_%s\n",quaternion_table[qt].operand_a);
                }
                else
                {
                    fprintf(a_asm,"\tpush\t%s\n",quaternion_table[qt].operand_a);
                }
            }
            else
            {
                _array(quaternion_table[qt].operand_a,name_offset,index_name,index_type);
                fprintf(a_asm,"\tpush\t[esi]\n");
            }
        }
        else if(quaternion_table[qt].operand_a[0]=='@')
        {
            fprintf(a_asm,"\tpush\tdword ptr [ebp+%d]\n",name_offset);
        }
        else
        {
            fprintf(a_asm,"\tpush\tdword ptr [ebp-%d]\n",name_offset);
        }
        qt++;
    } while (quaternion_table[qt].op == PARAV);
    qt--;
}

void _call()
{
    int counter=0;
    int pos=0;
    fprintf(a_asm,"\tcall\t_%s\n",quaternion_table[qt].operand_a);
    pos=table_position(quaternion_table[qt].operand_a);
    for(pos++;table[pos].kind==5;pos++)//para
    {
        counter++;
    }
    fprintf(a_asm,"\tadd\tesp,\t%d\n",counter*4);//leave space for parameters
}

void _jmp()
{
    //just jump
    fprintf(a_asm,"\tjmp\t%s\n",quaternion_table[qt].operand_result);
}
//不等时转移
void _j(QUATERNION_OP op)
{
    int index_name,index_type;
    if(quaternion_table[qt].label != -1)
    {
        fprintf(a_asm,"%s:\n",label_table[quaternion_table[qt].label]);
    }
    //name_offset1
    int name_offset1=get_var_offset(quaternion_table[qt].operand_a);
    if(name_offset1 == -1)
    {
        name_offset1=get_array_offset(quaternion_table[qt].operand_a,&index_name,&index_type);
        if(name_offset1 == -2)
        {
            if(isalpha(quaternion_table[qt].operand_a[0]))
                fprintf(a_asm,"\tmov\teax,\t_%s\n",quaternion_table[qt].operand_a);
            else
            {
                fprintf(a_asm,"\tmov\teax,\t%s\n",quaternion_table[qt].operand_a);
            }
        }
        else
        {
            _array(quaternion_table[qt].operand_a,name_offset1,index_name,index_type);
            fprintf(a_asm,"\tmov\teax,\t[esi]\n");
        }
    }
    else if(quaternion_table[qt].operand_a[0]=='@')
    {
        fprintf(a_asm,"\tmov\teax,\tdword ptr [ebp+%d]\n",name_offset1);
    }
    else
    {
        fprintf(a_asm,"\tmov\teax,\tdword ptr [ebp-%d]\n",name_offset1);
    }
    //name_offset2
    int name_offset2;
    if(strcmp(quaternion_table[qt].operand_b,""))
    {
        name_offset2=get_var_offset(quaternion_table[qt].operand_b);
        if(name_offset2==-1)
        {
            name_offset2=get_array_offset(quaternion_table[qt].operand_b,&index_name,&index_type);
            if(name_offset2 == -2)
            {
                if(isalpha(quaternion_table[qt].operand_b[0]))
                {
                    fprintf(a_asm,"\tmov\tecx,\t_%s\n",quaternion_table[qt].operand_b);
                }
                else
                {
                    fprintf(a_asm,"\tmov\tecx,\t%s\n",quaternion_table[qt].operand_b);
                }
            }
            else
            {
                _array(quaternion_table[qt].operand_b,name_offset2,index_name,index_type);
                fprintf(a_asm,"\tmov\tecx,\t[esi]\n");
            }
        }
        else if(quaternion_table[qt].operand_b[0]=='@')
        {
            fprintf(a_asm,"\tmov\tecx,\tdword ptr [ebp+%d]\n",name_offset2);
        }
        else
        {
            fprintf(a_asm,"\tmov\tecx,\tdword ptr [ebp-%d]\n",name_offset2);
        }
    }
    switch(op)
    {
        case JZ:
            fprintf(a_asm,"\tcmp\teax,\t0\n");
            fprintf(a_asm,"\tjz\t%s\n",quaternion_table[qt].operand_result);
            break;
        case JNZ:
            fprintf(a_asm,"\tcmp\teax,\tecx\n");
            fprintf(a_asm,"\tjnz\t%s\n",quaternion_table[qt].operand_result);
            break;
        case JL:
            fprintf(a_asm, "\tcmp\teax,\tecx\n");
            fprintf(a_asm, "\tjl\t%s\n",quaternion_table[qt].operand_result);
            break;
        case JLE:
            fprintf(a_asm, "\tcmp\teax,\tecx\n");
            fprintf(a_asm, "\tjle\t%s\n",quaternion_table[qt].operand_result);
            break;
        case JG:
            fprintf(a_asm, "\tcmp\teax,\tecx\n");
            fprintf(a_asm, "\tjg\t%s\n",quaternion_table[qt].operand_result);
            break;
        case JGE:
            fprintf(a_asm, "\tcmp\teax,\tecx\n");
            fprintf(a_asm, "\tjge\t%s\n",quaternion_table[qt].operand_result);
            break;
        case JE:
            fprintf(a_asm, "\tcmp\teax,\tecx\n");
            fprintf(a_asm, "\tje\t%s\n",quaternion_table[qt].operand_result);
            break;
        case JNE:
            fprintf(a_asm, "\tcmp\teax,\tecx\n");
            fprintf(a_asm, "\tjne\t%s\n",quaternion_table[qt].operand_result);
            break;
    }
}

void _nop()
{
    fprintf(a_asm,"\tnop\n");
}

void _array(char *name,int name_offset,int index_offset,int index_type)
{
    char buff[MAX_NAME];
    int i=0,j=0;
    fprintf(a_asm,"\tpush\teax\n");
    fprintf(a_asm,"\tpush\tecx\n");
    if(name_offset == -1 && index_offset == -1)   //name:全局变量；index:全局变量/立即数
    {
        //buff <- name
        while(name[i]!='[')
        {
            buff[i] = name[i];
            i++;
        }
        buff[i] = '\0';
        fprintf(a_asm, "\tmov\tesi,\toffset _%s\n", buff);
        
        //buff <- index
        i++,j=0;
        while(name[i]!=']')
        {
            buff[j] = name[i];
            j++,i++;
        }
        buff[j] = '\0';

		if(isalpha(buff[0]))
			fprintf(a_asm, "\tmov\teax,\t_%s\n", buff);
		else
			fprintf(a_asm, "\tmov\teax,\t%s\n", buff);
		fprintf(a_asm, "\tmov\tecx,\t4\n");
        fprintf(a_asm, "\timul\tecx\n");
        fprintf(a_asm, "\tadd\tesi,\teax\n");
    }
    else if(name_offset == -1 && index_offset != -1)   //name:全局变量;index:局部变量
    {
        //buff <- buff
        i=0;
        while(name[i]!='[')
        {
            buff[i]=name[i];
            i++;
        }
        buff[i] = '\0';
        fprintf(a_asm, "\tmov\tesi,\toffset _%s\n", buff);

        if(index_type)//not local
            fprintf(a_asm, "\tmov\teax,\tdword ptr [ebp-%d]\n", index_offset);
        else
            fprintf(a_asm, "\tmov\teax,\tdword ptr [ebp+%d]\n", index_offset);
        fprintf(a_asm, "\tmov\tecx,\t4\n");
        fprintf(a_asm, "\timul\tecx\n");
        fprintf(a_asm, "\tadd\tesi,\teax\n",index_offset);
    }
    else if(name_offset!= -1 && index_offset == -1)    //name:局部变量;index:立即数/全局变量
    {
        //参数不可能是数组
        fprintf(a_asm, "\tmov\tesi,\tebp\n");
        fprintf(a_asm, "\tsub\tesi,\t%d\n",name_offset);
        i=0;
        while(name[i]!='[')i++;
        //buff <- index
        i++,j=0;
        while(name[i]!=']')
        {
            buff[j]=name[i];
            i++,j++;
        }
        buff[j] = '\0';
		if(isalpha(buff[0]))
			fprintf(a_asm, "\tmov\teax,\t_%s\n",buff);
		else
			fprintf(a_asm, "\tmov\teax,\t%s\n",buff);
        fprintf(a_asm, "\tmov\tecx,\t4\n");
        fprintf(a_asm, "\timul\tecx\n");
        fprintf(a_asm, "\tsub\tesi,\teax\n");
    }
    else    //name:局部变量;index:局部变量
    {
        fprintf(a_asm, "\tmov\tesi,\tebp\n");
        if(!index_type)
            fprintf(a_asm, "\tadd\tesi,\t%d\n",index_offset);
        else
            fprintf(a_asm, "\tsub\tesi,\t%d\n",index_offset);
        fprintf(a_asm, "\tmov\teax,\t[esi]\n");
        fprintf(a_asm, "\tmov\tesi,\tebp\n");
        fprintf(a_asm, "\tsub\tesi,\t%d\n",name_offset);
        fprintf(a_asm, "\tmov\tecx,\t4\n");
        fprintf(a_asm, "\timul\tecx\n");
        fprintf(a_asm, "\tsub\tesi,\teax\n");
    }
	fprintf(a_asm, "\tpop\tecx\n");
    fprintf(a_asm, "\tpop\teax\n");
}

//tools

void put_in_assignaddr_table(char *name,int length,int type)
{
    strcpy(assignaddr_table[at].name,name);
    assignaddr_table[at].offset=esp;
    assignaddr_table[at].type=type;
    esp+=4*length;
    at++;
}//use as a stack

int get_var_offset(char *xname)
{
    int i;//temp
    int nth;//the nth local var
    int offset=0;//to return
    if(xname[0]=='@')//local var
    {
        nth=atoi(&xname[1]);
        for(i=0;i<=nth;i++)
        {
            offset+=4;
        }
        offset=offset+4;
        return offset;
    }
    for(i=0;i<at;i++)//not local var
    {
        if(!strcmp(xname,assignaddr_table[i].name))
            return assignaddr_table[i].offset;
    }
    return -1;//other situations
}

int get_array_offset(const char *xname,int *index_offset,int *index_type)
{
    int name_offset=0;//to return 
    //buff <- name[]
    int i=0,j=0;
    char buff[MAX_NAME];
    while(xname[i]!='\0' && xname[i]!='[')
    {
        buff[i]=xname[i];
        i++;
    }
    if(xname[i]=='\0')// not array
    {        
        *index_offset=-1;
        *index_type=-1;
        name_offset=-2;//not array
        return name_offset;
    }
    buff[i]='\0';
    name_offset=get_var_offset(buff);
    //name end
    //buff <- [index]
    i++,j=0;
    while(xname[i]!=']')
    {
        buff[j]=xname[i];
        i++,j++;
    }
    buff[j]='\0';
    *index_offset=get_var_offset(buff);
    //index end
    *index_type=(buff[0]=='@')?0:1;//0:local var
    return name_offset;
}

int type_of(char *name)
{
    int i,name_len;
    char temp[MAX_NAME];
    if(name[0]=='@')
        return 1;
    else if(name[0]=='$'||name[0]=='%')
    {
        for(i=0;name[i]!='[' && name[i]!='\0';i++)
        {
            temp[i]=name[i];
        }
        temp[i]='\0';
        name_len=i;
        for(i=0;i<at;i++)
        {
            if(!strcmp(temp,assignaddr_table[i].name))
            {
                return assignaddr_table[i].type;
            }
        }
        return -1;
    }
    else
    {
        for(i=0;i<table_index;i++)
        {
            if(!strcmp(name,table[i].name))
            {
                return table[i].type;
            }
        }
        return -1;
    }
}