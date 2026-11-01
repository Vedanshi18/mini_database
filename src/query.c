#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/database.h"
#include "../include/query.h"

enum Column
{
    COLUMN_ID,
    COLUMN_NAME,
    COLUMN_BRANCH,
    COLUMN_CGPA
};

/* The parser keeps its place in the query instead of copying each token. */
struct Parser
{
    const char *cursor;
};

/* A WHERE condition has one column, one operator, and one value. */
struct Filter
{
    enum Column column;
    char operatorText[3];
    int idValue;
    float cgpaValue;
    char textValue[STUDENT_NAME_SIZE];
};

static void skipWhitespace(struct Parser *parser)
{
    while (isspace((unsigned char)*parser->cursor))
        parser->cursor++;
}

static int equalsIgnoreCase(const char *left, const char *right)
{
    while (*left != '\0' && *right != '\0')
    {
        if (tolower((unsigned char)*left) !=
            tolower((unsigned char)*right))
        {
            return 0;
        }
        left++;
        right++;
    }

    return *left == '\0' && *right == '\0';
}

static int consumeKeyword(struct Parser *parser, const char *keyword)
{
    const char *cursor;
    const char *word = keyword;

    skipWhitespace(parser);
    cursor = parser->cursor;

    while (*word != '\0')
    {
        if (*cursor == '\0' ||
            tolower((unsigned char)*cursor) != tolower((unsigned char)*word))
        {
            return 0;
        }
        cursor++;
        word++;
    }

    if (isalnum((unsigned char)*cursor) || *cursor == '_')
        return 0;

    parser->cursor = cursor;
    return 1;
}

static int consumeCharacter(struct Parser *parser, char character)
{
    skipWhitespace(parser);
    if (*parser->cursor != character)
        return 0;

    parser->cursor++;
    return 1;
}

static int parseIdentifier(struct Parser *parser,
                           char output[],
                           size_t outputSize)
{
    size_t length = 0;

    skipWhitespace(parser);
    if (!(isalpha((unsigned char)*parser->cursor) ||
          *parser->cursor == '_'))
    {
        return 0;
    }

    while (isalnum((unsigned char)*parser->cursor) ||
           *parser->cursor == '_')
    {
        if (length + 1 >= outputSize)
            return 0;
        output[length++] = *parser->cursor++;
    }

    output[length] = '\0';
    return 1;
}
// Converts a string in the query into an integer
static int parseInteger(struct Parser *parser, int *value)
{
    char *end;
    long parsed;

    skipWhitespace(parser);
    /* strtol lets us detect both invalid text and numbers outside int range. */
    errno = 0;
    parsed = strtol(parser->cursor, &end, 10);
    if (end == parser->cursor || errno == ERANGE ||
        parsed < INT_MIN || parsed > INT_MAX)
    {
        return 0;
    }

    parser->cursor = end;
    *value = (int)parsed;
    return 1;
}

static int parseFloat(struct Parser *parser, float *value)
{
    char *end;
    float parsed;

    skipWhitespace(parser);
    /* Reject overflow and non-finite values before they reach record validation. */
    errno = 0;
    parsed = strtof(parser->cursor, &end);
    if (end == parser->cursor || errno == ERANGE || !isfinite(parsed))
        return 0;

    parser->cursor = end;
    *value = parsed;
    return 1;
}

static int parseString(struct Parser *parser, char output[], size_t outputSize)
{
    size_t length = 0;

    skipWhitespace(parser);
    if (*parser->cursor != '\'')
        return 0;
    parser->cursor++;

    while (*parser->cursor != '\0')
    {
        if (*parser->cursor == '\'')
        {
            /* SQL represents a quote inside a string as two single quotes. */
            if (parser->cursor[1] == '\'')
            {
                if (length + 1 >= outputSize)
                    return 0;
                output[length++] = '\'';
                parser->cursor += 2;
                continue;
            }

            parser->cursor++;
            output[length] = '\0';
            return 1;
        }

        if (length + 1 >= outputSize)
            return 0;
        output[length++] = *parser->cursor++;
    }

    return 0;
}

