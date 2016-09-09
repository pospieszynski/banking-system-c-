#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define MAX_NAME 30
#define MAX_ADDRESS 40

typedef struct
    {
    char first_name[MAX_NAME];
    char last_name[MAX_NAME];
    char address[MAX_ADDRESS];
    long int pesel;
    float balance;  
} AccountData;

AccountData Account;

bool checkIfEnoughMoneyOnAccount(long int pesel, float amount);
bool checkIfAccountsExist(long int pesel1);
bool checkIfProperPesel(long int pesel);
bool checkIfProperName(char str[]);
bool confirm(void);
unsigned int numberOfAccounts(FILE* p);
int findByPesel(long int pesel);
void ifUnableToOpenaFile(FILE* p);
void addNewAccount(void);
void listAllAccounts(void);
void printAccount(void);
void deposite(void);
void withdrawal(void);
void transfer(void);
void replaceParticularStruct(int specific_struct);
void search(void);
void searchByFirstName(void);
void searchByLastName(void);
void searchByAddress(void);
void searchByPesel(void);
void searchByBalance(void);
void readParticularStruct(FILE* p, int i);
void loadPesel(void);
void loadFirstName(void);
void loadLastName(void);
void contiune(void);
int menu(void);

int main()
{
    int decision = 0;

    do {

        decision = menu();

        switch (decision) {
        case 1:
            addNewAccount();
            break;
        case 2:
            listAllAccounts();
            break;
        case 3:
            deposite();
            break;
        case 4:
            withdrawal();
            break;
        case 5:
            transfer();
            break;
        case 6:
            search();
            break;
        case 7:
            break;
        default:
            printf("\n Not valid operation\n");
            break;
        }

    } while (decision != 7);

    system("clear");
    return 0;
}

void addNewAccount(void)
{
    loadPesel();
    loadFirstName();
    loadLastName();

    getchar(); /*gets rid of some newline char in buffer problem */

    printf("\nAddress: ");
    fgets(Account.address, MAX_ADDRESS, stdin);

    strtok(Account.address, "\n");

    Account.balance = 0.0;

    FILE* p = fopen("data.bin", "ab");
    ifUnableToOpenaFile(p);

    fwrite(&Account, sizeof(AccountData), 1, p); /* writes the whole structure into the binary file */
    fflush(p);

    fclose(p);
}

void listAllAccounts(void)
{
    FILE* p = fopen("data.bin", "rb");
    ifUnableToOpenaFile(p);
    int i;

    for (i = 1; i <= numberOfAccounts(p); i++) /* reads structure one by one and dispalay it */
    {
        readParticularStruct(p, i);
        printAccount();
    }
    fclose(p);
    getchar();
}

void deposite(void)
{
    long int entered_pesel;
    float money_to_deposite;
    bool exist = 0;
    int i;

    printf("\nEnter your account identificator(PESEL): ");
    scanf("%ld", &entered_pesel);
    printf("Deposite: ");
    scanf("%f", &money_to_deposite);

    FILE* p = fopen("data.bin", "rb");

    ifUnableToOpenaFile(p);

    for (i = 1; i <= numberOfAccounts(p); i++) /* reads structure one by one */
    {
        readParticularStruct(p, i);

        if (Account.pesel == entered_pesel) {
            exist = 1;
            Account.balance += money_to_deposite;
            replaceParticularStruct(i);
            contiune();
            break;
        }
    }
    if (!exist) /* if account does not exist */
    {
        while (getchar() != '\n'); /* clears the buffor with \n */
        printf("\nThe account with entered PESEL does not exist.\n");
        printf("\n\nPlease press <ENTER> to continue.\n");
        getchar();
    }
    fclose(p);
}

void replaceParticularStruct(int specific_struct)
{
    FILE* p = fopen("data.bin", "r+b");

    ifUnableToOpenaFile(p);
    fseek(p, (specific_struct - 1) * sizeof(AccountData), SEEK_SET);
    fwrite(&Account, sizeof(AccountData), 1, p);
    fclose(p);
}

void withdrawal(void)
{
    long int entered_pesel;
    float money_to_withdraw;
    bool exist = 0;
    int i;

    printf("\nEnter your account identificator(PESEL): ");
    scanf("%ld", &entered_pesel);
    printf("Withdrawal: ");
    scanf("%f", &money_to_withdraw);

    FILE* p = fopen("data.bin", "rb");

    ifUnableToOpenaFile(p);

    for (i = 1; i <= numberOfAccounts(p); i++) {
        readParticularStruct(p, i);
        if (Account.pesel == entered_pesel) {
            exist = 1;
            if (checkIfEnoughMoneyOnAccount(entered_pesel, money_to_withdraw)) {
                Account.balance -= money_to_withdraw;
                replaceParticularStruct(i);
                contiune();
                break;
            }
            else {
                printf("\nThere is not enough money on your account.\n");
                contiune();
                break;
            }
        }
    }
    if (!exist) /* if account does not exist */
    {
        while (getchar() != '\n'); /* clears the buffor with \n */
        printf("\nThe account with entered PESEL does not exist.\n");
        printf("\n\nPlease press <ENTER> to continue.\n");
        getchar();
    }
}

