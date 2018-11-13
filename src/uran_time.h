/*******************************************************************************
	> File Name: uran_timer.h
	> Author: lroyd
	> Mail: htzhangxmu@163.com
	> Created Time: 2014.6.24
 *******************************************************************************/
#ifndef __RECT_URAN_TIM_H__
#define __RECT_URAN_TIM_H__


#include "uran_tree.h"



typedef amx_rbtree_key_t      amx_msec_t;
typedef amx_rbtree_key_int_t  amx_msec_int_t;

#define AMX_TIMER_INFINITE  (amx_msec_t) -1

#define AMX_TIMER_LAZY_DELAY  300


typedef struct _tagTree
{    
	TPLock								amx_event_timer_mutex;
	amx_thread_volatile amx_rbtree_t	amx_event_timer_rbtree;
	amx_rbtree_node_t					amx_event_timer_sentinel;
}T_TreeInfo;




#define amx_add_timer        amx_event_add_timer
#define amx_del_timer        amx_event_del_timer

amx_int_t amx_event_timer_init(T_TreeInfo *);
amx_msec_t amx_event_find_timer(T_TreeInfo *);
void amx_event_expire_timers(T_TreeInfo *);




#endif 
