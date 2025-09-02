

# Reverse Shell 


A **reverse shell** is a technique where a target machine initiates a connection **back to an attacker's system**, giving the attacker remote command-line access.

* In a **bind shell**, the target listens for incoming connections.
* In a **reverse shell**, the target *calls out* to the attacker.

  * This bypasses firewalls that block inbound connections but allow outbound traffic.

---

## How It Works

1. **Attacker** sets up a listener (waiting for incoming connections).
2. **Target** runs a program/script that opens a network socket and connects to the attacker.
3. Once connected:

   * The target redirects its **stdin, stdout, stderr** to the socket.
   * The attacker can now send commands and receive output — just like a normal shell.

---

## 🖥️ Example Flow

```
[ Attacker ] <--- listening on port 4444
      ^
      |
   (TCP connection)
      |
      v
[ Target ] ---> runs reverse shell program
```

---


To learn basic click [here](./basic/)


In basics we have learned to connect two nodes within the same network to connect two nodes from different nodes click [here](./advance/)