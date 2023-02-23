/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vol1.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/22 17:31:43 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/02/23 12:13:12 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>


template<typename T1>
struct simple_int{
};

template<typename T1>
struct simple_int<T1*>{
};


template<typename T1>
struct simple_int<const T1*>{
};

template<typename T>
class tag_taker{
    public:
        tag_taker(simple_int<T>){std::cout<<"simple\n";}
        tag_taker(simple_int<T*>){std::cout<< "im 22\n";}
        tag_taker(simple_int<const T*>){std::cout<< "im 33\n";}
};

 //* Definition for singly-linked list.
  struct ListNode {
      int val;
      ListNode *next;
      ListNode() : val(0), next(nullptr) {}
      ListNode(int x) : val(x), next(nullptr) {}
      ListNode(int x, ListNode *next) : val(x), next(next) {}
  };
 
class Solution {
public:
    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
        ListNode* generated_node;
         ListNode* tmp1 = l1;
         ListNode* tmp2 = l2;
         int remind_add = 0;
         while(tmp1 && tmp2){
            std::cout<<tmp1->val<<std::endl;
            tmp1 = tmp1->next;
         }
        return nullptr;
    }
};

int main(){
    // ListNode* new_node = new ListNode(20);
    // ListNode* new_node1 = new ListNode(30,new_node);
    // Solution().addTwoNumbers(new_node1,new_node);
    // struct simple_int<int*> l;
    // tag_taker<int> tg(l);
    
    return 0;
}