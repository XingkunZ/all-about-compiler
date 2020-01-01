//功能说明：递归下降实现文法、生成四元式。
//文法参照：https://www.write-bug.com/article/1263.html?utm_source=qq&utm_medium=social

//＜程序＞ ::= ［＜常量说明＞］［＜变量说明＞］{＜有返回值函数定义＞|＜无返回值函数定义＞}＜主函数＞
void program()
{  
    if(symbol_tag==CONSTTK)
    {
        const_describe();
    }
    if(symbol_tag==INTTK||symbol_tag==CHARTK)//变量说明：＜类型标识符＞ ::= int | char
    {
        variable_describe();
    }
    if(symbol_tag==LPARENT)//(
    {
        with_return_function_define();
    }
    while(symbol_tag==INTTK||symbol_tag==CHARTK||symbol_tag==VOIDTK)
    {
        if(symbol_tag==VOIDTK)
        {
            no_return_function_define();
        }
        else
        {
            get_symbol();
            with_return_function_define();
        }
    }
    main_function_define();
}

//＜常量说明＞ ::= const＜常量定义＞;{const＜常量定义＞;}
void const_describe()
{
    get_symbol();
    do
    {
        if(symbol_tag==CONSTTK)
        {
            get_symbol();
        }
        //else error(8);//缺少'const'

        if(symbol_tag==INTTK||symbol_tag==CHARTK)
        {
            const_define();
        }
        else
        {
            error(4);//缺少int/char的常量定义
            while(symbol_tag!= SEMICN) get_symbol();
        }

        if(symbol_tag == SEMICN)
        {        
            get_symbol();
        }   
        else error(7);//缺少';'
    }while(symbol_tag==CONSTTK);
}

//＜常量定义＞ ::= int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞}
//              | char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}
/*
    ＜标识符＞ ::= ＜字母＞｛＜字母＞｜＜数字＞｝
        ＜字母＞ ::= ＿｜a｜．．．｜z｜A｜．．．｜Z
        ＜数字＞ ::= ０｜＜非零数字＞
            ＜非零数字＞ ::=１｜．．．｜９
    ＜整数＞ ::= ［＋｜－］＜无符号整数＞｜０
        ＜无符号整数＞ ::= ＜非零数字＞｛＜数字＞｝
            ＜数字＞ ::= ０｜＜非零数字＞
                ＜非零数字＞ ::=１｜．．．｜９
 */
void const_define()
{
    int pos;
    char temp[MAX_OP];
    if(symbol_tag==INTTK)
    {
        do
        {
            get_symbol();
            if(symbol_tag==IDEN)
            {
                pos=table_position(cur_identify);
                if((is_global && pos>=para_in_table)||(!is_global && pos>=0))
                    error(14);//存在重复定义的标识符
                get_symbol();
                if(symbol_tag==ASSIGN)//=
                {
                    get_symbol();
                    int_number();
                    if(type==1 && symbol_tag==INTCON)
                    {
                        sprintf(temp,"%d",cur_int);//把整型的cur_int打印成字符串放入temp
                        put_in_table(cur_identify,1,1,cur_int,0,NULL);
                        if(!is_global)//局部
                            put_in_quaternion_table(CONST,"INT",cur_identify,temp);
                        else
                            put_in_quaternion_table(CONST,"INT","",temp);   
                        get_symbol();
                    }
                    else error(10);//类型不匹配
                }
                else
                {
                    error(11);//缺少=
                    while(symbol_tag != COMMA && symbol_tag !=SEMICN) get_symbol();
                    //等待一个,或者;
                }
            }
            else
            {
                error(9);//缺少标识符
                while(symbol_tag != COMMA && symbol_tag !=SEMICN)get_symbol();                
            }
        }while(symbol_tag==COMMA);//,
    }
    else if(symbol_tag==CHARTK)
    {
        do
        {
            get_symbol();
            if(symbol_tag==IDEN)
            {
                pos=table_position(cur_identify);
                if((is_global && pos>=para_in_table)||(!is_global && pos>=0))
                    error(14);//存在重复定义的标识符
                get_symbol();
                if(symbol_tag==ASSIGN)//=
                {
                    get_symbol();
                    if(type==2 && symbol_tag==CHARCON)
                    {
                        sprintf(temp,"%c",cur_const_char);
                        put_in_table(cur_identify,1,2,0,cur_const_char,NULL);
                        if(!is_global)//局部
                            put_in_quaternion_table(CONST,"CHAR",cur_identify,temp);
                        else//全局
                            put_in_quaternion_table(CONST,"CHAR","",temp); 
                        get_symbol();
                    }
                    else error(10);//类型不匹配 
                }
                else
                {
                    error(11);//缺少=
                    while(symbol_tag != COMMA && symbol_tag !=SEMICN)get_symbol();
                    //等待一个,或者;
                }
            }
            else
            {
                error(9);//缺少标识符
                while(symbol_tag!=COMMA && symbol_tag!=SEMICN)get_symbol();
            }
        }while(symbol_tag==COMMA);
    }
}

