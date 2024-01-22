#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "list.h" 
#define SIZE 16






/*=================================================================================
 * Функция освобождает память, занимаемую списком (если он не пуст), и
 * делает список пустым. Переменную sizelist (размер списка) обнуляет, переменную
 * curlist, указывающую очередную свободную позицию в списке, тоже обнуляет.
 *=================================================================================
*/
void clearlist(List *lst)
  {
    int i;
    if (*lst==NULL) 
	  return;
    for (i=0; (*((*lst)+i))!=NULL; i++)
      free(*((*lst)+i));
    free(*lst);
    *lst=NULL;
  }

/*=================================================================================
 * Функция присваивает переменной buf значение NULL, переменной sizebuf
 * (размер буфера) присваивает значение 0, переменной curbuf, указывающей очередную
 * свободную позицию в буфере, присваивает значение 0
 *=================================================================================
*/
void nullbuf(int *sizebuf, int *curbuf, char **buf)
  {
    *buf=NULL;
    *sizebuf=0;
    *curbuf=0;
  }
  


/*=================================================================================
 * Функция печатает длину списка (число слов, без NULL), в каждой последующей –
 * очередной элемент списка. Если список пустой, ничего не делать.
 *=================================================================================
*/  
void printlist(List *lst)
  {
    int i;
    if (*lst==NULL) 
	    return;
    printf("\n");
    for (i=0; (*lst)[i] != NULL; i++)
      printf("%s\n",*(*lst + i));
  }



/*=================================================================================
 * Функция выполняет ввод большими блоками размера BUFSIZ и выдает символ 
 * по одному за раз
 *=================================================================================50
*/

int getsym(int *sizelist, int *curlist, char ***lst)
  {
    
    static char bufchar[BUFSIZ];
    static char * str = bufchar;
    static int n = 0;
    if (n == 0)
      {
        n = read(0, bufchar,BUFSIZ);
        str = bufchar;
      }
    
    if (--n >= 0) {
        return (unsigned char) *str++;
    }
    else
        return EOF;
  }


/*=================================================================================
 * Функция присваивает переменной lst, представляющую список, значение
 * (пустой список). Переменную sizelist (размер списка) обнуляет, переменную
 * curlist, указывающую очередную свободную позицию в списке, тоже обнуляет.
 *================================================================================= 
*/
void null_list(int *sizelist, int *curlist, char ***lst)
  {
    *sizelist=0;
    *curlist=0;
    *lst=NULL;
  }

/*=================================================================================
 * Функция завершает список, добавляя в позицию curlist и
 * обрезает память, занимаемую списком, до точного размера.
 *=================================================================================
*/
void termlist(int *sizelist, int *curlist, char ***lst)
  {
    if (*lst==NULL) 
	  return;
    if (*curlist>(*sizelist)-1)
      *lst=realloc(*lst,(*sizelist+1)*sizeof(**lst));
  
    *(*lst + (*curlist))=NULL;
    
    *lst=realloc(*lst,(*sizelist=(*curlist)+1)*sizeof(**lst));  /*выравниваем используемую под список память точно по размеру списка*/
  }
  

/*=================================================================================
 * Функция добавляет очередной символ в буфер в позицию curbuf , после чего
 * переменная curbuf увеличивается на 1 Если буфер был пуст, то он создается. Если
 * размер буфера превышен, то он увеличивается на константу SIZE, заданную директивой
 * define.
 *=================================================================================
*/
void addsym(int *sizebuf, int *curbuf, char **buf, char c)
  {
    if (*curbuf>(*sizebuf)-1)               /* увеличиваем буфер при необходимости */
      *buf=realloc(*buf, (*sizebuf)+=SIZE); 
	
	*(*buf +(*curbuf))=c;
        (*curbuf)++;
  }

