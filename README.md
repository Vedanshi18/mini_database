# MiniDB — Student Database Management System

## 1. Project Description

MiniDB is a menu-driven Student Database Management System developed in C. It allows users to manage student records through operations such as insertion, searching, updating, deletion, and sorting.

The project demonstrates core C programming concepts, including structures, arrays, functions, input validation, searching, sorting, and file handling. Student records are stored in a binary file so that saved data can be loaded when the application is restarted.

## 2. Project Goals

- Strengthen understanding of core C programming.
- Represent structured student information using structures.
- Implement common database operations.
- Apply searching and sorting algorithms.
- Learn file handling and persistent data storage.
- Organize a C project using a clean directory structure.
- Practise writing maintainable and modular code.

## 3. Features and Specifications

Each student record contains:
- **ID:** Automatically assigned student identifier.
- **Name:** Student's name.
- **Branch:** Student's academic branch.
- **CGPA:** Student's cumulative grade point average, between 0 and 10.

### Available Operations

1. **Insert Student:** Add a student record with an automatically assigned ID.
2. **Display Students:** View all stored student records.
3. **Search Student:** Search by ID, name, or branch.
4. **Delete Student:** Remove a student record using its ID.
5. **Update Student:** Modify a student's name, branch, or CGPA.
6. **Sort Students:** Sort records by ID, name, or CGPA.
7. **Student Statistics:** View the total number of students, average CGPA, highest CGPA, lowest CGPA, and student counts by branch.
8. **Save Database:** Save records without exiting the application.
9. **Exit:** Save the database and close the application.

## 4. Project Design

The application uses a menu-driven approach. Each major operation is implemented through a separate C function.

### Data Representation

A `Student` structure stores the ID, name, branch, and CGPA of each student. An array stores the student records in memory, with a maximum capacity of 100 students.

### File Storage

The application stores student records and database metadata in `data/database.dat` using binary file operations.

- At startup, the application attempts to load previously saved records.
- The Save Database option writes the current records to the file.
- The Exit option saves the records before terminating.

The database file is generated at runtime and is excluded from version control.

### Searching and Sorting

The application supports searching by student ID, name, and branch. Sorting is implemented using Bubble Sort, with options for ascending ID, alphabetical name, and descending CGPA.

## 5. Technologies and Concepts

- **Programming language:** C
- **Compiler:** GCC
- **Data structures:** Structures and arrays
- **Algorithms:** Linear search and Bubble Sort
- **File handling:** Binary file input/output
- **Other concepts:** Functions, loops, conditional statements, input validation, and string handling
- **Version control:** Git and GitHub

## 6. Project Structure

```text
mini_database/
├── data/
│   └── .gitkeep
├── src/
│   └── main.c
├── include/
├── .gitignore
├── LICENSE
├── Makefile
└── README.md
```

The `include/` directory is reserved for header files if the project is modularized further.

The `data/database.dat` file is created locally when the application saves records. It is not committed to GitHub.

## 7. Prerequisites

- GCC C compiler
- Windows PowerShell or another compatible terminal
- Git (optional, for version control)

## 8. Compilation and Execution

Open a terminal in the project root directory.

### Option A: Compile directly with GCC

```bash
gcc -Wall -Wextra src/main.c -o mini_db.exe
```

Run the program in Windows PowerShell:

```powershell
.\mini_db.exe
```

### Option B: Build using Make

If a compatible Make utility is installed, run:

```bash
make
```

On MinGW installations, the command may instead be:

```powershell
mingw32-make
```

Then run:

```powershell
.\mini_db.exe
```

## 9. Example Usage

1. Start the application.
2. Select **Insert Student** and enter the student's details.
3. Select **Display Students** to view the records.
4. Use **Search Student** to find a particular student.
5. Update, delete, or sort records as needed.
6. Select **Save Database** to save without exiting.
7. Restart the application to verify that saved records are loaded.

## 10. Limitations

- The application supports a maximum of 100 student records.
- Name and branch searches use exact matching.
- Records are stored in a binary file and are intended to be accessed through the application.
- The program uses a local file rather than a database server.

## 11. Future Improvements

- Separate declarations into header files and implementation into multiple source files.
- Add more robust recovery from corrupted or incomplete database files.
- Add automated tests for database operations.
- Improve search functionality with partial matching.
- Support larger datasets and more advanced indexing techniques.

## 12. License

This project is licensed under the MIT License. See the `LICENSE` file for details.