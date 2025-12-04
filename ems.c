#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* STRUCTURES */

typedef struct
{
    int id;
    char name[30];
    char addr[30];
    char gender[10];
    char desgn[20];
    char department[50];
    float salary;
    int isActive;
} EMP;

typedef struct
{
    char username[20];
    char password[20];
    int role; // 1 = Admin, 0 = Viewer
} USER;

/* UTILITIES */

void clearBuffer()
{
    while (getchar() != '\n')
        ;
}

void safeInput(char *buf, int size)
{
    fgets(buf, size, stdin);
    buf[strcspn(buf, "\n")] = 0;
}

void cleanString(char *s, int size)
{
    s[size - 1] = '\0';
    for (int i = 0; i < size; i++)
    {
        if (s[i] == '\n' || s[i] == '\r')
        {
            s[i] = '\0';
            break;
        }
    }
}

void toLowerStr(char *s)
{
    for (int i = 0; s[i]; i++)
        s[i] = tolower(s[i]);
}

/* USER SYSTEM */

int accountExists()
{
    FILE *fp = fopen("users.dat", "rb");
    if (!fp)
        return 0;
    fclose(fp);
    return 1;
}

void registerUser()
{
    USER u;
    FILE *fp = fopen("users.dat", "ab");
    if (!fp)
    {
        printf("Error creating user file.\n");
        return;
    }

    printf("\n--- REGISTER NEW USER ---\n");
    printf("Create Username: ");
    safeInput(u.username, sizeof(u.username));
    cleanString(u.username, sizeof(u.username));

    printf("Create Password: ");
    safeInput(u.password, sizeof(u.password));
    cleanString(u.password, sizeof(u.password));

    do
    {
        printf("Role (1 = Admin, 0 = Viewer): ");
        scanf("%d", &u.role);
        clearBuffer();
        if (u.role != 0 && u.role != 1)
            printf("Invalid input. Enter 1 for Admin or 0 for Viewer.\n");
    } while (u.role != 0 && u.role != 1);

    fwrite(&u, sizeof(u), 1, fp);
    fclose(fp);

    printf("Account created successfully.\n");
}

