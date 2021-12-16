#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "DateTime.h"
#include "Objects.h"
#include "Headers.h"
#include "Structs.h"
#define BUFLEN 80
#define TIMELEN 10

#pragma warning ( disable : 4996 )
int InsertNewObject(HeaderB** pStruct3, char* pNewID, int NewCode);
Object1* RemoveExistingObject(HeaderB** pStruct3, char* pExistingID);
bool CheckId(char* id);
HeaderB* LoadObjectsFromFile(const char* filename);
Object1* NewObject1(char* pNewID, int NewCode);
HeaderB* findOrAddHeaderB(HeaderB** pBeginPtr, char firstLetter);
HeaderA* findOrAddHeaderA(HeaderA** pBeginPtr, char secondLetter);
void PrintObjects(HeaderB* list);
void InsertNewObjectsFromConsole(HeaderB** list);
void AskToRemoveObjects(HeaderB** list);
Object1* RemoveObject(Object1* objectToRemove, Object1* list);
HeaderA* RemoveHeaderA(HeaderA* headerToRemove, HeaderA* list);
HeaderB* RemoveHeaderB(HeaderB* headerToRemove, HeaderB* list);
Node* CreateBinaryTree(HeaderB* headerB);
void TreeTraversal(Node* pTree);
Node* DeleteTreeNode(Node* pTree, unsigned long int code);
Node* AskToRemoveNodes(Node* pBeginPtr);

int main()
{
    srand(time(0));
    HeaderB* list = LoadObjectsFromFile("names.txt");
    PrintObjects(list);
    InsertNewObjectsFromConsole(&list);
    PrintObjects(list);
    AskToRemoveObjects(&list);
    PrintObjects(list);
    Node* tree = CreateBinaryTree(list);
    TreeTraversal(tree);
    tree = AskToRemoveNodes(tree);
    return 0;
}

void PrintObject(Object1* pObject) {
    printf("%s %lu %s\n", pObject->pID, pObject->Code, pObject->pTime);
}

void PrintObjects(HeaderB* headerB)
{
    int i = 1;

    printf("--------Printing linked list:----------\n");
    while (headerB != NULL) {
        HeaderA* headerA = headerB->pHeaderA;
        while (headerA != NULL) {
            Object1* pObjectTemp = (Object1*)headerA->pObject;
            while (pObjectTemp != NULL) {
                printf("%d) ", i); PrintObject(pObjectTemp);
                pObjectTemp = pObjectTemp->pNext;
                i++;
            }
            headerA = headerA->pNext;
        }
        headerB = headerB->pNext;
    }
}

Object1* NewObject1(char* pNewID, int NewCode) //uus objekt ja temale mälu küsimine
{
    Object1* newObject = (Object1*)malloc(sizeof(Object1));
    if (newObject == NULL) {
        return NULL;
    }
    newObject->pID = (char*)malloc((strlen(pNewID) + 1) * sizeof(char));
    if (newObject->pID == NULL) {
        return NULL;
    }
    strcpy(newObject->pID, pNewID);
    newObject->Code = NewCode;
    newObject->pNext = NULL;
    newObject->pTime = (char*)malloc(sizeof(char) * TIMELEN);
    GetTimeString(time(0), TIMELEN, newObject->pTime);
    
    return newObject;
}

HeaderB* LoadObjectsFromFile(const char* filename) {
    char line[BUFLEN];
    FILE* f = fopen(filename, "r");
    HeaderB* beginning = NULL;
    if (f == NULL) {
        printf("Ei saa avada andmete faili %s\n", filename);
        return NULL;
    }

    int i = 0;
    while (fgets(line, BUFLEN, f) != NULL) {
        if (CheckId(line)) {
            InsertNewObject(&beginning, line, rand());
        }
        else {
            printf("Error in line %s\n", line);
        }
    }
    fclose(f);
    return beginning;
}

void InsertNewObjectsFromConsole(HeaderB** pBeginPtr) {
    char line[BUFLEN];
    while (true) {
        printf("Enter name and surname or . to finish: ");
        gets_s(line, BUFLEN);
        if (line[0] == '.') {
            return;
        }
        if (!CheckId(line)) {
            printf("Error in line %s\n", line);
            continue;
        }
        InsertNewObject(pBeginPtr, line, rand());
    }
}

void AskToRemoveObjects(HeaderB** pBeginPtr) {
    char line[BUFLEN];
    while (true) {
        printf("Enter name and surname to remove or . to finish: ");
        gets_s(line, BUFLEN);
        if (line[0] == '.') {
            return;
        }
        if (!CheckId(line)) {
            printf("Error in line %s\n", line);
            continue;
        }
        Object1* removedObj = RemoveExistingObject(pBeginPtr, line);
        if (removedObj == NULL) {
            printf("Object not found\n");
        } else {
            printf("Removed object: %s %lu %s\n", removedObj->pID, removedObj->Code, removedObj->pTime);
        }
    }
}

bool CheckId(char* id) {
    bool new_word = true;
    int word_cnt = 0;
    bool small_letter = true;
    int len = strlen(id);
    if (id[len - 1] == '\n') {
        id[len - 1] = '\0';
    }

    for (int i = 0; id[i] != '\0'; i++) {
        if (new_word) {
            if (id[i] >= 'A' && id[i] <= 'Z') {
                new_word = false;
                word_cnt++;
                if (!small_letter) return false;
                small_letter = false;
                continue;
            }
            else return false;
        }
        else {
            if (id[i] >= 'a' && id[i] <= 'z') {
                small_letter = true;
                continue;
            }
            if (id[i] == ' ') {
                new_word = true;
                continue;
            }
            return false;
        }
    }
    if (new_word) return false;
    if (!small_letter) return false;
    if (word_cnt != 2) return false;
    return true;
}

