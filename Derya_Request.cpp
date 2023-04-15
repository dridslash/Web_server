#include "Derya_Request.hpp"
#include "Client_Gymir.hpp"

unsigned long convert_from_hex(std::string i){
    std::stringstream hold_hex;
    hold_hex <<  i;
    unsigned long hex_num;
    hold_hex >> std::hex >> hex_num;
    return (hex_num);
}

int Derya_Request::parseReq(Client_Gymir& Client) {
    int BufferSize = Client.chunckedRequest.size();
    if (BufferSize < 6)
        return 1;
    if (Client.ChunkedSize < 0) {
        if (Client.ChunkedSize == -2)
            Client.chunkedBuffer = Client.chunckedRequest;
        else
            Client.chunkedBuffer = Client.chunckedRequest.substr(2);
        Client.chunckedRequest = Client.chunkedBuffer.substr(Client.chunkedBuffer.find("\r\n") + 2);
        BufferSize = Client.chunckedRequest.size();
        Client.chunkedBuffer = Client.chunkedBuffer.substr(0, Client.chunkedBuffer.find("\r\n"));
        Client.ChunkedSize = static_cast<int>(convert_from_hex(Client.chunkedBuffer));
    }
    if (Client.ChunkedSize == 0)
        return 1;
    else if (Client.ChunkedSize < BufferSize) {
        Post_body_file << Client.chunckedRequest.substr(0, Client.ChunkedSize);
        Client.chunckedRequest = Client.chunckedRequest.substr(Client.ChunkedSize);
        BufferSize = Client.chunckedRequest.size();
        Client.ChunkedSize = -1;
    }
    else {
        Post_body_file << Client.chunckedRequest;
        Client.ChunkedSize -= BufferSize;
        if (!Client.ChunkedSize) Client.ChunkedSize = -1;
        Client.chunckedRequest.clear();
        BufferSize = 0;
    }
    if (BufferSize > 5 && Client.chunckedRequest[2] == '0')
        return 1;
    if (Client.returnRead < Max_Reads && Client.chunckedRequest.size())
        if (parseReq(Client)) return 1;
    return 0;
}

Derya_Request::Derya_Request(): HTTPMethod(),Path(),HTTPVersion(),stat_method_form(-1,GET),Hold_sliced_Request(),flag_fill_file(header_with_some_potential_payload),content_length(0),chuncked_size(0){}

Derya_Request::~Derya_Request(){}

int Derya_Request::Parse_Request(Client_Gymir& Client,Server_Master serv){
    std::stringstream get_Request;
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
    
        Hold_sliced_Request = Get_Requets_Header(Client.Request);
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
    if (HTTPMethod != "GET" && HTTPMethod != "POST" && HTTPMethod != "DELETE") {
        stat_method_form = std::make_pair(501,POST);
        return (501);
    }
    if (HTTPMethod == "GET" && (RequestHeader.find("Content-Length") != RequestHeader.end()
        || RequestHeader.find("Transfer-Encoding") != RequestHeader.end())){
        stat_method_form = std::make_pair(400,GET);
        return (400);
    }
    if (HTTPMethod == "POST"){
        if (((RequestHeader.find("Content-Length") == RequestHeader.end() && RequestHeader.find("Transfer-Encoding") == RequestHeader.end())) 
            || (RequestHeader.find("Content-Length") != RequestHeader.end() && RequestHeader.find("Transfer-Encoding") != RequestHeader.end())){
                stat_method_form = std::make_pair(400,POST);
                return (400);
        }
        else{
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
                    if (RequestHeader.find("Transfer-Encoding")->second != "chunked"){
                            stat_method_form = std::make_pair(400,POST);
                            return (400);
                    }
                    flag_fill_file = chuncked;
                    Client.FilePath = "../test";
                    Client.FilePath.append(serv.getReverseContentType(RequestHeader.at("Content-Type").c_str()));
                    Post_body_file.open(Client.FilePath ,std::ios::out | std::ios::app);
                    Client.chunckedRequest.append(Hold_sliced_Request.second);
                    if (parseReq(Client) == 1) {
                        Post_body_file.close();
                        stat_method_form = std::make_pair(200,POST);
                        return (200);
                    }
                }
            }
        }
    }
    else {
            if (flag_fill_file == pure_payload){
                Post_body_file << Hold_sliced_Request.first;
                if (check_file_size(Post_body_file) >= content_length){
                    Post_body_file.close();
                    stat_method_form = std::make_pair(200,POST);
                    return (200);
                }
            }else if (flag_fill_file == chuncked){
                Client.chunckedRequest.append(Hold_sliced_Request.first);
                std::cout << "chuncked continue" << std::endl;
                if (parseReq(Client) == 1) {
                    Post_body_file.close();
                    stat_method_form = std::make_pair(200,POST);
                    return (200);
                }
            }
    }
    
    //=============================== END_OF_PARSING ====================================
    
    //  for(std::map<std::string,std::string>::iterator it = RequestHeader.begin(); it != RequestHeader.end();it++){
    //     std::cout << "key -->" << it->first << "\nvalue -->" << it->second << std::endl;
    // }
    //==================================================================================
    std::cout << "stat_method_form.first : " << Path << std::endl;
    std::cout << stat_method_form.first << std::endl;
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

std::pair<std::string,std::string> Derya_Request::Get_Requets_Header(std::string Request){
    // std::cout << "SLICING" << std::endl;
    // std::cout << Request << std::endl;
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
        std::cout << "IN POST CONDITON" << std::endl;
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