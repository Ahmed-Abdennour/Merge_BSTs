////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////  HEADERS  ///////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#define n 50
#define m 10

////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////  STRUCTURES  /////////////////////////////////////////////

typedef struct Node {     
    int data;
    struct Node* left;
    struct Node* right;
} Node;

typedef struct StackNode {
    struct Node* node;
    struct StackNode* next;
} StackNode;

struct BSTs {      
  struct Node *A1, *A2;
  struct Node *A3;
};

struct Properties{
  double density;
  int minH, maxH;  
};

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////  PROTOTYPES  ///////////////////////////////////////////////

Node* createNode(int data);
Node* insertNode(Node* root, int data);
bool searchBST(Node* root, int target);
void freeBST(Node** node);

StackNode* createStackNode(Node* node);
void push(StackNode** root, Node* node);
Node* pop(StackNode** root);
int isEmpty(StackNode* root);

Node* FirstInOrder(Node* root);
Node* NextInOrder(Node* root, Node* current);
Node* FirstInPostorder(Node* root);
Node* NextInPostorder(Node* root, Node* current);
Node* NextInPreorder(Node* root, Node* current);

int findMinDepth(Node* root);
int findMaxDepth(Node* root);
int findNumNodes(Node* root);

void init_Tab(struct Properties* tab[][5], int w);
void free_Tab(struct Properties* tab[][5], int w);
void Fill(Node* tree, struct Properties* tab[][5], int w, int c);

void generateData(struct BSTs** Bst, int h);
void Merge(struct BSTs** Tree, struct Properties* tab[][5]);

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////  MAIN PROGRAM  ////////////////////////////////////////////////

