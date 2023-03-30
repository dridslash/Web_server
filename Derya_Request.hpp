/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Derya_Request.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/28 10:30:08 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/03/29 14:45:11 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "header.hpp"

class Derya_Request{
    public:
    Derya_Request();
    ~Derya_Request();
    std::string Request_Line;
    std::string HTTPMethod;
    std::string Path;
    std::string HTTPVersion;
    std::map<std::string,std::string> request_header;

    //FUNCTION PARSING REQUEST
    void Parse_Request(char *Request);
    
    //FUNCTION PARSING REQUEST LINE
    void bolkr_Request_Line(std::string request_line);
};