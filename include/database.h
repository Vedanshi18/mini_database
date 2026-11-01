#ifndef DATABASE_H
#define DATABASE_H

#include "student.h"

#define DATABASE_FILE "data/database.dat"

struct DatabaseHeader {
    int count;
    int nextId;
};

/* Save through a temporary file; return nonzero only after replacement succeeds. */
int saveDatabase(struct Student students[], int count, int nextId);
/* Returns 1 on success, 0 when no database exists, and -1 on an error. */
int loadDatabase(struct Student students[], int *count, int *nextId);

#endif
