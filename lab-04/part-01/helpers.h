#ifndef _HELPERS_H_
#define _HELPERS_H_

#define TEMPLATE_SIZE 1000

char *ENVIRON_TEMPLATE[TEMPLATE_SIZE];

char *STAT_TEMPLATE[TEMPLATE_SIZE];

int dopath(const char *pathname, int depth, FILE *dest);

#endif  // _HELPERS_H_