/*=================================================================================
 * Функция завершает текущее слово в буфере, добавляя ’\0’ в позицию curbuf
 * (увеличив, если нужно, буфер), и обрезает память, занимаемую словом, до точного
 * размера; затем добавляет слово в список в позицию curlist, после чего значение
 * curlist увеличивается на 1 Если список был пуст, то он создается. Если размер списка
 * превышен, то он увеличивается на константу SIZE.
 *=================================================================================
*/
void addword(int *sizebuf, int *curbuf, char **buf, int *sizelist, int *curlist, char ***lst)
  {
    if (*curbuf>*sizebuf-1)             /* для записи ’\0’ увеличиваем буфер при необходимости */
      *buf=realloc(*buf, *sizebuf+=1);   
	*(*buf +(*curbuf))='\0';
        (*curbuf)++;
	
    *buf=realloc(*buf,*sizebuf=*curbuf);  /*выравниваем используемую память точно по размеру слова*/
    
	if (*curlist>(*sizelist)-1)             /* увеличиваем массив под список при необходимости */
      *lst=realloc(*lst, (*sizelist+=SIZE)*sizeof(**lst)); 
    
	*(*lst + (*curlist)) =*buf;
        (*curlist)++;
  }


/*=================================================================================
 * Функция выдает тип символа. Выдает 2, если это символы |, &, > (т.к. они могут 
 * быть частью более приоритных команд ||, &&, >>), выдает 1, если это символы
 * ; , <, (, ), иначе выдает 0
 *=================================================================================
*/
int specsym(int c)
  {
    if (c == ';' || c == '<' || c == '(' || c == ')')
      return 1;
    else if (c == '|' || c == '&' || c == '>')
      return 2;
    else
      return 0;
  }

/*=================================================================================
 * Функция выдает тип символа. Выдает 1, если это любой символ, кроме пробела, 
 * табуляции, перевода строки и специальных символов (|, ||, &, &&, ; , >, >>, 
 * <, (, ).), не конец файла (EOF), иначе выдает 0
 *=================================================================================
*/
int symset(int c)
  {
    return specsym(c) == 0 &&
           c != '\n' &&
           c != ' ' &&
           c != '\t' &&
           c != EOF ;
  }



void ChangeList(List *lst)
{
    #define MAX_LENGTH_PATH 511
    #define MAX_LENGTH_NAME 511
    #define MAX_LENGTH_ID 12
    char id_line[MAX_LENGTH_ID];
    int i;
    
    if (*lst == NULL)
        return;
    for (i = 0; (*lst)[i] != NULL; i++) 
    {
        if (strcmp((*lst)[i], "$HOME") == 0)
        {
            free((*lst)[i]);
            (*lst)[i] = malloc(MAX_LENGTH_PATH * sizeof(char));
            strcpy((*lst)[i], getenv("HOME"));
        }

        else if (strcmp((*lst)[i], "$SHELL") == 0)
        {
            free ((*lst)[i]);
            (*lst)[i] = malloc(MAX_LENGTH_PATH * sizeof(char));
            strcpy((*lst)[i], getenv("PWD"));
        }
        
        else if (strcmp((*lst)[i], "$EUID") == 0)
        {
            sprintf(id_line, "%d", geteuid());
            free ((*lst)[i]);
            (*lst)[i] = malloc(MAX_LENGTH_ID * sizeof(char));
            strcpy((*lst)[i], id_line );
        }

        else if (strcmp((*lst)[i], "$USER") == 0)
        {
            free ((*lst)[i]);
            (*lst)[i] = malloc(MAX_LENGTH_ID * sizeof(char));
            strcpy((*lst)[i], getlogin());
        }
    }
}