//＜变量说明＞ ::= ＜变量定义＞;{＜变量定义＞;}
void variable_describe()
{
    do
    {
        variable_define();
        if(symbol_tag==LPARENT)break;//break to functions
        if(symbol_tag==SEMICN)
            get_symbol();
        else if (symbol_tag==LBRACE)
        {//缺少标识符（参数）的有返回值的函数
            get_symbol();
            compound_statement();
        }
        else error(7); //缺少;
    }while(symbol_tag==INTTK||symbol_tag==CHARTK);
}
//＜类型标识符＞ ::= int | char
//＜标识符＞ ::= ＜字母＞｛＜字母＞｜＜数字＞｝
//＜变量定义＞ ::= ＜类型标识符＞(＜标识符＞|＜标识符＞‘[’＜无符号整数＞‘]’){,(＜标识符＞|＜标识符＞‘[’＜无符号整数＞‘]’) }
void variable_define()
{
    int pos;
    char temp[MAX_OP];
    do
    {
        get_symbol();
        if(symbol_tag==IDEN)
        {
            pos=table_position(cur_identify);
            if((is_global && pos>=para_in_table)||(!is_global && pos>=0))
                error(14);//存在重复定义的标识符
            get_symbol();
            if(symbol_tag==LBRACK)//[
            {
                get_symbol();
                int_number();
                if(symbol_tag==INTCON && is_signed==0)
                {
                    sprintf(temp,"%d", cur_int);
                    get_symbol();
                    if(symbol_tag==RBRACK)//]
                    {
                        put_in_table(cur_identify,4,type,cur_int,0,NULL);//array
                        if(type==1)
                        {
                            put_in_quaternion_table(VAR,"INT","",temp);
                        }
                        else//2
                        {
                            put_in_quaternion_table(VAR,"CHAR","",temp);
                        }
                        //qtable_index+1
                        if(!is_global)//local
                            strcpy(quaternion_table[qtable_index-1].operand_b,cur_identify);
                        get_symbol();
                    }
                    else error(15);//缺少']'
                }
                else error(16);//数组元素个数应当为无符号整数
            }
            else if(symbol_tag!=LPARENT)//排除有返回函数、数组
            {
                put_in_table(cur_identify,2,type,0,0,NULL);//var
                if(type==1)
                    put_in_quaternion_table(VAR,"INT","","");
                else
                    put_in_quaternion_table(VAR,"CHAR","","");
                if(!is_global) 
                    strcpy(quaternion_table[qtable_index-1].operand_b,cur_identify);
            }
        }
        else
        {
            error(9);//缺少标识符
            while(symbol_tag!=COMMA && symbol_tag!=SEMICN && symbol_tag!=LBRACE)
                get_symbol();
        }
    } while (symbol_tag==COMMA);
    //symbol_tag==SEMICN || symbol_tag==LBRACE
}

//＜有返回值函数定义＞ ::= ＜声明头部＞‘(’＜参数＞‘)’ ‘{’＜复合语句＞‘}’
//＜声明头部＞ ::= int＜标识符＞|char＜标识符＞
void with_return_function_define()
{
    int pos;
    //got INTTK
    if(symbol_tag==IDEN)//declaration head
    {
        pos=table_position(cur_identify);
        if(pos>=0) error(14); //重复定义标识符
        get_symbol();
    }
    else if(symbol_tag != LPARENT)
    {
        error(23);//函数声明头部定义错误
        while(symbol_tag!=LBRACE && symbol_tag !=SEMICN)
            get_symbol();
        if(symbol_tag==LBRACE)
        {
            pos=-1;
            goto L_rf1;
        }
        else
        {
            get_symbol();
            is_global=1;//全局
            has_return=0;
            goto L_rf2;
        }
    }//error
    if(symbol_tag==LPARENT)
    {
        put_in_table(cur_identify,3,type,0,0,NULL);//func
        pos=table_index-1;
        para_in_table = table_index;
        get_symbol();
        parameters();
        if(symbol_tag==RPARENT)
            get_symbol();
        else error(21);//缺少)
L_rf1:
        if(symbol_tag==LBRACE)//{
        {
            is_global=1;
            if(pos>=0)
            {
                put_in_quaternion_table(FUNC,table[pos].name,"","");
            }
            has_return=0;
            get_symbol();
L_rf2:
            compound_statement();
            // if(!has_return)
            //     error(24);//无返回语句
            put_in_quaternion_table(EFUNC,table[pos].name,"","");
            is_global=0;//局部
            //清除参数名,防止重名
            for(pos=para_in_table;pos<localvar_in_table;pos++)
                memset(table[pos].name,0,MAX_NAME);
            table_index=localvar_in_table;
            if(symbol_tag==RBRACE)//}
                get_symbol();
            else error(19);//缺少}
        }
        else error(25);//缺少{
    }
    else error(23);//函数头定义错误
}
// ＜无返回值函数定义＞ ::= void＜标识符＞‘(’＜参数＞‘)’‘{’＜复合语句＞‘}’
void no_return_function_define()
{
    //get void 
    get_symbol();
    int pos;
    if(symbol_tag==IDEN)
    {
        pos=table_position(cur_identify);
        if(pos>=0)
            error(14);//重复定义标识符
        get_symbol();
    }
    else if(symbol_tag==MAINTK)
        return;
    else
    {
        error(9);//int/char后缺少标识符
        while(symbol_tag!=LBRACE && symbol_tag!=SEMICN)
            get_symbol();
        if(symbol_tag==LBRACE)//{
        {
            goto L_nf1;
        }
        else
        {
            is_global=1;
            get_symbol();
            has_return=0;
            goto L_nf2;
        }
    }
    if(symbol_tag==LPARENT)
    {
        put_in_table(cur_identify,3,0,0,0,NULL);
        pos=table_index-1;
        para_in_table=table_index;
        get_symbol();
        parameters();
        if(symbol_tag==RPARENT)
            get_symbol();
        else
        {
            error(21);//缺少）
        }
L_nf1:
        if(symbol_tag==LBRACE)
        {
            is_global=1;
            put_in_quaternion_table(FUNC,table[pos].name,"","");
            get_symbol();
            has_return=0;
L_nf2:      compound_statement();
            if(has_return)
                error(32);//无返回值函数无需返回
            put_in_quaternion_table(EFUNC,table[pos].name,"","");
            is_global=0;
            table_index=localvar_in_table;
            for(pos=para_in_table;pos<localvar_in_table;pos++)
                memset(table[pos].name,0,MAX_NAME);
            table_index=localvar_in_table;
            if(symbol_tag==RBRACE)//}
                get_symbol();
            else error(19);//缺少}
        }
        else
        {
            error(25);//该函数定义缺少{
        }
    }
    else
    {
        error(28);//缺少(
        while (symbol_tag != LBRACE && symbol_tag!=SEMICN)
        {
            get_symbol();
        }
        if(symbol_tag==LBRACE)
            goto L_nf1;
        else
        {
            is_global=1;
            get_symbol();
            has_return=0;
            goto L_nf2;
        }
    }
}
//＜参数＞ ::= ＜参数表＞
//＜参数表＞ ::= ＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞}|＜空＞
void parameters()
{
    //has symbol
    int pos;
    if(symbol_tag==RPARENT)//＜空＞
    {
        localvar_in_table=table_index;
        return;
    }
    else if(symbol_tag==LBRACE)//是{则出错，返回
        return;
    else
    {
        do
        {
            if(symbol_tag==COMMA)
                get_symbol();
            //＜类型标识符＞ ::= int | char
            if(symbol_tag==INTTK || symbol_tag==CHARTK)
            {
                get_symbol();
                if(symbol_tag==IDEN)
                {
                    pos=table_position(cur_identify);
                    if(para_in_table<pos)
                        error(14);//重复定义的标识符
                    put_in_table(cur_identify,5,type,0,0,NULL);
                    get_symbol();//comma?
                }
                else
                {
                    error(9);//缺少标识符
                    while(symbol_tag!=COMMA && symbol_tag != RPARENT)
                        get_symbol();  
                }
            }
            else
            {
                error(26);//参数列表缺少类型标识符
                while(symbol_tag!=COMMA && symbol_tag != RPARENT)
                    get_symbol();  
            }
        } while (symbol_tag==COMMA);
        localvar_in_table=table_index;
    }
}

