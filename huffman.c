#include "huffman.h"

#define MAX_CHAR_COUNT 256

static int id = 0;

struct HuffmanNode
{
    int id;
    unsigned char value;
    long frequency;
    struct HuffmanNode *left_child;
    struct HuffmanNode *right_child;
};
typedef struct HuffmanNode HuffmanNode;

static void count_frequency(int fd_input, long frequency[])
{
    int read_ret;
    unsigned char character;

    while((read_ret = read(fd_input, &character, 1)) != 0)
    {
        if(read_ret == -1){ err(1, NULL); }

        ++frequency[(unsigned int)character];
    }

    return;
}

static void write_header(int fd_output)
{
    int i;
    char header[8] = { 0x7B, 0x68, 0x75, 0x7C, 0x6D, 0x7D, 0x66, 0x66 };

    for(i = 0; i < 8; ++i)
    {
        write(fd_output, &header[i], 1);
    }

    return;
}

static void write_node(int fd_output, HuffmanNode *node)
{
    long node_bin = 0;

    if(node == NULL){ return; }

    if(node->left_child == NULL){ node_bin += node->value; }

    node_bin = node_bin << 55; // make space for frequency

    node_bin += (node->frequency - ((node->frequency >> 55) << 55));

    node_bin = node_bin << 1; // make space for sign bit

    if(node->left_child == NULL){ node_bin += 1; }

    write(fd_output, &node_bin, 8);

    write_node(fd_output, node->left_child);
    write_node(fd_output, node->right_child);

    return;
}

static void write_tree(int fd_output, HuffmanNode *root)
{
    int i;
    unsigned char zero = 0;

    write_node(fd_output, root);

    /* write 8 zero bytes delimiter */
    for(i = 0; i < 8; ++i)
    {
        write(fd_output, &zero, 1);
    }

    return;
}

static void get_codes(HuffmanNode *node, unsigned char temp_code[], unsigned char codes[][MAX_CHAR_COUNT + 1])
{
    if(node == NULL){ return; }

    if(node->left_child == NULL)
    {
        strcpy((char*)codes[(int)node->value], (char *)temp_code);
    }
    else
    {
        unsigned char temp[MAX_CHAR_COUNT + 1] = { 0 };
        snprintf((char*)temp, MAX_CHAR_COUNT + 1, "%s%c", temp_code, '0');
        get_codes(node->left_child, temp, codes);

        snprintf((char*)temp, MAX_CHAR_COUNT + 1, "%s%c", temp_code, '1');
        get_codes(node->right_child, temp, codes);
    }

    return;
}

static void write_encoded(int fd_input, int fd_output, HuffmanNode *root)
{
    int read_ret, length = 0;
    unsigned char character;
    unsigned char buffer[MAX_CHAR_COUNT + 1 + 32] = { 0 };
    unsigned char codes[MAX_CHAR_COUNT][MAX_CHAR_COUNT + 1] = { { 0 } };

    get_codes(root, (unsigned char*)"", codes);

    while((read_ret = read(fd_input, &character, 1)) != 0)
    {
        if(read_ret == -1){ err(1, NULL); }

        length += snprintf((char*)buffer + length, MAX_CHAR_COUNT + 1 + 32, "%s", codes[(int)character]);

        if(strlen((char*)buffer) > 32)
        {
            int i;
            unsigned int temp = 0;

            for(i = 0; i < 32; ++i)
            {
                temp >>= 1;
                if(buffer[i] == '1'){ temp += (1 << 31); }
            }
            write(fd_output, &temp, 4);

            memmove((char*)buffer, buffer + 32, strlen((char*)buffer) - 32 + 1);
            memset((char*)buffer + strlen((char*)buffer), 0, MAX_CHAR_COUNT + 1 + 32 - strlen((char*)buffer));
            length = strlen((char*)buffer);
        }
    }

    int buff_len = strlen((char*)buffer);
    if(buff_len > 0)
    {
        int remainder = buff_len % 8;
        if(remainder != 0)
        {
            int i;
            int append = 8 - remainder;

            for(i = 0; i < append; ++i)
            {
                buff_len += snprintf((char*)buffer + buff_len, MAX_CHAR_COUNT + 1 + 32, "%c", '0');
            }
        }

        while(strlen((char*)buffer) > 0)
        {
            int i;
            unsigned char temp = 0;

            for(i = 0; i < 8; ++i)
            {
                temp >>= 1;
                if(buffer[i] == '1'){ temp += (1 << 7); }
            }
            write(fd_output, &temp, 1);

            memmove(buffer, buffer + 8, buff_len - 8 + 1);
            buff_len = strlen((char*)buffer);
            memset(buffer + buff_len, 0, MAX_CHAR_COUNT + 1 + 32 - buff_len);
        }
    }

    return;
}