static int parseOperator(struct Parser *parser, char output[])
{
    skipWhitespace(parser);

    if ((parser->cursor[0] == '>' || parser->cursor[0] == '<' ||
         parser->cursor[0] == '!' || parser->cursor[0] == '=') &&
        parser->cursor[1] == '=')
    {
        output[0] = parser->cursor[0];
        output[1] = '=';
        output[2] = '\0';
        parser->cursor += 2;
        return 1;
    }

    if (parser->cursor[0] == '<' && parser->cursor[1] == '>')
    {
        strcpy(output, "<>");
        parser->cursor += 2;
        return 1;
    }

    if (*parser->cursor == '<' || *parser->cursor == '>' ||
        *parser->cursor == '=')
    {
        output[0] = *parser->cursor++;
        output[1] = '\0';
        return 1;
    }

    return 0;
}

static int finishStatement(struct Parser *parser)
{
    /* A semicolon is optional, but nothing may follow it. */
    skipWhitespace(parser);
    if (*parser->cursor == ';')
    {
        parser->cursor++;
        skipWhitespace(parser);
    }

    return *parser->cursor == '\0';
}

static int findColumn(const char *name, enum Column *column)
{
    if (equalsIgnoreCase(name, "id"))
        *column = COLUMN_ID;
    else if (equalsIgnoreCase(name, "name"))
        *column = COLUMN_NAME;
    else if (equalsIgnoreCase(name, "branch"))
        *column = COLUMN_BRANCH;
    else if (equalsIgnoreCase(name, "cgpa"))
        *column = COLUMN_CGPA;
    else
        return 0;

    return 1;
}

static const char *columnName(enum Column column)
{
    switch (column)
    {
        case COLUMN_ID: return "ID";
        case COLUMN_NAME: return "Name";
        case COLUMN_BRANCH: return "Branch";
        case COLUMN_CGPA: return "CGPA";
    }

    return "";
}

static int parseFilter(struct Parser *parser, struct Filter *filter)
{
    char columnNameBuffer[32];

    if (!parseIdentifier(parser, columnNameBuffer, sizeof(columnNameBuffer)) ||
        !findColumn(columnNameBuffer, &filter->column) ||
        !parseOperator(parser, filter->operatorText))
    {
        return 0;
    }

    switch (filter->column)
    {
        case COLUMN_ID:
            return parseInteger(parser, &filter->idValue);
        case COLUMN_CGPA:
            return parseFloat(parser, &filter->cgpaValue);
        case COLUMN_NAME:
            return parseString(parser, filter->textValue,
                               sizeof(filter->textValue));
        case COLUMN_BRANCH:
            return parseString(parser, filter->textValue,
                               STUDENT_BRANCH_SIZE);
    }

    return 0;
}

static int operatorMatchesInt(int value, const char *operatorText, int target)
{
    if (strcmp(operatorText, "=") == 0 || strcmp(operatorText, "==") == 0)
        return value == target;
    if (strcmp(operatorText, "!=") == 0 || strcmp(operatorText, "<>") == 0)
        return value != target;
    if (strcmp(operatorText, "<") == 0)
        return value < target;
    if (strcmp(operatorText, "<=") == 0)
        return value <= target;
    if (strcmp(operatorText, ">") == 0)
        return value > target;
    if (strcmp(operatorText, ">=") == 0)
        return value >= target;
    return 0;
}

static int operatorMatchesFloat(float value,
                                const char *operatorText,
                                float target)
{
    if (strcmp(operatorText, "=") == 0 || strcmp(operatorText, "==") == 0)
        return value == target;
    if (strcmp(operatorText, "!=") == 0 || strcmp(operatorText, "<>") == 0)
        return value != target;
    if (strcmp(operatorText, "<") == 0)
        return value < target;
    if (strcmp(operatorText, "<=") == 0)
        return value <= target;
    if (strcmp(operatorText, ">") == 0)
        return value > target;
    if (strcmp(operatorText, ">=") == 0)
        return value >= target;
    return 0;
}

