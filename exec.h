#include "tree.h"
#include <unistd.h>

void CLEAR_ZOMBIE(void);
void CREATE_PID_LIST(void);
void add_zombie(pid_t);
void exec_simple_com(Tree , int );
void exec_conv(Tree );
void exec_com_list(Tree );

