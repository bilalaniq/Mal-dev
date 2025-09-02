
# 🌍 Connecting Two Nodes Over the Internet 

When two computers are on the same LAN, direct communication is easy.
But across the internet, **NAT, firewalls, and dynamic IPs** make it much harder.
Below are the main theoretical methods to connect nodes across networks.

---

## 🔹 1. **Port Forwarding**

* The router is configured to forward a specific public port to a private machine inside the LAN.
* Anyone who connects to the public IP and port gets routed to the internal machine.

✅ Simple and direct
❌ Requires router access (not always possible, e.g., shared Wi-Fi or ISP restrictions)


click [here](./Port_Forwarding/) to learn


---

## 🔹 2. **Tunneling Services**

A third-party server relays connections between machines.

Examples:

* **ngrok**
* **LocalTunnel**
* **Cloudflare Tunnel**

How it works:

1. Target machine connects out to the tunnel server (allowed by firewall).
2. Attacker connects to the tunnel server.
3. Tunnel forwards traffic both ways.

✅ Works without router access
❌ Relies on a third-party service

click [here](./Tunneling/) to learn


---

## 🔹 3. **VPN (Virtual Private Network)**

* Both machines join the same **virtual network** via a VPN provider (or self-hosted).
* Once inside, they can communicate as if they were on the same LAN.

Examples:

* **Tailscale** (built on WireGuard)
* **ZeroTier**
* Classic VPN servers (OpenVPN, WireGuard, IPSec)

✅ Secure, flexible, persistent
❌ Requires setup and sometimes a central coordination server

click [here](./VPN/) to learn


---

## 🔹 4. **Direct P2P with Hole Punching**

* Uses techniques like **UDP hole punching** to establish direct connections even behind NAT.
* Often used in P2P applications (e.g., VoIP, torrents).

✅ No middleman for data flow
❌ Requires special protocols and sometimes a “rendezvous server” to start


click [here](./Direct_P2P/) to learn


---



## 🔹 5. **SSH Tunneling**

* Uses an SSH server as a secure tunnel.
* You can forward ports or create dynamic SOCKS proxies over SSH.

✅ Secure and encrypted
❌ Requires SSH server access


click [here](./SSH/) to learn


---

## 🔹 6. **Relay Servers / Proxies**

* A central **relay server** acts as a hub.
* Both nodes connect to the relay, which forwards traffic between them.

✅ Simple and reliable
❌ All traffic flows through the relay (possible bottleneck)


click [here](./Relay_Servers_Proxies/) to learn


---

# ✅ Summary

Connecting two nodes over the internet can be done through multiple approaches:

* **Port Forwarding** → direct, requires router control
* **Tunneling Services** → quick, uses third-party servers
* **VPNs** → puts both nodes in the same virtual LAN
* **Hole Punching** → P2P trick to bypass NAT
* **SSH Tunneling** → encrypted forwarding via SSH
* **Relay Servers** → central hub relays data

The choice depends on:

* Do you control the router?
* Do you want persistence?
* Do you need encryption/security?
* Do you want to avoid third-party services?

---