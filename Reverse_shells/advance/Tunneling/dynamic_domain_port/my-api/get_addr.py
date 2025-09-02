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

            # Prepare JSON data
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
