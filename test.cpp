/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/12 17:41:17 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/03/12 18:12:07 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>


template <typename T> void pr(T a){
    std::cout<< a << std::endl;
}

void pr (int k){
    std::cout<< k << "ppp" <<std::endl;
}

int main(){
    pr(10);
}