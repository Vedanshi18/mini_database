
#ifndef STUDENT_H
#define STUDENT_H

#define MAX_STUDENTS 100
#define STUDENT_NAME_SIZE 50
#define STUDENT_BRANCH_SIZE 20

/* The fixed record stored in memory and in the binary database file. */
struct Student {
    int id;
    char name[STUDENT_NAME_SIZE];
    char branch[STUDENT_BRANCH_SIZE];
    float cgpa;
};

/* Read validated values from the interactive menu. */
int readInteger(const char *message);
float readCGPA(const char *message);

/* Return nonzero only when every field contains a valid student value. */
int isValidStudent(const struct Student *student);

/* Menu operations use count as the number of active entries in students. */
void insertStudent(struct Student students[], int *count, int *nextId);
void displayStudents(struct Student students[], int count);
void printStudent(struct Student student);
void searchStudent(struct Student students[], int count);
void deleteStudent(struct Student students[], int *count);
void updateStudent(struct Student students[], int count);
void sortStudents(struct Student students[], int count);
void studentStatistics(struct Student students[], int count);

#endif