int login(USER *current)
{
    USER temp;
    FILE *fp = fopen("users.dat", "rb");
    if (!fp)
        return 0;

    char u[20], p[20];
    printf("Username: ");
    safeInput(u, sizeof(u));
    printf("Password: ");
    safeInput(p, sizeof(p));

    while (fread(&temp, sizeof(temp), 1, fp))
    {
        if (strcmp(u, temp.username) == 0 && strcmp(p, temp.password) == 0)
        {
            *current = temp;
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

void changePassword(USER current)
{
    FILE *fp = fopen("users.dat", "rb+");
    if (!fp)
    {
        printf("Error opening user file.\n");
        return;
    }

    USER temp;
    char newpass[20];

    printf("Enter New Password: ");
    safeInput(newpass, sizeof(newpass));
    cleanString(newpass, sizeof(newpass));

    while (fread(&temp, sizeof(temp), 1, fp))
    {
        if (strcmp(temp.username, current.username) == 0)
        {
            strcpy(temp.password, newpass);
            fseek(fp, -sizeof(temp), SEEK_CUR);
            fwrite(&temp, sizeof(temp), 1, fp);
            printf("Password Updated Successfully.\n");
            break;
        }
    }
    fclose(fp);
}

/* EMPLOYEE SYSTEM */

int idExists(int id)
{
    FILE *fp = fopen("emp.dat", "rb");
    if (!fp)
        return 0;

    EMP e;
    while (fread(&e, sizeof(e), 1, fp))
    {
        if (e.id == id && e.isActive)
        {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

void add_rec()
{
    FILE *fp = fopen("emp.dat", "ab");
    if (!fp)
    {
        printf("Error opening employee file.\n");
        return;
    }

    EMP e;
    printf("Enter ID: ");
    scanf("%d", &e.id);
    clearBuffer();

    if (idExists(e.id))
    {
        printf("Duplicate ID not allowed.\n");
        fclose(fp);
        return;
    }

    printf("Enter Name: ");
    safeInput(e.name, sizeof(e.name));
    cleanString(e.name, sizeof(e.name));

    printf("Enter Address: ");
    safeInput(e.addr, sizeof(e.addr));
    cleanString(e.addr, sizeof(e.addr));

    printf("Enter Gender: ");
    safeInput(e.gender, sizeof(e.gender));
    cleanString(e.gender, sizeof(e.gender));

    printf("Enter Designation: ");
    safeInput(e.desgn, sizeof(e.desgn));
    cleanString(e.desgn, sizeof(e.desgn));

    printf("Enter Department: ");
    safeInput(e.department, sizeof(e.department));
    cleanString(e.department, sizeof(e.department));

    printf("Enter Salary: ");
    scanf("%f", &e.salary);
    clearBuffer();

    if (e.salary < 0)
    {
        printf("Invalid Salary.\n");
        fclose(fp);
        return;
    }

    e.isActive = 1;
    fwrite(&e, sizeof(e), 1, fp);
    fclose(fp);

    printf("Employee Added Successfully.\n");
}

void displaySorted()
{
    FILE *fp = fopen("emp.dat", "rb");
    if (!fp)
    {
        printf("Error opening employee file.\n");
        return;
    }

    EMP employees[1000];
    int count = 0;
    EMP e;

    while (fread(&e, sizeof(e), 1, fp))
    {
        if (e.isActive)
            employees[count++] = e;
    }
    fclose(fp);

    // Sort by ID
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = i + 1; j < count; j++)
        {
            if (employees[i].id > employees[j].id)
            {
                EMP temp = employees[i];
                employees[i] = employees[j];
                employees[j] = temp;
            }
        }
    }

    printf("\nID\tName\tAddress\tGender\tDesignation\tDepartment\tSalary\n");
    printf("--------------------------------------------------------------------------\n");
    for (int i = 0; i < count; i++)
    {
        printf("%d\t%s\t%s\t%s\t%s\t%s\t%.2f\n",
               employees[i].id, employees[i].name, employees[i].addr,
               employees[i].gender, employees[i].desgn,
               employees[i].department, employees[i].salary);
    }
}

void search()
{
    FILE *fp = fopen("emp.dat", "rb");
    if (!fp)
    {
        printf("Error opening employee file.\n");
        return;
    }

    EMP e;
    int id, found = 0;
    printf("Enter ID to search: ");
    scanf("%d", &id);
    clearBuffer();

    while (fread(&e, sizeof(e), 1, fp))
    {
        if (e.id == id && e.isActive)
        {
            printf("\nID: %d\nName: %s\nAddress: %s\nGender: %s\nDesignation: %s\nDepartment: %s\nSalary: %.2f\n",
                   e.id, e.name, e.addr, e.gender, e.desgn, e.department, e.salary);
            found = 1;
            break;
        }
    }

    if (!found)
        printf("Record Not Found.\n");

    fclose(fp);
}

void editEmployee()
{
    FILE *fp = fopen("emp.dat", "rb+");
    if (!fp)
    {
        printf("Error opening employee file.\n");
        return;
    }

    EMP e;
    int id, found = 0;
    printf("Enter Employee ID to edit: ");
    scanf("%d", &id);
    clearBuffer();

    while (fread(&e, sizeof(e), 1, fp))
    {
        if (e.id == id && e.isActive)
        {
            found = 1;
            printf("\nCurrent Details:\n");
            printf("ID: %d\nName: %s\nAddress: %s\nGender: %s\nDesignation: %s\nDepartment: %s\nSalary: %.2f\n",
                   e.id, e.name, e.addr, e.gender, e.desgn, e.department, e.salary);

            printf("\nEnter new Name: ");
            safeInput(e.name, sizeof(e.name));
            cleanString(e.name, sizeof(e.name));

            printf("Enter new Address: ");
            safeInput(e.addr, sizeof(e.addr));
            cleanString(e.addr, sizeof(e.addr));

            printf("Enter new Gender: ");
            safeInput(e.gender, sizeof(e.gender));
            cleanString(e.gender, sizeof(e.gender));

            printf("Enter new Designation: ");
            safeInput(e.desgn, sizeof(e.desgn));
            cleanString(e.desgn, sizeof(e.desgn));

            printf("Enter new Department: ");
            safeInput(e.department, sizeof(e.department));
            cleanString(e.department, sizeof(e.department));

            printf("Enter new Salary: ");
            scanf("%f", &e.salary);
            clearBuffer();

            fseek(fp, -sizeof(e), SEEK_CUR);
            fwrite(&e, sizeof(e), 1, fp);
            printf("Employee details updated successfully.\n");
            break;
        }
    }

    if (!found)
        printf("Employee ID %d not found.\n", id);

    fclose(fp);
}

void del()
{
    FILE *fp = fopen("emp.dat", "rb+");
    if (!fp)
    {
        printf("Error opening employee file.\n");
        return;
    }

    EMP e;
    int id, found = 0;
    printf("Enter ID to delete: ");
    scanf("%d", &id);
    clearBuffer();

    while (fread(&e, sizeof(e), 1, fp))
    {
        if (e.id == id && e.isActive)
        {
            e.isActive = 0;
            fseek(fp, -sizeof(e), SEEK_CUR);
            fwrite(&e, sizeof(e), 1, fp);
            found = 1;
            break;
        }
    }

    if (found)
        printf("Record Soft Deleted.\n");
    else
        printf("Record Not Found.\n");

    fclose(fp);
}

void salaryReport()
{
    FILE *fp = fopen("emp.dat", "rb");
    if (!fp)
    {
        printf("Error opening employee file.\n");
        return;
    }

    EMP e, top;
    float total = 0;
    int count = 0, first = 1;

    while (fread(&e, sizeof(e), 1, fp))
    {
        if (e.isActive)
        {
            total += e.salary;
            count++;
            if (first || e.salary > top.salary)
            {
                top = e;
                first = 0;
            }
        }
    }

    printf("\nEmployees: %d\n", count);
    printf("Total Salary: %.2f\n", total);

    if (count > 0)
    {
        printf("Average Salary: %.2f\n", total / count);
        printf("Top Earner: %s (%.2f)\n", top.name, top.salary);
    }

    fclose(fp);
}

void filterByDepartment()
{
    FILE *fp = fopen("emp.dat", "rb");
    if (!fp)
    {
        printf("Error opening employee file.\n");
        return;
    }

    EMP e;
    char dept[50], eDept[50];
    printf("Enter Department: ");
    safeInput(dept, sizeof(dept));
    cleanString(dept, sizeof(dept));
    toLowerStr(dept);

    printf("\nEmployees in %s:\n", dept);
    int found = 0;

    while (fread(&e, sizeof(e), 1, fp))
    {
        if (e.isActive)
        {
            strcpy(eDept, e.department);
            toLowerStr(eDept);
            if (strcmp(eDept, dept) == 0)
            {
                printf("%d - %s\n", e.id, e.name);
                found = 1;
            }
        }
    }

    if (!found)
        printf("No employees found in this department.\n");

    fclose(fp);
}

/* MAIN */

int main()
{
    USER current;

    if (!accountExists())
        registerUser();

    if (!login(&current))
    {
        printf("Invalid Login. Access Denied.\n");
        return 0;
    }

    int ch;
    while (1)
    {
        printf("\nEMPLOYEE MANAGEMENT SYSTEM\n");
        printf("1. Add Record (Admin Only)\n");
        printf("2. Search Record\n");
        printf("3. Display All (Sorted by ID)\n");
        printf("4. Edit Employee Details (Admin Only)\n");
        printf("5. Delete Record (Admin Only)\n");
        printf("6. Salary Analytics\n");
        printf("7. Filter by Department\n");
        printf("8. Change Password\n");
        printf("9. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &ch);
        clearBuffer();

        switch (ch)
        {
        case 1:
            if (current.role == 1)
                add_rec();
            else
                printf("Admin Only.\n");
            break;
        case 2:
            search();
            break;
        case 3:
            displaySorted();
            break;
        case 4:
            if (current.role == 1)
                editEmployee();
            else
                printf("Admin Only.\n");
            break;
        case 5:
            if (current.role == 1)
                del();
            else
                printf("Admin Only.\n");
            break;
        case 6:
            salaryReport();
            break;
        case 7:
            filterByDepartment();
            break;
        case 8:
            changePassword(current);
            break;
        case 9:
            exit(0);
        default:
            printf("Invalid Choice.\n");
        }
    }
}
