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

struct DatabaseHeader {
    int count;
    int nextId;
};

// ---------- REMOVE NEWLINE ----------
void removeNewline(char str[]) {
    str[strcspn(str, "\n")] = '\0';
}

// ---------- CLEAR INPUT BUFFER ----------
void clearInputBuffer() {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}

// ---------- READ INTEGER ----------
int readInteger(const char *message) {
    int value;
    int result;

    while (1) {
        printf("%s", message);
        result = scanf("%d", &value);

        if (result == 1) {
            clearInputBuffer();
            return value;
        }

        if (result == EOF) {
            printf("\nInput ended. Exiting.\n");
            return -1;
        }

        printf("Invalid input! Please enter a whole number.\n");
        clearInputBuffer();
    }
}

// ---------- READ CGPA ----------
float readCGPA(const char *message) {
    float cgpa;
    char extra;

    while (1) {
        printf("%s", message);

        if (scanf("%f", &cgpa) != 1) {
            printf("Invalid input! Please enter a number.\n");
            clearInputBuffer();
            continue;
        }

        if (scanf("%c", &extra) == 1 && extra != '\n') {
            printf("Invalid input! Please enter a valid CGPA.\n");
            clearInputBuffer();
            continue;
        }

        if (cgpa < 0 || cgpa > 10) {
            printf("Invalid CGPA! CGPA must be between 0 and 10.\n");
            continue;
        }

        return cgpa;
    }
}

// ---------- SAVE DATABASE ----------
int saveDatabase(struct Student students[], int count, int nextId) {
    FILE *file = fopen(DATABASE_FILE, "wb");

    if (file == NULL) {
        printf("\nError: Could not open database file for writing.\n");
        printf("Check that the data folder exists and you are running "
               "the program from the project root.\n");
        return 0;
    }

    struct DatabaseHeader header;
    header.count = count;
    header.nextId = nextId;

    if (fwrite(&header, sizeof(header), 1, file) != 1 ||
        fwrite(students, sizeof(struct Student), count, file) !=
        (size_t)count) {
        printf("\nError: Could not save all database records.\n");
        fclose(file);
        return 0;
    }

    if (fclose(file) != 0) {
        printf("\nError: Could not finish saving the database.\n");
        return 0;
    }

    printf("\nDatabase saved successfully.\n");
    return 1;
}

// ---------- LOAD DATABASE ----------
int loadDatabase(struct Student students[], int *count, int *nextId) {
    FILE *file = fopen(DATABASE_FILE, "rb");

    if (file == NULL) {
        return 0;
    }

    struct DatabaseHeader header;

    if (fread(&header, sizeof(header), 1, file) != 1) {
        fclose(file);
        printf("\nError: Invalid or unreadable database file.\n");
        return 0;
    }

    if (header.count < 0 || header.count > MAX_STUDENTS ||
        header.nextId < 1) {
        fclose(file);
        printf("\nError: Invalid database metadata.\n");
        return 0;
    }

    if (fread(students, sizeof(struct Student), header.count, file) !=
        (size_t)header.count) {
        fclose(file);
        printf("\nError: Could not read database records.\n");
        return 0;
    }

    fclose(file);

    *count = header.count;
    *nextId = header.nextId;
    return 1;
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
    if (fgets(students[*count].name,
              sizeof(students[*count].name), stdin) == NULL) {
        printf("Could not read student name.\n");
        return;
    }
    removeNewline(students[*count].name);

    printf("Enter branch: ");
    if (fgets(students[*count].branch,
              sizeof(students[*count].branch), stdin) == NULL) {
        printf("Could not read branch.\n");
        return;
    }
    removeNewline(students[*count].branch);

    students[*count].cgpa = readCGPA("Enter CGPA (0 - 10): ");

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

// ---------- PRINT STUDENT ----------
void printStudent(struct Student student) {
    printf("\nID: %d\n", student.id);
    printf("Name: %s\n", student.name);
    printf("Branch: %s\n", student.branch);
    printf("CGPA: %.2f\n", student.cgpa);
}

// ---------- SEARCH ----------
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
        char key[50];

        if (choice == 2) {
            printf("Enter student name: ");
        } else {
            printf("Enter branch: ");
        }

        if (fgets(key, sizeof(key), stdin) == NULL) {
            printf("Could not read search value.\n");
            return;
        }
        removeNewline(key);

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

// ---------- DELETE ----------
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

// ---------- UPDATE ----------
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

    printf("Enter new name: ");
    if (fgets(students[found].name,
              sizeof(students[found].name), stdin) == NULL) {
        printf("Could not read name.\n");
        return;
    }
    removeNewline(students[found].name);

    printf("Enter new branch: ");
    if (fgets(students[found].branch,
              sizeof(students[found].branch), stdin) == NULL) {
        printf("Could not read branch.\n");
        return;
    }
    removeNewline(students[found].branch);

    students[found].cgpa = readCGPA("Enter new CGPA (0 - 10): ");
    printf("\nStudent updated successfully!\n");
}

// ---------- SORT ----------
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

// ---------- STATISTICS ----------
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

// ---------- MAIN ----------
int main(void) {
    struct Student students[MAX_STUDENTS];
    int count = 0;
    int nextId = 1;

    if (loadDatabase(students, &count, &nextId)) {
        printf("\nDatabase loaded successfully.\n");
        printf("Students loaded: %d\n", count);
    } else {
        printf("\nStarting with an empty database.\n");
    }

    while (1) {
        printf("\n========== MiniDB ==========\n");
        printf("1. Insert Student\n");
        printf("2. Display Students\n");
        printf("3. Search Student\n");
        printf("4. Delete Student\n");
        printf("5. Update Student\n");
        printf("6. Sort Students\n");
        printf("7. Student Statistics\n");
        printf("8. Save Database\n");
        printf("9. Exit\n");
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
                sortStudents(students, count);
                break;
            case 7:
                studentStatistics(students, count);
                break;
            case 8:
                saveDatabase(students, count, nextId);
                break;
            case 9:
                if (saveDatabase(students, count, nextId)) {
                    printf("\nExiting MiniDB...\n");
                    return 0;
                }
                printf("Exit cancelled because saving failed.\n");
                break;
            default:
                printf("\nInvalid choice! Please select 1-9.\n");
        }
    }
}