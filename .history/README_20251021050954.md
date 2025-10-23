# VoteMe (Minimal)

This repository contains a simplified core of the VoteMe system, keeping only:
- Enhanced data handling (CSV-backed CRUD)
- Admin console (data admin UI/logic)
- Voting algorithm (process votes and write results)

Everything else (legacy UIs, demos, extra docs) has been removed.

## Build

Requirements: GCC and Make

```
make
```

This builds the admin console at `bin/admin`.

## Run

```
./bin/admin
```

Use the admin menu to manage data and run the voting algorithm. Data is stored under `data/`.

## Tests

Two small tests are provided:
- Voting algorithm basic scenario
- Data handler smoke test

Build and run tests:

```
make test
```

## Data files (schemas)

- `data/approved_voters.txt`: voting_number,name,nic,district_id
- `data/approved_candidates.txt`: candidate_number,name,party_id,district_id,nic
- `data/votes.txt`: voter_id,candidate_id
- `data/party_name.txt`: party_id,party_name
- `data/district.txt`: district_id,district_name
- `data/parliament_candidates.txt`: candidate_number,party_id
- `data/voter_count.txt`: voting_number,candidate_number,party_id,district_id,count
- `data/system_config.txt`: key=value lines used by the admin console

If a file is missing, the admin console or tests will create minimal headers where necessary.
