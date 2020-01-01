void error(int index)
{
    switch(index)
    {
        case 0:
            fprintf(error_txt, "第%d行：字符常量的字符非法；\n",line_count);
            break;
        case 1:
            fprintf(error_txt, "第%d行：字符常量缺少右\' ；\n",line_count);
            break;
        case 2:
            fprintf(error_txt, "第%d行：字符串过长；\n",line_count);
            break;
        case 3:
            fprintf(error_txt, "第%d行：非法Token；\n",line_count);
            break;
        case 4:
            fprintf(error_txt, "第%d行：缺少int/char的常量定义；\n",line_count);
            break;
        case 5:
            fprintf(error_txt, "第%d行：源文件不完整；\n",line_count);
            break;
        case 6:
            fprintf(error_txt, "第%d行：非0数不可以0开头；\n",line_count);
            break;
        case 7:
            fprintf(error_txt, "第%d行：缺少\';\'；\n",line_count);
            break;
        case 8:
            fprintf(error_txt, "第%d行：缺少\'const\'；\n",line_count);
            break;
        case 9:
            fprintf(error_txt, "第%d行：int/char后缺少标识符；\n",line_count);
            break;
        case 10:
            fprintf(error_txt, "第%d行：类型不匹配；\n",line_count);
            break;
        case 11:
            fprintf(error_txt, "第%d行：缺少\'=\'；\n",line_count);
            break;
        case 12:
            fprintf(error_txt, "第%d行：0不该有符号；\n",line_count);
            break;
        case 13:
            fprintf(error_txt, "第%d行：符号后缺少数字；\n",line_count);
            break;
        case 14:
            fprintf(error_txt, "第%d行：存在重复定义的标识符；\n",line_count);
            break;
        case 15:
            fprintf(error_txt, "第%d行：缺少\']\'；\n",line_count);
            break;
        case 16:
            fprintf(error_txt, "第%d行：数组元素个数应当为无符号整数；\n",line_count);
            break;
        case 17:
            fprintf(error_txt, "第%d行：未定义的标识符；\n",line_count);
            break;
        case 18:
            fprintf(error_txt, "第%d行：无法识别该语句；\n",line_count);
            break;
        case 19:
            fprintf(error_txt, "第%d行：缺少\'}\'；\n",line_count);
            break;
        case 20:
            fprintf(error_txt, "第%d行：存在不期望的标识符；\n",line_count);
            break;
        case 21:
            fprintf(error_txt, "第%d行：缺少\')\'；\n",line_count);
            break;
        case 22:
            fprintf(error_txt, "第%d行：期待一个字符而非字符串；\n",line_count);
            break;
        case 23:
            fprintf(error_txt, "第%d行：函数声明头部定义错误；\n",line_count);
            break;
        case 24:
            fprintf(error_txt, "第%d行：该函数缺少返回语句；\n",line_count);
            break;
        case 25:
            fprintf(error_txt, "第%d行：该函数定义缺少\'{\'；\n",line_count);
            break;
        case 26:
            fprintf(error_txt, "第%d行：参数列表缺少类型标识符；\n",line_count);
            break;
        case 27:
            fprintf(error_txt, "第%d行：有返回值的函数不可返回NULL；\n",line_count);
            break;
        case 28:
            fprintf(error_txt, "第%d行：缺少\'(\'；\n",line_count);
            break;
        case 29:
            fprintf(error_txt, "第%d行：for循环条件有误；\n",line_count);
            break;
        case 30:
            fprintf(error_txt, "第%d行：步长有误；\n",line_count);
            break;
        case 31:
            fprintf(error_txt, "第%d行：\'scanf\'语句格式错误，期待标识符；\n",line_count);
            break;
        case 32:
            fprintf(error_txt, "第%d行：无返回值函数无需返回；\n",line_count);
            break;
        case 33:
            fprintf(error_txt, "第%d行：函数调用的参数个数或类型不匹配；\n",line_count);
            break;
        case 34:
            fprintf(error_txt, "第%d行：主函数定义出错；\n",line_count);
            break;
        case 35:
            fprintf(error_txt, "第%d行：主函数后有多余内容；\n",line_count);
            break;
        case 36:
            fprintf(error_txt, "第%d行：赋值类型不匹配；\n",line_count);
            break;
        case 37:
            fprintf(error_txt, "第%d行：缺少while；\n",line_count);
            break;
        case 38:
            fprintf(error_txt, "第%d行：数组引用越界；\n",line_count);
            break;
        case 39:        
            fprintf(error_txt, "第%d行：有返回值的函数缺少返回语句；\n",line_count);
            break;
    }
	error_flag = 1;
}
