/*******************************************************************************
	> File Name: uran_tree.c
	> Author: lroyd
	> Mail: htzhangxmu@163.com
	> Created Time: 2014.6.24
 *******************************************************************************/
#include "uran_tree.h"


static amx_inline void amx_rbtree_left_rotate(amx_rbtree_node_t **root, amx_rbtree_node_t *sentinel, amx_rbtree_node_t *node)
{
    amx_rbtree_node_t  *temp;

    temp = node->right;
    node->right = temp->left;

    if (temp->left != sentinel) 
	{
        temp->left->parent = node;
    }

    temp->parent = node->parent;

    if (node == *root) 
	{
        *root = temp;
    } 
	else if (node == node->parent->left) 
	{
        node->parent->left = temp;
    } 
	else 
	{
        node->parent->right = temp;
    }

    temp->left = node;
    node->parent = temp;
}


static amx_inline void amx_rbtree_right_rotate(amx_rbtree_node_t **root, amx_rbtree_node_t *sentinel, amx_rbtree_node_t *node)
{
    amx_rbtree_node_t  *temp;

    temp = node->left;
    node->left = temp->right;

    if (temp->right != sentinel) 
	{
        temp->right->parent = node;
    }

    temp->parent = node->parent;

    if (node == *root) 
	{
        *root = temp;
    }
	else if (node == node->parent->right) 
	{
        node->parent->right = temp;
    } 
	else 
	{
        node->parent->left = temp;
    }

    temp->right = node;
    node->parent = temp;
}

void amx_rbtree_insert(amx_thread_volatile amx_rbtree_t *tree, amx_rbtree_node_t *node)
{
    amx_rbtree_node_t  **root, *temp, *sentinel;

    root = (amx_rbtree_node_t **) &tree->root;
    sentinel = tree->sentinel;

    if (*root == sentinel) 
	{
        node->parent = NULL;
        node->left = sentinel;
        node->right = sentinel;
        amx_rbt_black(node);
        *root = node;

        return;
    }

    tree->insert(*root, node, sentinel);

    while (node != *root && amx_rbt_is_red(node->parent)) 
	{

        if (node->parent == node->parent->parent->left) 
		{
            temp = node->parent->parent->right;

            if (amx_rbt_is_red(temp)) 
			{
                amx_rbt_black(node->parent);
                amx_rbt_black(temp);
                amx_rbt_red(node->parent->parent);
                node = node->parent->parent;

            } else {
                if (node == node->parent->right) 
				{
                    node = node->parent;
                    amx_rbtree_left_rotate(root, sentinel, node);
                }

                amx_rbt_black(node->parent);
                amx_rbt_red(node->parent->parent);
                amx_rbtree_right_rotate(root, sentinel, node->parent->parent);
            }

        } else 
		{
            temp = node->parent->parent->left;

            if (amx_rbt_is_red(temp)) 
			{
                amx_rbt_black(node->parent);
                amx_rbt_black(temp);
                amx_rbt_red(node->parent->parent);
                node = node->parent->parent;

            } else {
                if (node == node->parent->left) 
				{
                    node = node->parent;
                    amx_rbtree_right_rotate(root, sentinel, node);
                }

                amx_rbt_black(node->parent);
                amx_rbt_red(node->parent->parent);
                amx_rbtree_left_rotate(root, sentinel, node->parent->parent);
            }
        }
    }

    amx_rbt_black(*root);
}


void amx_rbtree_insert_value(amx_rbtree_node_t *temp, amx_rbtree_node_t *node, amx_rbtree_node_t *sentinel)
{
    amx_rbtree_node_t  **p;

    for ( ;; ) 
	{
        p = (node->key < temp->key) ? &temp->left : &temp->right;

        if (*p == sentinel) 
		{
            break;
        }
        temp = *p;
    }

    *p = node;
    node->parent = temp;
    node->left = sentinel;
    node->right = sentinel;
    amx_rbt_red(node);
}


void amx_rbtree_insert_timer_value(amx_rbtree_node_t *temp, amx_rbtree_node_t *node, amx_rbtree_node_t *sentinel)
{
    amx_rbtree_node_t  **p;

    for ( ;; ) 
	{
        p = ((amx_rbtree_key_int_t) (node->key - temp->key) < 0)
            ? &temp->left : &temp->right;

        if (*p == sentinel) 
		{
            break;
        }
        temp = *p;
    }
    *p = node;
    node->parent = temp;
    node->left = sentinel;
    node->right = sentinel;
    amx_rbt_red(node);
}

