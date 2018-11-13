/*******************************************************************************
	> File Name: uran_timer.c
	> Author: lroyd
	> Mail: htzhangxmu@163.com
	> Created Time: 2014.6.24
 *******************************************************************************/
#include "uran_time.h"
#include "uran.h"


static amx_msec_t amx_get_sys_timer(void)
{	
  	struct 	timeval now_tv; 
    gettimeofday(&now_tv, NULL);
  	return (now_tv.tv_sec*1000 + now_tv.tv_usec/1000);
}

void amx_event_del_timer(T_EventInfo *ev)
{
	T_EvtMangerInfo *pThis = ev->m_pManger;
	
	
    LockTPLock(pThis->in_tTree.amx_event_timer_mutex);

    amx_rbtree_delete(&pThis->in_tTree.amx_event_timer_rbtree, &ev->timer);

    UnLockTPLock(pThis->in_tTree.amx_event_timer_mutex);

    ev->timer.left = NULL;
    ev->timer.right = NULL;
    ev->timer.parent = NULL;
    ev->m_iTimerSet = 0;	
}

void amx_event_add_timer(T_EventInfo *ev, amx_msec_t timer)
{
	T_EvtMangerInfo *pThis = ev->m_pManger;
    amx_msec_t      key;
    amx_msec_int_t  diff;

    key = amx_get_sys_timer() + timer;

    if (ev->m_iTimerSet) 
    {
        diff = (amx_msec_int_t) (key - ev->timer.key);
        if (amx_abs(diff) < AMX_TIMER_LAZY_DELAY) 
        {
           return;
        }
        amx_del_timer(ev);
    }
	
    ev->timer.key = key;
	
    LockTPLock(pThis->in_tTree.amx_event_timer_mutex);
	
    amx_rbtree_insert(&pThis->in_tTree.amx_event_timer_rbtree, &ev->timer);
	
    UnLockTPLock(pThis->in_tTree.amx_event_timer_mutex);
	
    ev->m_iTimerSet = 1;
}



amx_int_t amx_event_timer_init(T_TreeInfo *tree)
{
    amx_rbtree_init(&tree->amx_event_timer_rbtree, &tree->amx_event_timer_sentinel, amx_rbtree_insert_timer_value);
    InitTPLock(tree->amx_event_timer_mutex);
    return 0;
}


amx_msec_t amx_event_find_timer(T_TreeInfo *tree)
{
    amx_msec_int_t      timer;
    amx_rbtree_node_t  *node, *root, *sentinel;

    if (tree->amx_event_timer_rbtree.root == &tree->amx_event_timer_sentinel) 
	{
        return AMX_TIMER_INFINITE;
    }

    LockTPLock(tree->amx_event_timer_mutex);

    root = tree->amx_event_timer_rbtree.root;
    sentinel = tree->amx_event_timer_rbtree.sentinel;

    node = amx_rbtree_min(root, sentinel);

    UnLockTPLock(tree->amx_event_timer_mutex);

    timer = (amx_msec_int_t) (node->key - amx_get_sys_timer());

    return (amx_msec_t) (timer > 0 ? timer : 0);
}


void amx_event_expire_timers(T_TreeInfo *tree)
{
    T_EventInfo *ev;
    amx_rbtree_node_t  *node, *root, *sentinel;

    sentinel = tree->amx_event_timer_rbtree.sentinel;

    for ( ;; )
	{

        LockTPLock(tree->amx_event_timer_mutex);

        root = tree->amx_event_timer_rbtree.root;

        if (root == sentinel) 
		{
            return;
        }

        node = amx_rbtree_min(root, sentinel);

        if ((amx_msec_int_t) (node->key - amx_get_sys_timer()) <= 0) 
		{
            ev = (T_EventInfo *) node;
            amx_rbtree_delete(&tree->amx_event_timer_rbtree, &ev->timer); 
			
            UnLockTPLock(tree->amx_event_timer_mutex);
			
			ev->timer.left = NULL;
            ev->timer.right = NULL;
            ev->timer.parent = NULL;
            ev->m_iTimerSet = 0;
            if(ev->m_Handle)
            {
                ev->m_Handle(ev);
            }
            continue;
        }

        break;
    }

    UnLockTPLock(tree->amx_event_timer_mutex);
}
