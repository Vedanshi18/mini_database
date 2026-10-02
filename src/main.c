#include <stdio.h>
#include <string.h>

#define MAX_STUDENTS 100
#define DATABASE_FILE "data/database.dat"

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

// ---------- CLEAR INPUT BUFFER ----------
void clearInputBuffer() {
    while (getchar() != '\n');
}

// ---------- READ INTEGER ----------
int readInteger(const char *message) {

    int value;

    while (1) {

        printf("%s", message);

        if (scanf("%d", &value) == 1) {
            clearInputBuffer();
            return value;
        }

        printf("Invalid input! Please enter a whole number.\n");
        clearInputBuffer();
    }
}


// ---------- READ CGPA ----------
float readCGPA(const char *message) {

    float cgpa;

    while (1) {

        printf("%s", message);

        if (scanf("%f", &cgpa) != 1) {

            printf("Invalid input! Please enter a number.\n");
            clearInputBuffer();
        }
        else if (cgpa < 0 || cgpa > 10) {

            printf("Invalid CGPA! CGPA must be between 0 and 10.\n");
            clearInputBuffer();
        }
        else {

            clearInputBuffer();
            return cgpa;
        }
    }
}

// ---------- SAVE DATABASE ----------
void saveDatabase(struct Student students[], int count) {

    FILE *file = fopen(DATABASE_FILE, "wb");

    if (file == NULL) {
        printf("\nError: Could not open database file for writing.\n");
        return;
    }

    fwrite(&count, sizeof(int), 1, file);

    fwrite(
        students,
        sizeof(struct Student),
        count,
        file
    );

    fclose(file);

    printf("\nDatabase saved successfully.\n");
}

// ---------- INSERT ----------
void insertStudent(struct Student students[], int *count, int *nextId) {

    if (*count >= MAX_STUDENTS) {
        printf("\nDatabase is full!\n");
        return;
    }

    students[*count].id = *nextId;

    printf("\nStudent ID: %d\n", students[*count].id);

    printf("Enter student name: ");

    fgets(
        students[*count].name,
        sizeof(students[*count].name),
        stdin
    );

    removeNewline(students[*count].name);


    printf("Enter branch: ");

    fgets(
        students[*count].branch,
        sizeof(students[*count].branch),
        stdin
    );

    removeNewline(students[*count].branch);


    students[*count].cgpa =
        readCGPA("Enter CGPA (0 - 10): ");


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

    int id = readInteger("\nEnter student ID to search: ");

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

    int id = readInteger("\nEnter student ID to delete: ");

    int found = -1;

    // Find student
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

    // Shift remaining students left
    for (int i = found; i < *count - 1; i++) {
        students[i] = students[i + 1];
    }

    (*count)--;

    printf("\nStudent deleted successfully.\n");
}


// ---------- UPDATE ----------
void updateStudent(struct Student students[], int count) {

    if (count == 0) {
        printf("\nDatabase is empty.\n");
        return;
    }

    int id = readInteger("\nEnter student ID to update: ");

    int found = -1;

    // Find student
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


    printf("Enter new name: ");

    fgets(
        students[found].name,
        sizeof(students[found].name),
        stdin
    );

    removeNewline(students[found].name);


    printf("Enter new branch: ");

    fgets(
        students[found].branch,
        sizeof(students[found].branch),
        stdin
    );

    removeNewline(students[found].branch);


    students[found].cgpa =
        readCGPA("Enter new CGPA (0 - 10): ");


    printf("\nStudent updated successfully!\n");
}


// ---------- MAIN ----------
int main() {

    struct Student students[MAX_STUDENTS];

    int count = 0;
    int nextId = 1;

    while (1) {

        printf("\n========== MiniDB ==========\n");
        printf("1. Insert Student\n");
        printf("2. Display Students\n");
        printf("3. Search Student\n");
        printf("4. Delete Student\n");
        printf("5. Update Student\n");
        printf("6. Exit\n");
        printf("============================\n");

        int choice = readInteger("Enter choice: ");

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
                updateStudent(students, count);
                break;

            case 6:
                printf("\nExiting MiniDB...\n");
                return 0;

            default:
                printf("\nInvalid choice! Please select 1-6.\n");
        }
    }

    return 0;
}