void amx_rbtree_delete(amx_thread_volatile amx_rbtree_t *tree, amx_rbtree_node_t *node)
{
    amx_uint_t red;
    amx_rbtree_node_t  **root, *sentinel, *subst, *temp, *w;

    root = (amx_rbtree_node_t **) &tree->root;
    sentinel = tree->sentinel;

    if (node->left == sentinel) 
	{
        temp = node->right;
        subst = node;
    } 
	else if (node->right == sentinel) 
	{
        temp = node->left;
        subst = node;
    } 
	else 
	{
        subst = amx_rbtree_min(node->right, sentinel);

        if (subst->left != sentinel) 
		{
            temp = subst->left;
        } 
		else 
		{
            temp = subst->right;
        }
    }

    if (subst == *root) 
	{
        *root = temp;
        amx_rbt_black(temp);

        /* DEBUG stuff */
        node->left = NULL;
        node->right = NULL;
        node->parent = NULL;
        node->key = 0;

        return;
    }

    red = amx_rbt_is_red(subst);

    if (subst == subst->parent->left) 
	{
        subst->parent->left = temp;
    } 
	else 
	{
        subst->parent->right = temp;
    }

    if (subst == node) 
	{
        temp->parent = subst->parent;
    } 
	else 
	{
        if (subst->parent == node) 
		{
            temp->parent = subst;
        } 
		else 
		{
            temp->parent = subst->parent;
        }

        subst->left = node->left;
        subst->right = node->right;
        subst->parent = node->parent;
        amx_rbt_copy_color(subst, node);

        if (node == *root) 
		{
            *root = subst;
        } 
		else 
		{
            if (node == node->parent->left) 
			{
                node->parent->left = subst;
            } 
			else 
			{
                node->parent->right = subst;
            }
        }

        if (subst->left != sentinel) 
		{
            subst->left->parent = subst;
        }

        if (subst->right != sentinel) 
		{
            subst->right->parent = subst;
        }
    }

    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->key = 0;

    if (red) 
	{
        return;
    }

    while (temp != *root && amx_rbt_is_black(temp)) 
	{

        if (temp == temp->parent->left) 
		{
            w = temp->parent->right;
            if (amx_rbt_is_red(w)) 
			{
                amx_rbt_black(w);
                amx_rbt_red(temp->parent);
                amx_rbtree_left_rotate(root, sentinel, temp->parent);
                w = temp->parent->right;
            }

            if (amx_rbt_is_black(w->left) && amx_rbt_is_black(w->right)) 
			{
                amx_rbt_red(w);
                temp = temp->parent;
            } 
			else 
			{
                if (amx_rbt_is_black(w->right)) 
				{
                    amx_rbt_black(w->left);
                    amx_rbt_red(w);
                    amx_rbtree_right_rotate(root, sentinel, w);
                    w = temp->parent->right;
                }

                amx_rbt_copy_color(w, temp->parent);
                amx_rbt_black(temp->parent);
                amx_rbt_black(w->right);
                amx_rbtree_left_rotate(root, sentinel, temp->parent);
                temp = *root;
            }
        } 
		else 
		{
            w = temp->parent->left;

            if (amx_rbt_is_red(w)) 
			{
                amx_rbt_black(w);
                amx_rbt_red(temp->parent);
                amx_rbtree_right_rotate(root, sentinel, temp->parent);
                w = temp->parent->left;
            }

            if (amx_rbt_is_black(w->left) && amx_rbt_is_black(w->right)) 
			{
                amx_rbt_red(w);
                temp = temp->parent;
            } 
			else 
			{
                if (amx_rbt_is_black(w->left)) 
				{
                    amx_rbt_black(w->right);
                    amx_rbt_red(w);
                    amx_rbtree_left_rotate(root, sentinel, w);
                    w = temp->parent->left;
                }
                amx_rbt_copy_color(w, temp->parent);
                amx_rbt_black(temp->parent);
                amx_rbt_black(w->left);
                amx_rbtree_right_rotate(root, sentinel, temp->parent);
                temp = *root;
            }
        }
    }
    amx_rbt_black(temp);
}
