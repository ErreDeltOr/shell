#include "list.h"
#include "exec.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>


static pid_t *PID_LIST;
static int size_pid_list = 0;


void pwd () {
    char *p;
    char *buf = NULL;
    p = getwd(buf);
    while ((*p)!='\0'){
        putchar(*p);
        p++;
    }
}

void cd(char ** argv)
  {
    if (argv[1] != NULL && argv[2] != NULL)
      fprintf(stderr, "не совпадает число параметров в команде cd\n");
    else
      {
        if (argv[1] != NULL)
          if (chdir(argv[1]) == -1)
            fprintf(stderr, "не удалось выполнить команду cd\n");
          else ;
        else
          if (chdir("/") == -1)
            fprintf(stderr, "не удалось выполнить команду cd\n");
          else ;
      }
  }




void CLEAR_ZOMBIE() {
    if (size_pid_list == 0) {
        return;
    }
    for (;size_pid_list != 0; ) {
        waitpid(PID_LIST[size_pid_list-1], NULL, 0);
        printf("[%d] - Done\n", PID_LIST[size_pid_list-1]);
        PID_LIST = realloc(PID_LIST, (--size_pid_list + 1)*(sizeof(pid_t)));
        PID_LIST[size_pid_list] = -1;
    }
    free(PID_LIST);
}


void CREATE_PID_LIST() {
    PID_LIST = NULL;
    PID_LIST = malloc(sizeof(pid_t));
    PID_LIST[0] = -1;
}

void add_zombie(pid_t pid) {
    PID_LIST = realloc(PID_LIST, (++size_pid_list + 1)*(sizeof(pid_t)));
    PID_LIST[size_pid_list - 1] = pid;
    PID_LIST[size_pid_list] = -1;
}





void exec_simple_com(Tree p, int x) {
    int fd1, fd2, fdev;
    
    if (p->backgrnd == 1)
        signal(SIGINT, SIG_IGN);
    else
        signal(SIGINT, SIG_DFL);
    if (x == 0) {
        if (p->backgrnd == 1) {
            fdev = open("/dev/null", O_RDWR | O_CREAT, 0666);
            dup2(fdev, 0);
            close(fdev);
        }
    }
    if (p->backgrnd != 1) {
        if (p->infile != NULL) {
            fd1 = open(p->infile, O_RDONLY | O_CREAT, 0666);
            if (fd1 == -1) {
                fprintf(stderr,"Ошибка при открытии файла %s\n", p->outfile);
                exit(-1);
            }
            dup2(fd1, 0);
            close(fd1);
        }
    }
    if (p->outfile != NULL) {
        if (p->endoutfile == 1) {
            fd2 = open(p->outfile, O_WRONLY | O_CREAT | O_APPEND, 0666);
        }
        else {
            fd2 = open(p->outfile, O_WRONLY | O_CREAT, 0666);
        }        
        if (fd2 == -1) {
            fprintf(stderr, "Ошибка при открытии файла %s\n", p->outfile);
            exit(-1);
        }
        dup2(fd2, 1);
        close(fd2);     
    }

    if (!strcmp(((p)->argv)[0], "pwd")) {
        pwd();
        exit(0);
    }
    execvp(((p)->argv)[0], (p)->argv);
    fprintf(stderr, "Ошибка при запуске %s\n", (p->argv)[0]);        
}



void exec_conv(Tree p) {

    int fd[2], in, out, next_in, i = 1;
    pid_t pid;
    Tree cur_cmd_tree;
    cur_cmd_tree = p;
    if (p->pipe != NULL) {
    while (cur_cmd_tree->pipe != NULL) {
        i = i + 1;
        cur_cmd_tree = cur_cmd_tree->pipe;
    }
    if (i == 1) {
        fprintf(stderr,"В конвейере %d команда\n", i);
    }
    else if ( i>1 && i <5) {
        fprintf(stderr,"В конвейере %d команды\n", i);
    }
    else {
        fprintf(stderr,"В конвейере %d команд\n", i);
    }
    cur_cmd_tree = p;
    }
    if (((p)->pipe) == NULL) {
        if (!strcmp(((p)->argv)[0], "cd")) {
            cd((p)->argv);
            return;
        }
        if ((pid = fork()) == 0) {
            exec_simple_com(p, 0);
            exit(-1);
        }
        if (p->backgrnd == 1) {
            add_zombie(pid);
        }
    }
    else {
        pipe(fd); out=fd[1]; next_in=fd[0];
        if (!strcmp(((p)->argv)[0], "cd")) {
            cd((p)->argv);
        }
        else if ((pid = fork())==0){
            close(next_in);
            dup2(out,1);
            close(out);
            exec_simple_com(p, 0);
            exit(-1);
        }
        if (p->backgrnd == 1) {
            add_zombie(pid);
        }
        in=next_in;
        (cur_cmd_tree) = (cur_cmd_tree)->pipe;
        for (;(cur_cmd_tree)->pipe != NULL; ){
            close(out);
            pipe(fd);
            out=fd[1];
            next_in=fd[0];
            if (!strcmp(((cur_cmd_tree)->argv)[0], "cd")) {
                cd((cur_cmd_tree)->argv);
                continue;
            }
            else if((pid =fork())==0){
                close(next_in);
                dup2(in,0);
                close(in);
                dup2(out,1);
                close(out);
                exec_simple_com(cur_cmd_tree, 1);
                exit(-1);
            }
            if (cur_cmd_tree->backgrnd == 1) {
                add_zombie(pid);
            }
            close(in);
            in=next_in;
            (cur_cmd_tree) = (cur_cmd_tree)->pipe;
        }    
        close(out);
        if (!strcmp(((cur_cmd_tree)->argv)[0], "cd")) {
            cd((cur_cmd_tree)->argv);
            return;
        }
        if ((pid =fork())==0){
            dup2(in,0);
            close(in);
            exec_simple_com(cur_cmd_tree, 1);
            exit(-1);
        }
        if (cur_cmd_tree->backgrnd == 1) {
            add_zombie(pid);
        }
        close(in);
    }
    cur_cmd_tree = p;
    if ((cur_cmd_tree->backgrnd) == 0) {
        for (; cur_cmd_tree != NULL;) {
            wait(NULL);
            (cur_cmd_tree) = (cur_cmd_tree)->pipe;
        }
    }
}



void exec_com_list(Tree p) {
    Tree cur_tree = p;
    CREATE_PID_LIST();
    while (cur_tree != NULL) {
        exec_conv(cur_tree);
        cur_tree = cur_tree->next;
    }
    return;
}
