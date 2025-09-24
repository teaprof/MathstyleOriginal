#include <string>

bool init_mathomatic();
bool done_mathomatic();
void math_clear_all();  // очищает буфер команд, нужно вызывать как можно чаще, так как буфер ограничен, см. am.h:#define
                        // N_EQUATIONS     200

std::string math_process(const char* command);
std::string math_process(const char* command, bool* Ok, std::string* ErrorStr);
