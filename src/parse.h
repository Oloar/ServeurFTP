#ifndef __PARSE_H__
#define __PARSE_H__

#define BLOCKSIZE 8192

#define NBACTIONS 3
#define GET 0
#define PUT 1
#define QUIT 2


int command_code(char *action);
char **split_in_words(char *line);
int nb_words(char **t);



#endif