//＜主函数＞ ::= void main‘(’‘)’ ‘{’＜复合语句＞‘}’
void main_function_define()
{
    if(symbol_tag==VOIDTK)
        get_symbol();
    else if(symbol_tag!=MAINTK) error(34);//主函数定义错误
    if(symbol_tag==MAINTK)
    {
        get_symbol();
        if(symbol_tag==LPARENT)
        {
            para_in_table=table_index;
            get_symbol();
            if(symbol_tag==RPARENT)
            {
                localvar_in_table=table_index;
                get_symbol();
L_main1:        if(symbol_tag==LBRACE)//(
                {
                    is_global=1;
                    put_in_quaternion_table(MAINF,"","","");
                    get_symbol();
                    has_return=0;
L_main2:            compound_statement();
                    // if(!has_return)
                    put_in_quaternion_table(EMAINF,"","","");
                    is_global=0;
                    table_index=localvar_in_table;
                    if(symbol_tag==RBRACE)
                    {
                        if(!end_of_file)
                        {
                            error(35);//主函数后有多余内容
                            while(isspace(cur_char))
                            {
                                get_char();
                                if(end_of_file)return;
                            }
                        }
                    }
                    else
                        error(19);//缺少}
                }
                else
                    error(25);//缺少{
            }
            else
                error(21);//缺少)
        }
        else//非maintk
        {
            error(23);//函数头定义错误
            while(symbol_tag!=LBRACE && symbol_tag!=SEMICN)
            {
                get_symbol();
            }
            if(symbol_tag==LBRACE)
                goto L_main1;
            else
            {
                is_global=1;
                has_return=0;
                get_symbol();
                goto L_main2;
            }
        }
    }
    else
        error(23);//函数头定义错误
}

//＜复合语句＞ ::= ［＜常量说明＞］［＜变量说明＞］＜语句列＞
void compound_statement()
{
    if(symbol_tag==CONSTTK)//局部常量
        const_describe();
    if(symbol_tag==INTTK||symbol_tag==CHARTK)//局部变量
        variable_describe();
    statement_list();
}

//＜语句列＞ ::= ｛＜语句＞｝
void statement_list()
{
    while(symbol_tag==IFTK
        ||symbol_tag==DOTK||symbol_tag==FORTK||symbol_tag==LBRACE
        ||symbol_tag==IDEN
        ||symbol_tag==SCANFTK||symbol_tag==PRINTFTK
        ||symbol_tag==SEMICN||symbol_tag==SWITCHTK||symbol_tag==RETURNTK)
    {
        statement();
    }
}

