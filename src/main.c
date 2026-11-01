
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/student.h"
#include "../include/database.h"
#include "../include/query.h"

static int readQueryLine(const char *prompt, char query[], size_t capacity)
{
    size_t length;
    int ch;

    printf("%s", prompt);
    if (fgets(query, (int)capacity, stdin) == NULL)
        return 0;

    length = strcspn(query, "\r\n");
    if (query[length] != '\0')
    {
        query[length] = '\0';
        return 1;
    }

    if (!feof(stdin))
    {
        ch = getchar();
        if (ch != '\n' && ch != EOF)
        {
            while (ch != '\n' && ch != EOF)
                ch = getchar();
            printf("Query is too long. Maximum length is %lu characters.\n",
                   (unsigned long)(capacity - 1));
            return -1;
        }
    }

    return 1;
}

static int runMenu(struct Student students[], int *count, int *nextId)
{
    int choice;

    while (1)
    {
        printf("\n========== MAIN MENU ==========\n");
        printf("1. Insert Student\n");
        printf("2. Display All Students\n");
        printf("3. Search Student\n");
        printf("4. Delete Student\n");
        printf("5. Update Student\n");
        printf("6. Sort Students\n");
        printf("7. Student Statistics\n");
        printf("8. Execute SQL-like Query\n");
        printf("9. Save Database\n");
        printf("10. Return to SQL prompt\n");
        printf("0. Save and Exit\n");
        printf("===============================\n");

        choice = readInteger("Enter your choice: ");
        if (choice == -1 && feof(stdin))
        {
            return saveDatabase(students, *count, *nextId);
        }

        switch (choice)
        {
            case 1:
                insertStudent(students, count, nextId);
                break;

            case 2:
                displayStudents(students, *count);
                break;

            case 3:
                searchStudent(students, *count);
                break;

            case 4:
                deleteStudent(students, count);
                break;

            case 5:
                updateStudent(students, *count);
                break;

            case 6:
                sortStudents(students, *count);
                break;

            case 7:
                studentStatistics(students, *count);
                break;

            case 8:
            {
                char query[512];
                int readStatus;

                readStatus = readQueryLine("Enter SQL-like query: ", query,
                                           sizeof(query));
                if (readStatus == 0)
                    printf("Could not read query.\n");
                else if (readStatus > 0)
                    executeSQLQuery(query, students, count, nextId);

                break;
            }

            case 9:
                saveDatabase(students, *count, *nextId);
                break;

            case 10:
                return 0;

            case 0:
                if (saveDatabase(students, *count, *nextId))
                {
                    printf("Exiting MiniDB. Goodbye!\n");
                    return 1;
                }

                printf("Exit cancelled because saving failed.\n");
                break;

            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}

static void printSQLHelp(void)
{
    printf("MiniDB SQL prompt. End statements with a semicolon (optional).\n");
    printf("This build has one built-in table: Student (fixed schema).\n");
    printf("Examples:\n");
    printf("  SELECT * FROM Student;\n");
    printf("  SELECT name, cgpa FROM Student WHERE cgpa >= 9;\n");
    printf("  INSERT INTO Student VALUES (101, 'Riya', 'CSE', 9.1);\n");
    printf("  UPDATE Student SET cgpa = 9.5 WHERE id = 101;\n");
    printf("  DELETE FROM Student WHERE id = 101;\n");
    printf("Commands: .help  .menu  .exit\n");
}

int main(void)
{
    struct Student students[MAX_STUDENTS];
    int count = 0;
    int nextId = 1;
    int loadStatus;
    char query[512];

    printf("====================================\n");
    printf("  MiniDB - Lightweight SQL Database\n");
    printf("====================================\n");

    loadStatus = loadDatabase(students, &count, &nextId);
    if (loadStatus == 1)
    {
        printf("Loaded %d student record(s).\n", count);
    }
    else if (loadStatus == 0)
    {
        printf("No database file found. Starting with an empty database.\n");
    }
    else
    {
        printf("MiniDB cannot continue because the database could not be loaded.\n");
        return EXIT_FAILURE;
    }

    printSQLHelp();

    while (1)
    {
        int readStatus = readQueryLine("minidb> ", query, sizeof(query));

        if (readStatus == 0)
        {
            printf("\n");
            return saveDatabase(students, count, nextId)
                ? EXIT_SUCCESS : EXIT_FAILURE;
        }
        if (readStatus < 0)
            continue;

        if (strcmp(query, ".exit") == 0 ||
            strcmp(query, "exit") == 0 ||
            strcmp(query, "quit") == 0)
        {
            if (saveDatabase(students, count, nextId))
            {
                printf("Exiting MiniDB. Goodbye!\n");
                return EXIT_SUCCESS;
            }

            printf("Exit cancelled because saving failed.\n");
            continue;
        }
        if (strcmp(query, ".help") == 0 || strcmp(query, "help") == 0)
        {
            printSQLHelp();
            continue;
        }
        if (strcmp(query, ".menu") == 0)
        {
            if (runMenu(students, &count, &nextId))
                return EXIT_SUCCESS;
            continue;
        }

        executeSQLQuery(query, students, &count, &nextId);
    }
}