int main()
{
    struct BSTs* Arbres = malloc(sizeof(struct BSTs));
    struct Properties* Tab[m][5];
    init_Tab(Tab, m);

    for (int i = 0; i < m; i++) {  

        generateData(&Arbres, n);
        Merge(&Arbres, Tab);
    }

    free_Tab(Tab, m);
    free(Arbres);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////  IMPLEMENTATIONS  ///////////////////////////////////////////

Node* createNode(int data) {
    Node* newNode = malloc(sizeof(struct Node));
    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}
Node* insertNode(Node* root, int data) {
    if (root == NULL) {
        return createNode(data);
    }
    if (data <= root->data) {
        root->left = insertNode(root->left, data);
    } else {
        root->right = insertNode(root->right, data);
    }
    return root;
}
bool searchBST(Node* root, int target) {
    if (root == NULL) {
        return false;  // Cas de base: target n'est pas trouvé dans un arbre vide
    }

    if (target == root->data) {
        return true;   // Cas de base: target trouvé dans le noeud courant
    } else if (target < root->data) {
        return searchBST(root->left, target);  // Recherche recursive dans le sous-arbre gauche
    } else {
        return searchBST(root->right, target); // Recherche recursive dans le sous-arbre droite
    }
}
void freeBST(Node** node) {
    if (*node){
        freeBST(&((*node)->left));   // Appel récursif pour libérer les noeuds du sous-arbre gauche
        freeBST(&((*node)->right));  // Appel récursif pour libérer les noeuds du sous-arbre droit        
        free(*node);  // Libération de la mémoire occupée par le noeud courant
        *node = NULL;  // Réinitialisation du pointeur à NULL pour éviter toute sorte d'erreur
    }
}

StackNode* createStackNode(Node* node) {
    StackNode* stackNode = (StackNode*)malloc(sizeof(struct StackNode));
    stackNode->node = node;
    stackNode->next = NULL;
    return stackNode;
}
void push(StackNode** root, Node* node) {
    StackNode* stackNode = createStackNode(node);
    stackNode->next = *root;
    *root = stackNode;
}
Node* pop(StackNode** root) {
    if (*root == NULL)
        return NULL;

    StackNode* temp = *root;
    *root = (*root)->next;
    Node* popped = temp->node;
    free(temp);
    return popped;
}
int isEmpty(StackNode* root) {
    return root == NULL;
}

Node* FirstInOrder(Node* root) {
    if (root == NULL) {
        return NULL;  // Cas de base : sous-arbre vide
    }
    while (root->left) {
        root = root->left;  // Traverser le fils gauche
    }
    return root;  // Retourner le noeud le plus à gauche
}
Node* NextInOrder(Node* root, Node* current) {
    // Si le noeud courant a un fils droit
    if (current->right) {
        current = current->right;  // On déplace le noeud courant vers son fils droit
        while (current->left) {
            current = current->left;  // On déplace le noeud courant vers son fils gauche jusqu'à atteindre le noeud le plus à gauche
        }
        return current;  // Ce noeud est le successeur dans le parcours en ordre
    }

    Node* successor = NULL;
    while (root) {
        if (current->data < root->data) {
            successor = root;  // On met à jour le successeur avec le noeud actuel, car il pourrait être le successeur potentiel
            root = root->left;  // On déplace le pointeur de racine vers le fils gauche
        } else if (current->data > root->data) {
            root = root->right;  // On déplace le pointeur de racine vers le fils droit, car le successeur potentiel doit être plus grand
        } else {
            break;  // On a trouvé le noeud courant dans l'arbre, donc on sort de la boucle
        }
    }
    return successor;  // On retourne le successeur dans le parcours en ordre
}

Node* FirstInPostorder(Node* root) {
    if (root == NULL) {
        return NULL;  // Cas de base : sous-arbre vide
    }

    Node* prev = NULL;  // Stocke le noeud précédemment visité
    Node* current = root;  // Commence à partir de la racine

    // Vérifier si le noeud initial est une feuille ou s'il a déjà été visité
    if (current->left == NULL && current->right == NULL) {
        return current;
    }

    while (current != NULL) {
        // Si le fils gauche existe et n'a pas été visité, déplacez-vous vers le fils gauche
        if (current->left != NULL && prev != current->left && prev != current->right) {
            current = current->left;
        }
        // Si le fils droit existe et n'a pas été visité, déplacez-vous vers le fils droit
        else if (current->right != NULL && prev != current->right) {
            current = current->right;
        }
        // Si les deux enfants ont été visités ou n'existent pas, cela signifie que nous avons atteint un noeud feuille ou un noeud dont les enfants ont été visités
        else {
            break;
        }
        prev = current;
    }

    return current;  // Retourne le premier noeud dans le parcours en post-ordre
}
Node* NextInPostorder(Node* root, Node* current) {
    struct StackNode* stack1 = NULL;  // Stack1 pour le parcours en post-ordre
    struct StackNode* stack2 = NULL;  // Stack2 pour stocker les nœuds visités
    Node* prev = NULL;  // Nœud précédemment visité
    Node* next = NULL;  // Prochain nœud dans le parcours en post-ordre

    push(&stack1, root);  // Ajouter la racine à Stack1

    // Effectuer le parcours en post-ordre et stocker les nœuds visités dans Stack2
    while (!isEmpty(stack1)) {
        Node* temp = pop(&stack1);
        push(&stack2, temp);

        if (temp->left)
            push(&stack1, temp->left);
        if (temp->right)
            push(&stack1, temp->right);
    }

    // Parcourir les nœuds stockés dans Stack2 pour trouver le prochain nœud après le nœud courant
    while (!isEmpty(stack2)) {
        Node* temp = pop(&stack2);

        if (prev && prev == current) {
            next = temp;  // Le prochain nœud après le nœud courant a été trouvé
            break;
        }
        prev = temp;  // Mettre à jour le nœud précédemment visité
    }

    return next;  // Retourner le prochain nœud dans le parcours en post-ordre
}

Node* NextInPreorder(Node* root, Node* current) {
    StackNode* stack = NULL;
    push(&stack, root);

    while (!isEmpty(stack)) {
        root = pop(&stack);

        if (root->right) {
            push(&stack, root->right);
        }
        if (root->left) {
            push(&stack, root->left);
        }
        if (root == current) {
            return pop(&stack);
        }
    }

    return NULL;
}


int findMinDepth(struct Node *root) {
    if (root == NULL) {
        return 0;  // Cas de base : sous-arbre vide, donc hauteur minimale est 0
    }
    int leftDepth = findMinDepth(root->left);  // Hauteur minimale du sous-arbre gauche
    int rightDepth = findMinDepth(root->right);  // Hauteur minimale du sous-arbre droit
    if (leftDepth < rightDepth) {
        return leftDepth + 1;  // On retourne la hauteur minimale du sous-arbre gauche plus 1
    } else {
        return rightDepth + 1;  // On retourne la hauteur minimale du sous-arbre droit plus 1
    }
}
int findMaxDepth(Node* root) {
    if (root == NULL) {
        return 0;  // Cas de base : sous-arbre vide, donc hauteur maximale est 0
    }
    int leftDepth = findMaxDepth(root->left);  // Hauteur maximale du sous-arbre gauche
    int rightDepth = findMaxDepth(root->right);  // Hauteur maximale du sous-arbre droit
    if (leftDepth > rightDepth) {
        return leftDepth + 1;  // On retourne la hauteur maximale du sous-arbre gauche plus 1
    } else {
        return rightDepth + 1;  // On retourne la hauteur maximale du sous-arbre droit plus 1
    }
}
int findNumNodes(Node* root) {
    if (root) {
        return (findNumNodes(root->left) + findNumNodes(root->right) + 1); 
    }
    else {
        return 0;
    }
}

void init_Tab(struct Properties* tab[][5], int w) {
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < 5; j++) {
            tab[i][j] = malloc(sizeof(struct Properties));
            tab[i][j]->density = 0;
            tab[i][j]->minH = 0;
            tab[i][j]->maxH = 0;
        }
    }
}
void free_Tab(struct Properties* tab[][5], int w) {
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < 5; j++) {
            free(tab[i][j]);           
        }
    }
}
void Fill(Node* tree, struct Properties* tab[][5], int w, int c) {
    for (int e = 0; e < w; e++) {
            tab[e][c]->density = findNumNodes(tree) / pow(2, findMaxDepth(tree) - 1); 
            tab[e][c]->maxH = findMaxDepth(tree);     
            tab[e][c]->minH = findMinDepth(tree);     
    }
}

