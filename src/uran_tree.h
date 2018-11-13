/*******************************************************************************
	> File Name: amx_rbtree.h
	> Author: lroyd
	> Mail: htzhangxmu@163.com
	> Created Time: 2014.6.24
 *******************************************************************************/
#ifndef __RECT_URAN_TREE_H__
#define __RECT_URAN_TREE_H__

#include "common.h"

typedef intptr_t        amx_int_t;
typedef uintptr_t       amx_uint_t;
typedef intptr_t        amx_flag_t;

typedef amx_int_t   amx_rbtree_key_t;
typedef amx_int_t   amx_rbtree_key_int_t;

#ifndef amx_thread_volatile
	#define amx_thread_volatile   volatile
#endif

#ifndef amx_inline
	#define amx_inline  inline 
#endif

#define amx_abs(value)       (((value) >= 0) ? (value) : - (value))
#define amx_max(val1, val2)  ((val1 < val2) ? (val2) : (val1))
#define amx_min(val1, val2)  ((val1 > val2) ? (val2) : (val1))


typedef struct amx_rbtree_node_s  amx_rbtree_node_t;

struct amx_rbtree_node_s 
{
    amx_rbtree_key_t       key;
    amx_rbtree_node_t     *left;
    amx_rbtree_node_t     *right;
    amx_rbtree_node_t     *parent;
    u_char                 color;
    u_char                 data;
};


typedef struct amx_rbtree_s  amx_rbtree_t;

typedef void (*amx_rbtree_insert_pt) (amx_rbtree_node_t *root, amx_rbtree_node_t *node, amx_rbtree_node_t *sentinel);

struct amx_rbtree_s 
{
    amx_rbtree_node_t     *root;
    amx_rbtree_node_t     *sentinel;
    amx_rbtree_insert_pt   insert;
};


#define amx_rbtree_init(tree, s, i)                                           \
    amx_rbtree_sentinel_init(s);                                              \
    (tree)->root = s;                                                         \
    (tree)->sentinel = s;                                                     \
    (tree)->insert = i


void amx_rbtree_insert(amx_thread_volatile amx_rbtree_t *tree, amx_rbtree_node_t *node);
void amx_rbtree_delete(amx_thread_volatile amx_rbtree_t *tree, amx_rbtree_node_t *node);
void amx_rbtree_insert_value(amx_rbtree_node_t *root, amx_rbtree_node_t *node, amx_rbtree_node_t *sentinel);
void amx_rbtree_insert_timer_value(amx_rbtree_node_t *root, amx_rbtree_node_t *node, amx_rbtree_node_t *sentinel);


#define amx_rbt_red(node)               ((node)->color = 1)
#define amx_rbt_black(node)             ((node)->color = 0)
#define amx_rbt_is_red(node)            ((node)->color)
#define amx_rbt_is_black(node)          (!amx_rbt_is_red(node))
#define amx_rbt_copy_color(n1, n2)      (n1->color = n2->color)
#define amx_rbtree_sentinel_init(node)  amx_rbt_black(node)


static amx_inline amx_rbtree_node_t *amx_rbtree_min(amx_rbtree_node_t *node, amx_rbtree_node_t *sentinel)
{
    while (node->left != sentinel) 
	{
        node = node->left;
    }
    return node;
}


#endif 
