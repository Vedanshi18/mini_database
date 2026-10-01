#include <stdio.h>
#include <string.h>

#define MAX_STUDENTS 100

struct Student {
    int id;
    char name[50];
    char branch[20];
    float cgpa;
};


// ---------- REMOVE NEWLINE ----------
void removeNewline(char str[]) {
    str[strcspn(str, "\n")] = '\0';
}

// ---------- INSERT ----------
void insertStudent(struct Student students[], int *count, int *nextId) {

    if (*count >= MAX_STUDENTS) {
        printf("\nDatabase is full!\n");
        return;
    }

    students[*count].id = *nextId;

    printf("\nStudent ID: %d\n", students[*count].id);

    // Clear newline left by previous scanf
    while (getchar() != '\n');

    printf("Enter student name: ");
    fgets(students[*count].name,
          sizeof(students[*count].name),
          stdin);

    removeNewline(students[*count].name);


    printf("Enter branch: ");
    fgets(students[*count].branch,
          sizeof(students[*count].branch),
          stdin);

    removeNewline(students[*count].branch);


    // CGPA validation
    while (1) {

        printf("Enter CGPA (0 - 10): ");

        if (scanf("%f", &students[*count].cgpa) != 1) {

            printf("Invalid input! Please enter a number.\n");

            while (getchar() != '\n');
        }
        else if (students[*count].cgpa < 0 ||
                 students[*count].cgpa > 10) {

            printf("Invalid CGPA! CGPA must be between 0 and 10.\n");

            while (getchar() != '\n');
        }
        else {
            break;
        }
    }

    (*count)++;
    (*nextId)++;

    printf("\nStudent inserted successfully!\n");
}


// ---------- DISPLAY ----------
void displayStudents(struct Student students[], int count) {

    if (count == 0) {
        printf("\nDatabase is empty.\n");
        return;
    }

    printf("\n========== DATABASE ==========\n");

    for (int i = 0; i < count; i++) {

        printf("\nID: %d\n", students[i].id);
        printf("Name: %s\n", students[i].name);
        printf("Branch: %s\n", students[i].branch);
        printf("CGPA: %.2f\n", students[i].cgpa);
    }

    printf("\n==============================\n");
}


// ---------- SEARCH ----------
void searchStudent(struct Student students[], int count) {

    if (count == 0) {
        printf("\nDatabase is empty.\n");
        return;
    }

    int id;

    printf("\nEnter student ID to search: ");
    scanf("%d", &id);

    for (int i = 0; i < count; i++) {

        if (students[i].id == id) {

            printf("\nStudent found!\n");

            printf("ID: %d\n", students[i].id);
            printf("Name: %s\n", students[i].name);
            printf("Branch: %s\n", students[i].branch);
            printf("CGPA: %.2f\n", students[i].cgpa);

            return;
        }
    }

    printf("\nStudent with ID %d not found.\n", id);
}


// ---------- DELETE ----------
void deleteStudent(struct Student students[], int *count) {

    if (*count == 0) {
        printf("\nDatabase is empty.\n");
        return;
    }

    int id;
    int found = -1;

    printf("\nEnter student ID to delete: ");
    scanf("%d", &id);

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


// ---------- MAIN ----------
int main() {

    struct Student students[MAX_STUDENTS];

    int count = 0;
    int nextId = 1;
    int choice;

    while (1) {

        printf("\n========== MiniDB ==========\n");
        printf("1. Insert Student\n");
        printf("2. Display Students\n");
        printf("3. Search Student\n");
        printf("4. Delete Student\n");
        printf("5. Exit\n");
        printf("============================\n");

        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {

            case 1:
                insertStudent(students, &count, &nextId);
                break;

            case 2:
                displayStudents(students, count);
                break;

            case 3:
                searchStudent(students, count);
                break;

            case 4:
                deleteStudent(students, &count);
                break;

            case 5:
                printf("\nExiting MiniDB...\n");
                return 0;

            default:
                printf("\nInvalid choice! Please select 1-5.\n");
        }
    }

    return 0;
}