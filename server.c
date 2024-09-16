#include "include.h"


void* thread_func(void* arg) {
    ThreadArg* obj = (ThreadArg*)arg;
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "Welcome to Airplane System\n");
    send(obj->client_fd, buffer, strlen(buffer), 0);
    memset(buffer, 0, BUFFER_SIZE);
    snprintf(buffer, BUFFER_SIZE, "0 - Sign in\n1 - Register\n");
    send(obj->client_fd, buffer, strlen(buffer), 0);
    memset(buffer, 0, BUFFER_SIZE);
    int bytes_received = recv(obj->client_fd, buffer, sizeof(buffer), 0);
    int opt = atoi(buffer);

    switch (opt) {
        case 1:
            register_user(obj);
            snprintf(buffer, BUFFER_SIZE, "Account added successfully!");
            send(obj->client_fd, buffer, BUFFER_SIZE, 0);
            sign_in_after_register(obj);
            break;
        case 0:
            login_user(obj);
            send_messages(obj);
            break;
        default:
            memset(buffer, 0, BUFFER_SIZE);
            snprintf(buffer, BUFFER_SIZE, "Invalid input");
            send(obj->client_fd, buffer, BUFFER_SIZE, 0);
            break;
    }
    close(obj->client_fd);
    free(obj);
    pthread_exit(NULL);
}

void login_user(ThreadArg* arg) {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "Enter your login: ");
    send(arg->client_fd, buffer, BUFFER_SIZE, 0);
    int recv_bytes = recv(arg->client_fd, arg->client.data.login, LOGIN_MAX_LEN, 0);
    if (recv_bytes > 0) {
        arg->client.data.login[recv_bytes] = '\0';
    }
    memset(buffer, 0, BUFFER_SIZE);
    snprintf(buffer, BUFFER_SIZE, "Enter your password: ");
    send(arg->client_fd, buffer, BUFFER_SIZE, 0);
    int recv_bytes = recv(arg->client_fd, arg->client.data.password, PASS_LEN, 0);
    if (recv_bytes > 0) {
        arg->client.data.password[recv_bytes] = '\0';
    }

    if (check_correctness(arg->client.data)) {
        memset(buffer, 0, BUFFER_SIZE);
        snprintf(buffer, BUFFER_SIZE, "You are logged in successfully!");
        send(arg->client_fd, buffer, BUFFER_SIZE, 0);
    } else {
        memset(buffer, 0, BUFFER_SIZE);
        snprintf(buffer, BUFFER_SIZE, "Incorrect login or password");
        send(arg->client_fd, buffer, BUFFER_SIZE, 0);
        snprintf(buffer, BUFFER_SIZE, "Try again!");
        send(arg->client_fd, buffer, BUFFER_SIZE, 0);
        try_login(arg);
        if (check_correctness(arg->client.data)) {
            memset(buffer, 0, BUFFER_SIZE);
            snprintf(buffer, BUFFER_SIZE, "You are logged in successfully!");
            send(arg->client_fd, buffer, BUFFER_SIZE, 0);
        }
    }
}

void register_user(ThreadArg* arg) {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "Enter your name: ");
    send(arg->client_fd, buffer, BUFFER_SIZE, 0);
    int recv_bytes = recv(arg->client_fd, arg->client.data.name, NAME_MAX_LEN, 0);
    if (recv_bytes > 0) {
        arg->client.data.name[recv_bytes] = '\0';
    }
    memset(buffer, 0, BUFFER_SIZE);
    snprintf(buffer, BUFFER_SIZE, "Enter your login: ");
    send(arg->client_fd, buffer, BUFFER_SIZE, 0);
    int recv_bytes = recv(arg->client_fd, arg->client.data.login, NAME_MAX_LEN, 0);
    if (recv_bytes > 0) {
        arg->client.data.login[recv_bytes] = '\0';
    }
    snprintf(buffer, BUFFER_SIZE, "Enter your password: ");
    send(arg->client_fd, buffer, BUFFER_SIZE, 0);
    recv_bytes = recv(arg->client_fd, arg->client.data.password, PASS_LEN, 0);
    if (recv_bytes > 0) {
        arg->client.data.password[recv_bytes] = '\0';
    }

    Client* new_client = (Client*)malloc(sizeof(Client));
    if (new_client == NULL) {
        perror("memory allocation failed");
        exit(EXIT_FAILURE);
    }
    new_client->data = arg->client.data;
    add_client(&head, new_client);
    write_in_database(arg->client.data);
    memset(buffer, 0, sizeof(buffer));

    usleep(5000);
}

void add_client(Client** head, Client* new_client) {
    pthread_mutex_lock(&mutex);
    new_client->next = NULL;
    if (*head == NULL) {
        *head = new_client;
    } else {
        Client* tmp = *head;
        while (tmp->next != NULL) {
            tmp = tmp->next;
        }
        tmp->next = new_client;
    }
    pthread_mutex_unlock(&mutex);
}

void write_in_database(Client_info client) {
    pthread_mutex_lock(&mutex);
    size_t elements_written = fwrite(&client, sizeof(Client_info), 1, database);
    if (elements_written != 1) {
        perror("Error writing to file");
    }
    fflush(database);
    pthread_mutex_unlock(&mutex);
}

