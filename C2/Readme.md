

# 🔹 What is a C2 (Command and Control) Server?

A **Command and Control server (C2 or C\&C)** is a system that attackers use to remotely **coordinate, control, and communicate** with malware or compromised devices.

It provides the attacker with a way to **send instructions** and **receive data** back from the infected hosts.

Think of malware as a **drone** and the C2 as the **remote controller**.

---

# 🔹 Why Malware Uses C2

1. **Remote control** – Malware doesn’t need to contain all its logic; it just connects back and asks for orders.
2. **Adaptability** – Attackers can change the malware’s behavior after infection.
3. **Centralized management** – One C2 can control thousands of bots.
4. **Persistence** – If defenders detect one malware sample, the C2 can still issue new commands to others.

---

# 🔹 Typical C2 Communication Flow

1. **Infection:** The victim runs the malicious file (or gets compromised by an exploit).
2. **Beaconing:** Malware contacts the C2 (“I’m here, here’s my system info”).
3. **Tasking:** C2 responds with commands (e.g., run a process, collect files, scan a network).
4. **Execution:** Malware performs the command.
5. **Reporting:** Malware sends results back (success/failure/data).
6. **Repeat:** Communication continues on intervals or triggers.

---

# 🔹 Features C2 Servers Provide

Depending on the attacker’s goal, a C2 can let them:

* 📡 **Exfiltrate data** → Steal files, keystrokes, screenshots.
* 🛠 **Execute commands** → Run shell commands or scripts remotely.
* 🔄 **Update malware** → Push new payloads or patches.
* 👥 **Manage bots** → Coordinate botnets for DDoS, spam, or crypto-mining.
* 🖥 **Remote access** → Enable persistence like RDP, reverse shells, or VNC.
* 🎭 **Proxy/Relay traffic** → Use victims as stepping stones to hide the attacker.

---

