#pragma once
#include "Server_Master.hpp"

enum check_for_methods{
    POST,
    GET,
    DELETE,
    NONE = -1
};

enum phase{
    header_with_some_potential_payload,
    pure_payload,
    // content_lenght,
    chuncked
};

class Client_Gymir;
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
    phase flag_fill_file;
    unsigned long content_length;
    unsigned long chuncked_size;
    unsigned long margin_chuncked;

    //FUNCTION PARSING REQUEST
    int Parse_Request(Client_Gymir& Client,Server_Master serv);
    
    //FUNCTION PARSING REQUEST LINE
    void bolkr_Request_Line(std::string request_line);

    //LOOK FOR Bad SPACE IN FIELD NAME
    bool look_for_BWS(std::string field_name);

    //DETERMEN BODY AND HEADER
    std::pair<std::string,std::string> Get_Requets_Header(std::string Request);

    //CHECK SIZE OF FILE POSTED
    unsigned long check_file_size(std::fstream &file);

    int parseReq(Client_Gymir& Client);
};