int check_correctness(Client_info info) {
    pthread_mutex_lock(&mutex);
    char buffer[BUFFER_SIZE];
    Client_info data;
    fseek(database, 0, SEEK_SET);
    while (fread(&data, sizeof(data), 1, database) > 0) {
        if (strcmp(info.login, data.login) == 0 && strcmp(info.password, data.password) == 0) {
            pthread_mutex_unlock(&mutex);
            return 1;
        }
    }
    pthread_mutex_unlock(&mutex);
    return 0;
}

void sign_in_after_register(ThreadArg* arg) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    snprintf(buffer, BUFFER_SIZE, "Now log in to your account!");
    send(arg->client_fd, buffer, BUFFER_SIZE, 0);
    login_user(arg);
    send_messages(arg);
}

void* thread_for_server_mod(void*) {
    char buffer[BUFFER_SIZE];
    printf("Welcome to Airplane System\n");
    snprintf(buffer, BUFFER_SIZE, "\n1 - Print flights\n2 - Add flight\n3 - Remove flight\n4 - Exit");
    puts(buffer);
    int opt = 0;
    while (1) {
        printf("Enter your choice: ");
        scanf("%d", &opt);
        switch (opt) {
            case 1:
                print_flights(flight_head);
                break;
            case 2:
                FlightNode* new_flight = (FlightNode*)malloc(sizeof(FlightNode));
                new_flight = create_flight();
                add_flight(&flight_head, new_flight);
                write_in_flight_database(&new_flight->flight);
                break;
            case 3:
                remove_flight(&flight_head);
                printf("Remove flight");
                break;
            case 4:
                fclose(flight_database);
                close(sockfd);
                close(client_fd);
                exit(0);
            default:
                printf("Invalid Input");
        }
    }
    printf("\n");
}

FlightNode* create_flight() {
    FlightNode* new_flight = (FlightNode*)malloc(sizeof(FlightNode));
    printf("Enter flight ID: ");
    scanf("%d", &new_flight->flight.flight_id);
    printf("Enter the source: ");
    scanf("%s", new_flight->flight.source);
    printf("Enter the destination: ");
    scanf("%s", new_flight->flight.destination);
    printf("Enter the number of available seats: ");
    scanf("%d", &new_flight->flight.available_seats);
    new_flight->next = NULL;
    return new_flight;
}

void add_flight(FlightNode** flight_head, FlightNode* new_flight) {
    pthread_mutex_lock(&mutex);
    if (*flight_head == NULL) {
        *flight_head = new_flight;
    } else {
        FlightNode* temp = *flight_head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_flight;
    }
    pthread_mutex_unlock(&mutex);
}

void print_flights(FlightNode* head) {
    pthread_mutex_lock(&mutex);
    FlightNode* temp = head;
    printf("\n-List of Flights\n");
    while (temp != NULL) {
        printf("\nFlight ID: %d\nSource: %s\nDestination: %s\nSeats: %d\n", temp->flight.flight_id, temp->flight.source, temp->flight.destination, temp->flight.available_seats);
        temp = temp->next;
    }
    printf("\n");
    int fd = fileno(flight_database);
    if(fd == -1)
    {
        perror("fileno is failed");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_unlock(&mutex);
}

void remove_flight(FlightNode** flight_head) {
    FlightNode* temp = *flight_head;
    FlightNode* prev = *flight_head;
    int id;
    printf("Enter flight ID to delete: ");
    scanf("%d", &id);
    if (temp != NULL && temp->flight.flight_id == id) {
        *flight_head = temp->next;
        free(temp);
        return;
    }
    while (temp != NULL && temp->flight.flight_id != id) {
        prev = temp;
        temp = temp->next;
    }
    if (temp == NULL) return;
    prev->next = temp->next;
    free(temp);
}

void send_messages(ThreadArg* arg) {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "1 - Book flight\n2 - Check flight availability\n3 - Print all flights");
    send(arg->client_fd, buffer, BUFFER_SIZE, 0);
    int recv_bytes = recv(arg->client_fd, buffer, BUFFER_SIZE, 0);
    int opt = atoi(buffer);

    while(1) {
        snprintf(buffer, BUFFER_SIZE, "1 - Book flight\n2 - Check flight availability\n3 - Print all flights");
        send(arg->client_fd, buffer, BUFFER_SIZE, 0);
        int recv_bytes = recv(arg->client_fd, buffer, BUFFER_SIZE, 0);
        int opt = atoi(buffer);
        switch (opt) {
            case 1:
                book_flight(arg);
                break;
            case 2:
                check_flight_availability(arg);
                break;
            case 3:
                send_flight(arg);
                break;
            default:
                snprintf(buffer, BUFFER_SIZE, "Invalid input\n");
                send(arg->client_fd, buffer, BUFFER_SIZE, 0);
                break;
        }
    }
}

