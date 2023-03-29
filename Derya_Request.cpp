/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Derya_Request.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/28 10:30:08 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/03/29 14:28:50 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Derya_Request.hpp"


Derya_Request::Derya_Request(){}

Derya_Request::~Derya_Request(){}

void Derya_Request::Parse_Request(char *Request){
    std::stringstream get_Request;
    get_Request << Request;
    //============= GETTING REQUEST LINE =================
    std::getline(get_Request,Request_Line);
    while(get_Request){
        std::string key;
        std::string value;
        get_Request >> key;
        get_Request >> value;
        key = key.substr(0,key.find(":"));
        request_header.insert(std::make_pair(key,value));
    }
    for(std::map<std::string,std::string>::iterator it = request_header.begin(); it != request_header.end(); it++){
        std::cout << it->first << std::endl;
        std::cout << it->second << std::endl;
    }

    //================================================
    
    //"======== REQUEST LINE PARTING ==========
        bolkr_Request_Line(Request_Line);
    //"================================
}

void Derya_Request::bolkr_Request_Line(std::string request_line){
    std::stringstream parter;
    parter << request_line;
    while(parter){
        parter >> HTTPMethod >> Path >> HTTPVersion;
    }
}