/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Derya_Request.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/28 10:30:08 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/04/07 10:02:16 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Derya_Request.hpp"

Derya_Request::Derya_Request(){}

Derya_Request::~Derya_Request(){}

int Derya_Request::Parse_Request(std::string Request){
    std::stringstream get_Request;
    get_Request << Request;
    //============= GETTING REQUEST LINE =================
    std::getline(get_Request,Request_Line);
    //"===================== PARSING FIELDS ============================="
    std::string key_value_geminie;
    std::string key;
    std::string value;
    int index_offset = 0;
    while(std::getline(get_Request,key_value_geminie)){
        index_offset = key_value_geminie.find(":") + 1;
        key = key_value_geminie.substr(0,index_offset - 1);
        value = key_value_geminie.substr(index_offset + 1);
        if (look_for_BWS(key) || isspace(value[0]))
            return (400);
        RequestHeader.insert(std::make_pair(key,value)); 
    }
    //"======== REQUEST LINE PARTING ==========
        bolkr_Request_Line(Request_Line);
    //"================================
    return (0);
}

void Derya_Request::bolkr_Request_Line(std::string request_line){
    std::stringstream parter;
    parter << request_line;
    while(!parter.eof()){
        parter >> HTTPMethod >> Path >> HTTPVersion;
    }
}

bool Derya_Request::look_for_BWS(std::string field_name){
    for(std::string::iterator it = field_name.begin(); it != field_name.end();it++){
        if (isspace((*it)) && (*it) != '\r' && (*it) != '\n')
            return true;
    }
    return false;
}