//＜语句＞ ::= ＜条件语句＞
//          ｜＜循环语句＞|‘{’＜语句列＞‘}’
//          ｜＜有返回值函数调用语句＞; | ＜无返回值函数调用语句＞;｜＜赋值语句＞;
//          ｜＜读语句＞;｜＜写语句＞;
//          ｜＜空＞;|＜情况语句＞｜＜返回语句＞;
void statement()
{
    int pos;
    if(symbol_tag==IFTK)
        if_statement();
    else if(symbol_tag==FORTK)
        for_statement();
    else if(symbol_tag==DOTK)//参考的C0文法的循环语句没有do while
        dowhile_statement();
    else if (symbol_tag==LBRACE)//{语句列}
    {
        get_symbol();
        statement_list();
        if(symbol_tag==RBRACE)
            get_symbol();
        else error(19);//缺少}
    }
    else if(symbol_tag==IDEN)//函数调用，赋值语句
    {
        pos=table_position(cur_identify);
        if(pos<0)
        {
            error(17);//未定义的标识符
            while(symbol_tag!=RBRACE && symbol_tag!=SEMICN
            && symbol_tag!=IFTK && symbol_tag!=DOTK && symbol_tag!=IFTK)
                get_symbol();
            if(symbol_tag==SEMICN) get_symbol();  
        }
        get_symbol();
        if(symbol_tag==LPARENT)//(,函数调用
        {
            get_symbol();
            function_call(pos);
            if(symbol_tag==SEMICN)
                get_symbol();
            else error(7);//缺少;
        }
        else if(symbol_tag==LBRACK||symbol_tag==ASSIGN)
        {
            assign_statement(pos);
            if(symbol_tag==SEMICN)
                get_symbol();
            else error(7);//缺少;
        }
        else
        {
            error(20);//不期望的标识符
            while(symbol_tag!=RBRACE && symbol_tag!=SEMICN
                && symbol_tag!=IFTK && symbol_tag!=DOTK && symbol_tag!=IFTK)
				get_symbol();
			if(symbol_tag == SEMICN)
				get_symbol();
        }
    }
    else if(symbol_tag==SCANFTK)//输入语句
    {
        scanf_statement();
        if(symbol_tag==SEMICN) get_symbol();
        else error(7);//缺少;
    }
    else if(symbol_tag==PRINTFTK)
    {
        printf_statement();
        if(symbol_tag==SEMICN) get_symbol();
        else error(7);//缺少;
    }
    else if(symbol_tag==RETURNTK)
    {
        return_statement();
        if(symbol_tag==SEMICN) get_symbol();
        else error(7);//缺少;
    }
    else if(symbol_tag==SEMICN)//空语句
    {
        put_in_quaternion_table(NOP,"","","");
        get_symbol();
    }
    else if(symbol_tag==SWITCHTK)//switch
    {
        switch_statement();
        if(symbol_tag==SEMICN) get_symbol();
        else error(7);//缺少;
    }
    else
    {
        error(18);//无法识别该语句
        while(symbol_tag!=RBRACE && symbol_tag!=SEMICN
            && symbol_tag!=IFTK && symbol_tag!=DOTK && symbol_tag!=IFTK)
            get_symbol();
        if(symbol_tag==SEMICN) get_symbol();
    }
}
//＜赋值语句＞ ::= ＜标识符＞＝＜表达式＞|＜标识符＞‘[’＜表达式＞‘]’=＜表达式＞
void assign_statement(int pos)
{
    int j,flag;
    char op1[MAX_OP], op2[MAX_OP];
    int type1,type2;
    //record kind != var array para
    if(table[pos].kind != 2 && table[pos].kind !=4 && table[pos].kind != 5)
        error(20);//不期望的标识符
    if(symbol_tag==LBRACK)//array
    {
        if(table[pos].kind !=4)
            error(20);
        get_symbol();
        j=expression(&flag);
        type1=make_quaternion_operand(op1,1,j,flag,pos);
        if(symbol_tag==RBRACK)
            get_symbol();
        else
            error(15);//缺少]
    }
    else//var para
    {
        if(table[pos].kind!=2 && table[pos].kind!=5)
            error(20);//存在不期望的标识符
        else
            type1=make_quaternion_operand(op1,0,0,0,pos);
    }
    if(symbol_tag==ASSIGN)
    {
        get_symbol();
        j=expression(&flag);
        type2=make_quaternion_operand(op2,flag,0,0,j);
        if(type1>type2)
            error(36);//赋值类型不匹配
        else
        {
            put_in_quaternion_table(MOV,op2,"",op1);
        }
        
    }
    else
    {
        error(11);//缺少=
        while(symbol_tag!=SEMICN) get_symbol();
    }
}

//＜条件语句＞ ::= if ‘(’＜条件＞‘)’＜语句＞［else＜语句＞］
void if_statement()
{
    int con_qua;
    int label1=-1,label2=-1;
    get_symbol();
    if(symbol_tag==LPARENT)
    {
        get_symbol();
        con_qua=condition(0);
        label1=put_in_label_table();//默认不跳转
        strcpy(quaternion_table[con_qua].operand_result, label_table[label1]);
        if(symbol_tag==RPARENT)
        {
            get_symbol();
            statement();
            if(symbol_tag==ELSETK)
            {
                label2=put_in_label_table();
                put_in_quaternion_table(JMP,"","",label_table[label2]);
                get_symbol();
                quaternion_table[qtable_index].label=label1;
                statement();
                quaternion_table[qtable_index].label=label2;
            }
            else
            {
                quaternion_table[qtable_index].label=label1;
            }
        }
        else
        {
            error(21);//缺少)
        }
    }
    else
    {
        error(28);//缺少(
    }
}

