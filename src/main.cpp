#include <iostream>

struct Node {
    int keys[3];
    Node* children[4];
    int numKeys;
    bool isLeaf;
};

void splitChild(Node* parent, int index, Node* child);
void insertNonFull(Node* node, int key);
void removeKey(Node* node, int key);
void fill(Node* node, int index);
void borrowFromPrev(Node* node, int index);
void borrowFromNext(Node* node, int index);
void merge(Node* node, int index);

Node* createNode() {
    Node* newNode = new Node;
    for (int i = 0; i < 3; i++) {
        newNode->keys[i] = 0;
    }
    for (int i = 0; i < 4; i++) {
        newNode->children[i] = nullptr;
    }
    newNode->numKeys = 0;
    newNode->isLeaf = true;
    return newNode;
}

void insert(Node* root, int key) {
    if (root == nullptr) {
        root = createNode();
        root->keys[0] = key;
        root->numKeys = 1;
    } else {
        if (root->numKeys == 3) {
            Node* newRoot = createNode();
            newRoot->children[0] = root;
            splitChild(newRoot, 0, root);
            insertNonFull(newRoot, key);
            root = newRoot;
        } else {
            insertNonFull(root, key);
        }
    }
}

void insertNonFull(Node* node, int key) {
    int i = node->numKeys - 1;
    if (node->isLeaf) {
        while (i >= 0 && key < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->numKeys++;
    } else {
        while (i >= 0 && key < node->keys[i]) {
            i--;
        }
        i++;
        if (node->children[i]->numKeys == 3) {
            splitChild(node, i, node->children[i]);
            if (key > node->keys[i]) {
                i++;
            }
        }
        insertNonFull(node->children[i], key);
    }
}

void splitChild(Node* parent, int index, Node* child) {
    Node* newNode = createNode();
    newNode->isLeaf = child->isLeaf;
    newNode->numKeys = 1;
    newNode->keys[0] = child->keys[2];
    child->numKeys = 1;

    if (!child->isLeaf) {
        newNode->children[0] = child->children[2];
        newNode->children[1] = child->children[3];
        child->children[2] = nullptr;
        child->children[3] = nullptr;
    }

    for (int i = parent->numKeys; i > index; i--) {
        parent->children[i + 1] = parent->children[i];
    }
    parent->children[index + 1] = newNode;

    for (int i = parent->numKeys - 1; i >= index; i--) {
        parent->keys[i + 1] = parent->keys[i];
    }
    parent->keys[index] = child->keys[1];
    parent->numKeys++;
}

void remove(Node* root, int key) {
    if (root == nullptr) {
        return;
    }

    removeKey(root, key);

    if (root->numKeys == 0) {
        Node* temp = root;
        if (root->isLeaf) {
            root = nullptr;
        } else {
            root = root->children[0];
        }
        delete temp;
    }
}

void removeKey(Node* node, int key) {
    int index = 0;
    while (index < node->numKeys && key > node->keys[index]) {
        index++;
    }

    if (index < node->numKeys && key == node->keys[index]) {
        if (node->isLeaf) {
            for (int i = index; i < node->numKeys - 1; i++) {
                node->keys[i] = node->keys[i + 1];
            }
            node->numKeys--;
        } else {
            Node* predecessor = node->children[index];
            while (!predecessor->isLeaf) {
                predecessor = predecessor->children[predecessor->numKeys];
            }
            node->keys[index] = predecessor->keys[predecessor->numKeys - 1];
            removeKey(predecessor, predecessor->keys[predecessor->numKeys - 1]);
        }
    } else {
        if (node->isLeaf) {
            return;
        }

        bool flag = (index == node->numKeys);

        if (node->children[index]->numKeys < 2) {
            fill(node, index);
        }

        if (flag && index > node->numKeys) {
            removeKey(node->children[index - 1], key);
        } else {
            removeKey(node->children[index], key);
        }
    }
}

void fill(Node* node, int index) {
    if (index != 0 && node->children[index - 1]->numKeys >= 2) {
        borrowFromPrev(node, index);
    } else if (index != node->numKeys && node->children[index + 1]->numKeys >= 2) {
        borrowFromNext(node, index);
    } else {
        if (index != node->numKeys) {
            merge(node, index);
        } else {
            merge(node, index - 1);
        }
    }
}

void borrowFromPrev(Node* node, int index) {
    Node* child = node->children[index];
    Node* sibling = node->children[index - 1];

    for (int i = child->numKeys - 1; i >= 0; i--) {
        child->keys[i + 1] = child->keys[i];
    }

    if (!child->isLeaf) {
        for (int i = child->numKeys; i >= 0; i--) {
            child->children[i + 1] = child->children[i];
        }
    }

    child->keys[0] = node->keys[index - 1];

    if (!child->isLeaf) {
        child->children[0] = sibling->children[sibling->numKeys];
    }

    node->keys[index - 1] = sibling->keys[sibling->numKeys - 1];

    child->numKeys++;
    sibling->numKeys--;
}

void borrowFromNext(Node* node, int index) {
    Node* child = node->children[index];
    Node* sibling = node->children[index + 1];

    child->keys[child->numKeys] = node->keys[index];

    if (!child->isLeaf) {
        child->children[child->numKeys + 1] = sibling->children[0];
    }

    node->keys[index] = sibling->keys[0];

    for (int i = 1; i < sibling->numKeys; i++) {
        sibling->keys[i - 1] = sibling->keys[i];
    }

    if (!sibling->isLeaf) {
        for (int i = 1; i <= sibling->numKeys; i++) {
            sibling->children[i - 1] = sibling->children[i];
        }
    }

    child->numKeys++;
    sibling->numKeys--;
}

void merge(Node* node, int index) {
    Node* child = node->children[index];
    Node* sibling = node->children[index + 1];

    child->keys[1] = node->keys[index];

    for (int i = 0; i < sibling->numKeys; i++) {
        child->keys[i + 2] = sibling->keys[i];
    }

    if (!child->isLeaf) {
        for (int i = 0; i <= sibling->numKeys; i++) {
            child->children[i + 2] = sibling->children[i];
        }
    }

    for (int i = index + 1; i < node->numKeys; i++) {
        node->keys[i - 1] = node->keys[i];
    }

    for (int i = index + 2; i <= node->numKeys; i++) {
        node->children[i - 1] = node->children[i];
    }

    child->numKeys += sibling->numKeys + 1;
    node->numKeys--;

    delete sibling;
}

bool search(Node* root, int key) {
    if (root == nullptr) {
        return false;
    }

    int index = 0;
    while (index < root->numKeys && key > root->keys[index]) {
        index++;
    }

    if (index < root->numKeys && key == root->keys[index]) {
        return true;
    }

    if (root->isLeaf) {
        return false;
    }

    return search(root->children[index], key);
}

int main() {
    Node* root = nullptr;

    // Insert elements
    insert(root, 4);
    insert(root, 6);
    insert(root, 13);
    insert(root, 7);
    insert(root, 19);
    insert(root, 20);
    insert(root, 34);
    insert(root, 28);
    insert(root, 29);
    insert(root, 100);
    insert(root, 130);
    insert(root, 140);
    insert(root, 8);
    insert(root, 15);

    // Search for elements
    std::cout << "Search results:" << std::endl;
    std::cout << "4: " << (search(root, 4) ? "Found" : "Not found") << std::endl;
    std::cout << "8: " << (search(root, 8) ? "Found" : "Not found") << std::endl;
    std::cout << "13: " << (search(root, 13) ? "Found" : "Not found") << std::endl;
    std::cout << "34: " << (search(root, 34) ? "Found" : "Not found") << std::endl;
    std::cout << "130: " << (search(root, 130) ? "Found" : "Not found") << std::endl;

    // Delete elements
    remove(root, 8);
    remove(root, 130);
    remove(root, 13);
    remove(root, 34);

    return 0;
}
