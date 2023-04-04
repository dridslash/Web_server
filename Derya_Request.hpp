/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Derya_Request.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/28 10:30:08 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/03/31 09:23:10 by mnaqqad          ###   ########.fr       */
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
    std::map<std::string,std::string> RequestHeader;

    //FUNCTION PARSING REQUEST
    void Parse_Request(std::string Request);
    
    //FUNCTION PARSING REQUEST LINE
    void bolkr_Request_Line(std::string request_line);
};