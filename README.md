# MiniDB — Lightweight Relational Database Engine

MiniDB is a small student database written in C. It combines in-memory CRUD
operations with binary-file persistence and an interactive SQL-like command
prompt. It is intended as an educational project; it is not a replacement for
a production database server.

MiniDB is a single-table database: `Student` is the built-in table, and its
schema is defined by the C `Student` structure. The SQL prompt implements a
useful subset of queries over that table, but it does not yet support dynamic
table definitions or arbitrary SQL schemas.

## Features

- Insert, display, search, update, and delete student records.
- Sort by ID, name, or CGPA; calculate CGPA and branch statistics.
- Store up to 100 records in `data/database.dat`.
- Validate student data, duplicate IDs, database metadata, and loaded records.
- Start directly in a `minidb>` SQL-like prompt and execute `SELECT`, `INSERT`,
  `UPDATE`, and `DELETE` statements.
- Open the original student-management menu from the prompt with `.menu`.
- Save SQL mutations immediately; menu-based changes are saved with **Save**
  or **Save and Exit**.
- Replace the binary database through a temporary file so an unsuccessful save
  does not truncate the previous database.

Each student has a positive integer ID, a non-empty name and branch, and a
finite CGPA from 0 through 10. Names can contain up to 49 characters and
branches up to 19 characters.

## How the code fits together

- `student.h` defines one student record. All menu and SQL commands work with
  the same `struct Student`, so they see the same data while MiniDB is running.
- `student.c` handles the menu operations and checks values entered by a
  person.
- `query.c` reads a SQL-like statement one piece at a time, checks its syntax,
  and runs it against the student records in memory.
- `database.c` checks records when loading or saving them. It writes to a
  temporary file first so an incomplete save does not replace the previous
  database.
- `main.c` loads records at startup, shows the `minidb>` prompt, and routes
  commands to the query engine. Type `.menu` to use the original menu.

For example, `INSERT` is parsed and validated in `query.c`, added to the same
student array the menu uses, and saved through `database.c`. A later `SELECT`
reads from that array. The database file is loaded again the next time the
program starts.

## Project layout

```text
mini_database/
├── data/
│   └── database.dat       # Created/updated at runtime; ignored by Git
├── include/
│   ├── student.h          # Student model and student-operation declarations
│   ├── database.h         # Binary persistence interface
│   └── query.h            # SQL-like query interface
├── src/
│   ├── main.c             # Menu and application entry point
│   ├── student.c          # Input validation and student operations
│   ├── database.c         # Validated binary load and safe save
│   └── query.c            # SQL-like parser, selection, and mutations
├── Makefile
├── README.md
└── .gitignore
```

The database file retains the project's existing binary layout: a header
containing the record count and next ID, followed by the student structures.
Keeping this layout allows existing `database.dat` files to be loaded. As with
any raw-structure binary format, the file is intended for use with a compatible
compiler and structure layout.

## Supported query syntax

After launching the program, enter SQL statements at the `minidb>` prompt.
Keywords and the table name `Student` are case-insensitive. A final semicolon
is optional. `Student` is the built-in table; `CREATE TABLE` is not currently
supported because the storage engine uses the fixed student-record schema.

```sql
SELECT * FROM Student;
SELECT name, cgpa FROM Student;
SELECT * FROM Student WHERE id = 2;
SELECT id, name FROM Student WHERE cgpa >= 9;
SELECT * FROM Student WHERE id != 2;
SELECT name FROM Student WHERE branch = 'CSE';

INSERT INTO Student VALUES (101, 'Riya', 'CSE', 9.1);
UPDATE Student SET cgpa = 9.5 WHERE id = 101;
DELETE FROM Student WHERE id = 101;
```

Use `.help` to display examples, `.menu` to open the existing CRUD menu, and
`.exit` (or `quit`) to save and close MiniDB.

`SELECT` supports `*` or a comma-separated list from `id`, `name`, `branch`,
and `cgpa`. A single `WHERE` comparison is supported using `=`, `==`, `!=`,
`<>`, `<`, `<=`, `>`, or `>=` for numeric columns. Text columns support
equality and inequality comparisons. `UPDATE` currently updates `cgpa` for one
ID; `DELETE` requires one ID condition. INSERT text values use single quotes;
double single quotes represent a quote inside a value.

Malformed statements, unsupported columns or tables, out-of-range values, and
duplicate IDs are rejected with an error message. Query input is limited to 511
characters.

## Build and run (Windows / MinGW)

Install GCC/MinGW and GNU Make, then open a terminal in the project directory:

```powershell
mingw32-make clean
mingw32-make
.\mini_db.exe
```

The build uses `-std=c99 -Wall -Wextra`. To use GCC directly:

```powershell
gcc -std=c99 -Wall -Wextra -Iinclude src/main.c src/student.c src/database.c src/query.c -o mini_db.exe
```

At startup MiniDB loads `data/database.dat` and displays the `minidb>` prompt.
A missing file starts a new empty database. If an existing file is corrupt or
fails validation, MiniDB reports the error and stops rather than silently
replacing that file.

## Ignore rules

`.gitignore` excludes compiled executables and object files, including
`mini_db.exe`, and excludes the local `data/database.dat` file. The database is
created locally and should not be committed.