static int matchesFilter(const struct Student *student,
                         const struct Filter *filter)
{
    /* Compare values according to the column's type. */
    switch (filter->column)
    {
        case COLUMN_ID:
            return operatorMatchesInt(student->id, filter->operatorText,
                                      filter->idValue);
        case COLUMN_CGPA:
            return operatorMatchesFloat(student->cgpa, filter->operatorText,
                                        filter->cgpaValue);
        case COLUMN_NAME:
            if (strcmp(filter->operatorText, "=") == 0 ||
                strcmp(filter->operatorText, "==") == 0)
                return strcmp(student->name, filter->textValue) == 0;
            if (strcmp(filter->operatorText, "!=") == 0 ||
                strcmp(filter->operatorText, "<>") == 0)
                return strcmp(student->name, filter->textValue) != 0;
            return 0;
        case COLUMN_BRANCH:
            if (strcmp(filter->operatorText, "=") == 0 ||
                strcmp(filter->operatorText, "==") == 0)
                return strcmp(student->branch, filter->textValue) == 0;
            if (strcmp(filter->operatorText, "!=") == 0 ||
                strcmp(filter->operatorText, "<>") == 0)
                return strcmp(student->branch, filter->textValue) != 0;
            return 0;
    }

    return 0;
}

static int validFilterOperator(const struct Filter *filter)
{
    if (filter->column == COLUMN_NAME || filter->column == COLUMN_BRANCH)
    {
        return strcmp(filter->operatorText, "=") == 0 ||
               strcmp(filter->operatorText, "==") == 0 ||
               strcmp(filter->operatorText, "!=") == 0 ||
               strcmp(filter->operatorText, "<>") == 0;
    }

    return 1;
}

static void printColumnHeader(const enum Column columns[], int columnCount)
{
    printf("\n");
    for (int i = 0; i < columnCount; i++)
    {
        switch (columns[i])
        {
            case COLUMN_ID: printf("%-6s ", columnName(columns[i])); break;
            case COLUMN_NAME: printf("%-25s ", columnName(columns[i])); break;
            case COLUMN_BRANCH: printf("%-15s ", columnName(columns[i])); break;
            case COLUMN_CGPA: printf("%-8s ", columnName(columns[i])); break;
        }
    }
    printf("\n");
}

static void printStudentColumns(const struct Student *student,
                                const enum Column columns[],
                                int columnCount)
{
    for (int i = 0; i < columnCount; i++)
    {
        switch (columns[i])
        {
            case COLUMN_ID: printf("%-6d ", student->id); break;
            case COLUMN_NAME: printf("%-25s ", student->name); break;
            case COLUMN_BRANCH: printf("%-15s ", student->branch); break;
            case COLUMN_CGPA: printf("%-8.2f ", student->cgpa); break;
        }
    }
    printf("\n");
}