bool checkIfAccountsExist(long int pesel1)
{
    int i;
    FILE* p = fopen("data.bin", "rb");
    ifUnableToOpenaFile(p);

    for (i = 1; i <= numberOfAccounts(p); i++) /* reads structure one by one */
    {
        readParticularStruct(p, i);

        if (Account.pesel == pesel1) {
            return true;
        }
    }
    return false;
}

void transfer(void)
{
    long int pesel1, pesel2;
    float amount;

    printf("\nFrom(PESEL): ");
    scanf("%ld", &pesel1);
    printf("\nTo(PESEL): ");
    scanf("%ld", &pesel2);

    if (checkIfAccountsExist(pesel1) && checkIfAccountsExist(pesel2)) {
        printf("\nAmount: ");
        scanf("%f", &amount);

        if (checkIfEnoughMoneyOnAccount(pesel1, amount)) {
            pesel1 = findByPesel(pesel1);
            Account.balance -= amount;
            replaceParticularStruct(pesel1);
            pesel2 = findByPesel(pesel2);
            Account.balance += amount;
            replaceParticularStruct(pesel2);
        }
        else {
            while (getchar() != '\n'); /* clears the buffor with \n */
            printf("\nThere is not enough money on your account.\n");
            printf("\n\nPlease press <ENTER> to continue.\n");
            getchar();
        }
    }
    else {
        while (getchar() != '\n'); /* clears the buffor with \n */
        printf("\nInvalid account ID(PESEL).\n");
        printf("\n\nPlease press <ENTER> to continue.\n");
        getchar();
    }
}

bool checkIfEnoughMoneyOnAccount(long int pesel, float amount)
{
    int i;
    FILE* p = fopen("data.bin", "rb");
    ifUnableToOpenaFile(p);

    for (i = 1; i <= numberOfAccounts(p); i++) /* reads structure one by one */
    {
        readParticularStruct(p, i);

        if (Account.pesel == pesel) {
            if (Account.balance - amount >= 0.0) {
                fclose(p);
                return true;
            }
        }
    }
    fclose(p);
    return false;
}

int findByPesel(long int pesel)
{
    int i;

    FILE* p = fopen("data.bin", "rb");
    ifUnableToOpenaFile(p);

    for (i = 1; i <= numberOfAccounts(p); i++) /* reads structure one by one */
    {
        readParticularStruct(p, i);

        if (Account.pesel == pesel) {
            fclose(p);
            return i;
        }
    }
    fclose(p);
    return -1; //pesel not found
}

void searchByFirstName()
{
    int i;
    char first_name[MAX_NAME];
    printf("\nSearch by last name: ");
    scanf("%s", first_name);

    FILE* p = fopen("data.bin", "rb");
    ifUnableToOpenaFile(p);

    for (i = 1; i <= numberOfAccounts(p); i++) /* reads structure one by one */
    {
        readParticularStruct(p, i);

        if (strcmp(first_name, Account.first_name) == 0) {
            printAccount();
        }
    }
    fclose(p);
    contiune();
}

void searchByLastName()
{
    int i;
    char last_name[MAX_NAME];
    printf("\nSearch by last name: ");
    scanf("%s", last_name);

    FILE* p = fopen("data.bin", "rb");

    for (i = 1; i <= numberOfAccounts(p); i++) /* reads structure one by one */
    {
        readParticularStruct(p, i);

        if (strcmp(last_name, Account.last_name) == 0) {
            printAccount();
        }
    }
    fclose(p);
    contiune();
}

void searchByAddress()
{
    int i;
    char address[MAX_ADDRESS];
    printf("Search by address: ");
    fgets(address, MAX_ADDRESS, stdin);
    strtok(address, "\n");
    FILE* p = fopen("data.bin", "rb");

    for (i = 1; i <= numberOfAccounts(p); i++) /* reads structure one by one */
    {
        readParticularStruct(p, i);
        if (strcmp(address, Account.address) == 0) {
            printAccount();
        }
    }
    fclose(p);
    contiune();
}

