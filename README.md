# VoteMe

A minimal, portable C program to manage voters/candidates and run a simple voting process.

What’s included (runtime only):

- `src/` — program sources (admin menu, voter menu, voting interface, data handling)
- `data/` — CSV files the program reads/writes
- `Makefile` — Linux/macOS build
- `build.bat` — Windows build without Make

We intentionally removed non-runtime files from this branch to keep it lean and easy to run.

## Prerequisites

- Linux/macOS: GCC and Make
- Windows (choose one):
  - MSYS2 + MinGW-w64 (GCC) — recommended
  - LLVM Clang for Windows
  - Visual Studio (Developer Command Prompt for MSVC)

## Linux/macOS: build and run
# Special Note
If make file is not install run this command


```
sudo apt install make

```

1. Build

```
make voteme
```

This produces `bin/voteme` (main menu) and may create `bin/` and `obj/` automatically.

2. Run

```
./bin/voteme
```

Data is read/written under `data/`. If some files are missing, the program will create minimal headers.

Other useful builds:

```
make admin           # admin console
make voter-tools     # voter_register and candidate_register tools
make vote            # standalone voting CLI
```

## Windows: build and run (no Makefile)

1. Open a terminal in the repo root and run:

```
build.bat
```

The script auto-detects: gcc → clang → cl. It creates `bin\` and `data\` if missing, then builds:

- `bin\voteme.exe` (main menu)
- `bin\admin.exe` (admin console)
- `bin\voter_register.exe` (voter CLI)
- `bin\candidate_register.exe` (candidate CLI)
- `bin\vote.exe` (direct voting CLI)

2. Run

```
bin\voteme.exe
```

Tip: If using MSVC, launch the “Developer Command Prompt for VS” so `cl` is on PATH. For GCC, MSYS2 + MinGW-w64 is the easiest setup (https://www.msys2.org/).

## Directory layout

- `src/` — C sources and headers
- `data/` — CSV data files used by the program
- `bin/` — created by builds (not committed)
- `obj/` — created by builds (not committed)
- `.gitignore` — prevents build artifacts from being committed

## Data files (schemas)

- `data/approved_voters.txt`: `voting_number,name,nic,district_id`
- `data/approved_candidates.txt`: `candidate_number,name,party_id,district_id,nic`
- `data/party_name.txt`: `party_id,party_name`
- `data/district.txt`: `district_id,district_name`
- `data/parliament_candidates.txt`: `candidate_number,party_id`
- `data/votes.txt`: `voter_id,candidate_id`
- `data/voter_count.txt`: `voting_number,candidate_number,party_id,district_id,count`
- `data/system_config.txt`: `key=value` lines used by the admin console

If a file is missing, the program will create a minimal header where necessary.

## Troubleshooting

- If `make` is missing on Linux, install build tools (e.g., Debian/Ubuntu: `sudo apt-get install -y build-essential`).
- If Windows build can’t find a compiler, install MSYS2 (GCC) or use the VS Developer Command Prompt.
- Don’t commit `bin/` or `obj/`; they’re ignored by `.gitignore`.
