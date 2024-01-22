#include "list.h"
#include "exec.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>



int main(int argc, char ** argv)
  {
    List lst = NULL;
    Tree tcmd = NULL;
    while (1!=2) {
        CLEAR_ZOMBIE();
        clear_tree(&tcmd);
        clearlist(&lst);
        lst = NULL;
        tcmd = NULL;
        lst = MakeList();
        ChangeList(&lst);
        /*printlist(&lst);*/
        tcmd = build_tree(lst);
        /*print_tree(tcmd, 4);*/
        if (tcmd != NULL) {
           exec_com_list(tcmd);
        }
        else {
            break;
        }
        
        printf("\n");
    }	


    clear_tree(&tcmd);
    clearlist(&lst); 
    
    printf("\n");
    /*printf("FINISH!\n");*/
    return 0;
  }