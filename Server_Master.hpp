#pragma once
#include "header.hpp"
#include "Response/Response.hpp"

enum events_check{
    READ,
    WRITE
};

class Client_Gymir;
class Server_Master {
    public:
        struct addrinfo hints , *servinfo;
        int kq;
        static Config conf;
        std::map<std::string, std::string> ContentTypes;
        std::set<unsigned long>listeners;
        std::map<int, Client_Gymir*>Clients;
        std::set<struct kevent*>events_register;
        struct kevent retrieved_events[MAX_CONNECTIONS];
        struct kevent changelist[MAX_CONNECTIONS];
        int event_tracker;
        int Server_Socket;
        struct kevent *events;

        Server_Master();
        ~Server_Master();
        Server_Master(int sk);
        std::string getContentType(const char*);
        std::string getReverseContentType(const char* resp);
        void Upp_ports(char *Config_file);
        void Set_up_listeners(const char *port);
        int multiplexing();
        bool Add_Event_to_queue_ker(int fd , int filter);
        bool Disable_Event_from_queue_ker(int &fd , int filter);
        bool Delete_Event_to_queue_ker(int fd , int filter);
        void Change_Socket_To_Non_Block(int &fd);
        static Server_Master* Draupnir();
        void Add_Client(Client_Gymir *client_copy);
        void Delete_Client(Client_Gymir *client_copy);
        void Reading_Part(int n_ev);
        int Fill_Request_State_it(Client_Gymir* client_request_state);
        int Search_in_Events(int fd, struct kevent *retrieved_events,int n_ev);
        void Sending_Part(struct kevent *retreived_events, int how_many_events);
        void DropClient(std::map<int,Client_Gymir*>::iterator& it);
        void PrintStatus(int fd = 0, const char* HTTPMethod = 0, std::string Path = std::string(), int StatusCode = 0, int flag = 0);
        
    private:
        static int reuse; 
};