//＜循环语句＞ ::= for‘(’＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞‘)’＜语句＞
void for_statement()
{
    int pos;
    int flag;
    int con_qua;
    char op1[MAX_OP],op2[MAX_OP],temp[MAX_OP];
    int label1=-1,label2=-1;
    int increment=0;
    QUATERNION_OP increment_op;
    get_symbol();
    if(symbol_tag==LPARENT)
    {
        get_symbol();
        if(symbol_tag==IDEN)
        {
            pos=table_position(cur_identify);
            if(pos<0)
            {
                error(17);//未定义的标识符
                while(symbol_tag!=SEMICN && symbol_tag!=RBRACE && symbol_tag!=IFTK && symbol_tag!=DOTK && symbol_tag != FORTK && symbol_tag != WHILETK)
                    get_symbol();
                if(symbol_tag==SEMICN)
                    get_symbol();
            }
            if(table[pos].kind!=2 && table[pos].kind!=5)//var para
                error(20);//不期望的标识符
            make_quaternion_operand(op1,0,0,0,pos);
            get_symbol();
            if(symbol_tag==ASSIGN)
            {
                get_symbol();
                pos=expression(&flag);
                make_quaternion_operand(op2,flag,0,0,pos);
                put_in_quaternion_table(MOV,op2,"",op1);
                if(symbol_tag==SEMICN)
                {
                    get_symbol();
                    label1=put_in_label_table();
                    quaternion_table[qtable_index].label=label1;
                    con_qua=condition(0);//直到条件不满足则跳转
                    label2=put_in_label_table();
                    strcpy(quaternion_table[con_qua].operand_result,label_table[label2]);
                    if(symbol_tag==SEMICN)
                    {
                        get_symbol();
                        if(symbol_tag==IDEN)
                        {
                            pos=table_position(cur_identify);
                            if(pos<0)
                            {
                                error(17);//未定义的标识符
                                while(symbol_tag!=SEMICN && symbol_tag!=RBRACE && symbol_tag!=IFTK && symbol_tag!=DOTK && symbol_tag != FORTK && symbol_tag != WHILETK)
                                    get_symbol();
                                if(symbol_tag==SEMICN)
                                    get_symbol();
                            }
                            if(table[pos].kind!=2 && table[pos].kind!=5)//var para
                                error(20);//不期望的标识符
                            make_quaternion_operand(op1,0,0,0,pos);
                            get_symbol();
                            if(symbol_tag==ASSIGN)
                            {
                                get_symbol();
                                if(symbol_tag==IDEN)
                                {
                                    pos=table_position(cur_identify);
                                    if(pos<0)
                                    {
                                        error(17);//未定义的标识符
                                        while(symbol_tag!=SEMICN && symbol_tag!=RBRACE && symbol_tag!=IFTK && symbol_tag!=DOTK && symbol_tag != FORTK && symbol_tag != WHILETK)
                                            get_symbol();
                                        if(symbol_tag==SEMICN) 
                                            get_symbol();
                                    }
                                    if(table[pos].kind!=2 && table[pos].kind!=5)//var para
                                        error(20);//不期望的标识符
                                    make_quaternion_operand(op2,0,0,0,pos);
                                    get_symbol();
                                    if(symbol_tag==PLUS||symbol_tag==MINU)//+|-
                                    {
                                        increment_op=(symbol_tag == PLUS) ? ADD : SUB;
                                        get_symbol();
                                        // increment=step_length();
                                        int_number();
                                        if(symbol_tag==INTCON && ! is_signed)
                                        {
                                            increment=cur_int;
                                            get_symbol();
                                            if(symbol_tag==RPARENT)
                                            {
                                                get_symbol();
L_for1:
                                                statement();
                                                sprintf(temp,"%d",increment);
                                                put_in_quaternion_table(increment_op, op2, temp, op1);
                                                put_in_quaternion_table(JMP,"","",label_table[label1]);
                                                quaternion_table[qtable_index].label=label2;
                                            }
                                            else error(21);
                                        }
                                        else
                                        {
                                            error(29);
                                        }
                                    }
                                    else error(29);
                                }
                                else error(29);
                            }
                            else error(29);
                        }
                        else error(29);
                    }
                    else error(29);
                }
                else error(29);
            }
            else error(29);
        }
        else error(29);
    }
    else error(29);

    if(error_flag)
    {
        while(symbol_tag!=RPARENT && symbol_tag!=LBRACE)
            get_symbol();
        get_symbol();
        error_flag=0;
        goto L_for1;
    }
}

//＜步长＞ ::= ＜非零数字＞｛＜数字＞｝//就是无符号整数
//返回步长
int step_length()
{
    if(symbol_tag==INTCON)
    {
        if(cur_int)
        {
            is_signed=1;//signed
            cur_int=-cur_int;
        }
        else//cur_int==0
            is_signed=0;
    }
    else error(30);
    return cur_int;
}

//<dowhile语句>::= do <语句表> while '('<条件语句>')'
void dowhile_statement()
{
    get_symbol();
    int this_label;
    int con_qua;
    this_label=put_in_label_table();
    quaternion_table[qtable_index].label=this_label;
    statement();
    if(symbol_tag==WHILETK)
    {
        get_symbol();
        if(symbol_tag==LPARENT)
        {
            get_symbol();
            con_qua=condition(1);
            strcpy(quaternion_table[con_qua].operand_result,label_table[this_label]);
            if(symbol_tag==RPARENT)
            {
                get_symbol();
            }
            else error(21);//缺少）
        }
        else
        {
            while(symbol_tag!=IFTK && symbol_tag!=DOTK && symbol_tag!=FORTK
            && symbol_tag!=LBRACE && symbol_tag!= RBRACE && symbol_tag!=IDEN 
            && symbol_tag!=PRINTFTK && symbol_tag!=SCANFTK && symbol_tag!=RETURNTK
            && symbol_tag!=SEMICN)
            {
                get_symbol();
            }
            error(28);//缺少(
        }
    }
    else error(37);//缺少while
}

// ＜情况语句＞ ::= switch ‘(’＜表达式＞‘)’‘{’＜情况表＞＜缺省＞‘}’
// ＜情况表＞ ::= ＜情况子语句＞{＜情况子语句＞}
// ＜情况子语句＞ ::= case＜常量＞：＜语句＞
// ＜缺省＞ ::= default : ＜语句＞
void switch_statement()
{
    get_symbol();
}

//＜读语句＞ ::= scanf ‘(’＜标识符＞{,＜标识符＞}‘)’
void scanf_statement()
{
    get_symbol();
    int pos;
    char op[MAX_OP];
    if(symbol_tag==LPARENT)
    {
        do
        {
            get_symbol();
            if(symbol_tag==IDEN)
            {
                pos=table_position(cur_identify);
                if(pos<0)
                {
                    error(17);//未定义的标识符
                    while(symbol_tag!=SEMICN && symbol_tag!=RBRACE && symbol_tag!=IFTK && symbol_tag!=DOTK && symbol_tag!=FORTK)
                        get_symbol();
                    if(symbol_tag==SEMICN) get_symbol();
                }
                if(table[pos].kind !=2 && table[pos].kind !=5)
                    error(20);//不期望的标识符
                make_quaternion_operand(op,0,0,0,pos);
                put_in_quaternion_table(READ,"","",op);
                get_symbol();
            }
            else
            {
                error(31);//'scanf'语句格式错误，期待标识符。
                while(symbol_tag!=COMMA && symbol_tag !=RPARENT)
                    get_symbol();
            }
        } while (symbol_tag==COMMA);
        if(symbol_tag==RPARENT)
            get_symbol();
        else
            error(21);//缺少右括号
    }
    else
    {
        error(28);//缺少左括号
        while(symbol_tag!=SEMICN && symbol_tag!=RBRACE && symbol_tag!=LBRACE && symbol_tag !=IDEN
            && symbol_tag!=IFTK && symbol_tag!=DOTK && symbol_tag!=FORTK
            && symbol_tag!=PRINTFTK && symbol_tag !=SCANFTK && symbol_tag!=RETURNTK)
                get_symbol();
    }
}

