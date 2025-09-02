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
