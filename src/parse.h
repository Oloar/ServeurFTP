#ifndef __PARSE_H__
#define __PARSE_H__

#define BLOCKSIZE 512

#define NBACTIONS 6
#define GET 0
#define PUT 1
#define QUIT 2
#define LS 3
#define PWD 4
#define CD 5


int command_code(char *action);
char **split_in_words(char *line);
int nb_words(char **t);



#endif