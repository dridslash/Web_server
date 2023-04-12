/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Derya_Request.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/28 10:30:08 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/04/12 10:07:23 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Derya_Request.hpp"
#include "Client_Gymir.hpp"

Derya_Request::Derya_Request():HTTPMethod(),Path(),HTTPVersion(),stat_method_form(-1,GET),Hold_sliced_Request(),flag_fill_file(header_with_some_potential_payload),content_length(0){}

Derya_Request::~Derya_Request(){}

int Derya_Request::Parse_Request(Client_Gymir& Client,Server_Master serv,unsigned long R_v){
    std::stringstream get_Request;
    int take_chunck_to_return = 0;
    get_Request << Client.Request;
    
    //============= GETTING REQUEST LINE =================
    if (flag_fill_file == header_with_some_potential_payload){
        // std::cout << "GET REQUEST LINE AND PARTING" << std::endl;
    std::getline(get_Request,Request_Line);
    //==================================================
    
    
    //"======== REQUEST LINE PARTING ==========
        bolkr_Request_Line(Request_Line);
    }
    //"========================================
    
        Hold_sliced_Request = Get_Requets_Header(Client.Request,stat_method_form,serv);
        if (flag_fill_file != pure_payload && flag_fill_file != chuncked){
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
        if (look_for_BWS(key) || isspace(value[0])){
            stat_method_form = std::make_pair(400,GET);
            return (400);
        }
        RequestHeader.insert(std::make_pair(key, value));
    }
    if (HTTPMethod == "GET" && (RequestHeader.find("Content-Length") != RequestHeader.end()
        || RequestHeader.find("Transfer-Encoding") != RequestHeader.end())){
        // std::cout << "Bad Request" << std::endl;
        stat_method_form = std::make_pair(400,GET);
        return (400);
    }
    if (HTTPMethod == "POST"){
        if (((RequestHeader.find("Content-Length") == RequestHeader.end() && RequestHeader.find("Transfer-Encoding") == RequestHeader.end())) 
            || (RequestHeader.find("Content-Length") != RequestHeader.end() && RequestHeader.find("Transfer-Encoding") != RequestHeader.end())){
                // std::cout << "Here1!" << std::endl;
                stat_method_form = std::make_pair(400,POST);
                return (400);
        }
        else{
            // std::cout << "Here2!" << std::endl;
                if (RequestHeader.find("Content-Length") != RequestHeader.end()){
                    content_length = stoi(RequestHeader.at("Content-Length"));
                    Client.FilePath = "../test";
                    Client.FilePath.append(serv.getReverseContentType(RequestHeader.at("Content-Type").c_str()));
                    Post_body_file.open(Client.FilePath ,std::ios::out | std::ios::app);
                    Post_body_file << Hold_sliced_Request.second; 
                    flag_fill_file = pure_payload;
                    if (check_file_size(Post_body_file) >= content_length){
                        Post_body_file.close();
                        stat_method_form = std::make_pair(200,POST);
                        return (200);
                    }
                }else if (RequestHeader.find("Content-Length") == RequestHeader.end() && RequestHeader.find("Transfer-Encoding") != RequestHeader.end()){
                    flag_fill_file = chuncked;
                    std::cout << "chuncked" << std::endl;
                }
            }
        }
    }
    else{
        // std::cout << "Here3!" << std::endl;
        // std::cout << flag_fill_file << std::endl;
        // std::cout << Request << std::endl;
            if (flag_fill_file == pure_payload)
                Post_body_file << Hold_sliced_Request.first;
                if (check_file_size(Post_body_file) >= content_length){
                Post_body_file.close();
                stat_method_form = std::make_pair(200,POST);
                return (200);
            }else if (flag_fill_file == chuncked){
                std::cout << "cuncked continue" << std::endl;
            }
    }
    
    //=============================== END_OF_PARSING ====================================
    
    //  for(std::map<std::string,std::string>::iterator it = RequestHeader.begin(); it != RequestHeader.end();it++){
    //     std::cout << "key -->" << it->first << "\nvalue -->" << it->second << std::endl;
    // }
    //==================================================================================
    
    if (HTTPMethod == "GET" || HTTPMethod == "DELETE")
        stat_method_form = std::make_pair(200,GET);
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
    if ((HTTPMethod.compare("GET") == 0 || HTTPMethod.compare("DELETE") == 0) && flag_fill_file == header_with_some_potential_payload){
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
        if (flag_fill_file != pure_payload && flag_fill_file != chuncked){
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

bool Derya_Request::is_chuncked(){
    if (HTTPMethod == "POST" && RequestHeader.find("Content-Length") == RequestHeader.end()){
        if (RequestHeader.find("Transfer-Encoding") != RequestHeader.end() && RequestHeader.at("Transfer-Encoding") == "chunked"){
        std::cout << "it is chunked" << std::endl;
        return true;
        // flag_fill_file = 8000; 
        }
    }
    return false;
}

bool parsing_chuncked_request(){
    return false;
}