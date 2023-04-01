/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   t.cpp                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/01 10:37:58 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/04/01 10:50:59 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.hpp"
#define max_c 100


int main(){
    int kq = kqueue();
    
    struct kevent ev[1];
    struct kevent ret[max_c];
    EV_SET(&ev[0],0,EVFILT_READ,EV_ADD | EV_CLEAR,0,0,NULL);
    kevent(kq,&ev[0],1,NULL,0,NULL);

   while(true){
        int n = kevent(kq,ev,max_c,ret,max_c,NULL);
        for(int i = 0 ; i < n ; i++){
            if (ret[i].filter == EVFILT_READ && ret[i].ident == 0){
                    std::cout << "read happened" << std::endl;
            }
        }
   }
}