void generateData(struct BSTs** Bst, int h) {
  (*Bst)->A1 = NULL;
  (*Bst)->A2 = NULL;
  (*Bst)->A3 = NULL;
  int tempo;

  for (int l = 0; l < h; l++) {
    srand(time(NULL));  // Pour reéinitialiser le seed du rand() et éviter les erreurs
    do {
        tempo = rand(); // Génère un nombre aléatoire pour tempo
    } while(searchBST((*Bst)->A1, tempo)); // Vérifie si tempo existe déjà dans A1

    (*Bst)->A1 = insertNode((*Bst)->A1, tempo); // Insère tempo dans A1

    do {
        tempo = rand(); // Génère un autre nombre aléatoire pour tempo
    } while(searchBST((*Bst)->A2, tempo)); // Vérifie si tempo existe déjà dans A2
    
    (*Bst)->A2 = insertNode((*Bst)->A2, tempo); // Insère tempo dans A2

  }
}
void Merge(struct BSTs** Tree, struct Properties* tab[][5]) {

    Node *P = NULL, *Q = NULL;

    // CAS : I
    // X = Inordre, Y = Preordre
    // Récupération du premier noeud en Inordre de l'arbre A1
    P = FirstInOrder((*Tree)->A1);
    // Récupération du premier noeud en Preordre de l'arbre A2
    Q = (*Tree)->A2;

    while (Q || P) {

        if (P && (!Q || P->data <= Q->data)) {
            // Insérer le noeud de l'arbre A1 dans l'arbre de fusion
            (*Tree)->A3 = insertNode((*Tree)->A3, P->data);
            // Passer au prochain noeud en Inordre de l'arbre A1
            P = NextInOrder((*Tree)->A1, P);
        }
        else {
            // Insérer le noeud de l'arbre A2 dans l'arbre de fusion
            (*Tree)->A3 = insertNode((*Tree)->A3, Q->data);
            // Passer au prochain noeud en Preordre de l'arbre A2
            Q = NextInPreorder((*Tree)->A2, Q);
        }
    }
    printf("cas I : A3 : %d\n", findNumNodes((*Tree)->A3));
    // Remplir le tableau des proprietés
    Fill((*Tree)->A3, tab, m, 0);
    // Libérer l'arbre de fusion
    freeBST(&((*Tree)->A3));

    // CAS : II
    // X = Inordre, Y = Postordre
    // Récupération du premier noeud en Inordre de l'arbre A1
    P = FirstInOrder((*Tree)->A1);
    // Récupération du premier noeud en Postordre de l'arbre A2
    Q = FirstInPostorder((*Tree)->A2);

    while (Q || P) {

        if (P && (!Q || P->data <= Q->data)) {
            // Insérer le noeud de l'arbre A1 dans l'arbre de fusion
            (*Tree)->A3 = insertNode((*Tree)->A3, P->data);
            // Passer au prochain noeud en Inordre de l'arbre A1
            P = NextInOrder((*Tree)->A1, P);
        }
        else {
            // Insérer le noeud de l'arbre A2 dans l'arbre de fusion
            (*Tree)->A3 = insertNode((*Tree)->A3, Q->data);
            // Passer au prochain noeud en Postordre de l'arbre A2
            Q = NextInPostorder((*Tree)->A2, Q);
        }
    }
    printf("cas II : A3 : %d\n", findNumNodes((*Tree)->A3));
    // Remplir le tableau des proprietés
    Fill((*Tree)->A3, tab, m, 1);
    // Libérer l'arbre de fusion
    freeBST(&((*Tree)->A3));

    // CAS : III
    // X = Preordre, Y = Preordre
    // Récupération de la racine de l'arbre A1 (Preordre)
    P = (*Tree)->A1;
    // Récupération de la racine de l'arbre A2 (Preordre)
    Q = (*Tree)->A2;

    while (Q || P) {

        if (P && (!Q || P->data <= Q->data)) {
            // Insérer le noeud de l'arbre A1 dans l'arbre de fusion
            (*Tree)->A3 = insertNode((*Tree)->A3, P->data);
            // Passer au prochain noeud en Preordre de l'arbre A1
            P = NextInPreorder((*Tree)->A1, P);
        }
        else {
            // Insérer le noeud de l'arbre A2 dans l'arbre de fusion
            (*Tree)->A3 = insertNode((*Tree)->A3, Q->data);
            // Passer au prochain noeud en Preordre de l'arbre A2
            Q = NextInPreorder((*Tree)->A2, Q);
        }
    }
    printf("cas III : A3 : %d\n", findNumNodes((*Tree)->A3));
    // Remplir le tableau des proprietés
    Fill((*Tree)->A3, tab, m, 2);
    // Libérer l'arbre de fusion
    freeBST(&((*Tree)->A3));

    // CAS : IV
    // X = Postordre, Y = Postordre
    // Récupération du premier noeud en Postordre de l'arbre A1
    P = FirstInPostorder((*Tree)->A1);
    // Récupération du premier noeud en Postordre de l'arbre A2
    Q = FirstInPostorder((*Tree)->A2);

    while (Q || P) {

        if (P && (!Q || P->data <= Q->data)) {
            // Insérer le noeud de l'arbre A1 dans l'arbre de fusion
            (*Tree)->A3 = insertNode((*Tree)->A3, P->data);
            // Passer au prochain noeud en Postordre de l'arbre A1
            P = NextInPostorder((*Tree)->A1, P);
        }
        else {
            // Insérer le noeud de l'arbre A2 dans l'arbre de fusion
            (*Tree)->A3 = insertNode((*Tree)->A3, Q->data);
            // Passer au prochain noeud en Postordre de l'arbre A2
            Q = NextInPostorder((*Tree)->A2, Q);
        }
    }
    printf("cas IV : A3 : %d\n", findNumNodes((*Tree)->A3));
    // Remplir le tableau des proprietés
    Fill((*Tree)->A3, tab, m, 3);
    // Libérer l'arbre de fusion
    freeBST(&((*Tree)->A3));

    // CAS : V
    // X = Preordre, Y = Postordre
    // Récupération de la racine de l'arbre A1 (Preordre)
    P = (*Tree)->A1;
    // Récupération du premier noeud en Postordre de l'arbre A2
    Q = FirstInPostorder((*Tree)->A2);

    while (Q || P) {

        if (P && (!Q || P->data <= Q->data)) {
            // Insérer le noeud de l'arbre A1 dans l'arbre de fusion
            (*Tree)->A3 = insertNode((*Tree)->A3, P->data);
            // Passer au prochain noeud en Preordre de l'arbre A1
            P = NextInPreorder((*Tree)->A1, P);
        }
        else {
            // Insérer le noeud de l'arbre A2 dans l'arbre de fusion
            (*Tree)->A3 = insertNode((*Tree)->A3, Q->data);
            // Passer au prochain noeud en Postordre de l'arbre A2
            Q = NextInPostorder((*Tree)->A2, Q);
        }
    }
    printf("cas V : A3 : %d\n", findNumNodes((*Tree)->A3));
    // Remplir le tableau des proprietés
    Fill((*Tree)->A3, tab, m, 4);
    // Libérer l'arbre de fusion
    freeBST(&((*Tree)->A3));

    // Libérer les arbres d'origine
    freeBST(&((*Tree)->A1));
    freeBST(&((*Tree)->A2));
}
