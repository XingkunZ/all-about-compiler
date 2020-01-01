//fgets:
//如果到达文件末尾或者没有读取到任何字符，
//str 的内容保持不变，并返回一个空指针
void get_char()//从[in_txt]中读取字符
{
    if(cur_char_count >= line_len)//一行读完时
    {
        if(!fgets(cur_line,MAX_LINE-1,in_txt))
        {
            if(!end_of_file)//没有读到文件尾
            {
                end_of_file=1;
                cur_char=' ';
                return;
            }
            else
            {
                error(5);//源文件不完整
                printf("# Error happens!Check [error.txt] for more details.\n");
                exit(0);
            }
        }
        cur_char_count=0;//光标回到行首
        line_len=strlen(cur_line)-1;//更新行长
        line_count++;//多读入一行
    }
    cur_char=cur_line[cur_char_count++];//光标逐渐右移
}

void get_symbol()
{
    int len=0;//临时长度

    //跳过空白字符 //顺便进入第一行
    while(isspace(cur_char)) get_char();

    if(isalpha(cur_char)||cur_char=='_')//识别标识符，存于cur_identity[]
    {
        len=0;
        do
        {
            cur_identify[len]=cur_char;
            len++;
            get_char();
        }while(isalpha(cur_char) || isdigit(cur_char) || cur_char == '_');
        cur_identify[len]='\0';

        //清理之前的赋值可能多出来的几位
        if(cur_word_len < len)
        {
            cur_word_len=len;
        }
        else
        {
            while(cur_word_len >len)
                cur_identify[--cur_word_len]='\0';
        }
        //cur_word_len == len
        
        int i=0,not_iden=1;
        for(i=0;i<14;i++)//14 record types
        {
            not_iden = strcmp(cur_identify, RECORD_TYPE[i]);
            if(!not_iden)
            {
                symbol_tag= (enum SYMBOL) (i+4);//对应的token
                if(i==0) type=2; //char
                else if(i==6) type=1; //int
                break;
            }
        }
        if(not_iden) symbol_tag=IDEN;
    }
    else if (isdigit(cur_char))//数
    {
        if(cur_char=='0')
        {
            get_char();
            if(isdigit(cur_char)) 
            {
                error(10);//数不可以0开头
            }
        }

        cur_int=0;
        while(isdigit(cur_char))
        {
            cur_int=(cur_char-'0')+cur_int*10;
            get_char();
        }
        symbol_tag=INTCON;
    }
    else if(cur_char=='+')//加号
    {
        symbol_tag = PLUS;
        get_char();
    }
    else if(cur_char=='-')//减号
    {
        symbol_tag = MINU;
        get_char();
    }
    else if(cur_char=='*')//乘号
    {
        symbol_tag = MULT;
        get_char();
    }
    else if(cur_char=='/')//除号
    {
        symbol_tag = DIV;
        get_char();
    }
    else if(cur_char=='<')
    {
        get_char();
        if(cur_char=='=')//小于等于
        {
            symbol_tag=LEQ;
            get_char();
        }
        else
        {
            symbol_tag=LSS;//小于
        }
    }
    else if(cur_char=='=')
    {
        get_char();
        if(cur_char=='=')//等于
        {
            symbol_tag=EQL;
            get_char();
        }
        else
        {
            symbol_tag=ASSIGN;//赋值
        }
    }
    else if(cur_char=='>')
    {
        get_char();
        if(cur_char=='=')//大于等于
        {
            symbol_tag=GEQ;
            get_char();
        }
        else
        {
            symbol_tag=GRE;//大于
        }
    }
    else if(cur_char=='!')
    {
        get_char();
        if(cur_char=='=')//不等于
        {
            symbol_tag=NEQ;
            get_char();
        }
        else
        {
            error(3);//报错：非法token
        }
    }
    else if(cur_char=='\'')//单引号，期待字符
    {
        symbol_tag = SQUO;
        get_char();//char const

        cur_const_char=cur_char;
        if(!isalpha(cur_char) && !isdigit(cur_char))
        {
            if(cur_char!='+' && cur_char!='-' && cur_char!='*' && cur_char!='/')
            {
                error(0);//字符常量非法
                cur_const_char='\0';
            }
        }
        get_char();//right single quote
        if(cur_char!='\'') 
        {
            if(isalpha(cur_char))
                error(22);//期待一个字符而非字符串
            else error(1);//缺少右\'
        }
        symbol_tag=CHARCON;
        get_char();
    }
    else if(cur_char=='\"')//双引号，期待字符串
    {
        int len=0;
        symbol_tag=DQUO;
        const_string[0]='\0';
        
        get_char();
        while(cur_char !='\"')
        {
            const_string[len++]=cur_char;
            if(len==MAX_STRING-1) 
            {
                error(2);//字符串过长
            }
            get_char();
        }
        const_string[len]='\0';
        symbol_tag=STRCON;
        get_char();
    }
    else
    {
        switch(cur_char)
        {
            case ';': symbol_tag = SEMICN; break;
            case ',': symbol_tag = COMMA; break;
            case '(': symbol_tag = LPARENT; break;
            case ')': symbol_tag = RPARENT; break;
            case '[': symbol_tag = LBRACK; break;
            case ']': symbol_tag = RBRACK; break;
            case '{': symbol_tag = LBRACE; break;
            case '}': symbol_tag = RBRACE; break;
            default:
                error(3);//非法token
        }
        get_char();
    }
    //调试用
    // printf(" # tag[%d]:identity=[%s],char=[%c].\n",(enum SYMBOL)symbol_tag,cur_identify,cur_char);
}