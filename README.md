# 💬 TCP Chat Room – C Project

## 🚀 Overview

The **TCP Chat Room** is a multi-client chat application developed in **C using socket programming and multithreading (pthreads)**.

This project enables multiple users to connect to a server, authenticate themselves (Login/Signup), and communicate in real-time through private and group chats.

The server handles concurrent clients using threads and maintains active users using a linked list.

---

## 🎯 Features

### 🔐 Authentication

* User Login system
* User Signup system
* Stores user credentials in file (`user_info.txt`)
* Validates username and password

---

### 💬 Chat System

#### 🔹 Single Chat (Private Messaging)

* Send messages to a specific user
* Uses username-based routing

#### 🔹 Group Chat

* Broadcast message to all connected users
* Excludes sender

---

### 🟢 Presence System

* Notify when a user comes online
* Notify when a user goes offline
* Send list of online users to newly connected client

---

### ⚡ Real-Time Communication

* Multi-client support using threads
* Instant message delivery
* Concurrent handling using `pthread`

---

### 🛑 Robust Handling

* Detect client disconnection automatically (`recv() == 0`)
* Handles server crash notification
* Proper cleanup of client resources

---

## 🧠 Concepts Used

* TCP/IP Socket Programming
* Multithreading using pthreads
* Client-Server Architecture
* Mutex Synchronization
* Linked List Data Structure
* File Handling in C
* Signal Handling
* Inter-process Communication

---

## ⚙️ Technologies Used

* Language: C
* Platform: Linux
* Compiler: GCC
* Libraries: pthread, socket APIs

---

## 🏗️ Project Structure

```
TCP_Chat_Room/
│── server.c
│── client.c
│── header.h
│── user_info.txt
│── README.md
```

---

## 🔧 How to Compile

### Compile Server
```
gcc server.c -o server -lpthread
```

### Compile Client
```
gcc client.c -o client -lpthread
```

---

## ▶️ How to Run

### Step 1: Start Server
```
./server
```

### Step 2: Start Clients (in separate terminals)
```
./client
```

---

## 💡 Usage Flow

1. Start the server
2. Run multiple clients
3. Choose:
   - Login
   - Signup
4. After successful login:
   - Single Chat
   - Group Chat
   - Logout
5. Start messaging

---

## 📡 Message Structure

```c
typedef struct msg {
    char username[25];
    char message[256];
} msg;
```

---

## 🔄 Server Workflow

```
Start Server
   ↓
Listen for connections
   ↓
Accept client
   ↓
Create thread
   ↓
Authenticate user
   ↓
Add client to list
   ↓
Send online users
   ↓
Notify others (online)
   ↓
Chat loop
   ↓
Detect disconnect
   ↓
Remove client
   ↓
Notify others (offline)
```

---

## 🔐 Synchronization

* my_lock → protects file operations
* list_lock → protects client list

---

## 🧪 Sample Output

```
SERVER : user1 is online
user2 : Hello!
SERVER : user2 is offline
```

---

## ⚠️ Limitations

* CLI-based (no GUI)
* No encryption
* No chat history
* No duplicate login prevention

---

## 🔮 Future Enhancements

* GUI-based chat
* Message encryption
* Chat history storage
* Online users command
* Duplicate login restriction
* Heartbeat mechanism
* Auto reconnect

---

## 🧩 Challenges Faced

* Handling multiple clients safely
* Synchronization using mutex
* Designing message routing
* Handling server crash
* Managing real-time updates

---

## 📚 Learning Outcomes

* Strong understanding of socket programming
* Hands-on multithreading experience
* Real-world client-server design
* Debugging concurrent systems

---

## 📌 Author

Shubham Chaudhari

---

## 📜 License

This project is for educational purposes.
