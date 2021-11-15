#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#define AMOUNT_TO_PRINT_PER_ONCE 20
#define AMOUNT_OF_RECORDS 4000

using namespace std;

struct Record {
  unsigned char a[32];
  unsigned char b[18];
  short int c;
  short int d;
  unsigned char e[10];
};

struct Vertex {
  Record data;
  int w = 0;
  Vertex *left = nullptr;
  Vertex *right = nullptr;
};

class Node {
public:
  Node *next = nullptr;
  Record data;
};

std::ostream &operator<<(std::ostream &strm, const Record &data) {
  return strm << setw(32) << data.a << setw(18) << data.b << setw(5) << data.c
              << setw(5) << data.d << setw(10) << data.e << endl;
};

class Queue {
public:
  Queue() {
    head = nullptr;
    tail = head;
  }
  bool isSorted = false;
  unsigned int getSize();
  void addNode(Record record);
  Record get(unsigned int index);
  void print();
  Record **buildIndexArray();
  Node *head;
  Node *tail;
};

Record Queue::get(unsigned int index) {
  if (!this->head)
    throw std::invalid_argument("Empty");
  if (index < 0 || index > this->getSize())
    throw std::invalid_argument("Out of bounds");
  Node *temp = head;
  int iterator = 0;
  while (temp) {
    if (iterator == index) {
      return temp->data;
    } else {
      iterator++;
      temp = temp->next;
    };
  };
};

unsigned int Queue::getSize() {
  int size = 0;
  if (!head) {
    return size;
  } else {
    Node *temp = head;
    while (temp) {
      size++;
      temp = temp->next;
    }
  }
  return size;
}

void Queue::addNode(Record record) {
  Node *newnode = new Node;
  newnode->data = record;
  if (!head) {
    head = newnode;
  } else {
    Node *temp = head;
    while (temp->next) {
      temp = temp->next;
    }
    temp->next = newnode;
    tail = temp->next;
  }
}

void Queue::print() {
  if (!head) {
    return;
  } else {
    Node *temp = head;
    bool choiceToContinue = true;
    int i = 1, start_i = i;
    do {
      cout << setw(6) << i << ") " << temp->data;

      if (i == start_i + AMOUNT_TO_PRINT_PER_ONCE - 1 &&
          i != AMOUNT_OF_RECORDS) {
        start_i = i + 1;
        cout << "? 0/1 : ";
        cin >> choiceToContinue;
      }
      i++;
      temp = temp->next;
    } while (temp && choiceToContinue);
  }
}

Queue readFromFile(string path) {
  std::ifstream fin(path, std::ios::binary);
  Queue list;
  int counter = 0;
  while (counter < AMOUNT_OF_RECORDS) {
    Record tempRecord;
    fin.read((char *)&tempRecord, sizeof(tempRecord));
    list.addNode(tempRecord);
    counter++;
  }
  return list;
}

void radixSort(Node *&S) {
  Node *p;
  int j, i, L = sizeof(p->data.b);
  int d;
  Queue Q[256];
  for (j = L - 1; j >= 0; j--) {
    for (i = 0; i < 256; i++) {
      Q[i].tail = (Node *)&Q[i].head;
    }
    p = S;
    while (p) {
      d = int(p->data.b[j]);
      Q[d].tail->next = p;
      Q[d].tail = p;
      p = p->next;
    }
    p = (Node *)&S;
    for (i = 0; i < 256; i++) {
      if (Q[i].tail != (Node *)&Q[i].head) {
        p->next = Q[i].head;
        p = Q[i].tail;
      }
    }
    p->next = nullptr;
  }

  L = sizeof(p->data.a);
  for (j = L - 1; j >= 0; j--) {
    for (i = 0; i < 256; i++) {
      Q[i].tail = (Node *)&Q[i].head;
    }
    p = S;
    while (p) {
      d = int(p->data.a[j]);
      Q[d].tail->next = p;
      Q[d].tail = p;
      p = p->next;
    }
    p = (Node *)&S;
    for (i = 0; i < 256; i++) {
      if (Q[i].tail != (Node *)&Q[i].head) {
        p->next = Q[i].head;
        p = Q[i].tail;
      }
    }
    p->next = nullptr;
  }
}

std::string charArrayToString(unsigned char *charArray, int size) {
  std::string stringToBeReturned = "";
  for (int i = 0; i < size; i++) {
    stringToBeReturned += charArray[i];
  }
  return stringToBeReturned;
}

Record **Queue::buildIndexArray() {
  Record **newIndexArray = new Record *[AMOUNT_OF_RECORDS];
  Node *temp = this->head;
  int i = 0;
  while (temp && i < AMOUNT_OF_RECORDS) {
    newIndexArray[i++] = &(temp->data);
    temp = temp->next;
  }
  return newIndexArray;
}

bool areDatesEqual(unsigned char a1[10], unsigned char a2[10]) {
  int day1 = (a1[0] - '0') * 10 + (a1[1] - '0');
  int day2 = (a2[0] - '0') * 10 + (a2[1] - '0');
  int month1 = (a1[3] - '0') * 10 + (a1[4] - '0');
  int month2 = (a2[3] - '0') * 10 + (a2[4] - '0');
  int year1 = (a1[6] - '0') * 10 + (a1[7]);
  int year2 = (a2[6] - '0') * 10 + (a2[7]);
  return (day1 == day2) && (month1 == month2) && (year1 == year2);
}

