#ifndef QUERY_H
#define QUERY_H

#include "student.h"

/* Parse and run one statement against the shared in-memory student records. */
void executeSQLQuery(const char *query,
                     struct Student students[],
                     int *count,
                     int *nextId);

#endif
