#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "student.h"

static int readLine(const char *message, char buffer[], size_t capacity)
{
    size_t length;
    int ch;

    printf("%s", message);
    if (fgets(buffer, (int)capacity, stdin) == NULL)
        return 0;

    length = strcspn(buffer, "\r\n");
    if (buffer[length] != '\0')
    {
        buffer[length] = '\0';
    }
    else if (!feof(stdin))
    {
        /* Discard the rest of an overlong line so it cannot become new input. */
        ch = getchar();
        if (ch != '\n' && ch != EOF)
        {
            while (ch != '\n' && ch != EOF)
                ch = getchar();
            printf("Input is too long. Please try again.\n");
            return 0;
        }
    }

    return 1;
}

// Prevent users from entering only whitespace for name or branch fields.
static int onlyWhitespace(const char *text)
{
    while (*text != '\0')
    {
        if (!isspace((unsigned char)*text))
            return 0;
        text++;
    }
    return 1;
}

static int readText(const char *message, char buffer[], size_t capacity)
{
    if (!readLine(message, buffer, capacity))
    {
        printf("Could not read the value.\n");
        return 0;
    }

    if (onlyWhitespace(buffer))
    {
        printf("Value cannot be empty.\n");
        return 0;
    }

    return 1;
}

int readInteger(const char *message)
{
    char line[128];
    char *end;
    long value;

    for (;;)
    {
        if (!readLine(message, line, sizeof(line)))
        {
            printf("\nInput ended.\n");
            return -1;
        }

        /* Reject values that overflow long or contain non-numeric text. */
        errno = 0;
        value = strtol(line, &end, 10);
        while (isspace((unsigned char)*end))
            end++;

        if (end != line && *end == '\0' && errno != ERANGE &&
            value >= INT_MIN && value <= INT_MAX)
        {
            return (int)value;
        }

        printf("Invalid input. Please enter a whole number.\n");
    }
}

float readCGPA(const char *message)
{
    char line[128];
    char *end;
    float cgpa;

    for (;;)
    {
        if (!readLine(message, line, sizeof(line)))
        {
            printf("\nInput ended.\n");
            return -1.0f;
        }

        /* Reject overflow, trailing text, NaN, infinity, and out-of-range CGPA. */
        errno = 0;
        cgpa = strtof(line, &end);
        while (isspace((unsigned char)*end))
            end++;

        if (end != line && *end == '\0' && errno != ERANGE &&
            isfinite(cgpa) && cgpa >= 0.0f && cgpa <= 10.0f)
        {
            return cgpa;
        }

        printf("Invalid CGPA. Enter a number between 0 and 10.\n");
    }
}

int isValidStudent(const struct Student *student)
{
    /* Check for a terminator before treating fixed-size fields as strings. */
    if (student == NULL || student->id <= 0 ||
        memchr(student->name, '\0', sizeof(student->name)) == NULL ||
        memchr(student->branch, '\0', sizeof(student->branch)) == NULL ||
        onlyWhitespace(student->name) || onlyWhitespace(student->branch) ||
        !isfinite(student->cgpa) ||
        student->cgpa < 0.0f || student->cgpa > 10.0f)
    {
        return 0;
    }

    return 1;
}

void insertStudent(struct Student students[], int *count, int *nextId) {
    struct Student student = {0};

    if (*count >= MAX_STUDENTS) {
        printf("\nDatabase is full!\n");
        return;
    }

    if (*nextId <= 0 || *nextId == INT_MAX) {
        printf("\nCannot assign another student ID.\n");
        return;
    }

    student.id = *nextId;
    printf("\nStudent ID: %d\n", student.id);

    if (!readText("Enter student name: ", student.name,
                  sizeof(student.name))) {
        return;
    }

    if (!readText("Enter branch: ", student.branch,
                  sizeof(student.branch))) {
        return;
    }

    student.cgpa = readCGPA("Enter CGPA (0 - 10): ");
    if (student.cgpa < 0.0f)
        return;

    if (!isValidStudent(&student)) {
        printf("Invalid student record. Nothing was inserted.\n");
        return;
    }

    students[*count] = student;
    (*count)++;
    (*nextId)++;

    printf("\nStudent inserted successfully!\n");
}

void displayStudents(struct Student students[], int count) {
    if (count == 0) {
        printf("\nDatabase is empty.\n");
        return;
    }

    printf("\n========== DATABASE ==========\n");

    for (int i = 0; i < count; i++) {
        printStudent(students[i]);
    }

    printf("\n==============================\n");
}

void printStudent(struct Student student) {
    printf("\nID: %d\n", student.id);
    printf("Name: %s\n", student.name);
    printf("Branch: %s\n", student.branch);
    printf("CGPA: %.2f\n", student.cgpa);
}

