extern "C"
{
//#include <mathomatic/includes.h>
void free_mem(void);
void clear_all(void);
int process(char *cp);
void init_gvars(void);
int init_mem(void);
}
#include <mathomatic/includes.h>
#undef min
#include <stdlib.h>
#include <stdio.h>
#include <string>

bool init_mathomatic()
{
    init_gvars();
    gfp = stdout;
    if (!init_mem()) {
        return false;
    }
    return true;
};

bool done_mathomatic()
{
    free_mem();
    return true;
};

std::string math_process(const char* command, bool *Ok, std::string *ErrorStr)
{
std::string res;
char* p;
    p = new char[strlen(command)+1];
    strcpy(p, command);
    result_str = NULL;
    process(p);
    if(result_str != 0)
    {
        res = std::string(result_str);
        free(result_str);
        *Ok = true;
        ErrorStr->clear();
    }    else {
        if(error_str != 0)
            *ErrorStr = std::string(error_str);
        //error_str should not be free()d
        *Ok = false;
        res.clear();
    };
    delete [] p;
    return res;
};

std::string math_process(const char* command)
{
bool Ok;
std::string ErrorStr;
    return math_process(command, &Ok, &ErrorStr);
};


void math_clear_all()
{
//    math_process("clear all"); //почему-то завершается с ошибкой
    clear_all();
}
