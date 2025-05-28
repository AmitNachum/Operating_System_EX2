**README - Operating Systems Assignment 2: Molecule Counter**

**Students:** Amit Nachum 
**Email:** [nachum.amit@msmail.ariel.ac.il](mailto:nachum.amit@msmail.ariel.ac.il)

---

## Overview

This project is based on **Assignment 2** in Operating Systems at Ariel University, Semester B 2025. It focuses on building a server and client ecosystem to manage atomic inventory and molecule synthesis via both **TCP/UDP** and **Unix Domain Sockets**. It incrementally adds features across six structured stages, culminating in full coverage testing and modular, recursive `make` compilation.

---

## Features by Stages

### Stage 1: Atom Warehouse (15 pts)

* A TCP server (`atom_warehouse`) listens for connections.
* Accepts and parses commands:

  * `ADD CARBON <amount>`
  * `ADD OXYGEN <amount>`
  * `ADD HYDROGEN <amount>`
* Uses `select()` to support multiple clients.

### Stage 2: Molecule Supplier (15 pts)

* Adds UDP socket support for delivering molecules:

  * `DELIVER WATER <amount>`
  * `DELIVER CARBON DIOXIDE <amount>`
  * `DELIVER ALCOHOL <amount>`
  * `DELIVER GLUCOSE <amount>`
* Validates atom availability and sends back delivery confirmation or error.

### Stage 3: Console Mode (15 pts)

* Adds keyboard support for commands such as:

  * `GEN SOFT DRINK`
  * `GEN VODKA`
  * `GEN CHAMPAGNE`
* Calculates how many drinks can be synthesized from current atoms.

### Stage 4: Command Line Options (20 pts)

* Adds options for startup values and timeout:

  * `-o <oxygen>`
  * `-c <carbon>`
  * `-h <hydrogen>`
  * `-t <timeout>`
  * `-T <tcp-port>` (required)
  * `-U <udp-port>` (required)

### Stage 5: Unix Domain Sockets (15 pts)

* Adds UDS stream and datagram socket support:

  * `-s <stream-path>`
  * `-d <datagram-path>`
* Also supports clients that connect with `-f <UDS path>`

### Stage 6: File Persistence (20 pts)

* Adds file support:

  * `-f <file-path>` for loading/saving inventory.
* Automatically updates the file after every successful delivery.
* Server exits cleanly after timeout.

---

## Compilation Instructions

Each question (Q\_1 through Q\_6) resides in a separate folder.
Every folder has its own dedicated `makefile`.
To compile everything:

```bash
make -C Q_1
make -C Q_2
make -C Q_3
make -C Q_4
make -C Q_5
make -C Q_6
```

Or use the recursive master Makefile:

```bash
make all
```

To clean everything:

```bash
make clean
```

---

## Notes

* Ensure that all port/socket paths are not in conflict before launching.
* Use `make coverage` in each question directory to generate `.gcov` files.
* `select()` handles concurrent socket communication.
* UNIX Domain support is added in Q\_5.
* File loading/saving support is included in Q\_6.

---

✅ Developed with C, POSIX sockets, `select()`, `getopt_long()`, and `gcov`.
