
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define SIZE 10000

typedef  struct item {
    int busy;
    int key;
    int par;
    char *info;
} item;

typedef  struct table {
    int n;
    struct item first[SIZE];
} table;

char *getstring() {
    char *ptr = (char *) malloc(1);
    char buf[81];
    unsigned int n, len = 0;
    *ptr = '\0';
    fflush(stdin);
    do {
        n = scanf("%5[^\n]", buf);
        if (n < 0) {
            free(ptr);
            ptr = NULL;
            continue;
        }
        if (n == 0) scanf("%*c");
        else {
            len += strlen(buf);
            ptr = (char *) realloc(ptr, len + 1);
            strncat(ptr, buf, len);
        }
    } while (n > 0);
    return ptr;
}

int getIntSmall(int *a,int e) {
    int n;

    if (e == 1) {
        srand(rand()*time(0));
        printf("%s\n", "random");
        *a = rand() % 10000 + 1;
        return 1;
    }

    if (e == 2)
    { do {
            fflush(stdin);
            n = scanf("%i", a);
            printf("%s\n", "manual");
            if (n < 0)
                return 0;
            if (n == 0) {
                printf("%s\n", "Error! Repeat input");
                scanf("%*[^\n]", 0);
            }
        } while (n == 0);
        scanf("%*c");
        return 1;
    }
}

const char *msgs[] = {"0. Quit","1. Add","2. Find family","3. Delete family","4. Show"};

void delTable(table *pTable);

int insert(table *pTable, int k, int par, char *info);

int search(table table, int k);

int trashCollect(table *pTable);

int erase(table *pTable,int k);

int show (table *pTable);

int getIntSmallPar(int q, int n);

int eraseFamily(table* pTable,int k);

int searchChildren(table* pTable, int par, int ** children);

const int  N = sizeof(msgs) / sizeof(msgs[0]);

int dialog()
{
    char *errmsg = "";
    int rc;
    int i,n;

    do{
        puts(errmsg);
        errmsg = "You are wrong, repeat please";
        fflush(stdin);
        for(i = 0;i < N;++i)
            puts(msgs[i]);
        puts("Make your choise: -->");

        n = getIntSmall(&rc,2);
        if(n == 0)
            rc = 0;
    }while(rc < 0 || rc >= N);
    return rc;

}

const char *errorMessages[]={"OK","Duplicate key", "Table overflow","Parent not found"};

int add(table *pTab)
{
    int k, rc, n;
    char *info = NULL;
    int par;
    printf("Enter key: -->");
    n = getIntSmall(&k,1);
    if(n == 0)
        return 0; // обнаружен конец файла
    printf("Enter parent element: -->");
    par = pTab->first[getIntSmallPar(n,pTab->n)].key;
    if(pTab->n > 0)
        while (search(*pTab,par) == -1)
            par = pTab->first[getIntSmallPar(n,pTab->n)].key;
    printf("Enter info:\n");
    info = getstring();
    if(info == NULL)
        return 0;
    rc = insert(pTab, k, par, info);
    free(info);

    printf("%s: %d\n", errorMessages[rc], k);
    return 1;
}
int getIntSmallPar(int q, int n) {
    srand(rand()*time(0));
    printf("%s\n", "random");
    if(n != 0)
        q = rand() % n;
    else
        q = 0;
    return q;
}

int insert(table *pTable, int k, int par, char *info) {
    int rc;
    int i= search(*pTable, k);
    int j = search(*pTable, par);
    if (i>=0)
        return 1;
    if (j == -1 && par != 0)
        return 3;
    if(pTable->n==SIZE) {
        rc = trashCollect(pTable);
        if (rc == SIZE)
            return 2;
        pTable->n=rc;
    }

    pTable->first[pTable->n].key = k;
    pTable->first[pTable->n].par = par;
    if(pTable->first[pTable->n].info == NULL)
        pTable->first[pTable->n].info = (char *) calloc(strlen(info) + 1,1);
    pTable->first[pTable->n].busy = 1;
    *pTable->first[pTable->n].info='\0';
    strcat(pTable->first[pTable->n].info,info);
    pTable->n++;
    return 0;
}

int trashCollect(table *pTable){
    int i=0,j=0;
    item  p;
    while (1){
        if(i<pTable->n){
            if(pTable->first[i].busy == 1) {
                p = pTable->first[j];
                pTable->first[j] = pTable->first[i];
                pTable->first[i] = p;
                if(i!=j)
                    pTable->first[i].busy=0;
                ++j;
            }
            ++i;
        } else return j;
    }
}

int search(table table, int k) {
    int rc, i;

    for (i = 0; i < table.n; i++) {
        if(table.first[i].busy == 1 && table.first[i].key==k)
            return i;
    }
    return -1;
}

int findFamily (table* pTable){
    int k, rc, *children [SIZE];
    int buf;
    printf("Enter key: -->");
    table family = {0};
    getIntSmall(&k,2);
    rc = searchChildren(pTable, k, children);
    if (rc == -1)
        printf("Not found");
    else {
        buf = search(*pTable, k);
        insert(&family, pTable->first[buf].key, pTable->first[buf].par, pTable->first[buf].info);
        for (int i = 0; i < rc; i++) {
            buf = search(*pTable, *children[i]);
            insert(&family, *children[i], pTable->first[buf].par, pTable->first[buf].info);
        }
        show(&family);
        delTable(&family);
        return 1;
    }
    
}

//int delete(table * pTable){
//    int k, rc;
//    printf("Enter key: -->");
//    getIntSmall(&k,2);
//    rc = erase(pTable, k);
//    if (rc == -1)
//        printf("Not found");
//    else
//        printf("Deleted");
//    return 1;
//}

int erase(table *pTable,int k){
    int rc;
    rc = search(*pTable, k);
    if (rc == -1)
        return -1;
    else {
        pTable->first[rc].busy = 0;
        return rc;
    }

}

int show (table *pTable){
    printf("busy parent      Key          info\n");
    for (int i = 0; i < pTable->n; i++) {
        printf("   %d| %10d| %10d| %10s\n",pTable->first[i].busy,pTable->first[i].par,pTable->first[i].key,pTable->first[i].info);
    }
    return 1;
}

int deleteFamily(table * pTable)
{
    int k, rc;
    printf("Enter key: -->");
    getIntSmall(&k,2);
    rc = eraseFamily(pTable, k);
    if (rc == -1)
        printf("Not found");
    else
        printf("Deleted");
    return 1;
}

int eraseFamily(table* pTable,int k) //рекурсивно вырезает семью
{  int *children[SIZE];
    int n;
    if (search(*pTable,k) == -1)
        return -1;
    int par = k;
    n = searchChildren(pTable,par,children);
    erase(pTable,k);
    for(int i=0; i < n; i++)
    {
        eraseFamily(pTable, *children[i]);
    }
    return 0;
}

int searchChildren(table* pTable, int par , int ** children){        //выдает массив детей
    int n = 0;
    int j = 0;
    for (int i = 0; i < pTable->n;i++)
        if(pTable->first[i].par == par) {
            children[j] = &pTable->first[i].key;
            n++;
            j++;
        }
    return n;
}

int main() {
    int (*fptr[])(table *) = {NULL,add,findFamily, deleteFamily,show}; //,find,delete,show
    table table = {0};
    int rc;
    while(rc = dialog())
        if(!fptr[rc](&table))

            break;
    printf("Thanks");
    delTable(&table);
    return 0;
}

void delTable(table *pTable) {
}
