#include "sdb.h"
#include <memory/vaddr.h>

#define NR_WP 32
/*
typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  
  word_t var;   //to be scaned address
  int old_value;

} WP;
*/
static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;
static WP a={-1,0,-1,-1};
static WP b={-1,0,-1,-1};
static WP * h_head=&a;
static WP *h_free=&b;

void init_wp_pool() {
  printf("init_wp_pool() start\n");
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
    wp_pool[i].var = 0;
    wp_pool[i].old_value = 0;
  }

  head = NULL;
  free_ = wp_pool;
  h_head->next=head;
  h_free->next = free_;
  printf("init_wp_pool() end\n");
}

/* TODO: Implement the functionality of watchpoint */

WP * new_wp(word_t va,int v){
    //1.find wp_pool not used wp(temp)
    //2.add temp to head -> tail
    WP * temp;
    WP * temp_head;
    int count = 0;   //prevent infinite loop
    
    if(free_==NULL){
        assert(0);
        //return;
    }
    
    temp=h_free->next;
    temp->var = va;
    temp->old_value = v;
    
    h_free->next=free_->next;
    temp_head = head;
    while(temp_head->next!=NULL){
        temp_head=temp_head->next;
        count++;
        if(count==NR_WP){
            break;
        }
    }
    
    temp_head->next=temp;
    return temp;
}

void free_wp(int n){
    WP * used_temp = h_head;
    WP * temp = NULL;
    WP * free_temp = h_free;
    while(used_temp!=NULL){
        if(used_temp->next->NO == n){
            temp=used_temp->next;
            used_temp->next = temp->next;
            break;   
        }
        used_temp=used_temp->next;
        if(used_temp->next==NULL){
            printf("there is no WP NO==n\n");
            break;
        }
    }
    
    while(free_temp->next!=NULL){
        free_temp=free_temp->next;
    }
    temp->var = 0;
    temp->old_value = 0;
    free_temp->next=temp;
}

int traverse_pool(){
   WP * temp = h_head->next;
   word_t old_var = 0;
   int old_v = 0;
   int temp_value = 0;
   int change = -1;
   
   while(temp!=NULL){
       old_var = temp->var;
       old_v = temp->old_value;
       temp_value = vaddr_read(old_var,4);
       if(temp_value!=old_v){
           change = 1;
           printf("watch point value change\n");
       }
       temp=temp->next;
   }
   
   return change;
}








