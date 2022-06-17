#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>

#define __INFINITY__ 1000000007

enum Position {
    EMPTY = '0',
    FIRST_PLAYER = '1',
    SECOND_PLAYER = '2'
};

enum Type {
    AND,
    OR
};

enum Result {
    UNKNOWN = -1,
    FALSE,
    TRUE
};

struct Game {
    int n, m, k;
    char player;
    char** map = NULL;

    Game(int n, int m, int k, char player, char** map);
    ~Game();
};

struct Node {
    Game* game = NULL;
    Node* parent = NULL;
    Node* children = NULL;
    Node* sibling = NULL;
    int proof = 1;
    int disproof = 1;
    int value = UNKNOWN;
    bool expanded = false;
    int type = OR;
    char** state = NULL;
    char player;

    Node(Game* G, int type, Node* parent, char** state, char player);
    Node(int type, Node* parent, char player);
    void addChild(Node* node);
    void deleteSubtree();
    ~Node();
};

char** createMap(int n, int m);
void freeMemory(char** map, int n);
bool checkWinCond(char** map, int N, int M, int K, char ActivePlayer);
bool checkHorizontalAxis(char** map, int N, int M, int K, char ActivePlayer);
bool checkVerticalAxis(char** map, int N, int M, int K, char ActivePlayer);
bool checkVerticalAxis(char** map, int N, int M, int K, char ActivePlayer);
bool checkDiagonalsAxis(char** map, int N, int M, int K, char ActivePlayer);
bool checkAntiDiagonalsAxis(char** map, int N, int M, int K, char ActivePlayer);
int countPosMoves(char** map, int N, int M);
char changePlayer(char ActivePlayer);
void solveGame(Game* G);
char** cpyMap(char** map, int N, int M);

void PN(Node* root, void(*evaluate)(Node*));
void setProofAndDisproofNumbers(Node* node);
Node* selectMostProvingNode(Node* node);
void expandNode(Node* node, void(*evaluate)(Node*));
Node* updateAncestors(Node* node, Node* root);
void generateAllChildren(Node* node);
void evaluateWin(Node* node);
void evaluateTie(Node* node);

int main() {
    int N = 1, M = 1, K = 1;
    char player = FIRST_PLAYER;
    char** map = NULL;
    while (true) {
        scanf("SOLVE_GAME_STATE %d %d %d %c\n", &N, &M, &K, &player);
        if (feof(stdin) != 0) {
            break;
        }
        map = createMap(N, M);
        for (int y = 0; y < N; y++) {
            for (int x = 0; x < M; x++) {
                scanf("%c ", &map[y][x]);
            }
        }
        Game* G = new Game(N, M, K, player, map);
        solveGame(G);
        delete G;
    }
    return 0;
}

Game::Game(int n, int m, int k, char player, char** map)
    : n(n), m(m), k(k), player(player), map(map) {}

Game::~Game() {
    if (map) {
        freeMemory(map, n);
        map = NULL;
    }
}

Node::Node(Game* G, int type, Node* parent, char** state, char player) {
    this->game = G;
    this->parent = parent;
    this->state = cpyMap(state, game->n, game->m);
    this->player = player;
    this->type = type;
}

Node::Node(int type, Node* parent, char player) {
    this->game = parent->game;
    this->type = type;
    this->parent = parent;
    this->state = cpyMap(parent->state, game->n, game->m);
    this->player = player;
}

void Node::addChild(Node* node) {
    if (children == NULL)
        children = node;
    else {
        Node* tmp = children;
        while (tmp->sibling != NULL) {
            tmp = tmp->sibling;
        }
        tmp->sibling = node;
    }
}

void Node::deleteSubtree() {
    Node* elem = children;
    while (children != NULL) {
        elem = children;
        children = children->sibling;
        delete elem;
    }
}

Node::~Node() {
    if (state) {
        freeMemory(state, game->n);
        state = NULL;
    }
    deleteSubtree();
}

char** createMap(int n, int m) {
    char** map = new char* [n];
    for (int i = 0; i < n; i++) {
        map[i] = new char[m];
    }
    return map;
}

