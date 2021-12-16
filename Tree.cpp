#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "DateTime.h"
#include "Objects.h"
#include "Headers.h"
#include "Structs.h"

#define BUFLEN 80

void PrintObject(Object1* pObject);

Node* InsertNode(Node* pTree, Object1* pNewRecord) {
    Node* pNew = (Node*)malloc(sizeof(Node));
    if (pNewRecord == NULL || pNew == NULL) {
        printf("InsertNode(): Error!\n");
        return NULL;
    }
    pNew->pObject = pNewRecord;
    pNew->pLeft = pNew->pRight = NULL;
    if (!pTree) {
        return pNew;
    }
    Node* p = pTree;
    while (true) {
        if (pNewRecord->Code < ((Object1*)(p->pObject))->Code) {
            if (p->pLeft == NULL) {
                p->pLeft = pNew;
                return pTree;
            }
            else
                p = p->pLeft;
        }
        else {
            if (p->pRight == NULL) {
                p->pRight = pNew;
                return pTree;
            }
            else
                p = p->pRight;
        }
    }
}

Node* CreateBinaryTree(HeaderB* headerB)
{
    int i = 1;
    Node* pBinaryTree = NULL;

    while (headerB != NULL) {
        HeaderA* headerA = headerB->pHeaderA;
        while (headerA != NULL) {
            Object1* pObjectTemp = (Object1*)headerA->pObject;
            while (pObjectTemp != NULL) {
                pBinaryTree = InsertNode(pBinaryTree, pObjectTemp);
                pObjectTemp = pObjectTemp->pNext;
                i++;
            }
            headerA = headerA->pNext;
        }
        headerB = headerB->pNext;
    }
    return pBinaryTree;
}

Stack* Push(Stack* pStack, Node* pRecord) {
    if (pRecord == NULL) {
        return pStack;
    }
    Stack* pNew = (Stack*)malloc(sizeof(Stack));
    if (pNew == NULL) {
        printf("Push(): Error'!\n");
        return NULL;
    }
    pNew->pObject = pRecord;
    pNew->pNext = pStack;
    return pNew;
}

Stack* Pop(Stack* pStack, void** pResult) {
    Stack* p;
    if (pStack == NULL) {
        *pResult = NULL;
        return pStack;
    }
    *pResult = (Object1*)pStack->pObject;
    p = pStack->pNext;
    free(pStack);
    return p;
}

void TreeTraversal(Node* pTree) {
    printf("--------Printing binary tree:----------\n");
    Stack* pStack = NULL;
    Node* p = pTree, * p2;
    
    if (pTree == NULL) {
        printf("TreeTraversal(): Error\n");
        return;
    }

    do {
        while (p != NULL) {
            pStack = Push(pStack, p);
            p = p->pLeft;
        }
        pStack = Pop(pStack, (void**)&p2);
        PrintObject((Object1*)(p2->pObject));
        p = p2->pRight;
    } while (pStack != NULL || p != NULL);
}

Node* DeleteTreeNodeNone(Node* root, Node* par, Node* ptr)
{
	if (par == NULL) {
		root = NULL;
	} else if (ptr == par->pLeft) {
		par->pLeft = NULL;
	} else {
		par->pRight = NULL;
	}
	free(ptr);
	return root;
}

Node* DeleteTreeNodeOne(Node* root, Node* par, Node* ptr) {
	Node* child;

	if (ptr->pLeft != NULL) {
		child = ptr->pLeft;
	} else {
		child = ptr->pRight;
	}

	if (par == NULL) {
		root = child;
	} else if (ptr == par->pLeft) {
		par->pLeft = child;
	} else  {
		par->pRight = child;
	}
	free(ptr);
	return root;
}

Node* DeleteTreeNodeBoth(Node* root, Node* par, Node* ptr) {
	Node* successor = ptr, *parentSuccessor = ptr->pRight;

	while (successor->pLeft != NULL) {
		parentSuccessor = successor;
		successor = successor->pLeft;
	}

	ptr->pObject = successor->pObject;

	if (successor->pLeft == NULL && successor->pRight == NULL) {
		root = DeleteTreeNodeNone(root, parentSuccessor, successor);
	} else {
		root = DeleteTreeNodeOne(root, parentSuccessor, successor);
	}
	return root;
}


Node* DeleteTreeNode(Node* pTree, unsigned long int Code) {
	printf("Deleting %d\n", Code);
	Node* par = NULL, *ptr = pTree;

	while (ptr != NULL)	{
		if (Code == ((Object1*)ptr->pObject)->Code) {
			break;
		}
		par = ptr;
		if (Code < ((Object1*)ptr->pObject)->Code) {
			ptr = ptr->pLeft;
		} else {
			ptr = ptr->pRight;
		}
	}

	if (ptr == NULL) {
		printf("Code not found in tree\n");
        return pTree;
	}
	
    if (ptr->pLeft != NULL && ptr->pRight != NULL) { 
		pTree = DeleteTreeNodeBoth(pTree, par, ptr);
	}
	else if (ptr->pLeft != NULL) { 
		pTree = DeleteTreeNodeOne(pTree, par, ptr);
	}
	else if (ptr->pRight != NULL) {
		pTree = DeleteTreeNodeOne(pTree, par, ptr);
	}
	else {
		pTree = DeleteTreeNodeNone(pTree, par, ptr);
	}

	return pTree;
}

Node* AskToRemoveNodes(Node* tree) {
    int code;
    char buf[BUFLEN];
    while (true) {
        printf("Enter code to remove or -1 to finish: ");
        gets_s(buf, BUFLEN);
        int retVal = sscanf_s(buf, "%d", &code);
        if (retVal != 1) {
            continue;
        }

        if (code < 0) {
            return tree;
        }
        
        tree = DeleteTreeNode(tree, code);
        TreeTraversal(tree);
    }
    return tree;
}