#pragma once

#include "Config/ConfigFile.hpp"
#include "Response/Response.hpp"
#include "Client_Smár.hpp"

enum events_check{
    READ,
    WRITE
};

class Server_Eyjafjörður {
    public:
        struct addrinfo hints , *servinfo;
        static int kq;
        static Config conf;
        std::map<std::string, std::string> ContentTypes;
        std::set<unsigned long>listeners;
        std::map<int, Client_Smár*>Clients;
        std::set<struct kevent*>events_register;
        struct kevent retrieved_events[MAX_CONNECTIONS];
        struct kevent changelist[MAX_CONNECTIONS];
        int event_tracker;
        int Server_Socket;
        struct kevent *events;
        
        
        
        Server_Eyjafjörður();
        ~Server_Eyjafjörður();
        Server_Eyjafjörður(int sk,const char *port);
        std::string getContentType(const char*);
        void Upp_ports(char *Config_file);
        void Set_up_listeners(const char *port);
        int multiplexing();
        bool Add_Event_to_queue_ker(int fd , int filter);
        bool Disable_Event_from_queue_ker(int &fd , int filter);
        bool Delete_Event_to_queue_ker(int fd , int filter);
        bool Enable_Event_from_queue_ker(int &fd , int filter);
        void Change_Socket_To_Non_Block(int &fd);
        int Get_Server_Socket()const;
        void Print_Connection_Info();
        static Server_Eyjafjörður* Draupnir();
        void Add_Client(Client_Smár *client_copy);
        void Delete_Client(Client_Smár *client_copy);
        bool Check_Hamr_Clients();
        int Fill_Request_State_it(Client_Smár* client_request_state);
        bool Is_in_write_event(int &client_fd,struct kevent *retrieved_events);
        void Send_Response_State_It(Client_Smár* client_request_state, Response& ResponsePath);
        int Search_in_Events(int fd, struct kevent *retrieved_events,int n_ev);
        void Client_loop(struct kevent *retreived_events, int how_many_events);
        void DropClient(std::map<int,Client_Smár*>::iterator& it);
        void Reading_Part(int n_ev);
        void Sending_Part(struct kevent *retreived_events, int how_many_events);
        void PrintStatus(int fd = 0, const char* HTTPMethod = 0, std::string Path = std::string(), int StatusCode = 0, int flag = 0);
        void Parsing_Post(Client_Smár* client_request_state);
        std::string getReverseContentType(const char* resp);
        
    private:
        static int reuse; 
};