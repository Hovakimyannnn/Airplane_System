# Airplane System

## Overview
Design and implement an airplane management system that uses a client-server model. The server will manage airplane operations such as flight booking, flight availability, passenger information, etc. Clients will connect to the server to perform these operations.

## Server Requirements

### Server Setup:
- Create a TCP/IP server that listens for incoming client connections.
- Use socket programming to handle connections.
- Implement multi-threading to handle multiple clients concurrently.

### Airplane Management Operations:
- Maintain information about available flights, booked flights, and passenger details.
- Implement functions to add, modify, and delete flight information.
- Implement functions to book flights, check flight availability, and cancel bookings.
- Handle passenger information such as name, contact details, and flight preferences.

### User Authentication:
- Implement a **user registration** system where new users can create accounts with usernames and passwords.
- Implement a **user login** system where existing users must log in before performing any operations.
- Ensure only authenticated users can access airplane management operations (e.g., booking flights, viewing passenger details).

### Thread Safety:
- Use synchronization mechanisms (e.g., mutex locks) to ensure thread safety when accessing shared data structures (e.g., flight information, passenger details).

## Client Requirements

### Client Interface:
- Develop a client interface that allows users to connect to the server.
- Provide options to **register** a new account and **log in** to an existing account.
- Once logged in, users can book flights, check availability, view passenger details, etc.
- Use socket programming (`socket`, `connect`, `send`, `recv`) to communicate with the server.

### User Interaction:
- **User Registration**: Handle user input for creating new accounts (e.g., entering username, password, and contact details).
- **User Login**: Handle user input for logging in (e.g., entering username and password).
- Once authenticated, allow users to perform operations like booking flights, checking availability, etc.
- Display appropriate messages and responses received from the server.

## Additional Features (Optional)

### Ticketing and Boarding Passes:
- Implement functionality for issuing tickets and generating boarding passes.
- Include options for printing or sending digital copies to passengers.

### Admin Panel:
- Create an admin interface on the server side to manage flight schedules, ticket prices, etc.
- Secure admin access with authentication if required.

## Example Scenario
Imagine a scenario where multiple clients connect to the airplane management server:

1. **User Registration**: A new user creates an account by providing a username, password, and contact details.
2. **User Login**: A registered user logs in to the system.
3. Client X wants to book a flight from City A to City B on a specific date.
4. Client Y wants to check flight availability for a particular route.
5. Client Z wants to view passenger details for a booked flight.

The server manages these requests concurrently, updates flight availability, stores passenger information, and responds to each client appropriately.


## Getting Started

To get started with the implementations in this repository:

1. **Clone the repository**: `git clone https://github.com/Hovakimyannnn/Airplane_System.git`
2. **Navigate to the directory**: `cd Airplane_System`
3. **Compile the code**: `cd server` ` gcc server.c -o server` and `cd client` `gcc client.c -o client`
4. **Run the code**: The first run server code `./server` then ran client code `./client`
5. **Experiment**: Modify the code, add your own implementations, and test different scenarios.