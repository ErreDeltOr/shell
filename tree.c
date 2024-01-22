#include "tree.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void ClearArgvList(List *lst) {
    int i;
    if (*lst==NULL) 
	    return;
    for (i=0; *(*lst + i) != NULL; i++)
      free( *(*lst + i) );
    free(*lst);
    *lst=NULL;
}

void clear_tree(Tree *t)
  {
    if (*t == NULL)
      return;
    clear_tree(&((*t)->pipe));
    clear_tree(&((*t)->next));

    ClearArgvList(&((*t)->argv));
    free(*t);
  }
void cnt_space(int k) {
    while (k--) {
        printf(" ");
    }
}

void print_argv_list(char **argv, int shift_cnt) {
    int i = 0;
    if (argv != NULL) {
        while(*(argv + i) != NULL) {
            cnt_space(shift_cnt);
            printf("argv[%d] = %s\n", i, *(argv + i) );
            i++;
        }
    }
}
void print_tree(Tree t, int shift_cnt) {
    char **argv;

    if (t == NULL) {
        return;
    }
    argv = t->argv;
    if (argv != NULL) {
        print_argv_list(argv, shift_cnt);
    }
    
    cnt_space(shift_cnt);
        if(t->infile==NULL)
      printf("infile=NULL\n");
    else
      printf("infile=%s\n", t->infile);
      
    cnt_space(shift_cnt);
    if(t->outfile==NULL)
      printf("outfile=NULL\n");
    else
      printf("outfile=%s\n", t->outfile);
    
    cnt_space(shift_cnt);
    printf("endoutfile=%d\n", t->endoutfile);
    cnt_space(shift_cnt);
    printf("background=%d\n", t->backgrnd);
    
    cnt_space(shift_cnt);
    if(t->pipe==NULL)
      printf("pipe=NULL \n");
    else
      {
        printf("pipe--> \n");
        print_tree(t->pipe, shift_cnt+4);
      }
    
    cnt_space(shift_cnt);
    if(t->next==NULL)
      printf("next=NULL \n");
    else
      {
        printf("next--> \n");
        print_tree(t->next, shift_cnt+4);
      }
  
}
Tree Start_Tree () {
    Tree t;
    
    t = malloc(sizeof(*t));
    t->argv = NULL;
    t->infile = NULL;
    t->outfile = NULL;
    t->endoutfile = 0;
    t->backgrnd = 0;
    t->pipe = NULL;
    t->next = NULL;

    return t;
}

void with_backgrnd(Tree t) {
    t -> backgrnd = 1;
    if (t->pipe != NULL) {
        with_backgrnd(t->pipe);
    }
}

void add_argv(Tree t,char *plex) {
    
    int cnt = 0;
    List str;
    if (t->argv == NULL){
        if ( ((t->argv) = realloc(t->argv,2*sizeof(char *)) ) == NULL) {
            fprintf(stderr, "Ошибка выделения памяти!");
            clear_tree(&t);
            return;
        }
        if ( ( *(t->argv) = malloc( (strlen(plex)+1)*sizeof(char) ) ) == NULL )
          {
            fprintf(stderr, "Ошибка выделения памяти!");
            clear_tree(&t);
            return;
          }
        strcpy(*(t->argv), plex);
        *(t->argv+1) = NULL;
      }
    else {
        str = t->argv;
        while (*str != NULL)
          {
            cnt++;
            str++;
          }
        
        if ( ( t->argv=realloc(t->argv,(cnt+2)*sizeof(char *)) ) == NULL )
          {
            fprintf(stderr, "Ошибка выделения памяти!");
            clear_tree(&t);
            return;
          }
        if ( ( *(t->argv+cnt) = malloc( (strlen(plex)+1)*sizeof(char) ) ) == NULL)
          {
            fprintf(stderr, "Ошибка выделения памяти!");
            clear_tree(&t);
            return;
          }
        strcpy(*(t->argv+cnt), plex);
        *(t->argv+cnt+1) = NULL;
    }
    
    
}

int check_specsym_word(char *str) {
    if (str == NULL || strcmp(str, "|") == 0 || strcmp(str, ">") == 0 || strcmp(str, ">>") == 0 ||
    strcmp(str, "&") == 0 || strcmp(str, "<") == 0)
        return 1;
    else
        return 0;
}


