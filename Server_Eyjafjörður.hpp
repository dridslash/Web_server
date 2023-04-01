/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_Eyjafjörður.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/21 16:54:59 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/04/01 12:49:49 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "header.hpp"
#include "ConfigFile.hpp"
#include "Client_Smár.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Derya_Request.hpp"

enum events_check{
    READ,
    WRITE
};

class Server_Eyjafjörður{
    public:
        struct addrinfo hints , *servinfo;
        static int kq;
        static Config conf;
        std::set<unsigned long>listeners;
        std::map<int,Client_Smár*>Clients;
        static Derya_Request Request_parser;
        std::set<struct kevent*>events_register;
        struct kevent retrieved_events[MAX_CONNECTIONS];
        struct kevent changelist[MAX_CONNECTIONS];
        int event_tracker;
        int Server_Socket;
        struct kevent *events;
        
        
        
        Server_Eyjafjörður();
        ~Server_Eyjafjörður();
        Server_Eyjafjörður(int sk,const char *port);
        void Upp_ports(char *Config_file);
        void Set_up_listeners(const char *port);
        int multiplexing();
        bool Add_Event_to_queue_ker(int fd , int filter);
        bool Disable_Event_from_queue_ker(int &fd , int filter);
        bool Delete_Event_to_queue_ker(int fd , int filter);
        bool Enable_Event_from_queue_ker(int &fd , int filter);
        bool Add_Event_to_ENDOF(int &fd , int filter);
        void Change_Socket_To_Non_Block(int &fd);
        int Get_Server_Socket()const;
        void Print_Connection_Info();
        static Server_Eyjafjörður* Draupnir();
        void Add_Client(Client_Smár *client_copy);
        void Delete_Client(Client_Smár *client_copy);
        bool Check_Hamr_Clients();
        void Fill_Request_State_it(Client_Smár* client_request_state, Response& ResponsePath);
        bool Is_in_write_event(int &client_fd,struct kevent *retrieved_events);
        void Send_Response_State_It(Client_Smár* client_request_state, Response& ResponsePath);
        int Search_in_Events(int fd, struct kevent *retrieved_events,int n_ev);
        bool Add_Event_to_queue_ker_witout_clear(int fd , int filter);
        void trigger_user_event();
        bool Add_EVENT_USER();
    private:
        static int reuse;
        
};