/*
 Single-file Student Result Management System (Full Version, No CSV)
 Features:
 - Admin login (masked), change password
 - Flexible subjects (saved to subjects.cfg)
 - Add / Display / Search / Update / Delete students
 - Duplicate roll prevention
 - Sorting & Ranking (roll, name, percentage)
 - Pagination (5 records per page)
 - Report card generation (reports/report_roll_<roll>.txt)
 - Backup & restore
 - Analytics & statistics
 - Colored UI (ANSI escape codes)
 - All data in student.dat (binary)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#ifdef _WIN32
  #include <conio.h>
  #include <windows.h>
#else
  #include <termios.h>
  #include <unistd.h>
#endif

// -------- CONFIG --------
#define DATA_FILE "student.dat"
#define SUBJECTS_FILE "subjects.cfg"
#define ADMIN_FILE "admin.cfg"
#define BACKUP_FILE "student_backup.dat"
#define REPORTS_DIR "reports"
#define MAX_NAME_LEN 100
#define MAX_SUBJECTS 10
#define RECORDS_PER_PAGE 5

// Color codes (ANSI)
#define COL_RESET "\033[0m"
#define COL_RED   "\033[1;31m"
#define COL_GREEN "\033[1;32m"
#define COL_YELLOW "\033[1;33m"
#define COL_BLUE  "\033[1;34m"
#define COL_CYAN  "\033[1;36m"

// -------- DATA STRUCTURES --------
typedef struct {
    int rollNo;
    char name[MAX_NAME_LEN];
    float marks[MAX_SUBJECTS];
    float total;
    float percentage;
    char grade;
} Student;

int SUBJECT_COUNT = 3; // default
char SUBJECT_NAMES[MAX_SUBJECTS][50];

// -------- CROSS-PLATFORM getch (masked input) --------
int getch_noecho() {
#ifdef _WIN32
    return _getch();
#else
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
#endif
}

void sleep_ms(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

// -------- UTILS --------
void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pause_anykey() {
    printf("\n---------------------------------------------------------\n");
    printf("Press ENTER to continue...");
    getchar();
}


void to_titlecase(char *s) {
    int i = 0, capNext = 1;
    while (s[i]) {
        if (isspace((unsigned char)s[i])) { capNext = 1; }
        else if (capNext) { s[i] = toupper((unsigned char)s[i]); capNext = 0; }
        else s[i] = tolower((unsigned char)s[i]);
        i++;
    }
}

void safe_fgets(char *buf, int n) {
    if (fgets(buf, n, stdin)) {
        size_t ln = strcspn(buf, "\n");
        if (ln < strlen(buf)) buf[ln] = '\0';
        else buf[ln] = '\0';
    } else {
        buf[0] = '\0';
    }
}

// Create reports dir
void ensure_reports_dir() {
#ifdef _WIN32
    CreateDirectoryA(REPORTS_DIR, NULL);
#else
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "mkdir -p %s", REPORTS_DIR);
    system(cmd);
#endif
}

// -------- WELCOME SCREEN --------
void show_welcome_screen() {
    clear_screen();

    printf("\n\n");
    printf(COL_CYAN "=========================================================\n" COL_RESET);
    printf(COL_GREEN "            C PROGRAMMING FINAL PROJECT                \n" COL_RESET);
    printf(COL_GREEN "        STUDENT RESULT MANAGEMENT SYSTEM               \n" COL_RESET);
    printf(COL_YELLOW "     Submitted by Dhruv Jha to Pankaj Badoni Sir         \n" COL_RESET);
    printf(COL_CYAN "=========================================================\n" COL_RESET);

    printf("\n\n");
    printf("              Press ENTER to continue...               ");

    while (getchar() != '\n');
}



// -------- SUBJECTS MANAGEMENT --------
void load_subjects() {
    FILE *fp = fopen(SUBJECTS_FILE, "r");
    if (!fp) {
        SUBJECT_COUNT = 3;
        strncpy(SUBJECT_NAMES[0], "Math", sizeof(SUBJECT_NAMES[0]));
        strncpy(SUBJECT_NAMES[1], "Physics", sizeof(SUBJECT_NAMES[1]));
        strncpy(SUBJECT_NAMES[2], "Chemistry", sizeof(SUBJECT_NAMES[2]));
        return;
    }
    if (fscanf(fp, "%d\n", &SUBJECT_COUNT) != 1) SUBJECT_COUNT = 3;
    if (SUBJECT_COUNT < 1) SUBJECT_COUNT = 1;
    if (SUBJECT_COUNT > MAX_SUBJECTS) SUBJECT_COUNT = MAX_SUBJECTS;
    char line[200];
    for (int i = 0; i < SUBJECT_COUNT; ++i) {
        if (fgets(line, sizeof(line), fp)) {
            line[strcspn(line, "\n")] = '\0';
            strncpy(SUBJECT_NAMES[i], line, sizeof(SUBJECT_NAMES[i]));
        } else {
            snprintf(SUBJECT_NAMES[i], sizeof(SUBJECT_NAMES[i]), "Subject%d", i+1);
        }
    }
    fclose(fp);
}

void save_subjects() {
    FILE *fp = fopen(SUBJECTS_FILE, "w");
    if (!fp) {
        printf(COL_RED "Error writing subjects file!\n" COL_RESET);
        return;
    }
    fprintf(fp, "%d\n", SUBJECT_COUNT);
    for (int i = 0; i < SUBJECT_COUNT; ++i) fprintf(fp, "%s\n", SUBJECT_NAMES[i]);
    fclose(fp);
}

void configure_subjects() {
    printf(COL_CYAN "----- Configure Subjects (max %d) -----\n" COL_RESET, MAX_SUBJECTS);
    printf("Current subject count: %d\n", SUBJECT_COUNT);
    printf("Enter new subject count (1-%d): ", MAX_SUBJECTS);
    int n;
    if (scanf("%d", &n) != 1) {
        printf(COL_RED "Invalid input. Aborting.\n" COL_RESET);
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');
    if (n < 1 || n > MAX_SUBJECTS) {
        printf(COL_RED "Out of range. Aborting.\n" COL_RESET);
        return;
    }
    SUBJECT_COUNT = n;
    for (int i = 0; i < SUBJECT_COUNT; ++i) {
        printf("Enter name for subject %d: ", i+1);
        safe_fgets(SUBJECT_NAMES[i], sizeof(SUBJECT_NAMES[i]));
        if (strlen(SUBJECT_NAMES[i]) == 0) snprintf(SUBJECT_NAMES[i], sizeof(SUBJECT_NAMES[i]), "Subject%d", i+1);
        to_titlecase(SUBJECT_NAMES[i]);
    }
    save_subjects();
    printf(COL_GREEN "Subjects updated and saved.\n" COL_RESET);
    pause_anykey();
}

// -------- ADMIN LOGIN & PASSWORD --------
void ensure_admin_file() {
    FILE *fp = fopen(ADMIN_FILE, "r");
    if (!fp) {
        fp = fopen(ADMIN_FILE, "w");
        if (fp) {
            fprintf(fp, "admin\n"); // default password
            fclose(fp);
            printf(COL_YELLOW "No admin.cfg found — default password 'admin' created.\n" COL_RESET);
            pause_anykey();
        }
    } else fclose(fp);
}

void change_admin_password() {
    printf("Enter new admin password: ");
    char pwd1[200], pwd2[200];
    int idx = 0, ch;
    idx = 0;
    while (1) {
        ch = getch_noecho();
        if (ch == '\n' || ch == '\r') { pwd1[idx] = '\0'; printf("\n"); break; }
        if (ch == 127 || ch == 8) { if (idx>0) { idx--; printf("\b \b"); } continue; }
        if (idx < (int)sizeof(pwd1)-1) { pwd1[idx++] = ch; printf("*"); }
    }
    printf("Confirm new password: ");
    idx = 0;
    while (1) {
        ch = getch_noecho();
        if (ch == '\n' || ch == '\r') { pwd2[idx] = '\0'; printf("\n"); break; }
        if (ch == 127 || ch == 8) { if (idx>0) { idx--; printf("\b \b"); } continue; }
        if (idx < (int)sizeof(pwd2)-1) { pwd2[idx++] = ch; printf("*"); }
    }
    if (strcmp(pwd1, pwd2) != 0) {
        printf(COL_RED "Passwords do not match. Aborting.\n" COL_RESET);
        pause_anykey();
        return;
    }
    FILE *fp = fopen(ADMIN_FILE, "w");
    if (!fp) { printf(COL_RED "Unable to change password file.\n" COL_RESET); pause_anykey(); return; }
    fprintf(fp, "%s\n", pwd1);
    fclose(fp);
    printf(COL_GREEN "Password changed successfully.\n" COL_RESET);
    pause_anykey();
}

int admin_login() {
    ensure_admin_file();
    char stored[200];
    FILE *fp = fopen(ADMIN_FILE, "r");
    if (!fp) return 0;
    if (!fgets(stored, sizeof(stored), fp)) { fclose(fp); return 0; }
    stored[strcspn(stored, "\n")] = '\0';
    fclose(fp);

    int attempts = 0;
    while (attempts < 3) {
        printf("Enter admin password: ");
        char input[200];
        int idx = 0, ch;
        while (1) {
            ch = getch_noecho();
            if (ch == '\n' || ch == '\r') { input[idx] = '\0'; printf("\n"); break; }
            if (ch == 127 || ch == 8) { if (idx>0) { idx--; printf("\b \b"); } continue; }
            if (idx < (int)sizeof(input)-1) { input[idx++] = ch; printf("*"); }
        }
        if (strcmp(input, stored) == 0) {
            printf(COL_GREEN "Login successful.\n" COL_RESET);
            return 1;
        } else {
            attempts++;
            printf(COL_RED "Incorrect password. Attempts left: %d\n" COL_RESET, 3 - attempts);
        }
    }
    printf(COL_RED "Too many failed attempts. Returning to menu.\n" COL_RESET);
    pause_anykey();
    return 0;
}

// -------- CORE: file operations, validation --------
int roll_exists(int roll) {
    FILE *fp = fopen(DATA_FILE, "rb");
    if (!fp) return 0;
    Student s;
    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        if (s.rollNo == roll) { fclose(fp); return 1; }
    }
    fclose(fp);
    return 0;
}

void recalc_student(Student *s) {
    s->total = 0.0f;
    for (int i = 0; i < SUBJECT_COUNT; ++i) s->total += s->marks[i];
    s->percentage = s->total / SUBJECT_COUNT;
    if (s->percentage >= 90.0f) s->grade = 'A';
    else if (s->percentage >= 75.0f) s->grade = 'B';
    else if (s->percentage >= 60.0f) s->grade = 'C';
    else if (s->percentage >= 40.0f) s->grade = 'D';
    else s->grade = 'F';
}

// -------- ADD STUDENT --------
void addStudent_feature() {
    Student s;
    clear_screen();
    printf(COL_CYAN "----- Add Student -----\n" COL_RESET);

    printf("Enter Roll Number: ");
    if (scanf("%d", &s.rollNo) != 1) {
        printf(COL_RED "Invalid input.\n" COL_RESET);
        while (getchar() != '\n');
        pause_anykey();
        return;
    }
    while (getchar() != '\n');

    if (roll_exists(s.rollNo)) {
        printf(COL_RED "Roll number already exists. Aborting.\n" COL_RESET);
        pause_anykey();
        return;
    }

    printf("Enter Full Name: ");
    safe_fgets(s.name, sizeof(s.name));
    if (strlen(s.name) == 0) strcpy(s.name, "Unnamed Student");
    to_titlecase(s.name);

    for (int i = 0; i < SUBJECT_COUNT; ++i) {
        printf("Enter marks for %s: ", SUBJECT_NAMES[i]);
        if (scanf("%f", &s.marks[i]) != 1) {
            printf(COL_RED "Invalid input.\n" COL_RESET);
            while (getchar() != '\n');
            pause_anykey();
            return;
        }
    }
    while (getchar() != '\n');

    recalc_student(&s);

    FILE *fp = fopen(DATA_FILE, "ab");
    if (!fp) { printf(COL_RED "Error opening data file.\n" COL_RESET); pause_anykey(); return; }
    fwrite(&s, sizeof(Student), 1, fp);
    fclose(fp);

    printf(COL_GREEN "Student added successfully.\n" COL_RESET);
    pause_anykey();
}

// -------- DISPLAY (sorting & pagination) --------
int compare_by_roll(const void *a, const void *b) {
    const Student *sa = a, *sb = b;
    return sa->rollNo - sb->rollNo;
}
int compare_by_name(const void *a, const void *b) {
    const Student *sa = a, *sb = b;
#ifdef _WIN32
    return _stricmp(sa->name, sb->name);
#else
    return strcasecmp(sa->name, sb->name);
#endif
}
int compare_by_percentage_desc(const void *a, const void *b) {
    const Student *sa = a, *sb = b;
    if (sb->percentage > sa->percentage) return 1;
    if (sb->percentage < sa->percentage) return -1;
    return 0;
}

Student *load_all_students(int *count) {
    *count = 0;
    FILE *fp = fopen(DATA_FILE, "rb");
    if (!fp) return NULL;
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    rewind(fp);
    int total = (int)(sz / sizeof(Student));
    if (total <= 0) { fclose(fp); return NULL; }
    Student *arr = malloc(sizeof(Student) * total);
    if (!arr) { fclose(fp); return NULL; }
    int idx = 0;
    while (fread(&arr[idx], sizeof(Student), 1, fp) == 1) idx++;
    fclose(fp);
    *count = idx;
    return arr;
}

void print_student_row(const Student *s) {
    printf("%-8d | %-25s |", s->rollNo, s->name);
    for (int i = 0; i < SUBJECT_COUNT; ++i) {
        printf(" %6.2f |", s->marks[i]);
    }
    printf(" %7.2f | %6.2f |   %c\n", s->total, s->percentage, s->grade);
}

void display_table_header() {
    printf(COL_YELLOW "Roll     | Name                      |" COL_RESET);
    for (int i = 0; i < SUBJECT_COUNT; ++i) {
        printf(" %-6s |", SUBJECT_NAMES[i]);
    }
    printf("  Total  |   Perc | Grade\n");
    printf("--------------------------------------------------------------------------------\n");
}

void paginate_and_display(Student *arr, int count) {
    if (!arr || count == 0) {
        printf(COL_RED "No records to display.\n" COL_RESET);
        pause_anykey();
        return;
    }
    int pages = (count + RECORDS_PER_PAGE - 1) / RECORDS_PER_PAGE;
    int current = 0;
    while (1) {
        clear_screen();
        printf(COL_CYAN "----- All Student Records (Page %d of %d) -----\n" COL_RESET, current+1, pages);
        display_table_header();
        int start = current * RECORDS_PER_PAGE;
        int end = start + RECORDS_PER_PAGE;
        if (end > count) end = count;
        for (int i = start; i < end; ++i) print_student_row(&arr[i]);
        printf("--------------------------------------------------------------------------------\n");
        printf("n: next page, p: prev page, q: quit display\n");
        int ch = getchar();
        if (ch == '\n') ch = getchar(); // handle previous newline
        if (ch == 'n' || ch == 'N') {
            if (current + 1 < pages) current++;
        } else if (ch == 'p' || ch == 'P') {
            if (current > 0) current--;
        } else {
            break;
        }
    }
    while (getchar() != '\n'); // clear trailing
}

void displayAll_feature() {
    int count = 0;
    Student *arr = load_all_students(&count);
    if (!arr) { printf(COL_RED "No records found.\n" COL_RESET); pause_anykey(); return; }

    printf("Sort by: 1) Roll 2) Name 3) Percentage(desc) 4) No sort\nEnter choice: ");
    int choice;
    if (scanf("%d", &choice) != 1) { choice = 4; }
    while (getchar() != '\n');

    if (choice == 1) qsort(arr, count, sizeof(Student), compare_by_roll);
    else if (choice == 2) qsort(arr, count, sizeof(Student), compare_by_name);
    else if (choice == 3) qsort(arr, count, sizeof(Student), compare_by_percentage_desc);

    paginate_and_display(arr, count);
    free(arr);
}

// -------- SEARCH (by roll, name, grade) --------
void search_feature() {
    printf("Search by: 1) Roll\n  2) Name\n  3) Grade\nEnter choice: ");
    int c;
    if (scanf("%d", &c) != 1) { while (getchar()!='\n'); printf("Invalid.\n"); pause_anykey(); return; }
    while (getchar() != '\n');

    FILE *fp = fopen(DATA_FILE, "rb");
    if (!fp) { printf(COL_RED "No records found.\n" COL_RESET); pause_anykey(); return; }

    Student s;
    int found = 0;
    if (c == 1) {
        printf("Enter roll to search: ");
        int r;
        if (scanf("%d", &r) != 1) { printf("Invalid.\n"); while (getchar()!='\n'); fclose(fp); pause_anykey(); return; }
        while (fread(&s, sizeof(Student), 1, fp) == 1) {
            if (s.rollNo == r) {
                printf(COL_GREEN "Student found:\n" COL_RESET);
                print_student_row(&s);
                found = 1; break;
            }
        }
    } else if (c == 2) {
        printf("Enter name or substring (case-insensitive): ");
        char q[200];
        safe_fgets(q, sizeof(q));
        for (int i = 0; q[i]; ++i) q[i] = tolower((unsigned char)q[i]);
        while (fread(&s, sizeof(Student), 1, fp) == 1) {
            char nm[MAX_NAME_LEN];
            strncpy(nm, s.name, sizeof(nm));
            for (int i = 0; nm[i]; ++i) nm[i] = tolower((unsigned char)nm[i]);
            if (strstr(nm, q)) {
                if (!found) printf(COL_GREEN "Matching students:\n" COL_RESET);
                print_student_row(&s);
                found = 1;
            }
        }
    } else if (c == 3) {
        printf("Enter grade (A/B/C/D/F): ");
        char g = getchar();
        while (getchar() != '\n');
        if (g >= 'a' && g <= 'z') g = toupper(g);
        while (fread(&s, sizeof(Student), 1, fp) == 1) {
            if (s.grade == g) {
                if (!found) printf(COL_GREEN "Matching students:\n" COL_RESET);
                print_student_row(&s);
                found = 1;
            }
        }
    } else {
        printf("Invalid choice.\n");
    }

    if (!found) printf(COL_RED "No matching records found.\n" COL_RESET);
    fclose(fp);
    pause_anykey();
}

// -------- UPDATE --------
void update_feature() {
    printf("Enter roll number to update: ");
    int r;
    if (scanf("%d", &r) != 1) { printf("Invalid input.\n"); while (getchar()!='\n'); pause_anykey(); return; }
    while (getchar() != '\n');

    FILE *fp = fopen(DATA_FILE, "rb");
    if (!fp) { printf(COL_RED "No records found.\n" COL_RESET); pause_anykey(); return; }
    FILE *temp = fopen("temp.dat", "wb");
    if (!temp) { fclose(fp); printf(COL_RED "Error creating temp file.\n" COL_RESET); pause_anykey(); return; }

    Student s;
    int found = 0;
    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        if (s.rollNo == r) {
            found = 1;
            printf("Current name: %s\n", s.name);
            printf("Enter new name (leave blank to keep): ");
            char newname[MAX_NAME_LEN];
            safe_fgets(newname, sizeof(newname));
            if (strlen(newname) > 0) { strncpy(s.name, newname, sizeof(s.name)); to_titlecase(s.name); }
            for (int i = 0; i < SUBJECT_COUNT; ++i) {
                printf("Current marks for %s: %.2f\n", SUBJECT_NAMES[i], s.marks[i]);
                printf("Enter new marks (or -1 to keep): ");
                float m;
                if (scanf("%f", &m) != 1) { while (getchar()!='\n'); printf("Invalid input, keeping old.\n"); continue; }
                if (m >= 0.0f) s.marks[i] = m;
            }
            while (getchar() != '\n');
            recalc_student(&s);
            printf(COL_GREEN "Record updated.\n" COL_RESET);
        }
        fwrite(&s, sizeof(Student), 1, temp);
    }
    fclose(fp);
    fclose(temp);
    remove(DATA_FILE);
    rename("temp.dat", DATA_FILE);
    if (!found) printf(COL_RED "Roll number not found.\n" COL_RESET);
    pause_anykey();
}

// -------- DELETE --------
void delete_feature() {
    printf("Enter roll number to delete: ");
    int r;
    if (scanf("%d", &r) != 1) { printf("Invalid input.\n"); while (getchar()!='\n'); pause_anykey(); return; }
    while (getchar() != '\n');

    FILE *fp = fopen(DATA_FILE, "rb");
    if (!fp) { printf(COL_RED "No records found.\n" COL_RESET); pause_anykey(); return; }
    FILE *temp = fopen("temp.dat", "wb");
    if (!temp) { fclose(fp); printf(COL_RED "Error creating temp file.\n" COL_RESET); pause_anykey(); return; }

    Student s;
    int found = 0;
    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        if (s.rollNo == r) {
            found = 1;
            printf(COL_GREEN "Record deleted for roll %d\n" COL_RESET, r);
        } else {
            fwrite(&s, sizeof(Student), 1, temp);
        }
    }
    fclose(fp);
    fclose(temp);
    remove(DATA_FILE);
    rename("temp.dat", DATA_FILE);
    if (!found) printf(COL_RED "Roll number not found.\n" COL_RESET);
    pause_anykey();
}

// -------- BACKUP & RESTORE --------
void backup_data() {
    FILE *src = fopen(DATA_FILE, "rb");
    if (!src) { printf(COL_RED "No data to backup.\n" COL_RESET); pause_anykey(); return; }
    FILE *dst = fopen(BACKUP_FILE, "wb");
    if (!dst) { fclose(src); printf(COL_RED "Cannot create backup file.\n" COL_RESET); pause_anykey(); return; }
    char buf[4096];
    size_t r;
    while ((r = fread(buf, 1, sizeof(buf), src)) > 0) fwrite(buf, 1, r, dst);
    fclose(src); fclose(dst);
    printf(COL_GREEN "Backup saved to %s\n" COL_RESET, BACKUP_FILE);
    pause_anykey();
}

void restore_data() {
    FILE *src = fopen(BACKUP_FILE, "rb");
    if (!src) { printf(COL_RED "Backup not found.\n" COL_RESET); pause_anykey(); return; }
    FILE *dst = fopen(DATA_FILE, "wb");
    if (!dst) { fclose(src); printf(COL_RED "Cannot restore (permission?).\n" COL_RESET); pause_anykey(); return; }
    char buf[4096];
    size_t r;
    while ((r = fread(buf, 1, sizeof(buf), src)) > 0) fwrite(buf, 1, r, dst);
    fclose(src); fclose(dst);
    printf(COL_GREEN "Data restored from backup.\n" COL_RESET);
    pause_anykey();
}

// -------- REPORT CARD GENERATION --------
void generate_report(int roll) {
    FILE *fp = fopen(DATA_FILE, "rb");
    if (!fp) { printf(COL_RED "No data file.\n" COL_RESET); pause_anykey(); return; }
    Student s;
    int found = 0;
    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        if (s.rollNo == roll) { found = 1; break; }
    }
    fclose(fp);
    if (!found) { printf(COL_RED "Student not found.\n" COL_RESET); pause_anykey(); return; }
    ensure_reports_dir();
    char fname[256];
    snprintf(fname, sizeof(fname), "%s/report_roll_%d.txt", REPORTS_DIR, roll);
    FILE *rp = fopen(fname, "w");
    if (!rp) { printf(COL_RED "Cannot create report file.\n" COL_RESET); pause_anykey(); return; }
    fprintf(rp, "----- Report Card -----\n");
    fprintf(rp, "Roll Number: %d\n", s.rollNo);
    fprintf(rp, "Name: %s\n", s.name);
    for (int i = 0; i < SUBJECT_COUNT; ++i) {
        fprintf(rp, "%-12s : %.2f\n", SUBJECT_NAMES[i], s.marks[i]);
    }
    fprintf(rp, "Total       : %.2f\n", s.total);
    fprintf(rp, "Percentage  : %.2f\n", s.percentage);
    fprintf(rp, "Grade       : %c\n", s.grade);
    fprintf(rp, "Generated on: %s", ctime(&(time_t){time(NULL)}));
    fclose(rp);
    printf(COL_GREEN "Report generated: %s\n" COL_RESET, fname);
    pause_anykey();
}

void generate_report_for_student_feature() {
    printf("Enter roll number to generate report: ");
    int r;
    if (scanf("%d", &r) != 1) { printf("Invalid input.\n"); while (getchar()!='\n'); pause_anykey(); return; }
    while (getchar() != '\n');
    generate_report(r);
}

// -------- STATISTICS & ANALYTICS --------
void analytics_feature() {
    int count = 0;
    Student *arr = load_all_students(&count);
    if (!arr) { printf(COL_RED "No records found.\n" COL_RESET); pause_anykey(); return; }
    clear_screen();
    printf(COL_CYAN "----- Analytics & Statistics -----\n" COL_RESET);
    float class_total = 0.0f;
    float subj_totals[MAX_SUBJECTS] = {0};
    float subj_max[MAX_SUBJECTS]; for (int i=0;i<MAX_SUBJECTS;++i) subj_max[i] = -1.0f;
    int subj_topper_idx[MAX_SUBJECTS]; for (int i=0;i<MAX_SUBJECTS;++i) subj_topper_idx[i] = -1;
    float highest = -1.0f; int highest_idx = -1;
    float lowest = 1e9; int lowest_idx = -1;
    int grade_counts[5] = {0}; // A,B,C,D,F

    for (int i = 0; i < count; ++i) {
        class_total += arr[i].percentage;
        if (arr[i].percentage > highest) { highest = arr[i].percentage; highest_idx = i; }
        if (arr[i].percentage < lowest) { lowest = arr[i].percentage; lowest_idx = i; }
        for (int j = 0; j < SUBJECT_COUNT; ++j) {
            subj_totals[j] += arr[i].marks[j];
            if (arr[i].marks[j] > subj_max[j]) { subj_max[j] = arr[i].marks[j]; subj_topper_idx[j] = i; }
        }
        if (arr[i].grade == 'A') grade_counts[0]++;
        else if (arr[i].grade == 'B') grade_counts[1]++;
        else if (arr[i].grade == 'C') grade_counts[2]++;
        else if (arr[i].grade == 'D') grade_counts[3]++;
        else grade_counts[4]++;
    }

    printf("Class size: %d\n", count);
    printf("Class average percentage: %.2f\n", class_total / count);
    if (highest_idx >= 0) printf("Topper (overall): %s (Roll %d) - %.2f%%\n", arr[highest_idx].name, arr[highest_idx].rollNo, arr[highest_idx].percentage);
    if (lowest_idx >= 0) printf("Lowest (overall): %s (Roll %d) - %.2f%%\n", arr[lowest_idx].name, arr[lowest_idx].rollNo, arr[lowest_idx].percentage);

    printf("\nSubject-wise toppers:\n");
    for (int j = 0; j < SUBJECT_COUNT; ++j) {
        if (subj_topper_idx[j] >= 0)
            printf(" %s : %s (Roll %d) - %.2f\n", SUBJECT_NAMES[j], arr[subj_topper_idx[j]].name, arr[subj_topper_idx[j]].rollNo, subj_max[j]);
    }

    printf("\nGrade distribution:\n");
    printf(" A: %d\n B: %d\n C: %d\n D: %d\n F: %d\n", grade_counts[0], grade_counts[1], grade_counts[2], grade_counts[3], grade_counts[4]);

    free(arr);
    pause_anykey();
}

// -------- TOPPER & RANKING --------
void show_topper_and_ranking() {
    int count = 0;
    Student *arr = load_all_students(&count);
    if (!arr) { printf(COL_RED "No records found.\n" COL_RESET); pause_anykey(); return; }
    qsort(arr, count, sizeof(Student), compare_by_percentage_desc);
    clear_screen();
    printf(COL_CYAN "----- Class Ranking -----\n" COL_RESET);
    display_table_header();
    for (int i = 0; i < count; ++i) {
        printf("%2d) ", i+1);
        print_student_row(&arr[i]);
    }
    if (count > 0) printf(COL_GREEN "\nTopper: %s (Roll %d) - %.2f%%\n" COL_RESET, arr[0].name, arr[0].rollNo, arr[0].percentage);
    free(arr);
    pause_anykey();
}

// -------- MENU & MAIN LOOP --------
void show_main_menu() {
    printf(COL_BLUE "===== Student Result Management System - Full Version =====\n" COL_RESET);
    printf("1. Add Student Record\n");
    printf("2. Display All Records\n");
    printf("3. Search Students\n");
    printf("4. Update Student\n");
    printf("5. Delete Student\n");
    printf("6. Backup Data\n");
    printf("7. Restore Data\n");
    printf("8. Generate Report Card (single)\n");
    printf("9. Analytics & Statistics\n");
    printf("10. Show Topper & Ranking\n");
    printf("11. Configure Subjects\n");
    printf("12. Admin Menu (change password)\n");
    printf("0. Exit\n");
    printf(COL_YELLOW "Enter your choice: " COL_RESET);
}

void admin_submenu() {
    if (!admin_login()) return;
    while (1) {
        clear_screen();
        printf(COL_CYAN "----- Admin Menu -----\n" COL_RESET);
        printf("1. Change Admin Password\n");
        printf("2. Configure Subjects\n");
        printf("9. Back\n");
        printf("Enter choice: ");
        int ch;
        if (scanf("%d", &ch) != 1) { while (getchar()!='\n'); continue; }
        while (getchar() != '\n');
        if (ch == 1) change_admin_password();
        else if (ch == 2) configure_subjects();
        else if (ch == 9) break;
        else printf("Invalid choice.\n");
        pause_anykey();
    }
}

int main() {
    show_welcome_screen();
    load_subjects();
    ensure_admin_file();
    ensure_reports_dir();

    while (1) {
        clear_screen();
        show_main_menu();
        int choice;
        if (scanf("%d", &choice) != 1) { printf("Invalid input.\n"); while (getchar()!='\n'); pause_anykey(); continue; }
        while (getchar() != '\n');

        switch (choice) {
            case 1: addStudent_feature(); break;
            case 2: displayAll_feature(); break;
            case 3: search_feature(); break;
            case 4: update_feature(); break;
            case 5: delete_feature(); break;
            case 6: backup_data(); break;
            case 7: restore_data(); break;
            case 8: generate_report_for_student_feature(); break;
            case 9: analytics_feature(); break;
            case 10: show_topper_and_ranking(); break;
            case 11: configure_subjects(); break;
            case 12: admin_submenu(); break;
            case 0: printf(COL_GREEN "Exiting. Goodbye!\n" COL_RESET); exit(0);
            default: printf(COL_RED "Invalid choice. Try again.\n" COL_RESET); pause_anykey(); break;
        }
    }

    return 0;
}
