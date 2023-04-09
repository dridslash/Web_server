/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Derya_Request.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/28 10:30:08 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/04/09 13:57:43 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Derya_Request.hpp"

Derya_Request::Derya_Request():HTTPMethod(),Path(),HTTPVersion(),stat_method_form(-1,NONE),Hold_sliced_Request(),flag_fill_file(1000){}

Derya_Request::~Derya_Request(){}

int Derya_Request::Parse_Request(std::string Request,Server_Master serv){
    std::stringstream get_Request;
    get_Request << Request;
    
    //============= GETTING REQUEST LINE =================
    if (flag_fill_file == 1000){
        std::cout << "GET REQUEST LINE AND PARTING" << std::endl;
    std::getline(get_Request,Request_Line);
    //==================================================
    
    
    //"======== REQUEST LINE PARTING ==========
        bolkr_Request_Line(Request_Line);
    }
    //"========================================
    
        Hold_sliced_Request = Get_Requets_Header(Request,stat_method_form,serv);
        if (flag_fill_file == 1000){
        if (Hold_sliced_Request.size() > 0){
            std::cout << "GOT REPLACED" << std::endl;
            get_Request << Hold_sliced_Request;
        }
    //"===================== PARSING FIELDS ============================="
    std::string key_value_geminie;
    std::string key;
    std::string value;
    int index_offset = 0;
    while(!std::getline(get_Request,key_value_geminie).eof()){
        if (key_value_geminie.find(":")  != std::string::npos){
            index_offset = key_value_geminie.find(":") + 1;
            key = key_value_geminie.substr(0,index_offset - 1);
            value = key_value_geminie.substr(index_offset + 1);
        }else
            break;
        if (look_for_BWS(key) || isspace(value[0]))
            return (400);
        RequestHeader.insert(std::make_pair(key, value));
    }
    if (HTTPMethod == "GET" && (RequestHeader.find("Content-Length") != RequestHeader.end()
        || RequestHeader.find("Transfer-Encoding") != RequestHeader.end())){
        std::cout << "Bad Request" << std::endl;
        return (400);
    }
        }
    //===================================================================
    // for(std::map<std::string,std::string>::iterator it = RequestHeader.begin(); it != RequestHeader.end();it++){
    //     std::cout << "key -->" << it->first << "\nvalue -->" << it->second << std::endl;
    // }
    //===================================================================
    
    return (-1);
}

void Derya_Request::bolkr_Request_Line(std::string request_line){
    std::stringstream parter;
    parter << request_line;
    parter >> HTTPMethod >> Path >> HTTPVersion;
}

bool Derya_Request::look_for_BWS(std::string field_name){
    for(std::string::iterator it = field_name.begin(); it != field_name.end();it++){
        if (isspace((*it)) && ((*it) != '\r' && (*it) != '\n')){
            std::cout << "IN" << std::endl;
            return true;
        }
    }
    return false;
}

std::string Derya_Request::Get_Requets_Header(std::string Request,std::pair<int,check_for_methods> stat_method,Server_Master serv){
    std::cout << "SLICING" << std::endl;
    if ((HTTPMethod.compare("GET") == 0 || HTTPMethod.compare("DELETE") == 0) && flag_fill_file == 1000){
        std::cout << "IN GET/DELETE WORK" << std::endl;
        std::string Request_holder = Request.substr(0,Request.find("\r\n\r\n"));
        if (HTTPMethod == "GET")
        stat_method_form = std::make_pair(-1,GET);
        else
        stat_method_form = std::make_pair(-1,DELETE);
        return (Request_holder);
    }
    else if (HTTPMethod.compare("POST") == 0){
        std::cout << "IN POST CONDITON" << std::endl;
        std::string Request_holder = Request.substr(0,Request.find("\r\n\r\n"));
        if (flag_fill_file != 9000){
        // std::string take_body = Request.substr(Request.find("\r\n\r\n"));
        // if (take_body.size() > 0){
        //     std::cout << "Have a body" << std::endl;
        //     // std::cout << take_body << std::endl;
        // }
        flag_fill_file = 9000;
        }else{
            std::cout << "Have a body2" << std::endl;
        }
        stat_method_form = std::make_pair(-1,POST);
        return (Request_holder);
    }
    return ("");
}
