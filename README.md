![C](https://img.shields.io/badge/Language-C-blue.svg)
![DSA](https://img.shields.io/badge/Project-DSA--Mini--Project-green.svg)
![Status](https://img.shields.io/badge/Status-Complete-success.svg)
# PillPilot - Medication Tracker
DSA Mini Project - 3rd Semester

## What is this?
A medication management system built in C that helps track medicines and reminds you when to take them. Made for our Data Structures course to understand how different data structures work in real applications.

## Data Structures Used
1. **HashMap** - Stores medicines with fast O(1) lookup by ID
2. **Priority Queue (Min-Heap)** - Keeps schedule sorted by time automatically 
3. **Linked List** - Tracks medication history chronologically

## How to Run

**Linux/Mac:**
```bash
gcc pillpilot.c -o pillpilot
./pillpilot
```

**Windows:**
```bash
gcc pillpilot.c -o pillpilot.exe
pillpilot.exe
```

**Online Compiler:**
- Go to https://www.onlinegdb.com/online_c_compiler
- Paste the code and click Run

## Features
- Add medicines with name, dosage, time, and instructions
- View all medicines (HashMap demo)
- View today's schedule sorted by time (Priority Queue demo)
- Mark medicines as taken or missed
- View medication history (Linked List demo)
- See statistics and adherence rate
- Search medicine by ID
- Delete medicine

## Why These Data Structures?

### HashMap vs Array
- Array: O(n) search time - have to check each medicine one by one
- HashMap: O(1) average search time - jump directly to the medicine using ID
- Used hash function with linear probing for collision handling

### Priority Queue vs Sorting Array
- Sorting entire array every time: O(n log n)
- Priority Queue insertion: O(log n)
- Priority Queue keeps medicines sorted by time automatically
- Min-heap structure - smallest time always at root

### Linked List for History
- Insert at head is O(1) - very fast
- Don't need fixed size like arrays
- Perfect for adding new entries (most recent first)

## Time Complexity

| Operation | Time | Data Structure |
|-----------|------|----------------|
| Add medicine | O(1) avg | HashMap |
| Search by ID | O(1) avg | HashMap |
| Delete medicine | O(1) avg | HashMap |
| Add to schedule | O(log n) | Priority Queue |
| Add history entry | O(1) | Linked List |
| View history | O(n) | Linked List |

## Sample Output
```
Enter choice: 1
Enter medicine name: Paracetamol
Enter dosage: 500mg
Enter time: 08:30
Enter instructions: After breakfast

✓ Medicine added successfully! ID: 3421

Enter choice: 7
=== TODAY'S STATISTICS ===
Total Medicines: 2
Medicines Taken Today: 1
Medicines Missed Today: 0
Adherence Rate: 100.0%
```

## Known Limitations
- Data is lost when program exits (no file saving yet)
- Hash table has fixed size of 100 entries
- Time input format not validated
- Priority queue display not fully sorted (TODO)

## Future Improvements
- Add file I/O to save/load data
- Input validation for time format
- Fix priority queue display sorting
- Maybe add GUI if possible in C

## Real World Use
Around 50% of patients forget to take their medicines. This system can help by:
- Automatic time-based reminders
- Tracking what was taken/missed
- Calculating adherence rates
- Could be scaled up for hospitals or clinics

## What I Learned
- How hash tables work and handle collisions
- Min-heap implementation for priority queues
- When to use linked lists vs arrays
- Time complexity trade-offs between data structures
- Combining multiple data structures in one program

## Project Files
- `pillpilot.c` - Main source code
- `test_cases.txt` - Test scenarios and expected outputs
- `README.md` - This file

## Team - C2 Batch
- Syeda Midhath Javeria - USN: 162
- [Syed Badruddin Quadri] - USN: [160]
- [Syed Sani] - USN: [161]

**Course:** Data Structures & Applications (22AI34)  
**Semester:** 3  
**Department:** Artificial Intelligence & Machine Learning

---

Built for DSA course project - Academic Year 2025-26
