# TDS Setup and Installation Guide

This guide explains how to install, build, and run the Taylor Decomposition System (TDS) in a reproducible environment. It reflects the recommended setup used in the original TDS documentation and internal usage.

---

## Table of Contents

1. Overview
2. Recommended VirtualBox + Linux Mint Setup
3. Native Linux Setup
4. Building TDS from Source
5. Running TDS
6. Validation Tests
7. Troubleshooting

---

## 1. Overview

TDS is designed primarily for Linux. The most stable installation method uses:

- VirtualBox as the host environment
- Linux Mint inside the VM
- Build dependencies installed via APT

This guarantees compatibility with the original source code and ensures consistent behavior across systems.

---

## 2. Recommended VirtualBox + Linux Mint Setup

### Step 1: Install VirtualBox

Download and install VirtualBox from the official website.

Optional (recommended): Install VirtualBox Extension Pack.

---

### Step 2: Create a Linux Mint VM

1. Download Linux Mint ISO.
2. Create a new VM:
   - Type: Linux
   - Version: Ubuntu (64-bit)
3. Attach the ISO and install Linux Mint.
4. Make note of your username and password.

---

### Step 3: Install Guest Additions

In the VM window:

These packages are required by TDS for:

Building the executable

Rendering DOT graphs

Editing scripts

Step 5: Create a Working Directory
```bash
mkdir ~/tds_source
cd ~/tds_source
```

Drag-and-drop the tds-linux.tar file into this folder and extract:

```bash
tar -xf tds-linux.tar
```
Step 6: Build TDS

Navigate into the unpacked TDS directory:

```bash
make clean
make
```

Successful compilation will produce the tds executable.

3. Native Linux Installation (No VM)

If you prefer not to use VirtualBox, install dependencies:

```bash
sudo apt update
sudo apt install build-essential libreadline-dev libncurses-dev graphviz evince
```

Then follow the same source extraction and build steps:

```bash
tar -xf tds-linux.tar
cd <tds-folder>
make
```

4. Running TDS

Run the tool:

```bash
./tds
```

You should see:

```bash
Tds 01>
```

Basic navigation:

help       Show all commands
man <cmd>  Show command manual
exit       Quit

5. Validation Tests

Use these examples to confirm your installation.

Example A: Polynomial Input
```bash
Tds 01> vars c b a
Tds 02> poly F = a*b + a*c
Tds 03> show --ted
Tds 04> ted2dfg -n
Tds 05> show --dfg
```

Example B: Exponent Handling and Linearization

```bash
Tds 01> poly F = 5*a^3 + 2*b^2
Tds 02> linearize
Tds 03> show
```

Example C: Factorization Exploration
```bash
Tds 01> vars a b c d
Tds 02> poly F = a*b + a*c + d*b + d*c
Tds 03> ted2dfg -f
Tds 04> show --dfg
```

6. Troubleshooting

Issue: Digits not typing correctly inside TDS shell

Certain VirtualBox keyboard layouts send incorrect scancodes.

Fix:
```bash
Settings → Keyboard → Layout → English (US)
```

Issue: Drag-and-drop not working

Install Guest Additions and enable bidirectional clipboard:
```bash
Devices → Drag and Drop → Bidirectional
Devices → Shared Clipboard → Bidirectional
```
Issue: Graphs not opening

Install graphviz and a PDF viewer:

```bash
sudo apt install graphviz evince
```

Known Issues: TDS has a known bug with recent versions of Ubuntu, wherein a pointer resolution has to be looked at. Hence for this version of the tool, Ubuntu 16 seems to be working fine for all circuit purposes.