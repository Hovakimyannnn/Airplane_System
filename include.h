#ifndef INCLUDE_H
#define INCLUDE_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <sys/uio.h>

#define PORT 8082
#define MAX_USERS 5
#define NAME_MAX_LEN 15
#define LOGIN_MAX_LEN 20
#define PASS_LEN 9
#define BUFFER_SIZE 1024
#define COUNTRY_NAME 50

typedef struct {
    int sockfd;
    struct sockaddr_in server;
} Arg;

int sockfd, client_fd;

typedef struct {
    char name[NAME_MAX_LEN];
    char login[LOGIN_MAX_LEN];
    char password[PASS_LEN];
} Client_info;

typedef struct Node {
    Client_info data;
    struct Node* next;
} Client;


typedef struct {
    int client_fd;
    struct sockaddr_in client_struct;
    Client client;
} ThreadArg;


typedef struct {
    int flight_id;
    char source[COUNTRY_NAME];
    char destination[COUNTRY_NAME];
    int available_seats;
} Flight;

typedef struct FlightNode {
    Flight flight;
    struct FlightNode* next;
} FlightNode;


Client* head = NULL;          
FlightNode* flight_head = NULL;
FILE* flight_database;          
FILE* database;  
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;              


void login_user(ThreadArg* arg);
void register_user(ThreadArg* arg);
void add_client(Client** head, Client* new_client);
void write_in_database(Client_info client);
int check_correctness(Client_info info);
void sign_in_after_register(ThreadArg* arg);
void* thread_for_server_mod(void* arg);
FlightNode* create_flight();
void add_flight(FlightNode** flight_head, FlightNode* new_flight);
void print_flights(FlightNode* head);
void remove_flight(FlightNode** flight_head);
void send_messages(ThreadArg* arg);
void send_flight(ThreadArg* arg);
void book_flight(ThreadArg* arg);
void check_flight_availability(ThreadArg* arg);
void try_login(ThreadArg* arg);
void write_in_flight_database(Flight* new_flight);

#endif // INCLUDE_H

