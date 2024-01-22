#include "list.h"

struct cmd_inf {
    List argv;
    char *infile;
    char *outfile;
    int endoutfile; /* случай ">>" , тогда поле = 1, в противном случае = 0 */
    int backgrnd;
    struct cmd_inf* pipe;
    struct cmd_inf* next;
};

List plex; 
              
typedef struct cmd_inf * Tree;

void print_tree(Tree , int);

Tree build_tree(List );

void clear_tree(Tree *);