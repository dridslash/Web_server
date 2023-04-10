/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Derya_Request.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/28 10:30:08 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/04/10 14:02:52 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Derya_Request.hpp"

Derya_Request::Derya_Request():HTTPMethod(),Path(),HTTPVersion(),stat_method_form(-1,GET),Hold_sliced_Request(),flag_fill_file(1000),content_length(0){}

Derya_Request::~Derya_Request(){}

int Derya_Request::Parse_Request(std::string Request,Server_Master serv,unsigned long R_v){
    std::stringstream get_Request;
    get_Request << Request;
    
    //============= GETTING REQUEST LINE =================
    if (flag_fill_file == 1000){
        // std::cout << "GET REQUEST LINE AND PARTING" << std::endl;
    std::getline(get_Request,Request_Line);
    //==================================================
    
    
    //"======== REQUEST LINE PARTING ==========
        bolkr_Request_Line(Request_Line);
    }
    //"========================================
    
        Hold_sliced_Request = Get_Requets_Header(Request,stat_method_form,serv);
        if (flag_fill_file != 9000){
        if (Hold_sliced_Request.first.size() > 0){
            // std::cout << "GOT REPLACED" << std::endl;
            get_Request << Hold_sliced_Request.first;
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
            value.pop_back();
        }else
            break;
        if (look_for_BWS(key) || isspace(value[0]))
            return (400);
        RequestHeader.insert(std::make_pair(key, value));
    }
    if (HTTPMethod == "GET" && (RequestHeader.find("Content-Length") != RequestHeader.end()
        || RequestHeader.find("Transfer-Encoding") != RequestHeader.end())){
        // std::cout << "Bad Request" << std::endl;
        stat_method_form = std::make_pair(400,GET);
        return (400);
    }
    if (HTTPMethod == "POST"){
        if ((RequestHeader.find("Content-Type") != RequestHeader.end() && RequestHeader.find("Transfer-Encoding") != RequestHeader.end())){
                stat_method_form = std::make_pair(400,POST);
                return (400);
        }
        else{
        content_length = stoi(RequestHeader.at("Content-Length"));
        Post_body_file.open("test" + serv.getReverseContentType(RequestHeader.at("Content-Type").c_str()) ,std::ios::out | std::ios::app);
        Post_body_file << Hold_sliced_Request.second; 
        flag_fill_file = 9000;
            if (check_file_size(Post_body_file) >= content_length){
            Post_body_file.close();
            stat_method_form = std::make_pair(200,POST);
            return (200);
            }
        }
    }
    }
    else{
        // std::cout << flag_fill_file << std::endl;
        // std::cout << Request << std::endl;
       Post_body_file << Hold_sliced_Request.first;
       if (check_file_size(Post_body_file) >= content_length){
            Post_body_file.close();
            stat_method_form = std::make_pair(200,POST);
            return (200);
       }
    }
    //===================================================================
    //  for(std::map<std::string,std::string>::iterator it = RequestHeader.begin(); it != RequestHeader.end();it++){
    //     std::cout << "key -->" << it->first << "\nvalue -->" << it->second << std::endl;
    // }
    //===================================================================
    // stat_method_form = std::make_pair(200,NONE);
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
            // std::cout << "IN" << std::endl;
            return true;
        }
    }
    return false;
}

std::pair<std::string,std::string> Derya_Request::Get_Requets_Header(std::string Request,std::pair<int,check_for_methods> stat_method,Server_Master serv){
    // std::cout << "SLICING" << std::endl;
    std::string Request_holder;
    if ((HTTPMethod.compare("GET") == 0 || HTTPMethod.compare("DELETE") == 0) && flag_fill_file == 1000){
        // std::cout << "IN GET/DELETE WORK" << std::endl;
         Request_holder = Request.substr(0,Request.find("\r\n\r\n"));
        if (HTTPMethod == "GET")
        stat_method_form = std::make_pair(-1,GET);
        else
        stat_method_form = std::make_pair(-1,DELETE);
        return (std::make_pair(Request_holder,""));
    }
    else if (HTTPMethod.compare("POST") == 0){
        // std::cout << "IN POST CONDITON" << std::endl;
        if (flag_fill_file != 9000){
            // std::cout << "Taking first header/body" << std::endl;
            Request_holder = Request.substr(0,Request.find("\r\n\r\n"));
            std::string take_body = Request.substr(Request.find("\r\n\r\n") + 4);
            stat_method_form = std::make_pair(-1,POST);
            return (std::make_pair(Request_holder,take_body));
        }else{
            // std::cout << "just taking the body" << std::endl;
            return (std::make_pair(Request,""));
        }
    }
    return (std::make_pair("",""));
}

unsigned long Derya_Request::check_file_size(std::fstream &file){
    file.seekg(0,std::ios::end);
    unsigned long size = file.tellg();
    file.seekg(0,std::ios::beg);
    return (size);
}