Tree build_tree(List plex) {
    typedef enum {Start, End, SyntaxConveyer, Backgrnd, In, Out, OutFromEnd, Conveyer, Seq} vertex;
    vertex V=Start;
    
    Tree beg_cmd;
    Tree cur_cmd;
    Tree prev_cmd;
    Tree conv_cmd;
    
    while(1) {
        switch(V) {
            case Start:
                if (plex == NULL) {
                    beg_cmd = NULL; /* Корень строящегося дерева */
                    V = End;
                }
                else {
                    if ((strcmp(*plex, "|") == 0) || (strcmp(*plex, "&") == 0)) {
                        fprintf(stderr, "Неверное использование спецсимвола %s в команде\n", *plex);
                        clear_tree(&beg_cmd);
                        return NULL;
                    }
                    beg_cmd = Start_Tree();
                    cur_cmd = beg_cmd;
                    prev_cmd = cur_cmd;
                    conv_cmd = cur_cmd;
                    V = SyntaxConveyer;
                }
            break;
                
            case SyntaxConveyer :
                
                if (*plex == NULL) {
                    V = End;
                }
                else if (strcmp(*plex, "|") == 0) {
                    V = Conveyer;
                    plex++;
                }
                else if (strcmp(*plex, "&") == 0) {
                    V = Backgrnd;
                    plex++;
                }
                else if (strcmp(*plex, ">") == 0) {
                    plex ++;
                    V = Out;
                }
                else if (strcmp(*plex, ">>") == 0) {
                    plex++;
                    V = OutFromEnd;
                }
                else if (strcmp(*plex, "<") == 0) {
                    plex++;
                    V = In;
                }
                else if (strcmp(*plex, ";") == 0) {
                    plex++;
                    V = Seq;
                } 
                else {
                    add_argv(cur_cmd, *plex);
                    plex++;
                }
                break;
            case Conveyer:
                if ((strcmp(*plex, "|") == 0) || (strcmp(*plex, "&") == 0)) {
                    fprintf(stderr, "Неверное использование спецсимвола %s в команде\n", *plex);
                    clear_tree(&beg_cmd);
                    return NULL;
                }
                cur_cmd = Start_Tree();
                prev_cmd->pipe = cur_cmd;
                prev_cmd = cur_cmd;
                V = SyntaxConveyer;
                break;
            case In:
                if (check_specsym_word(*plex)) {
                    fprintf(stderr, "Неверное использование спецсимвола %s в команде\n", *plex);
                    clear_tree(&beg_cmd);
                    return NULL;
                }
                cur_cmd->infile = *plex;
                plex ++;
                V = SyntaxConveyer;
                break;
            case Out:
                if (check_specsym_word(*plex)) {
                    fprintf(stderr, "Неверное использование спецсимвола %s в команде\n", *plex);
                    clear_tree(&beg_cmd);
                    return NULL;
                }
                cur_cmd->outfile = *plex;
                plex ++;
                V = SyntaxConveyer;
                break;
            case OutFromEnd:
                if (check_specsym_word(*plex)) {
                    fprintf(stderr, "Неверное использование спецсимвола %s в команде\n", *plex);
                    clear_tree(&beg_cmd);
                    return NULL;
                }
                cur_cmd->outfile = *plex;
                cur_cmd->endoutfile = 1;
                plex ++;
                V = SyntaxConveyer;
                break;
            case Backgrnd:
                with_backgrnd(conv_cmd);
                
                if (*plex == NULL) {
                    V = End;
                }
                else {
                    cur_cmd = Start_Tree();
                    conv_cmd->next = cur_cmd; 
                    prev_cmd = cur_cmd; 
                    conv_cmd = cur_cmd;
                    V = SyntaxConveyer;                  
                }
            break;
            
            case Seq:
                    cur_cmd = Start_Tree();
                    conv_cmd->next = cur_cmd; 
                    prev_cmd = cur_cmd; 
                    conv_cmd = cur_cmd;
                    V = SyntaxConveyer;
            break;


            case End:
                return beg_cmd;
                break;
        }
    }
}