List MakeList() 
  {
    typedef enum {Start, Word, Greater, GreaterOr, GreaterAnd, GreaterSd, Greater2, Quotes, Grid, Slash, Stop} vertex;
    vertex V=Start;

    int c;         /*текущий символ */
    char ** lst;   /* список слов (в виде массива)*/
    char * buf;    /* буфер для накопления текущего слова*/
    int sizebuf;   /* размер буфера текущего слова*/
    int sizelist;  /* размер списка слов*/
    int curbuf;    /* индекс текущего символа в буфере*/
    int curlist;   /* индекс текущего слова в списке*/
    
    null_list(&sizelist, &curlist, &lst);
    c=getsym(&sizelist, &curlist, &lst);
    while(1) 
      switch(V)
	      {
          case Start:
            if(c==' '|| c=='\t') 
	        c=getsym(&sizelist, &curlist, &lst);
            else if ((c==EOF) || (c=='\n')) 
		          {
                termlist(&sizelist, &curlist, &lst);
                V=Stop;
              }
            else if (c == '#')
                V = Grid;
            else if (c == '\\') {
                V = Slash;
                c=getsym(&sizelist, &curlist, &lst);
            }
            else if (c == '"') {
                c=getsym(&sizelist, &curlist, &lst);
                nullbuf(&sizebuf, &curbuf, &buf);
                V = Quotes;
            }
            else 
		    		  {
                nullbuf(&sizebuf, &curbuf, &buf);
                addsym(&sizebuf, &curbuf, &buf, c);
                
                if (specsym(c) == 1)
                  V = Greater;
                else if (c == '|')
                  V = GreaterOr;
                else if (c == '&')
                  V = GreaterAnd;
                else if (c == '>')
                  V = GreaterSd;
                else
                  V = Word;
                
                c=getsym(&sizelist, &curlist, &lst);
              }
            break;
		case Grid:
		    c = getsym(&sizelist, &curlist, &lst);
		    if ((c == '\n') || (c == EOF)) {
		        V = Start;
		        addword(&sizebuf, &curbuf, &buf, &sizelist, &curlist, &lst);
		    }
		    break;
		case Slash:
		    c=getsym(&sizelist, &curlist, &lst);
		    if (c != '\n' && c != EOF)
		    {
		         addsym(&sizebuf, &curbuf, &buf, c);
		         V = Word;
		         c=getsym(&sizelist, &curlist, &lst);
		    }
		    else
		    {
		        V = Start;
		        addword(&sizebuf, &curbuf, &buf, &sizelist, &curlist, &lst);
		    }
		    break;		    
		
		
		
		
		
        case Word:
            if (c == '\\') {
                V = Slash;
                break;
            }
            if (c == '"') {
                V = Quotes;
                c = getsym(&sizelist, &curlist, &lst);
                break;
            }
            
            if (c == '#') {
                V = Grid;
                break;
            }
            if(symset(c)) {
                addsym(&sizebuf, &curbuf, &buf, c);
                c=getsym(&sizelist, &curlist, &lst);
            }
            else {
                V=Start;
                addword(&sizebuf, &curbuf, &buf, &sizelist, &curlist, &lst);
              }
            break;
		  
	    case Greater:
            V=Start;
            addword(&sizebuf, &curbuf, &buf, &sizelist, &curlist, &lst);
            break;
        
        case Quotes:
            if (c == '"') {
                V = Word;
                c = getsym(&sizelist, &curlist, &lst);
                break;
            }
            if ((c != '\n') && (c != EOF)) {
                addsym(&sizebuf, &curbuf, &buf, c);
                c=getsym(&sizelist, &curlist, &lst);
            }
            else {
                V = Start;
                addword(&sizebuf, &curbuf, &buf, &sizelist, &curlist, &lst);
            }
            break;
        
        
          
        case GreaterOr:
            if(c == '|') 
		    	  	{
                addsym(&sizebuf, &curbuf, &buf, c);
                c=getsym(&sizelist, &curlist, &lst);
                V=Greater2;
              }
            else 
		    	  	{
                V=Start;
                addword(&sizebuf, &curbuf, &buf, &sizelist, &curlist, &lst);
              }
            break;
          
        case GreaterAnd:
            if(c == '&') 
		    	  	{
                addsym(&sizebuf, &curbuf, &buf, c);
                c=getsym(&sizelist, &curlist, &lst);
                V=Greater2;
              }
            else 
		    	  	{
                V=Start;
                addword(&sizebuf, &curbuf, &buf, &sizelist, &curlist, &lst);
              }
            break;
          
        case GreaterSd:
            if(c == '>') 
		    	  	{
                addsym(&sizebuf, &curbuf, &buf, c);
                c=getsym(&sizelist, &curlist, &lst);
                V=Greater2;
              }
            else 
		    	  	{
                V=Start;
                addword(&sizebuf, &curbuf, &buf, &sizelist, &curlist, &lst);
              }
            break;
		  
        case Greater2:
            V=Start;
            addword(&sizebuf, &curbuf, &buf, &sizelist, &curlist, &lst);
            break;
		    
        case Stop:
            return lst;
            break;
        }
    }