//＜写语句＞ ::= printf ‘(’ ＜字符串＞,＜表达式＞ ‘)’|printf ‘(’＜字符串＞‘)’| printf ‘(’＜表达式＞‘)’
void printf_statement()
{
    //get printf
    int flag;
    int pos;
    int str_index;
    char op[MAX_OP],str[MAX_OP];
    get_symbol();
    if(symbol_tag==LPARENT)
    {
        get_symbol();
        if(symbol_tag == STRCON)
        {
            str_index=put_in_string_table();
            sprintf(str,"__str%d",str_index);
            get_symbol();
            if(symbol_tag==COMMA)
            {
                //‘(’ ＜字符串＞,＜表达式＞ ‘)’
                get_symbol();
                pos=expression(&flag);
                make_quaternion_operand(op,flag,0,0,pos);
                put_in_quaternion_table(WRITE,str,op,"");
            }
            else
            {
                //‘(’＜字符串＞‘)’
                put_in_quaternion_table(WRITE,str,"","");
            }
        }
        else
        {
            //‘(’＜表达式＞‘)’
            pos=expression(&flag);
            make_quaternion_operand(op,flag,0,0,pos);
            put_in_quaternion_table(WRITE,"",op,"");
        }
        if(symbol_tag==RPARENT)
            get_symbol();
        else
            error(21);//缺少右括号
    }
    else
    {
        error(28);//缺少左括号
        while(symbol_tag!=SEMICN && symbol_tag!=RBRACE && symbol_tag!=LBRACE && symbol_tag !=IDEN
            && symbol_tag!=IFTK && symbol_tag!=DOTK && symbol_tag!=FORTK
            && symbol_tag!=PRINTFTK && symbol_tag !=SCANFTK && symbol_tag!=RETURNTK)
                get_symbol();
    }
}

//＜返回语句＞ ::= return[‘(’＜表达式＞‘)’]
void return_statement()
{
    int i,flag;
    char op[MAX_OP];
    //get return
    get_symbol();
    if(symbol_tag==LPARENT)//(
    {
        has_return=1;
        get_symbol();
        i=expression(&flag);
        make_quaternion_operand(op,flag,0,0,i);
        put_in_quaternion_table(RET,op,"","");//op
        if(symbol_tag==RPARENT)
            get_symbol();
        else error(21);//缺少)
    }
    else 
    {
        put_in_quaternion_table(RET,"","","");
        if(symbol_tag!=SEMICN)
            error(28);//缺少(
        while(symbol_tag!=SEMICN)
            get_symbol();
    }
}

//＜条件＞ ::= ＜表达式＞＜关系运算符＞＜表达式＞
//          ｜＜表达式＞
//＜关系运算符＞ ::= <｜<=｜>｜>=｜!=｜==
//返回四元式号，条件真值为res时执行跳转
int condition(int res)
{
    enum SYMBOL op_symbol;
    int flag,i;
    char op_a[MAX_OP], op_b[MAX_OP];
    i=expression(&flag);
    make_quaternion_operand(op_a,flag,0,0,i);
    if(symbol_tag==LSS || symbol_tag==LEQ || symbol_tag==GRE || symbol_tag==GEQ || symbol_tag==EQL || symbol_tag==NEQ)
    {
        op_symbol=symbol_tag;
    }
    else if(symbol_tag!=ASSIGN)//jump
    {
        if (res)//jump if not zero
        {
            put_in_quaternion_table(JNZ,op_a,"","");
        }
        else//jump if zero
        {
            put_in_quaternion_table(JZ,op_a,"","");
        }
        return qtable_index-1;
    }
    get_symbol();
    i=expression(&flag);
    make_quaternion_operand(op_b,flag,0,0,i);
    switch(op_symbol)
    {
        case LSS:
            if(res)
                put_in_quaternion_table(JL,op_a,op_b,"");
            else
                put_in_quaternion_table(JGE,op_a,op_b,"");
            break;
        case LEQ:
            if(res)
                put_in_quaternion_table(JLE,op_a,op_b,"");
            else
                put_in_quaternion_table(JG,op_a,op_b,"");
            break;
        case GRE:
            if(res)
                put_in_quaternion_table(JG,op_a,op_b,"");
            else
                put_in_quaternion_table(JLE,op_a,op_b,"");
            break;
        case GEQ:
            if(res)
                put_in_quaternion_table(JGE,op_a,op_b,"");
            else
                put_in_quaternion_table(JL,op_a,op_b,"");
            break;
        case EQL:
            if(res)
                put_in_quaternion_table(JE,op_a,op_b,"");
            else
                put_in_quaternion_table(JNE,op_a,op_b,"");
            break;
        case NEQ:
            if(res)
                put_in_quaternion_table(JNE,op_a,op_b,"");
            else
                put_in_quaternion_table(JE,op_a,op_b,"");
            break;
        case ASSIGN:
            put_in_quaternion_table(MOV,op_b,"",op_a);
            if (res)
                put_in_quaternion_table(JNZ,op_a,"","");
            else
                put_in_quaternion_table(JZ,op_a,"","");
            break;
        default:;
    }
    return qtable_index-1;
}

