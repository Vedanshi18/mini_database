#include <errno.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif
 
#include "../include/database.h"
#define DATABASE_TEMP_FILE DATABASE_FILE ".tmp"

/*
 * The file stores raw Student records, so check every field before writing them and reject duplicate IDs before they become persistent.
*/
static int validDatabaseState(const struct Student students[], int count, int nextId)
{
    int largestId = 0;
    if (count < 0 || count > MAX_STUDENTS || nextId < 1)
        return 0;
    for (int i = 0; i < count; i++)
    {
        if (!isValidStudent(&students[i]))
            return 0;
        for (int j = 0; j < i; j++)
        {
            if (students[i].id == students[j].id)
                return 0;
        }
        if (students[i].id > largestId)
            largestId = students[i].id;
    }
    return nextId > largestId;
}

// Writes the student records to the binary database file
int saveDatabase(struct Student students[], int count, int nextId)
{
    FILE *file;
    struct DatabaseHeader header;
    int writeSucceeded = 1;

    if (!validDatabaseState(students, count, nextId))
    {
        printf("Error: Refusing to save invalid database records or metadata.\n");
        return 0;
    }
    /*
     * Write a complete replacement first. Only swap it into place after all  writes and the close succeed, leaving the old database intact on error.
     */
    file = fopen(DATABASE_TEMP_FILE, "wb");
    if (file == NULL)
    {
        printf("Error: Could not open temporary database file for saving.\n");
        return 0;
    }
    header.count = count;
    header.nextId = nextId;

    if (fwrite(&header, sizeof(header), 1, file) != 1)
        writeSucceeded = 0;

    if (writeSucceeded && count > 0 &&
        fwrite(students, sizeof(*students), (size_t)count, file)
            != (size_t)count)
    {
        writeSucceeded = 0;
    }

    if (writeSucceeded && fflush(file) != 0)
        writeSucceeded = 0;

    if (fclose(file) != 0)
        writeSucceeded = 0;

    if (!writeSucceeded)
    {
        printf("Error: Could not write the complete database file.\n");
        remove(DATABASE_TEMP_FILE);
        return 0;
    }

// Replaces the original database file with the completed temporary file
#ifdef _WIN32
    if (!MoveFileExA(DATABASE_TEMP_FILE, DATABASE_FILE,
                     MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
    {
        printf("Error: Could not replace database file (Windows error %lu).\n",
               (unsigned long)GetLastError());
        remove(DATABASE_TEMP_FILE);
        return 0;
    }
#else
if (rename(DATABASE_TEMP_FILE, DATABASE_FILE) != 0)
    {
        printf("Error: Could not replace database file: %s.\n",
               strerror(errno));
        remove(DATABASE_TEMP_FILE);
        return 0;
    }
#endif
    printf("Database saved successfully.\n");
    return 1;
}

int loadDatabase(struct Student students[], int *count, int *nextId)
{
    FILE *file = fopen(DATABASE_FILE, "rb");
    struct DatabaseHeader header;
    struct Student loadedStudents[MAX_STUDENTS];
    int loadedNextId;
    int extra;

    if (file == NULL)
    {
        if (errno == ENOENT)
        {
            *count = 0;
            *nextId = 1;
            return 0;
        }

        printf("Error: Could not open database file: %s.\n", strerror(errno));
        return -1;
    }

    if (fread(&header, sizeof(header), 1, file) != 1)
    {
        printf("Error: Database file has an invalid or incomplete header.\n");
        fclose(file);
        return -1;
    }

    if (header.count < 0 || header.count > MAX_STUDENTS ||
        header.nextId < 1)
    {
        printf("Error: Database file contains invalid metadata.\n");
        fclose(file);
        return -1;
    }

    /*
     * Read into a temporary array so a damaged file cannot leave the caller
     * with a partially replaced in-memory database.
     */
    if (header.count > 0 &&
        fread(loadedStudents, sizeof(*loadedStudents), (size_t)header.count,
              file) != (size_t)header.count)
    {
        printf("Error: Database file contains incomplete student records.\n");
        fclose(file);
        return -1;
    }

    extra = fgetc(file);
    if (extra != EOF || ferror(file))
    {
        printf("Error: Database file contains unexpected trailing data.\n");
        fclose(file);
        return -1;
    }

    if (fclose(file) != 0)
    {
        printf("Error: Could not close database file after reading.\n");
        return -1;
    }

    loadedNextId = header.nextId;
    if (!validDatabaseState(loadedStudents, header.count, loadedNextId))
    {
        printf("Error: Database records or metadata failed validation.\n");
        return -1;
    }

    if (header.count > 0)
        memcpy(students, loadedStudents,
               (size_t)header.count * sizeof(*students));
    *count = header.count;
    *nextId = loadedNextId;

    return 1;
}
