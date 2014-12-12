#ifndef HUFFMAN_H
#define HUFFMAN_H

#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#define CHAR_COUNT 256

struct HuffmanNode
{
    char value;
    long frequency;
    struct HuffmanNode *left_child;
    struct HuffmanNode *right_child;
};
typedef struct HuffmanNode HuffmanNode;

void huffman_coding(char input[], char output[]);

void count_frequency(int fd_input, long frequency[CHAR_COUNT]);

void write_header(int fd_output);

void write_tree(int fd_output, HuffmanNode* root);

void write_encoded(int fd_output, HuffmanNode* root);

HuffmanNode* create_node(char value, long frequency);

HuffmanNode* create_tree(long frequency[CHAR_COUNT]);

void delete_tree(HuffmanNode* root);

#endif // HUFFMAN_H
