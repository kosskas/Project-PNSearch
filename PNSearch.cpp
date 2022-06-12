#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string.h>
#include <cstdio>

using namespace std;

#define checkString(a, b) (strcmp(a, b) == 0)
#define __MAX_CMD_LENGTH__ 100
#define __max__(a, b) (a > b ? a : b)
#define __min__(a, b) (a < b ? a : b)
#define __INFINITY__ 1000

enum Position {
    EMPTY = '0',
    FIRST_PLAYER = '1',
    SECOND_PLAYER = '2'
};

enum class Type {
    AND,
    OR
};

enum class Result {
    UNKNOWN = -1,
    FALSE,
    TRUE
};

struct Node {
    Node* parent = nullptr;
    Node* children = nullptr;
    Node* sibling = nullptr;
    char** state = nullptr;
    int n, m, k = 0;
    int proof = 1;
    int disproof = 1;
    Result value = Result::UNKNOWN;
    Type type = Type::OR;
    bool expanded = false;
    char player = FIRST_PLAYER;
    Node(Type type, Node* parent, char** state, int N, int M, int K, char player);
    Node(Type type, Node* parent, char player);
    void addChild(Node* node);
    void deleteSubtree();
    ~Node();
};

char** createMap(int n, int m);
void freeMemory(char** map, int n);
void printMap(char** map, int n, int m);
bool checkWinCond(char** map, int N, int M, int K, char ActivePlayer);
bool checkHorizontalAxis(char** map, int N, int M, int K, char ActivePlayer);
bool checkVerticalAxis(char** map, int N, int M, int K, char ActivePlayer);
bool checkVerticalAxis(char** map, int N, int M, int K, char ActivePlayer);
bool checkDiagonalsAxis(char** map, int N, int M, int K, char ActivePlayer);
bool checkAntiDiagonalsAxis(char** map, int N, int M, int K, char ActivePlayer);
int countPosMoves(char** map, int N, int M);
char changePlayer(char ActivePlayer);
void solveGame(char** map, int N, int M, int K, char player);
char** cpyMap(char**map, int N, int M);

void setProofAndDisproofNumbers(Node* node);
Node* updateAncestors(Node* node, Node* root);
Node* selectMostProvingNode(Node* node);
void evaluateWin(Node* root);
void evaluateTie(Node* root);
void generateAllChildren(Node* node);
void expandNode(Node* node, void (*evaluate)(Node*));
void PN(Node* root, void (*evaluate)(Node*));

char activeP;

int main() {
    int N = 0, M = 0, K = 0;
    char player = FIRST_PLAYER;
    char** map = nullptr;
    char command[__MAX_CMD_LENGTH__];
    while (true) {
        //scanf("SOLVE_GAME_STATE %d %d %d %c\n", &N, &M, &K, &player);
        cin >> command >> N >> M >> K >> player;
        if (feof(stdin) != 0) {
            break;
        }
        activeP = player;
        map = createMap(N, M);
        for (int y = 0; y < N; y++) {
            for (int x = 0; x < M; x++) {
                cin >> map[y][x];
               // scanf("%c ", &map[y][x]);
            }
        }
        solveGame(map, N, M, K, player);
        freeMemory(map, N);
    }
    return 0;
}

Node::Node(Type type, Node* parent, char** state, int N, int M, int K, char player)
    : type(type), parent(parent), state(cpyMap(state, N, M)) {
    n = N;
    m = M;
    k = K;
    this->player = player;
}

Node::Node(Type type, Node* parent, char player)
    : type(type), parent(parent), state(cpyMap(parent->state, parent->n, parent->m)) {
    n = parent->n;
    m = parent->k;
    k = parent->k;
    this->player = player;
}

Node::~Node() {
    if (state)
        freeMemory(state, n);
    deleteSubtree();
}

void Node::addChild(Node*node) {
    if (children == nullptr) {
        children = node;
    }
    else {
        Node* tmp = children;
        while (tmp->sibling != nullptr)
            tmp = tmp->sibling;
        tmp->sibling = node;
    }
}

void Node::deleteSubtree() {
    Node* elem = children;
    while (elem != nullptr) {
        elem = children;
        children = children->sibling;
        delete elem;
        elem = nullptr;
    }
}

