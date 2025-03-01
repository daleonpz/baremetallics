---
layout: post
title: Secure Boot for Embedded Devices - An Example
category: programming
description: Learn how to implement secure boot for embedded devices using an example. Secure boot ensures that only authorized firmware is executed on the device, protecting it from unauthorized modifications.
---

Secure boot is a critical security feature for embedded devices. It ensures that only authorized firmware runs on a device, protecting against unauthorized modifications or attacks. In this post, I'll explain how secure boot works using a simple example.

## Chain of Trust

Secure boot relies on the **chain of trust**—a sequence of steps where each step verifies the integrity of the next before executing it.

For example, in a boot sequence `A -> B -> C`, secure boot ensures that `A` verifies `B`, and `B` verifies `C`. If any step fails, the boot process is halted to prevent running untrusted code.

### Questions you may have:

- Why not just verify the firmware once? Because firmware can be updated, replaced, or tampered with.
- Why multiple steps? Each stage has different privileges and responsibilities. Also you may update an application without modifying the OS or bootloader.

## Quick Security Concepts

To understand secure boot, you need to be familiar with a few key concepts:

- **Public/Private Key**: A cryptographic key pair where the private key is secret, and the public key is shared.
- **Encryption/Decryption**: Data encoded with a public key can only be decrypted by the corresponding private key (and vice versa).
- **Hash**: A function that generates a unique fixed-size output for an input. Even a small change in input drastically alters the hash.
- **Digital Signature**: A hash encrypted with a private key.
- **Signing/Verification**: The process of signing data with a private key and verifying it with a public key.

## Secure Boot Process

Secure boot typically involves these stages:

1. **Boot ROM** (Immutable, in hardware): Loads the first-stage bootloader.
2. **First-Stage Bootloader (FSBL)**: A simple, secure bootloader that verifies and loads the second-stage bootloader.
3. **Second-Stage Bootloader**: A more complex bootloader responsible for loading the OS and handling updates.
4. **OS Kernel**: The core of the operating system.
5. **Application**: The final software running on the device.

![secure boot](/images/posts/secure_boot.png)

### Why multiple bootloaders?

The second-stage bootloader is complex (handling tasks like OS loading and OTA updates) and may have vulnerabilities. The first-stage bootloader is kept simple, secure, and immutable, ensuring a reliable anchor in the chain of trust.

## Example: Secure Boot Concept

For simplicity, let's consider a setup with only an **FSBL and an Application**.

### Step 1: Generate Keys

We generate two key pairs—one for the FSBL and one for the Application.

```bash
openssl ecparam -genkey -name prime256v1 -noout -out root_private.pem
openssl ec -in root_private.pem -pubout -out root_public.pem
openssl ecparam -genkey -name prime256v1 -noout -out second_private.pem
openssl ec -in second_private.pem -pubout -out second_public.pem
```

### Step 2: Store FSBL Public Key Hash

```bash
openssl dgst -sha256 -binary root_public.pem | openssl base64 > OTP.txt
```

### Step 3: Sign Bootloader

```bash
echo "bootloader code" > bootloader.txt
openssl dgst -sha256 -sign root_private.pem -out bootloader.sig bootloader.txt
```

### Step 4: Sign Application

```bash
echo "application code" > application.txt
openssl dgst -sha256 -sign second_private.pem -out application.sig application.txt
```

## Simulating Secure Boot Verification

The following script (`secure_boot.sh`) checks each step before executing it:

```bash
#!/bin/bash

set -e  # Exit on error (remove this if you want to continue on failure)

# File paths
ROOT_HASH_FILE="OTP.txt"
BOOTLOADER_FILE="bootloader.txt"
BOOTLOADER_SIG="bootloader.sig"
APPLICATION_FILE="application.txt"
APPLICATION_SIG="application.sig"
ROOT_PUBLIC_KEY="root_public.pem"
SECOND_PUBLIC_KEY="second_public.pem"

echo "[*] Starting Secure Boot Verification"

# Step 1: Verify Root Public Key Hash
echo "[*] Checking root public key against OTP memory"
computed_hash=$(openssl dgst -sha256 -binary "$ROOT_PUBLIC_KEY" | openssl base64)
stored_hash=$(cat "$ROOT_HASH_FILE")

if [ "$computed_hash" != "$stored_hash" ]; then
    echo "FAILED on root public key verification"
    exit 1
fi
echo "[+] Root public key is valid"

# Step 2: Verify Bootloader Signature
echo "[*] Verifying bootloader signature"
if ! openssl dgst -sha256 -verify "$ROOT_PUBLIC_KEY" -signature "$BOOTLOADER_SIG" "$BOOTLOADER_FILE"; then
    echo "FAILED on bootloader signature verification"
    exit 1
fi
echo "[+] Bootloader signature is valid"

# Step 4: Verify Application Signature
echo "[*] Verifying application signature"
if ! openssl dgst -sha256 -verify "$SECOND_PUBLIC_KEY" -signature "$APPLICATION_SIG" "$APPLICATION_FILE"; then
    echo "FAILED on application signature verification"
    exit 1
fi
echo "[+] Application signature is valid"

echo "[SUCCESS] Secure boot verification completed"
```

## Running the Simulation

Run the setup and secure boot scripts:

```bash
bash setup.sh
bash secure_boot.sh
```

Expected output:

```bash
[*] Starting Secure Boot Verification
[*] Checking root public key against OTP memory
[+] Root public key is valid
[*] Verifying bootloader signature
Verified OK
[+] Bootloader signature is valid
[*] Verifying application signature
Verified OK
[+] Application signature is valid
[SUCCESS] Secure boot verification completed
```

### Testing Failure Cases

If you modify `root_public.pem`, the root public key verification fails:

```bash
FAILED on root public key verification
```

If you modify `bootloader.txt` with unauthorized content, the bootloader verification fails:

```bash
FAILED on bootloader signature verification
```

If you modify `application.txt`, the application verification fails:

```bash
FAILED on application signature verification
```

## Conclusion

Secure boot ensures that only trusted firmware runs on a device. This post covered the **chain of trust**, **key security concepts**, and a **working example** of a simple secure boot implementation.

Understanding and implementing secure boot is critical for securing embedded devices against tampering and unauthorized code execution. Hopefully, this post made the concept clearer and easier to grasp.