//＜表达式＞ ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}
int expression(int *flag)//返回当前结果存储位置table_index
{
    int is_minus=0;
    int index,index_type;
    int i;
    int type1, type2;//for op_a, op_b
    QUATERNION_OP op;
    char op_a[MAX_OP] = {0};
    char op_b[MAX_OP] = {0};
	char tmp[MAX_OP] = {0};
    int cal_flag=0;
    if(symbol_tag==PLUS || symbol_tag==MINU)
    {
        if(symbol_tag==MINU) is_minus=1;
        get_symbol();
    }
    i=term(flag,&index,&index_type);
    if(*flag==1)//array，取出数组值存为临时变量
    {
        cal_flag=1;
        type1=make_quaternion_operand(op_a,*flag,index,index_type,i);
        i=allocate_tempvar(tmp,type1);
        put_in_quaternion_table(MOV,op_a,"",tmp);
        *flag=0;
        //get_symbol();
    }
    while(symbol_tag==PLUS||symbol_tag==MINU)
    {
        type1=make_quaternion_operand(op_a,*flag,index,index_type,i);
        op=(symbol_tag==PLUS)?ADD:SUB;
        get_symbol();
        i=term(flag,&index,&index_type);
        type2=make_quaternion_operand(op_b,*flag,index,index_type,i);
        if(type1 == 2 && type2 == 2)
            type1=2;
        else type1=1;
        i=allocate_tempvar(tmp,type1);
        put_in_quaternion_table(op,op_a,op_b,tmp);
        *flag=0;
        cal_flag=1;
    }
    if(is_minus)
    {
        if(cal_flag)
        {
            i=allocate_tempvar(op_b,type1);
            put_in_quaternion_table(NEG,tmp,"",op_b);
        }
        else
        {
            type1=make_quaternion_operand(op_a,*flag,index,index_type,i);
            if(*flag > 1)
                i=-i;
            else
            {
                i = allocate_tempvar(op_b,type1);
                put_in_quaternion_table(NEG,op_a,"",op_b);
            }
        }
    }
    return i;
}
//＜项＞ ::= ＜因子＞{＜乘法运算符＞＜因子＞}
int term(int *flag,int *index,int *index_type)
{
    int i;
    int type1,type2;
    QUATERNION_OP op;
    char op_a[MAX_OP]={0};
    char op_b[MAX_OP]={0};
    char temp[MAX_OP];
    i=factor(flag,index,index_type);
    while(symbol_tag==MULT||symbol_tag==DIV)
    {
        type1=make_quaternion_operand(op_a,*flag,*index,*index_type,i);
        op=(symbol_tag==MULT)?MUL:DIVV;
        get_symbol();
        i=factor(flag,index,index_type);
        type2=make_quaternion_operand(op_b,*flag,*index,*index_type,i);
        if(type1==2 && type2==2 )
            type1=2;//char
        else type1=1;//int
        i=allocate_tempvar(temp,type1);
        put_in_quaternion_table(op,op_a,op_b,temp);
        *flag=0;
    }
    return i;
}
//＜因子＞ ::= ＜标识符＞
//          ｜＜标识符＞‘[’＜表达式＞‘]’
//          ｜＜整数＞|＜字符＞｜＜有返回值函数调用语句＞|‘(’＜表达式＞‘)’
int factor(int *flag,int *index,int *index_type)
{
    int pos;
    *flag=0;
    char op[MAX_OP];
    char temp[MAX_OP];
    int this_pos;
    if(symbol_tag==IDEN)
    {
        pos=table_position(cur_identify);
        if(pos<0)
        {
            error(17);//未定义的标识符
            while(symbol_tag!=SEMICN && symbol_tag!=RBRACE && symbol_tag!=DOTK && symbol_tag!=IFTK && symbol_tag!=FORTK && symbol_tag!=SWITCHTK)
                get_symbol();
            if(symbol_tag==SEMICN)
                get_symbol();
        }
        get_symbol();
        if(symbol_tag==LPARENT)//有返回值函数调用
        {
            if(table[pos].kind!=3)
                error(17);//未定义的标识符
            else if(!table[pos].type)
                error(27);//有返回值的函数不可返回NULL
            get_symbol();
            pos=function_call(pos);
            pos=allocate_tempvar(temp,pos);
            put_in_quaternion_table(MOV,"~eax","",temp);
        }
        else if(symbol_tag==LBRACK)//array
        {
            if(table[pos].kind!=4)
                error(17);//未定义的标识符
            get_symbol();
            *index=expression(flag);
            *index_type=*flag;
            *flag=1;
            make_quaternion_operand(op,*flag,*index,*index_type,pos);
            this_pos=pos;
            pos=allocate_tempvar(temp,table[pos].type);
            put_in_quaternion_table(MOV,op,"",temp);
            *flag=0;
            if(*index_type && (table[this_pos].int_value <= *index||*index<0))
                error(38);//数组引用越界
            if(symbol_tag==RBRACK)
                get_symbol();
            else error(15);//缺少]
        }
        else
        {
            if(table[pos].kind!=1 && table[pos].kind!=2 && table[pos].kind!=5)
                error(17);//未定义的标识符
        }
    }
    else if(symbol_tag==INTCON ||symbol_tag==CHARCON||symbol_tag==PLUS||symbol_tag==MINU)
    {//立即数常量
        *flag=(symbol_tag==CHARCON)?3:2;
        int_number();
        pos=(symbol_tag==INTCON)?cur_int:cur_const_char;
        get_symbol();
    }
    else if(symbol_tag==LPARENT)
    {
        get_symbol();
        pos=expression(flag);
        if(symbol_tag==RPARENT)
        {
            get_symbol();
        }
        else
        {
            error(21);//缺少)
        }
    }
    return pos;
}

