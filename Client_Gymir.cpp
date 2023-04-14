#include "Client_Gymir.hpp"

//--------------STATIC_METHODES------------

Client_Gymir* Client_Gymir::Draupnir_Smár(int sockcl){
    return (new Client_Gymir(sockcl));
}

//-----------------------------------------

Client_Gymir::Client_Gymir():Client_Hamr(Still_Reading_Request), Bytes_received(0), Bytes_Sended(0), IsHeaderSended(false) , Request(), ChunkedSize(-2), RequestSize(0) {}

Client_Gymir::Client_Gymir(int sockcl):Client_Socket(sockcl), Client_Hamr(Still_Reading_Request), Bytes_received(0), Bytes_Sended(0), IsHeaderSended(false) , Request(), ChunkedSize(-2), RequestSize(0) {}

Client_Gymir::~Client_Gymir() {}

void Client_Gymir::Set_up_ip_port() {
    memset(&Own_addr,0,sizeof(Own_addr));
    int client_addlen = sizeof(Own_addr);
    getsockname(Client_Socket,(struct sockaddr*)&Own_addr,(socklen_t*)&client_addlen);
    Client_Ip_Port_Connected.first = inet_ntoa(Own_addr.sin_addr);
    Client_Ip_Port_Connected.second = ntohs(Own_addr.sin_port);
    ResponsePath.setHost(Client_Ip_Port_Connected.first);
    std::stringstream port_string;
    port_string << Client_Ip_Port_Connected.second;
    ResponsePath.setPort(port_string.str());
}