static void get_tree(HuffmanNode *node)
{
    if (node->left_child != NULL)
    {
        printf(" %ld", node->frequency);
        get_tree(node->left_child);
        get_tree(node->right_child);
    }
    else { printf(" *%d:%ld", node->value, node->frequency); }

    return;
}

static HuffmanNode* create_node(unsigned char value, long frequency)
{
    HuffmanNode* node = (HuffmanNode*)malloc(sizeof(HuffmanNode));

    node->id = id++;
    node->frequency = frequency;
    node->value = value;
    node->left_child = NULL;
    node->right_child = NULL;

    return node;
}

static int get_lightest_node(HuffmanNode *nodes[])
{
    // desperately inefective implementation

    int i;
    int lightest = -1;

    for(i = 0; i < MAX_CHAR_COUNT; ++i)
    {
        if(nodes[i] == NULL){ continue; }
        else{ if(lightest == -1){ lightest = i; continue; } }

        if(nodes[i]->frequency != nodes[lightest]->frequency)
        {
            if(nodes[i]->frequency < nodes[lightest]->frequency){ lightest = i; }
            continue;
        }
        if(nodes[i]->left_child == NULL && nodes[lightest]->left_child != NULL)
        {
            lightest = i;
            continue;
        }
        if(nodes[i]->left_child == NULL && nodes[lightest]->left_child == NULL)
        {
            if(nodes[i]->value < nodes[i]->value){ lightest = i; continue; }
        }
        if(nodes[i]->left_child != NULL && nodes[lightest] != NULL)
        {
            if(nodes[i]->id < nodes[lightest]->id){ lightest = i; continue; }
        }
    }

    return lightest;
}

static HuffmanNode* create_tree(long frequency[MAX_CHAR_COUNT])
{
    int pos, i, count = 0; // top points right behind the last element
    HuffmanNode* root = NULL;
    HuffmanNode* nodes[MAX_CHAR_COUNT] = { NULL };

    for(i = 0; i < MAX_CHAR_COUNT; ++i)
    {
        if(frequency[i] != 0)
        {
            nodes[count] = create_node((unsigned char)i, frequency[i]);
            count++;
        }
    }

    while(count > 1)
    {
        pos = get_lightest_node(nodes);
        HuffmanNode* left = nodes[pos];
        nodes[pos] = NULL;
        pos = get_lightest_node(nodes);
        HuffmanNode* right = nodes[pos];

        HuffmanNode* newnode = create_node(0, left->frequency + right->frequency);
        newnode->left_child = left;
        newnode->right_child = right;

        nodes[pos] = newnode;
        --count;
    }

    root = nodes[pos];
    return root;
}

static void delete_tree(HuffmanNode *root)
{
    HuffmanNode* stack[MAX_CHAR_COUNT];
    int stack_size = 1;
    stack[0] = root;

    while(stack_size != 0)
    {
        HuffmanNode *left, *right, *del;
        del = stack[stack_size - 1];

        if(del != NULL)
        {
            left = stack[stack_size - 1]->left_child;
            right = stack[stack_size - 1]->right_child;
        }

        stack_size--;
        free(del);

        if(left != NULL){ stack[stack_size] = left; stack_size++; }
        if(right != NULL){ stack[stack_size] = right; stack_size++; }
    }

    return;
}

void huffman_coding(char input[], char output[])
{
    long frequency[MAX_CHAR_COUNT] = { 0 };
    HuffmanNode* root;
    int fd_input, fd_output;

    //open files
    if((fd_input = open(input, O_RDONLY)) == -1){ err(1, "%s", input); }
    if((fd_output = open(output, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1){ err(1, "%s", output); }

    count_frequency(fd_input, frequency);

    lseek(fd_input, 0, SEEK_SET); // reset offset in input file

    root = create_tree(frequency);

    /*get_tree(root);
    printf("\n");*/

    write_header(fd_output);
    write_tree(fd_output, root);
    write_encoded(fd_input, fd_output, root);

    delete_tree(root);

    //close files
    if(close(fd_input) == -1){ err(1, "%s", input); };
    if(close(fd_output) == -1){ err(1, "%s", output); };

    return;
}