void searchByPesel()
{
    int i;
    long int pesel;
    printf("Search by PESEL: ");
    scanf("%ld", &pesel);

    FILE* p = fopen("data.bin", "rb");
    ifUnableToOpenaFile(p);

    for (i = 1; i <= numberOfAccounts(p); i++) /* reads structure one by one */
    {
        readParticularStruct(p, i);

        if (Account.pesel == pesel) {
            printAccount();
        }
    }
    fclose(p);
    contiune();
}

void searchByBalance()
{
    int i;
    float balance;
    printf("Search by balance: ");
    scanf("%f", &balance);

    FILE* p = fopen("data.bin", "rb");
    ifUnableToOpenaFile(p);

    for (i = 1; i <= numberOfAccounts(p); i++) /* reads structure one by one */
    {
        readParticularStruct(p, i);

        if (Account.balance == balance) {
            printAccount();
        }
    }
    fclose(p);
    contiune();
}

void search()
{
    int by = 0;
    int c;
    printf("\n\n1. First name\n2. Last name\n3. Address\n4. PESEL\n5. Balance\n ");
    do {
        fflush(stdin);
        printf("\nSearch by: ");
        fflush(stdin);
    } while (scanf("%d", &by) != 1);
    getchar();
    switch (by) {
    case 1:
        searchByFirstName();
        break;
    case 2:
        searchByLastName();
        break;
    case 3:
        searchByAddress();
        break;
    case 4:
        searchByPesel();
        break;
    case 5:
        searchByBalance();
        break;
    default:
        printf("\nNo such operation.");
        break;
    }
}

unsigned int numberOfAccounts(FILE* p)
{
    fseek(p, 0, SEEK_END);
    unsigned long len = (unsigned long)ftell(p); /* determines no of bytes in a file */
    unsigned long no_of_accounts = (unsigned long)(len / sizeof(AccountData)); /* divides it over the size of a struct to obtain no of structs */
    return no_of_accounts;
}

void ifUnableToOpenaFile(FILE* p)
{
    if (!p) {
        printf("\n Unable to open a file.\n");
        exit(-10);
    }
}

bool checkIfProperPesel(long int pesel)
{
    int count = 0;
    while (pesel != 0) {
        pesel /= 10;
        ++count;
    }
    if (count == 11) return true;
    else return false;
}

void loadPesel(void)
{
    int i = 0;
    long int pesel;
    do {
        if (i)
            printf("\nInvalid input, try again.");
        fflush(stdin);
        printf("\nPESEL: ");
        i++;
    } while ((scanf("%ld", &pesel) != 1) || (!checkIfProperPesel(pesel)) || (checkIfAccountsExist(pesel)));
    Account.pesel = pesel;
}

void contiune(void)
{
    while (getchar() != '\n'); /* clears the buffor from \n */
    printf("\n\nPlease press <ENTER> to continue.\n");
    getchar();
}

void readParticularStruct(FILE* p, int i)
{
    fseek(p, (i - 1) * sizeof(AccountData), SEEK_SET); /*offset on i th structure */
    fread(&Account, sizeof(AccountData), 1, p); /* reads ith struct */
}

void printAccount()
{
    printf("\nFirst Name: %s", Account.first_name);
    printf("\nLast Name: %s", Account.last_name);
    printf("\nAddress: %s", Account.address);
    printf("\nPESEL: %ld", Account.pesel);
    printf("\nBalance: %.2f", Account.balance);
    printf("\n******************************\n");
}

bool checkIfProperName(char str[])
{
    int i;
    for (i = 0; i < strlen(str); i++) {
        if (!isalpha(str[i])) return false;
    }
    return true;
}

void loadFirstName(void)
{
    int i = 0;
    do {
        if (i)
        printf("\nInvalid input, try again.");
        printf("\nFirst name: ");
        scanf("%s", Account.first_name);
        i = 1;
    } while (!checkIfProperName(Account.first_name));
}

void loadLastName(void)
{
    int i = 0;
    do {
        if (i)
        printf("\nInvalid input, try again.");
        printf("\nLast name: ");
        scanf("%s", Account.last_name);
        i = 1;
    } while (!checkIfProperName(Account.last_name));
}

bool confirm(void)
{
    getchar();
    int c = getchar();
    if (c == 13) return true;
    else return false;
}

int menu()
{
    int decision = 0, control = 0;
    do {
        system("clear");

        printf("\n.::BANKING SYSTEM::.\n\n");

        printf("1. CREATE A NEW ACCOUNT\n2. LIST ALL ACCOUNTS\n3. DEPOSITE\n4. WITHDRAWAL\n5. TRANSFER \n6. SEARCH\n7. EXIT");

        printf("\n\nOperation no: ");
        control = scanf("%d", &decision);
        getchar();
        fflush(stdin);
    } while (control != 1);

    printf("\n");

    return decision;
}
