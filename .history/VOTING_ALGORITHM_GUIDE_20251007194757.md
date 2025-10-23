# VoteMe Voting Algorithm Usage Guide

## Overview

The VoteMe voting algorithm processes votes according to admin-defined parameters and selects parliament members based on:

1. **Minimum Vote Requirements** - Candidates must receive at least the minimum number of votes to be eligible for parliament
2. **Maximum Parliament Seats** - Only the top candidates (by vote count) up to the maximum limit are selected
3. **Ranking System** - Candidates are ranked by total votes received (highest first)

## How It Works

### 1. **Data Input**

The algorithm reads from these files:

- `data/approved_candidates.txt` - List of approved candidates
- `data/votes.txt` - Voter choices (voter_id,candidate_id)

### 2. **Processing Steps**

1. **Load Candidates**: Reads all approved candidates
2. **Count Votes**: Tallies votes for each candidate from the votes file
3. **Apply Minimum Threshold**: Filters out candidates below minimum vote requirement
4. **Rank Candidates**: Sorts by vote count (descending order)
5. **Select Parliament**: Chooses top candidates up to maximum parliament seats

### 3. **Output**

- **Console Report**: Detailed results with statistics and rankings
- **Results File**: `data/voting_results.txt` with complete data

## Usage Examples

### Scenario 1: Basic Election

**Configuration:**

- Minimum votes for parliament: 100
- Maximum parliament members: 225

**Process:**

```bash
# Run through admin system (Option 5)
./bin/admin

# Or run demonstration
./demo_voting.sh
```

### Scenario 2: High Competition

**Configuration:**

- Minimum votes for parliament: 500
- Maximum parliament members: 50

**Result:** Only candidates with 500+ votes qualify, top 50 selected

### Scenario 3: Low Participation

**Configuration:**

- Minimum votes for parliament: 10
- Maximum parliament members: 100

**Result:** Most candidates qualify, parliament fills based on availability

## Admin Configuration

### Setting Parameters

1. Open Admin System: `./bin/admin`
2. Select "System Limits" (Option 3)
3. Configure:
   - **Minimum Votes for Parliament**: Set threshold for eligibility
   - **Maximum Parliament Members**: Set total parliament size

### Example Settings

```
System Limits Configuration:
1. Maximum Voters: 10000
2. Maximum Candidates: 500
3. Minimum Votes for Parliament: 100  ← Key parameter
4. Maximum Parliament Members: 225    ← Key parameter
5. Maximum Parties: 50
6. Maximum Districts: 25
```

## Sample Vote Processing

### Input Data

**Candidates:**

```
C001,Alice Brown,P01,D01
C002,Charlie Davis,P02,D01
C003,Diana Wilson,P01,D02
```

**Votes:**

```
V001,C001  (John votes for Alice)
V002,C001  (Jane votes for Alice)
V003,C002  (Bob votes for Charlie)
```

### Algorithm Processing

1. **Vote Counting:**

   - Alice Brown (C001): 2 votes
   - Charlie Davis (C002): 1 vote
   - Diana Wilson (C003): 0 votes

2. **Minimum Threshold Check** (min_votes = 1):

   - Alice Brown: ✓ Qualified (2 ≥ 1)
   - Charlie Davis: ✓ Qualified (1 ≥ 1)
   - Diana Wilson: ✗ Failed (0 < 1)

3. **Parliament Selection** (max_parliament = 2):
   - Rank 1: Alice Brown (2 votes) → Selected
   - Rank 2: Charlie Davis (1 vote) → Selected

### Results Output

```
🏛️ PARLIAMENT MEMBERS:
┌─────────────────────────────────────────────────┐
│ Rank │ Candidate │ Name         │ Votes │ Status │
├─────────────────────────────────────────────────┤
│    1 │ C001      │ Alice Brown  │   2   │ ✓ MP   │
│    2 │ C002      │ Charlie Davis│   1   │ ✓ MP   │
└─────────────────────────────────────────────────┘
```

## Key Features

### 1. **Flexible Parameters**

- Adjustable minimum vote requirements
- Configurable parliament size
- Real-time parameter updates

### 2. **Comprehensive Reporting**

- Detailed statistics
- Complete candidate rankings
- Parliament member list
- Failed candidate analysis

### 3. **Data Integrity**

- Vote validation
- Candidate verification
- Error handling for missing data

### 4. **Result Persistence**

- Results saved to file
- Timestamped reports
- Complete audit trail

## Error Handling

### Common Issues

1. **No Votes File**: System offers to create sample data
2. **No Candidates**: Algorithm reports error and exits
3. **Insufficient Qualified Candidates**: Reports actual vs. expected parliament size
4. **Invalid Data**: Skips malformed records and continues

### Recovery Options

- Automatic sample data generation
- Graceful degradation for missing data
- Detailed error reporting with suggestions

## Integration with Admin System

### Menu Access

The voting algorithm is integrated into the admin system as Option 5:

```
🏛️ VoteMe Admin System - Main Menu
═══════════════════════════════════════
1. 📝 CRUD Operations
2. 📊 Data Viewing
3. ⚙️ System Limits
4. 📈 System Status
5. 🗳️ Execute Voting  ← Voting Algorithm
0. 🚪 Exit
```

### Workflow

1. **Configure System**: Set voting parameters via System Limits
2. **Manage Data**: Add voters/candidates via CRUD Operations
3. **Execute Voting**: Run algorithm when ready
4. **Review Results**: Check generated reports

## Best Practices

### Before Running

1. ✅ Configure minimum vote thresholds appropriately
2. ✅ Set realistic parliament size limits
3. ✅ Verify candidate and voter data
4. ✅ Ensure votes file exists and is valid

### After Running

1. 📋 Review detailed results report
2. 💾 Archive results file for records
3. 📊 Analyze voting patterns
4. 🔍 Verify parliament member selection

## Technical Notes

### Algorithm Complexity

- **Time Complexity**: O(n log n) where n = number of candidates
- **Space Complexity**: O(n) for candidate storage
- **Sorting Method**: Quick sort for candidate ranking

### File Formats

- **Input**: CSV format with headers
- **Output**: Mixed format (statistics + CSV data)
- **Encoding**: UTF-8 text files

### Dependencies

- Enhanced data handler for file operations
- System configuration for parameters
- Error handling system for validation

---

**🎉 Your VoteMe voting algorithm is now ready to process elections with confidence!**