int InsertNewObject(HeaderB** pBeginPtr, char* id, int code) { 
    
    char firstLetter = '\0', secondLetter = '\0';
    for (int i = 0; id[i] != '\0'; i++) {
        if (id[i] >= 'A' && id[i] <= 'Z') {
            if (firstLetter == '\0') firstLetter = id[i];
            else secondLetter = id[i];
        }
    }

    HeaderB* headB = findOrAddHeaderB(pBeginPtr, firstLetter);
    HeaderA* headA = findOrAddHeaderA(&(headB->pHeaderA), secondLetter);
    Object1* firstObject = (Object1*)headA->pObject;
    Object1* newObject = NewObject1(id, code);
    newObject->pNext = firstObject;
    headA->pObject = newObject;

    return 0;
}

HeaderB* findOrAddHeaderB(HeaderB** pBeginPtr, char letterToFind) {
    HeaderB* prevLetter = NULL, * currLetter = NULL, * nextLetter = NULL;
    for (HeaderB* ptr = *pBeginPtr; ptr != NULL; ptr = ptr->pNext) {
        if (ptr->cBegin == letterToFind) {
            currLetter = ptr;
            break;
        }
        if (ptr->cBegin < letterToFind) {
            prevLetter = ptr;
            continue;
        }
        if (ptr->cBegin > letterToFind) {
            nextLetter = ptr;
            break;
        }
    }

    if (!currLetter) {
        currLetter = (HeaderB*)malloc(sizeof(HeaderB));
        if (currLetter == NULL) {
            return NULL;
        }
        currLetter->cBegin = letterToFind;
        currLetter->pNext = nextLetter;
        currLetter->pHeaderA = NULL;
        if (prevLetter) {
            prevLetter->pNext = currLetter;
        }
        else {
            *pBeginPtr = currLetter;
        }
    }
    return currLetter;
}

HeaderA* findOrAddHeaderA(HeaderA** pBeginPtr, char letterToFind) {
    HeaderA* prevLetter = NULL, * currLetter = NULL, * nextLetter = NULL;
    for (HeaderA* ptr = *pBeginPtr; ptr != NULL; ptr = ptr->pNext) {
        if (ptr->cBegin == letterToFind) {
            currLetter = ptr;
            break;
        }
        if (ptr->cBegin < letterToFind) {
            prevLetter = ptr;
            continue;
        }
        if (ptr->cBegin > letterToFind) {
            nextLetter = ptr;
            break;
        }
    }

    if (!currLetter) {
        currLetter = (HeaderA*)malloc(sizeof(HeaderA));
        if (currLetter == NULL) {
            return NULL;
        }
        currLetter->cBegin = letterToFind;
        currLetter->pNext = nextLetter;
        currLetter->pObject = NULL;
        if (prevLetter) {
            prevLetter->pNext = currLetter;
        }
        else {
            *pBeginPtr = currLetter;
        }
    }
    return currLetter;
}

Object1* RemoveExistingObject(HeaderB** list, char* pExistingID) {
    HeaderB* headerB = *list;
    while (headerB != NULL) {
        HeaderA* headerA = headerB->pHeaderA;
        while (headerA != NULL) {
            Object1* pObjectTemp = (Object1*)headerA->pObject;
            while (pObjectTemp != NULL) {
                if (strcmp(pExistingID, pObjectTemp->pID) == 0) {
                    headerA->pObject = RemoveObject(pObjectTemp, (Object1*)headerA->pObject);
                    if (headerA->pObject == NULL) { // list of objects in HeaderA is empty now
                        headerB->pHeaderA = RemoveHeaderA(headerA, headerB->pHeaderA);
                        if (headerB->pHeaderA == NULL) { // list of HeaderA in HeaderB is empty now
                            *list = RemoveHeaderB(headerB, *list);
                        }
                    }
                    return pObjectTemp;
                }
                pObjectTemp = pObjectTemp->pNext;
            }
            headerA = headerA->pNext;
        }
        headerB = headerB->pNext;
    }
    return NULL;
}

Object1* RemoveObject(Object1* objectToRemove, Object1* list) {
    Object1* prev = NULL;
    for (Object1* obj = list; obj != NULL; obj = obj->pNext) {
        if (obj == objectToRemove) {
            if (prev != NULL) {
                prev->pNext = obj->pNext;
                return list;
            } else {
                return obj->pNext;
            }
        }
        prev = obj;
    }
    return list;
}

HeaderA* RemoveHeaderA(HeaderA* headerToRemove, HeaderA* list) {
    HeaderA* prev = NULL;
    for (HeaderA* obj = list; obj != NULL; obj = obj->pNext) {
        if (obj == headerToRemove) {
            if (prev != NULL) {
                prev->pNext = obj->pNext;
                return list;
            }
            else {
                return obj->pNext;
            }
        }
        prev = obj;
    }
    return list;
}

HeaderB* RemoveHeaderB(HeaderB* headerToRemove, HeaderB* list) {
    HeaderB* prev = NULL;
    for (HeaderB* obj = list; obj != NULL; obj = obj->pNext) {
        if (obj == headerToRemove) {
            if (prev != NULL) {
                prev->pNext = obj->pNext;
                return list;
            }
            else {
                return obj->pNext;
            }
        }
        prev = obj;
    }
    return list;
}