char** cpyMap(char** map, int N, int M) {
    char** cpy = createMap(N, M);
    for (int y = 0; y < N; y++)
        for (int x = 0; x < M; x++)
            cpy[y][x] = map[y][x];
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

void solveGame(Game* G) {
    Node* root = new Node(G, OR, NULL, G->map, G->player);
    PN(root, evaluateWin);
    if (root->proof == 0) {
        if (G->player == FIRST_PLAYER)
            printf("FIRST_PLAYER_WINS\n");
        else
            printf("SECOND_PLAYER_WINS\n");
    }
    else {
        delete root;
        root = new Node(G, OR, NULL, G->map, G->player);
        PN(root, evaluateTie);
        if (root->proof == 0) {
            printf("BOTH_PLAYERS_TIE\n");
        }
        else {
            if (G->player == FIRST_PLAYER)
                printf("SECOND_PLAYER_WINS\n");
            else
                printf("FIRST_PLAYER_WINS\n");
        }
    }
    delete root;
}

void PN(Node* root, void(*evaluate)(Node*)) {
    evaluate(root);
    setProofAndDisproofNumbers(root);
    Node* currentNode = root;
    while (root->proof != 0 && root->disproof != 0) {
        Node* mostProvingNode = selectMostProvingNode(currentNode);
        expandNode(mostProvingNode, evaluate);
        currentNode = updateAncestors(mostProvingNode, root);
    }
}

void setProofAndDisproofNumbers(Node* node) {
    if (node->expanded) //Internal node;
        if (node->type == AND) {
            node->proof = 0;
            node->disproof = __INFINITY__;
            Node* n = node->children;
            while (n != NULL) {
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
            while (n != NULL) {
                node->disproof = node->disproof + n->disproof;
                if (n->proof < node->proof)
                    node->proof = n->proof;
                n = n->sibling;
            }
        }
    else //Leaf
        switch (node->value) {
        case FALSE:
            node->proof = __INFINITY__;
            node->disproof = 0;
            break;
        case TRUE:
            node->proof = 0;
            node->disproof = __INFINITY__;
            break;
        case UNKNOWN:
            node->proof = 1;
            node->disproof = 1;
            break;
        }
}
//Select the most-proving node
Node* selectMostProvingNode(Node* node) {
    while (node->expanded) {
        Node* n = node->children;
        if (node->type == OR) //OR Node
            while (n->proof != node->proof)
                n = n->sibling;
        else //AND Node
            while (n->disproof != node->disproof)
                n = n->sibling;
        node = n;
    }
    return node;
}
//Expand node
void expandNode(Node* node, void(*evaluate)(Node*)) {
    generateAllChildren(node);
    Node* n = node->children;
    while (n != NULL) {
        evaluate(n);
        setProofAndDisproofNumbers(n);
        //Addition to original code
        if ((node->type == OR && n->proof == 0) ||
            (node->type == AND && n->disproof == 0))
            break;
        n = n->sibling;
    }
    node->expanded = true;
}
//Update ancestors
Node* updateAncestors(Node* node, Node* root) {
    do {
        int oldProof = node->proof;
        int oldDisProof = node->disproof;
        setProofAndDisproofNumbers(node);
        //No change on the path
        if (node->proof == oldProof &&
            node->disproof == oldDisProof)
            return node;
        //Delete (dis)proved trees
        if (node->proof == 0 || node->disproof == 0)
            node->deleteSubtree();
        if (node == root)
            return node;
        node = node->parent;
    } while (true);
}

void generateAllChildren(Node* node) {
    for (int y = 0; y < node->game->n; y++) {
        for (int x = 0; x < node->game->m; x++) {
            if (node->state[y][x] == EMPTY) {
                node->state[y][x] = node->player;
                if (node->type == OR)
                    node->addChild(new Node(AND, node, changePlayer(node->player)));
                else
                    node->addChild(new Node(OR, node, changePlayer(node->player)));
                node->state[y][x] = EMPTY;
            }
        }
    }
}

void evaluateWin(Node* node) {
    if (checkWinCond(node->state, node->game->n, node->game->m, node->game->k, node->game->player))
        node->value = TRUE;
    else if (countPosMoves(node->state, node->game->n, node->game->m) == 0
        || checkWinCond(node->state, node->game->n, node->game->m, node->game->k, changePlayer(node->game->player)))
        node->value = FALSE;
    else
        node->value = UNKNOWN;
}

void evaluateTie(Node* node) {
    if (checkWinCond(node->state, node->game->n, node->game->m, node->game->k, changePlayer(node->game->player)))
        node->value = FALSE;
    else if (countPosMoves(node->state, node->game->n, node->game->m) == 0 ||
        checkWinCond(node->state, node->game->n, node->game->m, node->game->k, node->game->player))
        node->value = TRUE;
    else
        node->value = UNKNOWN;
}