char** createMap(int n, int m) {
    char** map = new char* [n];
    for (int i = 0; i < n; i++) {
        map[i] = new char[m];
    }
    return map;
}

char** cpyMap(char** map,int N, int M) {
    char** cpy = createMap(N, M);
    for (int i = 0; i < N; i++)
        for (int j = 0; j < M; j++)
            cpy[i][j] = map[i][j];
    return cpy;
}

void freeMemory(char** map, int n) {
    if (map) {
        for (int i = 0; i < n; i++) {
            if (map[i]) {
                delete[] map[i];
            }
        }
        delete[] map;
        map = nullptr;
    }
}

void printMap(char** map, int n, int m) {
    for (int y = 0; y < n; y++) {
        for (int x = 0; x < m; x++) {
            printf("%c ", map[y][x]);
        }
        printf("\n");
    }
}

bool checkWinCond(char** map, int N, int M, int K, char ActivePlayer) {
    return checkHorizontalAxis(map, N, M, K, ActivePlayer)
        || checkVerticalAxis(map, N, M, K, ActivePlayer)
        || checkDiagonalsAxis(map, N, M, K, ActivePlayer)
        || checkAntiDiagonalsAxis(map, N, M, K, ActivePlayer);
}
//poziom
bool checkHorizontalAxis(char** map, int N, int M, int K, char ActivePlayer) {
    for (int y = 0; y < N; y++) {
        for (int x = 0; x + K - 1 < M; x++) {
            if (map[y][x] == ActivePlayer) {
                int k = 0;
                for (k = 0; k < K; k++) {
                    if (map[y][x] != map[y][x + k]) {
                        break;
                    }
                }
                if (k == K)
                    return true;
            }
        }
    }
    return false;
}
//pion
bool checkVerticalAxis(char** map, int N, int M, int K, char ActivePlayer) {
    for (int y = 0; y + K - 1 < N; y++) {
        for (int x = 0; x < M; x++) {
            if (map[y][x] == ActivePlayer) {
                int k = 0;
                for (k = 0; k < K; k++) {
                    if (map[y][x] != map[y + k][x]) {
                        break;
                    }
                }
                if (k == K)
                    return true;
            }
        }
    }
    return false;
}

bool checkDiagonalsAxis(char** map, int N, int M, int K, char ActivePlayer) {
    for (int y = 0; y + K - 1 < N; y++) {
        for (int x = 0; x + K - 1 < M; x++) {
            if (map[y][x] == ActivePlayer) {
                int k = 0;
                for (k = 0; k < K; k++) {
                    if (map[y][x] != map[y + k][x + k]) {
                        break;
                    }
                }
                if (k == K)
                    return true;
            }
        }
    }
    return false;
}

bool checkAntiDiagonalsAxis(char** map, int N, int M, int K, char ActivePlayer) {
    for (int y = 0; y + K - 1 < N; y++) {
        for (int x = M - 1; x - K + 1 >= 0; x--) {
            if (map[y][x] == ActivePlayer) {
                int k = 0;
                for (k = 0; k < K; k++) {
                    if (map[y][x] != map[y + k][x - k]) {
                        break;
                    }
                }
                if (k == K)
                    return true;
            }
        }
    }
    return false;
}

int countPosMoves(char** map, int N, int M) {
    int pos_moves = 0;
    for (int y = 0; y < N; y++) {
        for (int x = 0; x < M; x++) {
            if (map[y][x] == EMPTY)
                pos_moves++;
        }
    }
    return pos_moves;
}

char changePlayer(char ActivePlayer) {
    if (ActivePlayer == FIRST_PLAYER)
        return SECOND_PLAYER;
    else
        return FIRST_PLAYER;
}

void solveGame(char** map, int N, int M, int K, char player) {
    Node* root = new Node(Type::OR, nullptr, map, N, M, K,player);
    PN(root, evaluateWin);
    
    if (root->proof == 0) {
        if (player == FIRST_PLAYER)
            printf("FIRST_PLAYER_WINS\n");
        else
            printf("SECOND_PLAYER_WINS\n");
    }
    else {
        delete root;
        root = new Node(Type::OR, nullptr, map, N, M, K, player);
        PN(root, evaluateTie);
        if (root->proof == 0) {
            printf("BOTH_PLAYERS_TIE\n");
        }
        else {
            if (player == FIRST_PLAYER)
                printf("SECOND_PLAYER_WIN1S\n");
            else
                printf("FIRST_PLAYER_WINS\n");
        } 
    }
    delete root;  
}