static void executeSelect(struct Parser *parser,
                          struct Student students[],
                          int count)
{
    enum Column columns[4];
    int columnCount = 0;
    int selectAll = 0;
    int hasFilter = 0;
    struct Filter filter;
    char identifier[32];

    if (consumeCharacter(parser, '*'))
    {
        selectAll = 1;
    }
    else
    {
        do
        {
            if (columnCount >= 4 ||
                !parseIdentifier(parser, identifier, sizeof(identifier)) ||
                !findColumn(identifier, &columns[columnCount]))
            {
                printf("Invalid SELECT column list. Use id, name, branch, cgpa, or *.\n");
                return;
            }
            columnCount++;

            skipWhitespace(parser);
            if (*parser->cursor != ',')
                break;
            parser->cursor++;
        } while (1);
    }

    if (!consumeKeyword(parser, "FROM") ||
        !parseIdentifier(parser, identifier, sizeof(identifier)) ||
        !equalsIgnoreCase(identifier, "Student"))
    {
        printf("Invalid SELECT table. The supported table is Student.\n");
        return;
    }

    if (consumeKeyword(parser, "WHERE"))
    {
        hasFilter = 1;
        if (!parseFilter(parser, &filter) || !validFilterOperator(&filter))
        {
            printf("Invalid WHERE condition. Check the column, operator, and value.\n");
            return;
        }

        if (filter.column == COLUMN_CGPA &&
            (filter.cgpaValue < 0.0f || filter.cgpaValue > 10.0f))
        {
            printf("Invalid CGPA filter. It must be between 0 and 10.\n");
            return;
        }
    }

    if (!finishStatement(parser))
    {
        printf("Unexpected text after SELECT statement.\n");
        return;
    }

    if (selectAll)
    {
        /* Expand * to the four columns in the Student record. */
        columns[0] = COLUMN_ID;
        columns[1] = COLUMN_NAME;
        columns[2] = COLUMN_BRANCH;
        columns[3] = COLUMN_CGPA;
        columnCount = 4;
    }

    printColumnHeader(columns, columnCount);
    {
        int found = 0;
        for (int i = 0; i < count; i++)
        {
            if (!hasFilter || matchesFilter(&students[i], &filter))
            {
                printStudentColumns(&students[i], columns, columnCount);
                found++;
            }
        }

        if (found == 0)
            printf("No matching student records found.\n");
    }
}

static int findStudentById(const struct Student students[],
                           int count,
                           int id)
{
    for (int i = 0; i < count; i++)
    {
        if (students[i].id == id)
            return i;
    }
    return -1;
}

static void executeInsert(struct Parser *parser,
                          struct Student students[],
                          int *count,
                          int *nextId)
{
    struct Student student = {0};
    int originalNextId = *nextId;

    if (!consumeKeyword(parser, "INTO") ||
        !parseIdentifier(parser, student.name, sizeof(student.name)) ||
        !equalsIgnoreCase(student.name, "Student") ||
        !consumeKeyword(parser, "VALUES") ||
        !consumeCharacter(parser, '(') ||
        !parseInteger(parser, &student.id) ||
        !consumeCharacter(parser, ',') ||
        !parseString(parser, student.name, sizeof(student.name)) ||
        !consumeCharacter(parser, ',') ||
        !parseString(parser, student.branch, sizeof(student.branch)) ||
        !consumeCharacter(parser, ',') ||
        !parseFloat(parser, &student.cgpa) ||
        !consumeCharacter(parser, ')') ||
        !finishStatement(parser))
    {
        printf("Invalid INSERT syntax.\n");
        printf("Example: INSERT INTO Student VALUES (101, 'Riya', 'CSE', 9.1);\n");
        return;
    }

    if (*count >= MAX_STUDENTS)
    {
        printf("Database is full. Cannot insert more students.\n");
        return;
    }
    if (student.id == INT_MAX || !isValidStudent(&student))
    {
        printf("Invalid student data. IDs must be positive and CGPA must be 0 to 10.\n");
        return;
    }
    if (findStudentById(students, *count, student.id) != -1)
    {
        printf("Duplicate student ID %d.\n", student.id);
        return;
    }

    students[*count] = student;
    (*count)++;
    if (student.id >= *nextId)
        *nextId = student.id + 1;

    if (saveDatabase(students, *count, *nextId))
    {
        printf("Student inserted successfully.\n");
    }
    else
    {
        /* Keep memory consistent with disk if the save did not succeed. */
        (*count)--;
        *nextId = originalNextId;
        printf("Insert cancelled because the database could not be saved.\n");
    }
}

