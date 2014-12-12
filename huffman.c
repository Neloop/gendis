#include "huffman.h"

void huffman_coding(char input[], char output[])
{
    long frequency[CHAR_COUNT];
    HuffmanNode* root;
    int fd_input, fd_output;

    //open files

    count_frequency(fd_input, frequency);

    root = create_tree(frequency);

    write_header(fd_output);
    write_tree(fd_output, root);
    write_encoded(fd_output, root);

    delete_tree(root);

    //close files

    return;
}

void count_frequency(int fd_input, long frequency[])
{
    return;
}

void write_header(int fd_output)
{
    return;
}

void write_tree(int fd_output, HuffmanNode *root)
{
    return;
}

void write_encoded(int fd_output, HuffmanNode *root)
{
    HuffmanNode* position = root;



    return;
}

HuffmanNode* create_node(char value, long frequency)
{
    HuffmanNode* node = (HuffmanNode*)malloc(sizeof(HuffmanNode));

    node->frequency = frequency;
    node->value = value;
    node->left_child = NULL;
    node->right_child = NULL;

    return node;
}

HuffmanNode* create_tree(long frequency[CHAR_COUNT])
{
    HuffmanNode* root;

    return root;
}

void delete_tree(HuffmanNode *root)
{
    HuffmanNode* stack[256];
    int stack_size = 1;
    stack[0] = root;

    while(stack_size != 0)
    {
        HuffmanNode *left, *right, *del;
        left = stack[stack_size - 1]->left_child;
        right = stack[stack_size - 1]->right_child;
        del = stack[stack_size - 1];

        stack_size--;
        free(del);

        if(left != NULL){ stack[stack_size] = left; stack_size++; }
        if(right != NULL){ stack[stack_size] = right; stack_size++; }
    }

    return;
}
