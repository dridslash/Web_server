/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Derya_Request.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/28 10:30:08 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/03/29 13:17:23 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Derya_Request.hpp"


Derya_Request::Derya_Request(){}

Derya_Request::~Derya_Request(){}

void Derya_Request::Parse_Request(char *Request){
    std::stringstream get_Request;
    get_Request << Request;
    std::getline(get_Request,Request_Line);
    std::cout << Request_Line << std::endl;
    while(get_Request){
        std::string key;
        std::string value;
        get_Request >> key >> value;
        key = key.substr(0,key.find(":"));
        request_header.insert(std::make_pair(key,value));
    }
}