//＜有返回值函数调用语句＞::= ＜标识符＞‘(’＜值参数表＞‘)’
//＜无返回值函数调用语句＞::= ＜标识符＞‘(’＜值参数表＞‘)’
int function_call(int pos)
{
    int this_pos=pos;
    if(table[pos].kind!=3)//func
        error(20);//不期望的标识符
    parameter_value_list(pos);
    put_in_quaternion_table(CALL,table[this_pos].name,"","");

    if(symbol_tag==RPARENT) get_symbol();
    else error(21);//缺少)

    return table[this_pos].type;
}

//＜值参数表＞ ::= ＜表达式＞{,＜表达式＞}｜＜空＞
void parameter_value_list(int pos)
{
    //has LPARENT
    int this_pos,flag;
    int tmp_pos=pos+1;//next para
    char op[MAX_OP];
    char op_stack[MAX_STACK][MAX_OP];
    int top=0;
    if(symbol_tag==RPARENT)
    {
        if(table[pos+1].kind==5)
        {
            error(33);//函数调用的参数个数或类型不匹配
        }
        return;
    }
    do
    {
        if(symbol_tag==COMMA)
            get_symbol();
        if(tmp_pos==table_index || table[tmp_pos].kind!=5)
        {
            error(33);//函数调用的参数个数或类型不匹配
            while(symbol_tag!=RPARENT && symbol_tag !=SEMICN)
                get_symbol();
            return ;
        }
        this_pos=expression(&flag);
        if((flag<=1 && table[this_pos].type >= table[tmp_pos].type)||
            (flag >=2 && flag-1>=table[tmp_pos].type))
        {
            make_quaternion_operand(op,flag,0,0,this_pos);
            strcpy(op_stack[top++],op);
        }
        else
        {
            error(33);//函数调用的参数个数或类型不匹配
        }
        tmp_pos++;
    } while (symbol_tag == COMMA);
    if(table[tmp_pos].kind==5)//参数少写
    {
        error(33);//函数调用的参数个数或类型不匹配
    }
    for(top--;top>=0;top--)
        put_in_quaternion_table(PARAV,op_stack[top],"","");
}

//＜整数＞ ::= ［＋｜－］＜无符号整数＞｜０
void int_number()
{
    if(symbol_tag==PLUS||symbol_tag==MINU)
    {
        get_symbol();
        if(symbol_tag==INTCON)
        {
            if(cur_int)
            {
                is_signed=1;//signed
                cur_int=-cur_int;
            }
            else//cur_int==0
                error(12);//0不该有符号
        }
        else error(13);//符号后缺少数字
    }
    else if(symbol_tag==INTCON)
        is_signed=0;//unsigned;
}

//-----TOOLS------

//table中的位置//可优化
int table_position(char *name)
{
    int pos;
    for(pos=table_index;pos>=0;pos--)
    {
        if(!strcmp(name,table[pos].name))
        {
            return pos;
        }
    }
    return -1;
}

//放入符号表
void put_in_table(char *name,int kind,int type,int int_value,char char_value,char *string_value)//放入符号表
{
    strcpy(table[table_index].name,name);
    table[table_index].kind=kind;
    table[table_index].type=type;
    if(kind==1)
    {
        switch (type)
        {
            case 1:table[table_index].int_value=int_value;break;
            case 2:table[table_index].char_value=char_value;break;
            case 3:table[table_index].string_value=string_value;break;
        }
    }
    else if(kind==4) //array
    {
        table[table_index].int_value=int_value;
    }
    else
    {
        table[table_index].int_value=0;
    }
    table_index++;
}

//放入四元式表
void put_in_quaternion_table(QUATERNION_OP op,char *a,char *b,char *r)//填入四元式表
{
    quaternion_table[qtable_index].op=op;
    strcpy(quaternion_table[qtable_index].operand_a,a);
    strcpy(quaternion_table[qtable_index].operand_b,b);
    strcpy(quaternion_table[qtable_index].operand_result,r);
    qtable_index++;
}

//分配临时变量寄存器，输入name,type，输出符号表位置
int allocate_tempvar(char *name,int type)
{
    sprintf(name,"$%d",tempvar_count);
    put_in_table(name,6,type,0,0,NULL);
    tempvar_count++;
    return table_index-1;
}

//生成四元式的操作符
int make_quaternion_operand(char *q_op,int flag,int index, int index_type,int pos)
{
    int type;
    char temp1[MAX_OP],temp2[MAX_OP];
    if (flag>1)
    {
        sprintf(q_op,"%d",pos);
        type=flag-1;
    }
    else if(flag==1)
    {
        if(pos<localvar_in_table)
        {
            strcpy(q_op,table[pos].name);
        }
        else//pos>=localvar_in_table
        {
            sprintf(q_op,"%%%d",pos-localvar_in_table);
        }
        if(!index_type)
        {
            make_quaternion_operand(temp2,0,0,0,index);
            sprintf(temp1,"[%s]",temp2);
        }
        else
        {
            sprintf(temp1,"[%d]",index);
        }
        strcat(q_op,temp1);
        type=table[pos].type;
    }
    else//flag<1
    {
        if(table[pos].kind == 6)//tmp
        {
            strcpy(q_op,table[pos].name);
        }
        else if(pos<para_in_table)
        {
            strcpy(q_op,table[pos].name);
        }
        else if(pos>=para_in_table && pos<localvar_in_table)
        {
            sprintf(q_op,"@%d",pos-para_in_table);
        }
        else //pos>localvar_in_table
        {
            sprintf(q_op,"%%%d",pos-localvar_in_table);
        }
        type=table[pos].type;
    }
    return type;
}

//分配标签，return当前标签号
int put_in_label_table()
{
    sprintf(label_table[ltable_index],"label_%d",ltable_index);
    ltable_index++;
    return ltable_index-1;
}

//放入字符串表，返回字符串标签号
int put_in_string_table()
{
    strcpy(string_table[stable_index],const_string);
    stable_index++;
    return stable_index-1;
}