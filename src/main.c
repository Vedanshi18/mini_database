#include <stdio.h>
#include <string.h>

#define MAX_STUDENTS 100

struct Student
{
    int id;
    char name[50];
    char branch[20];
    float cgpa;
};

void insertStudent(struct Student students[], int *count)
{
    if (*count >= MAX_STUDENTS)
    {
        printf("Database is full!\n");
        return;
    }

    printf("\nEnter student ID: ");
    scanf("%d", &students[*count].id);

    printf("Enter student name: ");
    scanf("%49s", students[*count].name);

    printf("Enter branch: ");
    scanf("%19s", students[*count].branch);

    printf("Enter CGPA: ");
    scanf("%f", &students[*count].cgpa);

    (*count)++;

    printf("Student inserted successfully!\n");
}

void displayStudents(struct Student students[], int count)
{
    if (count == 0)
    {
        printf("\nDatabase is empty.\n");
        return;
    }

    printf("\n========== DATABASE ==========\n");
    for (int i = 0; i < count; i++)
    {
        printf("\nID: %d\n", students[i].id);
        printf("Name: %s\n", students[i].name);
        printf("Branch: %s\n", students[i].branch);
        printf("CGPA: %.2f\n", students[i].cgpa);
    }
}

void searchStudent(struct Student students[], int count)
{
    if (count == 0)
    {
        printf("\nDatabase is empty.\n");
        return;
    }
    int id;
    printf("\nEnter student ID to search: ");
    scanf("%d", &id);

    for (int i = 0; i < count; i++)
    {
        if (students[i].id == id)
        {
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

void deleteStudent(struct Student students[], int *count)
{
    if (*count == 0)
    {
        printf("\nDatabase is empty.\n");
        return;
    }
    int id;
    printf("\nEnter student ID to delete: ");
    scanf("%d", &id);

    for (int i = 0; i < *count; i++)
    {
        if (students[i].id == id)
        {
            for (int j = i; j < *count - 1; j++)
            {
                students[j] = students[j + 1];
            }
            (*count)--;
            printf("\nStudent with ID %d deleted successfully.\n", id);
            return;
        }
    }
    printf("\nStudent with ID %d not found.\n", id);
}
int main()
{
    struct Student students[MAX_STUDENTS];
    int count = 0;
    int choice;

    while (1)
    {
        printf("\n========== MiniDB ==========\n");
        printf("1. Insert Student\n");
        printf("2. Display Students\n");
        printf("3. Search Student\n");
        printf("4. Delete Student\n");
        printf("5. Exit\n");
        printf("============================\n");

        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            insertStudent(students, &count);
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
            printf("Exiting MiniDB...\n");
            return 0;

        default:
            printf("Invalid choice!\n");
        }
    }
    return 0;
}