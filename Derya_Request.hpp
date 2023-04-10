/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Derya_Request.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/28 10:30:08 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/04/10 11:00:58 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Server_Master.hpp"

enum check_for_methods{
    POST,
    GET,
    DELETE,
    NONE = -1
};

enum file_state{
    still_reading,
    readed
};

class Derya_Request{
    public:
    Derya_Request();
    ~Derya_Request();
    check_for_methods which_method;
    std::string Request_Line;
    std::string HTTPMethod;
    std::string Path;
    std::string HTTPVersion;
    std::map<std::string,std::string> RequestHeader;
    std::fstream Post_body_file;
    std::pair<int,check_for_methods> stat_method_form;
    std::pair<std::string,std::string> Hold_sliced_Request;
    unsigned long content_length;
    int flag_fill_file;

    //FUNCTION PARSING REQUEST
    int Parse_Request(std::string Request,Server_Master serv,unsigned long R_v);
    
    //FUNCTION PARSING REQUEST LINE
    void bolkr_Request_Line(std::string request_line);

    //LOOK FOR Bad SPACE IN FIELD NAME
    bool look_for_BWS(std::string field_name);

    //DETERMEN BODY AND HEADER
    std::pair<std::string,std::string> Get_Requets_Header(std::string Request,std::pair<int,check_for_methods> stat_method,Server_Master serv);

    //CHECK SIZE OF FILE POSTED

    unsigned long check_file_size(std::fstream &file);

    // client_request_state->Request.substr(0,client_request_state->Request.find("\r\n\r\n"))
};