void send_flight(ThreadArg* arg) {
    pthread_mutex_lock(&mutex);
    FlightNode* temp = flight_head;
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "-List of Flights\n");
    send(arg->client_fd, buffer, BUFFER_SIZE, 0);
    memset(buffer, 0, BUFFER_SIZE);
    while (temp != NULL) {
        snprintf(buffer, BUFFER_SIZE, "Flight ID: %d\nSource: %s\nDestination: %s\nSeats: %d\n", temp->flight.flight_id, temp->flight.source, temp->flight.destination, temp->flight.available_seats);
        send(arg->client_fd, buffer, BUFFER_SIZE, 0);
        memset(buffer, 0, BUFFER_SIZE);
        temp = temp->next;
    }
    pthread_mutex_unlock(&mutex);
}

void book_flight(ThreadArg* arg) {
    FlightNode* temp = flight_head;
    int id;
    int seats_to_book;
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "Enter flight ID: ");
    send(arg->client_fd, buffer, BUFFER_SIZE, 0);
    recv(arg->client_fd, buffer, BUFFER_SIZE, 0);
    id = atoi(buffer);
    while (temp != NULL && temp->flight.flight_id != id) {
        temp = temp->next;
    }
    if (temp != NULL) {
        snprintf(buffer, BUFFER_SIZE, "Enter the number of seats to book: ");
        send(arg->client_fd, buffer, BUFFER_SIZE, 0);
        recv(arg->client_fd, buffer, BUFFER_SIZE, 0);
        seats_to_book = atoi(buffer);
        if (seats_to_book <= temp->flight.available_seats) {
            temp->flight.available_seats -= seats_to_book;
            snprintf(buffer, BUFFER_SIZE, "Booking successful! Remaining seats: %d\n", temp->flight.available_seats);
            send(arg->client_fd, buffer, BUFFER_SIZE, 0);
        } else {
            snprintf(buffer, BUFFER_SIZE, "Not enough seats available\n");
            send(arg->client_fd, buffer, BUFFER_SIZE, 0);
        }
    } else {
        snprintf(buffer, BUFFER_SIZE, "Flight not found\n");
        send(arg->client_fd, buffer, BUFFER_SIZE, 0);
    }
}

void check_flight_availability(ThreadArg* arg) {
    FlightNode* temp = flight_head;
    int id;
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "Enter flight ID: ");
    send(arg->client_fd, buffer, BUFFER_SIZE, 0);
    recv(arg->client_fd, buffer, BUFFER_SIZE, 0);
    id = atoi(buffer);
    while (temp != NULL && temp->flight.flight_id != id) {
        temp = temp->next;
    }
    if (temp != NULL) {
        snprintf(buffer, BUFFER_SIZE, "Available seats: %d\n", temp->flight.available_seats);
        send(arg->client_fd, buffer, BUFFER_SIZE, 0);
    } else {
        snprintf(buffer, BUFFER_SIZE, "Flight not found\n");
        send(arg->client_fd, buffer, BUFFER_SIZE, 0);
    }
}

void try_login(ThreadArg* arg) {
    login_user(arg);
}

void write_in_flight_database(Flight* new_flight)
{
    int fd = fileno(flight_database);
    if(fd == -1)
    {
        perror("fileno is failed");
        exit(EXIT_FAILURE);
    }

    struct iovec iov;
    iov.iov_base = (void*)new_flight;
    iov.iov_len = sizeof(Flight);
    int write_bytes = writev(fd, &iov, 1);
    if(write_bytes == -1)
    {
        perror("writev is failed");
        close(fd);
        exit(EXIT_FAILURE);
    }
    
}

int main() {
    struct sockaddr_in server_struct, client_struct;
    database = fopen("database.txt", "r+");
    flight_database = fopen("flight_database.txt", "r+");
    if (!database || !flight_database) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_struct.sin_family = AF_INET;
    server_struct.sin_addr.s_addr = INADDR_ANY;
    server_struct.sin_port = htons(PORT);

    int reuseaddr = 0;
    if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&reuseaddr,sizeof(reuseaddr)) == -1) {
        perror("reuseaddr");
        exit(1);
    }

    int reuseport = 0;
    if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEPORT,&reuseport,sizeof(reuseport)) == -1) {
        perror("reuseport");
    }

    if (bind(sockfd, (struct sockaddr*)&server_struct, sizeof(server_struct)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, MAX_USERS) < 0) {
        perror("listen failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    pthread_t server;
    pthread_create(&server, NULL, thread_for_server_mod, NULL);
    pthread_detach(server);

    printf("Server is listening on port %d\n", PORT);

    pthread_t thread_id;
    while (1) {
        socklen_t addrlen = sizeof(client_struct);
        client_fd = accept(sockfd, (struct sockaddr*)&client_struct, &addrlen);
        if (client_fd < 0) {
            perror("accept failed");
            continue;
        }

        ThreadArg* thread_arg = (ThreadArg*)malloc(sizeof(ThreadArg));
        thread_arg->client_fd = client_fd;
        thread_arg->client_struct = client_struct;

        pthread_create(&thread_id, NULL, thread_func, (void*)thread_arg);
        pthread_detach(thread_id);
    }

    return 0;
}