void evaluateWin(Node* root) {
    if (checkWinCond(root->state, root->n, root->m, root->k, activeP)) {
        root->value = Result::TRUE;
    }
    else if (checkWinCond(root->state, root->n, root->m, root->k, changePlayer(activeP))) {
        root->value = Result::FALSE;
    }
    root->value = Result::UNKNOWN;
}

void evaluateTie(Node* root) {
    if (countPosMoves(root->state, root->n, root->m) == 0) {
        root->value = Result::TRUE;
    }
    else if (checkWinCond(root->state, root->n, root->m, root->k, changePlayer(activeP))) {
        root->value = Result::FALSE;
    }
    root->value = Result::UNKNOWN;
}

void generateAllChildren(Node* node) {
    for (int y = 0; y < node->n; y++) {
        for (int x = 0; x < node->m; x++) {
            if (node->state[y][x] == EMPTY) {
                if (node->type == Type::OR) {
                    node->state[y][x] = node->player;
                    node->addChild(new Node(Type::AND, node, node->player));
                }
                else {
                    node->state[y][x] = changePlayer(node->player);
                    node->addChild(new Node(Type::OR, node, changePlayer(node->player)));
                }
                node->state[y][x] = EMPTY;
            }
        }
    }
}
//1
void setProofAndDisproofNumbers(Node* node) {
    if (node->expanded) {//Internal node;
        if (node->type == Type::AND) {
            node->proof = 0;
            node->disproof = __INFINITY__;
            Node* n = node->children;
            while (n != nullptr) {
                node->proof = node->proof + n->proof;
                if (n->disproof < node->disproof)
                    node->disproof = n->disproof;
                n = n->sibling;
            }
        }
        else { //OR node
            node->proof = __INFINITY__;
            node->disproof = 0;
            Node* n = node->children;
            while (n != nullptr) {
                node->disproof = node->disproof + n->disproof;
                if (n->proof < node->proof)
                    node->proof = n->proof;
                n = n->sibling;
            }
        }
    }
    else { //Leaf
        switch (node->value) {
            case Result::FALSE:
                node->proof = __INFINITY__;
                node->disproof = 0;
                break;
            case Result::TRUE:
                node->proof = 0;
                node->disproof = __INFINITY__;
                break;
            case Result::UNKNOWN:
                node->proof = 1;
                node->disproof = 1;
                break;
        }
    }
}

Node* selectMostProvingNode(Node* node) {    
    while (node->expanded) {
        Node* n = node->children;
        if (node->type == Type::OR) { //OR Node
            while (n->proof != node->proof) { //!
                n = node->sibling;
            }
        }
        else { //AND Node
            while (n->disproof != node->disproof) {
                n = node->sibling;
            }
        }
        node = n;
    }
    return node;
}

void expandNode(Node* node, void (*evaluate)(Node*)) {
    generateAllChildren(node);
    Node* n = node->children;
    while (n != nullptr) {
        evaluate(n);
        setProofAndDisproofNumbers(n);
        if ((node->type == Type::OR && n->proof == 0) || (node->type == Type::AND && n->disproof == 0)) {
            break;
        }
        n = n->sibling;
    }
    node->expanded = true;
}

Node* updateAncestors(Node* node, Node* root) {
    do {
        int oldProof = node->proof; //!
        int oldDisProof = node->disproof; //!
        setProofAndDisproofNumbers(node);
        //No change on the path
        if (node->proof == oldProof && node->disproof == oldDisProof) {
            return node;
        }
        //Delete (dis)proved trees
        if (node->proof == 0 || node->disproof == 0) {
            node->deleteSubtree();
        }
        if (node == root) {
            return node;
        }
        node = node->parent;
    } while (true);
}

void PN(Node* root, void (*evaluate)(Node*)) {
    evaluate(root);
    setProofAndDisproofNumbers(root);
    Node* currentNode = root;
    while (root->proof != 0 && root->disproof != 0) {
        Node* mostProvingNode = selectMostProvingNode(currentNode);
        expandNode(mostProvingNode, evaluate);
        currentNode = updateAncestors(mostProvingNode, root);
    }
}