bool isLarger(Record record1, Record record2) {
  if (record1.c > record2.c) {
    return true;
  } else if (record1.c == record2.c) {
    if (record1.d > record2.d) {
      return true;
    } else if (record1.d == record2.d) {
      if (charArrayToString(record1.e, 10) > charArrayToString(record2.e, 10)) {
        return true;
      } else {
        return false;
      }
    } else {
      return false;
    }
  } else {
    return false;
  }
}

bool isEqual(Record record1, Record record2) {
  return (record1.c == record2.c) && (record1.d == record2.d) &&
         areDatesEqual(record1.e, record2.e);
}

bool treeSearch(Vertex *p, Record record) {
  while (p) {

    if (isEqual(p->data, record)) {
      return true;
    } else if (isLarger(p->data, record)) {
      p = p->left;
    } else {
      p = p->right;
    }
  };
  return false;
}
void addToSDPrec(Record record, Vertex *&p) {
  if (!p) {
    p = new Vertex;
    p->data = record;
  } else if (isLarger(record, p->data)) {
    addToSDPrec(record, p->right);
  } else {
    addToSDPrec(record, p->left);
  }
}

void A1(Vertex *&root, Record **indexArray, int size) {
  int W[size];
  for (int i = 0; i < size; i++) {
    W[i] = rand() % 100;
  };
  int i, j;
  for (i = 0; i < size - 1; i++)
    for (j = 0; j < size - i - 1; j++)
      if (W[j] < W[j + 1]) {
        int tmp = W[j];
        W[j] = W[j + 1];
        W[j + 1] = tmp;
        Record tmpRecord = *indexArray[j];
        *indexArray[j] = *indexArray[j + 1];
        *indexArray[j + 1] = tmpRecord;
      }
  for (int i = 0; i < size; i++) {
    addToSDPrec(*indexArray[i], root);
  }
}

int cnt = 1;

void printFromLeftToRight(Vertex *p) {
  if (p) {
    printFromLeftToRight(p->left);
    cout << setw(6) << cnt++ << ") " << p->data;
    printFromLeftToRight(p->right);
  }
}

void menuForTree(Queue list) {
  Vertex *root = nullptr;
  A1(root, list.buildIndexArray(), list.getSize());
  printFromLeftToRight(root);
  while (true) {
    int choice = 0;
    std::cout << "1. Print tree\n2. Tree search\n3. Exit from tree menu\n\n";
    std::cout << ">> ";
    std::cin >> choice;
    switch (choice) {
    case 1: {
      cnt = 1;
      printFromLeftToRight(root);
      break;
    }
    case 2: {
      std::cout << "Search mode\n";
      std::cout << "Input C >>";
      short c;
      std::cin >> c;
      std::cout << "Input D >>";
      short d;
      std::cin >> d;
      std::cout << "Input E >>";
      unsigned char e[10];
      std::cin >> e;
      Record record{"", "", c, d};
      for (int i = 0; i < 10; i++) {
        record.e[i] = e[i];
      }
      std::cout << "Searching for..." << record << "\n\n";
      if (treeSearch(root, record)) {
        std::cout << "Found\n\n";
      } else {
        std::cout << "Not found\n\n";
      }
      break;
    }
    case 3: {
      return;
    }
    }
  }
}

void binarySearch(Record **indexArray, std::string key) {
  int L = 0;
  int R = AMOUNT_OF_RECORDS - 1;
  Queue listWithFound;
  while (L < R) {
    int m = (L + R) / 2;

    if (charArrayToString(indexArray[m]->a, 3) < key) {
      L = m + 1;
    } else {
      R = m;
    }
  }
  while (charArrayToString(indexArray[R]->a, 3) == key) {
    listWithFound.addNode(*indexArray[R++]);
  }
  Node *temp = listWithFound.head;
  int number = 1;
  while (temp) {
    cout << setw(6) << number++ << ") " << temp->data;
    temp = temp->next;
  }
  if (number > 1) {
    int choice;
    std::cout << "Do you want to see a tree? 0/1 :";
    std::cin >> choice;
    if (choice) {
      menuForTree(listWithFound);
    }
  }
}

int main() {
  string db = "testBase4.dat";
  Queue list = readFromFile(db);
  while (true) {
    short int choice;
    cout << "1. Print records\n2. Radix Sort of records\n3. Binary Search\n0. "
            "Exit\n\n";
    cout << ">> ";
    cin >> choice;
    switch (choice) {
    case 1: {
      list.print();
      break;
    }
    case 2: {
      if (!list.isSorted) {
        radixSort(list.head);
        list.isSorted = true;
        cout << "Sorting...\n";
      } else {
        cout << "List already sorted\n";
      }
      break;
    }
    case 3: {
      if (!list.isSorted) {
        radixSort(list.head);
        list.isSorted = true;
      }
      cout << "Input key >> ";
      string key;
      cin >> key;
      binarySearch(list.buildIndexArray(), key);
      break;
    }
    case 0: {
      return EXIT_SUCCESS;
    }
    default: {
      cerr << "There's no any option with that number\n";
    }
    }
  };
};