static void executeUpdate(struct Parser *parser,
                          struct Student students[],
                          int count,
                          int nextId)
{
    float oldCgpa;
    int id;
    int index;
    float cgpa;
    char column[32];

    if (!parseIdentifier(parser, column, sizeof(column)) ||
        !equalsIgnoreCase(column, "Student") ||
        !consumeKeyword(parser, "SET") ||
        !parseIdentifier(parser, column, sizeof(column)) ||
        !equalsIgnoreCase(column, "cgpa") ||
        !consumeCharacter(parser, '=') ||
        !parseFloat(parser, &cgpa) ||
        !consumeKeyword(parser, "WHERE") ||
        !parseIdentifier(parser, column, sizeof(column)) ||
        !equalsIgnoreCase(column, "id") ||
        !consumeCharacter(parser, '=') ||
        !parseInteger(parser, &id) ||
        !finishStatement(parser))
    {
        printf("Invalid UPDATE syntax.\n");
        printf("Example: UPDATE Student SET cgpa = 9.5 WHERE id = 101;\n");
        return;
    }

    if (cgpa < 0.0f || cgpa > 10.0f)
    {
        printf("Invalid CGPA. It must be between 0 and 10.\n");
        return;
    }

    index = findStudentById(students, count, id);
    if (index == -1)
    {
        printf("No student found with ID %d.\n", id);
        return;
    }

    oldCgpa = students[index].cgpa;
    students[index].cgpa = cgpa;

    if (saveDatabase(students, count, nextId))
    {
        printf("Student CGPA updated successfully.\n");
    }
    else
    {
        /* Restore the old CGPA when persistence fails. */
        students[index].cgpa = oldCgpa;
        printf("Update could not be saved; the database remains unchanged.\n");
    }
}

static void executeDelete(struct Parser *parser,
                          struct Student students[],
                          int *count,
                          int nextId)
{
    int id;
    int index;
    struct Student deletedStudent;
    char table[32];
    char column[32];

    if (!consumeKeyword(parser, "FROM") ||
        !parseIdentifier(parser, table, sizeof(table)) ||
        !equalsIgnoreCase(table, "Student") ||
        !consumeKeyword(parser, "WHERE") ||
        !parseIdentifier(parser, column, sizeof(column)) ||
        !equalsIgnoreCase(column, "id") ||
        !consumeCharacter(parser, '=') ||
        !parseInteger(parser, &id) ||
        !finishStatement(parser))
    {
        printf("Invalid DELETE syntax. Example: DELETE FROM Student WHERE id = 101;\n");
        return;
    }

    index = findStudentById(students, *count, id);
    if (index == -1)
    {
        printf("No student found with ID %d.\n", id);
        return;
    }

    deletedStudent = students[index];
    for (int i = index; i < *count - 1; i++)
        students[i] = students[i + 1];
    (*count)--;

    if (saveDatabase(students, *count, nextId))
    {
        printf("Student deleted successfully.\n");
    }
    else
    {
        /* Put the removed record back if the new file was not saved. */
        for (int i = *count; i > index; i--)
            students[i] = students[i - 1];
        students[index] = deletedStudent;
        (*count)++;
        printf("Delete cancelled because the database could not be saved.\n");
    }
}

// ***** Main SQL entry point ***
void executeSQLQuery(const char *query,
                     struct Student students[],
                     int *count,
                     int *nextId)
{
    struct Parser parser;

    if (query == NULL)
    {
        printf("Query cannot be empty.\n");
        return;
    }

    parser.cursor = query;
    skipWhitespace(&parser);
    if (*parser.cursor == '\0')
    {
        printf("Query cannot be empty.\n");
        return;
    }

    /* Consume the command once; its handler parses the rest of the statement. */
    if (consumeKeyword(&parser, "SELECT"))
    {
        executeSelect(&parser, students, *count);
    }
    else if (consumeKeyword(&parser, "INSERT"))
    {
        executeInsert(&parser, students, count, nextId);
    }
    else if (consumeKeyword(&parser, "UPDATE"))
    {
        executeUpdate(&parser, students, *count, *nextId);
    }
    else if (consumeKeyword(&parser, "DELETE"))
    {
        executeDelete(&parser, students, count, *nextId);
    }
    else
    {
        printf("Unsupported command. Use SELECT, INSERT, UPDATE, or DELETE.\n");
    }
}
