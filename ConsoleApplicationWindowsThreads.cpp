#include <iostream>
#include <windows.h>
using namespace std;

struct Node {
    int id;
    int distance;
    bool visited;
    Node* next;
};

Node* head = NULL;

void addNode(int id) {
    Node* newNode = new Node;
    newNode->id = id;
    newNode->distance = 100;
    newNode->visited = false;
    newNode->next = NULL;

    if (head == NULL) {
        head = newNode;
    }
    else {
        Node* temp = head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}

void addEdge(int source, int destination, int weight) {
    Node* srcNode = head;
    while (srcNode->id != source) {
        srcNode = srcNode->next;
    }

    Node* destNode = head;
    while (destNode->id != destination) {
        destNode = destNode->next;
    }

    Node* newNode = new Node;
    newNode->id = destination;
    newNode->distance = weight;
    newNode->next = srcNode->next;
    srcNode->next = newNode;

    newNode = new Node;
    newNode->id = source;
    newNode->distance = weight;
    newNode->next = destNode->next;
    destNode->next = newNode;
}

DWORD WINAPI findShortestPath(LPVOID param) {
    int* threadParams = (int*)param;
    int startNodeId = threadParams[0];
    int endNodeId = threadParams[1];

    Node* currentNode = head;
    while (currentNode != NULL) {
        if (currentNode->id == startNodeId) {
            currentNode->distance = 0;
            break;
        }
        currentNode = currentNode->next;
    }

    while (true) {
        bool foundUnvisitedNode = false;
        currentNode = head;

        while (currentNode != NULL) {
            if (!currentNode->visited) {
                foundUnvisitedNode = true;
                break;
            }
            currentNode = currentNode->next;
        }

        if (!foundUnvisitedNode) {
            break;
        }

        currentNode->visited = true;

        Node* neighbor = currentNode->next;
        while (neighbor != NULL) {
            if (!neighbor->visited &&
                currentNode->distance + neighbor->distance < neighbor->distance) {
                neighbor->distance = currentNode->distance + neighbor->distance;
            }
            neighbor = neighbor->next;
        }
    }

    return 0;
}

int main()
{
    setlocale(LC_ALL, "ru");

    addNode(1);
    addNode(2);
    addNode(3);
    addNode(4);
    addNode(5);

    addEdge(1, 2, 10);
    addEdge(1, 3, 20);
    addEdge(2, 4, 5);
    addEdge(3, 4, 15);
    addEdge(3, 5, 30);
    addEdge(4, 5, 10);

    int source = 1;
    int threadParams1[2] = { 1, 3 };
    int threadParams2[2] = { 4, 5 };

    HANDLE hThread1 = CreateThread(NULL, 0, findShortestPath, threadParams1, 0, NULL);
    HANDLE hThread2 = CreateThread(NULL, 0, findShortestPath, threadParams2, 0, NULL);

    if (hThread1 == NULL || hThread2 == NULL) {
        return GetLastError();
    }

    WaitForSingleObject(hThread1, INFINITE);
    WaitForSingleObject(hThread2, INFINITE);
    CloseHandle(hThread1);
    CloseHandle(hThread2);

    Node* destNode = head;
    while (destNode->id != 5) {
        destNode = destNode->next;
    }

    cout << "Кратчайший путь от 1 до 5: " << destNode->distance << endl;

    return 0;
}