void searchStudent(struct Student students[], int count) {
    if (count == 0) {
        printf("\nDatabase is empty.\n");
        return;
    }

    printf("\n========== SEARCH ==========\n");
    printf("1. Search by ID\n");
    printf("2. Search by Name\n");
    printf("3. Search by Branch\n");

    int choice = readInteger("Enter search choice: ");
    int found = 0;

    if (choice == 1) {
        int id = readInteger("Enter student ID to search: ");

        for (int i = 0; i < count; i++) {
            if (students[i].id == id) {
                printStudent(students[i]);
                found = 1;
                break;
            }
        }

        if (!found) {
            printf("\nStudent with ID %d not found.\n", id);
        }
    } else if (choice == 2 || choice == 3) {
        char key[STUDENT_NAME_SIZE];
        const char *prompt;

        prompt = (choice == 2) ? "Enter student name: " : "Enter branch: ";

        if (!readLine(prompt, key, sizeof(key))) {
            printf("Could not read search value.\n");
            return;
        }

        for (int i = 0; i < count; i++) {
            const char *value =
                (choice == 2) ? students[i].name : students[i].branch;

            if (strcmp(value, key) == 0) {
                printStudent(students[i]);
                found = 1;
            }
        }

        if (!found) {
            printf("\nNo matching student found.\n");
        }
    } else {
        printf("\nInvalid search choice!\n");
    }
}

void deleteStudent(struct Student students[], int *count) {
    if (*count == 0) {
        printf("\nDatabase is empty.\n");
        return;
    }

    int id = readInteger("Enter student ID to delete: ");
    int found = -1;

    for (int i = 0; i < *count; i++) {
        if (students[i].id == id) {
            found = i;
            break;
        }
    }

    if (found == -1) {
        printf("\nStudent with ID %d not found.\n", id);
        return;
    }

    for (int i = found; i < *count - 1; i++) {
        students[i] = students[i + 1];
    }

    (*count)--;
    printf("\nStudent deleted successfully.\n");
}

void updateStudent(struct Student students[], int count) {
    if (count == 0) {
        printf("\nDatabase is empty.\n");
        return;
    }

    int id = readInteger("Enter student ID to update: ");
    int found = -1;

    for (int i = 0; i < count; i++) {
        if (students[i].id == id) {
            found = i;
            break;
        }
    }

    if (found == -1) {
        printf("\nStudent with ID %d not found.\n", id);
        return;
    }

    printf("\nUpdating Student ID: %d\n", students[found].id);

    struct Student updatedStudent = students[found];

    if (!readText("Enter new name: ", updatedStudent.name,
                  sizeof(updatedStudent.name))) {
        return;
    }

    if (!readText("Enter new branch: ", updatedStudent.branch,
                  sizeof(updatedStudent.branch))) {
        return;
    }

    updatedStudent.cgpa = readCGPA("Enter new CGPA (0 - 10): ");
    if (updatedStudent.cgpa < 0.0f)
        return;

    if (!isValidStudent(&updatedStudent)) {
        printf("Invalid student record. No changes were made.\n");
        return;
    }

    students[found] = updatedStudent;
    printf("\nStudent updated successfully!\n");
}

void sortStudents(struct Student students[], int count) {
    if (count == 0) {
        printf("\nDatabase is empty.\n");
        return;
    }

    printf("\n========== SORT ==========\n");
    printf("1. Sort by ID\n");
    printf("2. Sort by Name\n");
    printf("3. Sort by CGPA (highest first)\n");

    int choice = readInteger("Enter sort choice: ");

    if (choice < 1 || choice > 3) {
        printf("\nInvalid sort choice!\n");
        return;
    }

    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            int shouldSwap = 0;

            if (choice == 1 && students[j].id > students[j + 1].id) {
                shouldSwap = 1;
            } else if (choice == 2 &&
                       strcmp(students[j].name, students[j + 1].name) > 0) {
                shouldSwap = 1;
            } else if (choice == 3 &&
                       students[j].cgpa < students[j + 1].cgpa) {
                shouldSwap = 1;
            }

            if (shouldSwap) {
                struct Student temp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }
        }
    }

    printf("\nStudents sorted successfully!\n");
    displayStudents(students, count);
}

void studentStatistics(struct Student students[], int count) {
    if (count == 0) {
        printf("\nDatabase is empty.\n");
        return;
    }

    float totalCGPA = 0;
    float highestCGPA = students[0].cgpa;
    float lowestCGPA = students[0].cgpa;

    for (int i = 0; i < count; i++) {
        totalCGPA += students[i].cgpa;

        if (students[i].cgpa > highestCGPA) {
            highestCGPA = students[i].cgpa;
        }

        if (students[i].cgpa < lowestCGPA) {
            lowestCGPA = students[i].cgpa;
        }
    }

    printf("\n========== STATISTICS ==========\n");
    printf("Total Students: %d\n", count);
    printf("Average CGPA: %.2f\n", totalCGPA / count);
    printf("Highest CGPA: %.2f\n", highestCGPA);
    printf("Lowest CGPA: %.2f\n", lowestCGPA);
    printf("\nStudents by Branch:\n");

    for (int i = 0; i < count; i++) {
        int alreadyCounted = 0;

        for (int j = 0; j < i; j++) {
            if (strcmp(students[i].branch, students[j].branch) == 0) {
                alreadyCounted = 1;
                break;
            }
        }

        if (!alreadyCounted) {
            int branchCount = 0;

            for (int j = 0; j < count; j++) {
                if (strcmp(students[i].branch, students[j].branch) == 0) {
                    branchCount++;
                }
            }
            printf("%s: %d\n", students[i].branch, branchCount);
        }
    }
    printf("================================\n");
}