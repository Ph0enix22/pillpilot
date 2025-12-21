/*
 PILLPILOT - MEDICATION TRACKER 
 Data Structures & Applications Mini Project
 Authors: [SMJ;]
 Language: C
 Data Structures Used: HashMap, Priority Queue, Linked List
 -------- 
 TODO:  fix sorting in display
        add file saving maybe
        Add notifications (try, if possible in C)
        check if gui is possible in C
*/

//imports, link section
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

//GLOBAL CONSTANTS AND DEFINITIONS
#define MAX_NAME 50
#define MAX_DOSAGE 20 
#define MAX_INSTRUCTIONS 100
#define TABLE_SIZE 100 //hash table size (could be made dynamic later)

//STRUCTURE DEF
typedef struct Medicine {
    int id; // Unique ID
    char name[MAX_NAME];    // Medicine name
    char dosage[MAX_DOSAGE];    // Dosage info
    char time[6];  // HH:MM format
    char instructions[MAX_INSTRUCTIONS]; // Additional instructions
    int priority;  // Countdown -Minutes from midnight
    /*priority = hours*60 + minutes, so 08:30 = 8*60+30 = 510 min*/
} Medicine; 

// HashMap Node - for storing medicines with fast lookup
typedef struct HashNode {
    Medicine medicine; // The actual medicine data
    int isOccupied; // Flag to check if this slot is taken (1=occupied, 0=free)
} HashNode;

// HashMap Structure - main storage for medicines
typedef struct HashMap {
    HashNode table[TABLE_SIZE]; //Array of HashNodes
    int count;
} HashMap;

// Priority Queue Node  (for scheduling reminders)
typedef struct PQNode {
    Medicine medicine; // Medicine data
    int priority;// Priority based on time; Using a min-heap for the priority queue
} PQNode;

// Priority Queue (Min-Heap) -keeps medicines sorted by time
typedef struct PriorityQueue {
    PQNode heap[TABLE_SIZE];// Array-based heap
    int size;
    //heap[0] is root, for node at index i:
    //left child at 2*i+1
    //right child at 2*i+2  
    //parent at (i-1)/2
} PriorityQueue;

// History Node (Linked List) -for tracking taken/missed medicines
typedef struct HistoryNode {
    int medicineId;// Medicine ID
    char medicineName[MAX_NAME];// Medicine name
    char status[10];  // "taken" or "missed"
    time_t timestamp;// Time of entry
    struct HistoryNode* next;// Pointer to next node
    //New entries are added at the head for O(1) insertion
} HistoryNode;

//Linked List Structure -for history tracking
typedef struct LinkedList {
    HistoryNode* head;// Pointer to head node
    int count;// Count of history entries
} LinkedList;

//HashMap -Hash tables give us fast lookup; way better than searching arrays!
//Hash function -converts medicine ID to array index
int hashFunction(int id) {
    //using simple modulo for hashing
    return id % TABLE_SIZE;
    //basic hash function, could be improved
}

// Initialize empty HashMap
void initHashMap(HashMap* map) {
    // Set all slots to unoccupied
    for (int i = 0; i < TABLE_SIZE; i++) {
        map->table[i].isOccupied = 0;// Mark slot as free
    }
    map->count = 0;
}

// Insert into HashMap
int insertMedicine(HashMap* map, Medicine med) {
    int index = hashFunction(med.id);// Get initial index
    int originalIndex = index;// To keep track of where we started
    
    // Linear probing: find next free slot
    while (map->table[index].isOccupied) {
        index = (index + 1) % TABLE_SIZE;//move to next index
        // If we looped back to original index, table is full
        if (index == originalIndex) {
            printf("HashMap is full!\n");
            return 0;// Insertion failed
        }
    }
    // Found free slot, insert medicine
    map->table[index].medicine = med;// Copy medicine data
    map->table[index].isOccupied = 1;// Mark slot as occupied
    map->count++;
    //printf("Inserted medicine ID %d at index %d\n", med.id, index); // Debugging line
    return 1;// Insertion successful
}

// Search in HashMap - return pointer to Medicine or NULL if not found
Medicine* searchMedicine(HashMap* map, int id) {
    int index = hashFunction(id);// Get initial index
    int originalIndex = index;// To avoid infinite loops
    // Linear probing to find the medicine
    while (map->table[index].isOccupied) {
        if (map->table[index].medicine.id == id) {
            // found the medicine! return pointer to it
            return &map->table[index].medicine;
        }
        index = (index + 1) % TABLE_SIZE;//move to next index
        if (index == originalIndex) break;//not found
    }
    return NULL;// Not found
    //printf("Medicine ID %d not found in HashMap\n", id); //Debugging line
}

// Delete from HashMap by ID
int deleteMedicine(HashMap* map, int id) {
    int index = hashFunction(id);
    int originalIndex = index;
    while (map->table[index].isOccupied) {
        if (map->table[index].medicine.id == id) {
            map->table[index].isOccupied = 0;
            map->count--;
            return 1;// Deletion successful
        }
        index = (index + 1) % TABLE_SIZE;
        if (index == originalIndex) break;// Came full circle
    }
    return 0;//Not found
    //printf("Medicine ID %d not found for deletion\n", id); //Debugging
}

// Display all meds
void displayAllMedicines(HashMap* map) {
    printf("\n=== ALL MEDICINES ===\n");
    printf("%-5s %-20s %-10s %-6s %-30s\n", "ID", "Name", "Dosage", "Time", "Instructions");
    printf("--------------------------------------------------------------------------------\n");
    // Iterate through the hash table and display occupied slots
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (map->table[i].isOccupied) {
            Medicine m = map->table[i].medicine;// Get medicine data
            printf("%-5d %-20s %-10s %-6s %-30s\n", 
                   m.id, m.name, m.dosage, m.time, m.instructions);
        }
    }
    printf("\nTotal Medicines: %d\n", map->count);
    //printf("Displayed all medicines in HashMap\n"); //Debugg
}

//Priority Queue - O(log n) insertion and deletion //to manage reminders efficiently; for scheduling tasks.

// Initialize empty Priority Queue
void initPriorityQueue(PriorityQueue* pq) {
    pq->size = 0;
}

// Swap two nodes
void swap(PQNode* a, PQNode* b) {
    PQNode temp = *a;
    *a = *b;
    *b = temp;
}

// Heapify up - moves the node at index up to maintain heap property
void heapifyUp(PriorityQueue* pq, int index) {
    
    while (index > 0) {
        int parent = (index - 1) / 2;// Calculate parent index
        // If current element has smaller priority than parent, swap
        if (pq->heap[index].priority < pq->heap[parent].priority) {
            swap(&pq->heap[index], &pq->heap[parent]);
            index = parent;//move up to parent index
        } else {
            break;
        }
    }
}

// Heapify down: moves the element at index down to maintain heap property
void heapifyDown(PriorityQueue* pq, int index) {
    while (1) {// continue until no swaps are needed
        int smallest = index;// Assume current index is smallest
        int left = 2 * index + 1;// Left child index
        int right = 2 * index + 2;// Right child index
        // Check if left child exists and is smaller
        if (left < pq->size && pq->heap[left].priority < pq->heap[smallest].priority) {
            smallest = left;
        }// Check if right child exists and is smaller
        if (right < pq->size && pq->heap[right].priority < pq->heap[smallest].priority) {
            smallest = right;
        }
        // If smallest is not current index, swap and continue
        if (smallest != index) {
            swap(&pq->heap[index], &pq->heap[smallest]);
            index = smallest;
        } else {
            break;//break if heap property is satisfied
        }
    }
    //printf("Heapified down from index %d\n", index); //Debugg
}

// Insert medicine into Priority Queue
void enqueuePQ(PriorityQueue* pq, Medicine med) {
    // Check if queue is full
    if (pq->size >= TABLE_SIZE) {
        printf("Priority Queue is full!\n");
        return;
    }
    // Insert at the end
    pq->heap[pq->size].medicine = med;
    pq->heap[pq->size].priority = med.priority;
    heapifyUp(pq, pq->size);//move up to maintain heap prop
    pq->size++;
    //printf("Enqueued medicine ID %d with priority %d\n", med.id,med.priority); //Debugg
}

// Display today's schedule sorted by time
void displayPriorityQueue(PriorityQueue* pq) {
    printf("\n=== TODAY'S SCHEDULE (Sorted by Time) ===\n");
    printf("%-6s %-20s %-10s %-30s\n", "Time", "Medicine", "Dosage", "Instructions");
    printf("--------------------------------------------------------------------------------\n");
    // Create a temporary array to hold sorted medicines
    for (int i = 0; i < pq->size; i++) {
        Medicine m = pq->heap[i].medicine;
        printf("%-6s %-20s %-10s %-30s\n", 
               m.time, m.name, m.dosage, m.instructions);
    }
    //TODO: sort display o/p later
}

// LinkedList; good for history tracking; dynamic data where we frequently add/remove entries!
void initLinkedList(LinkedList* list) {
    list->head = NULL;// Empty list
    list->count = 0;
}

// Insert new history entry at head (most recent first)
void insertAtHead(LinkedList* list, int medId, char* medName, char* status) {
    HistoryNode* newNode = (HistoryNode*)malloc(sizeof(HistoryNode));// Allocate memory for new node
    if (!newNode) {// Check for memory allocation failure
        printf("Memory allocation failed!\n");
        return;
    }// Initialize new node
    newNode->medicineId = medId;
    strcpy(newNode->medicineName, medName);// Copy medicine name
    strcpy(newNode->status, status);
    newNode->timestamp = time(NULL);// Current time
    // Insert at head
    newNode->next = list->head;// Point new node to old head
    list->head = newNode;
    list->count++;
}

// Display history -most recent first
void displayHistory(LinkedList* list) {
    printf("\n=== MEDICATION HISTORY ===\n");
    printf("%-20s %-10s %-30s\n", "Medicine", "Status", "Timestamp");
    printf("--------------------------------------------------------------------------------\n");
    // Traverse the list and display entries
    HistoryNode* current = list->head;
    while (current != NULL) {
        // Format timestamp
        char timeStr[26];// Buffer for formatted time
        struct tm* timeInfo = localtime(&current->timestamp);
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeInfo);
        
        printf("%-20s %-10s %-30s\n", 
               current->medicineName, current->status, timeStr);
        current = current->next;//move to next node
    }
    printf("\nTotal Entries: %d\n", list->count);
    if(list->count == 0) {
        printf("No medication history recorder yet.\n");
    }
}

// Get statistics for today: total taken, total missed
void getStatistics(LinkedList* list, int* taken, int* missed) {
    *taken = 0;//counter initialization
    *missed = 0;
    
    time_t now = time(NULL);//current time
    struct tm* nowTm = localtime(&now);// Local time structure
    // Traverse the list and count today's entries
    HistoryNode* current = list->head;
    while (current != NULL) {
        struct tm* entryTm = localtime(&current->timestamp);
        
        // Check if entry is from today // Compare year and day of year
        if (nowTm->tm_year == entryTm->tm_year &&
            nowTm->tm_yday == entryTm->tm_yday) {//same day
            if (strcmp(current->status, "taken") == 0) {
                (*taken)++;
            } else {
                (*missed)++;
            }
        }
        current = current->next;
    }
}

// UTIL FUNCS
// Calculate priority from time string (HH:MM)
int calculatePriority(char* time) {
    int hours, minutes;
    sscanf(time, "%d:%d", &hours, &minutes);
    return hours * 60 + minutes;// Convert to total minutes from midnight
    //e.g., 08:30 = 8*60 + 30 = 510min
}

// simple unique ID generation based on current time (dummy implementation)
int generateId() {
    return (int)time(NULL) % 10000;// Last 4 digits of current time
    // In real application there's a database for unique IDs or UUIDs
}

int main() {//declare main data structures
    HashMap medicineMap;// Main storage for medicines
    PriorityQueue reminderQueue;// For scheduling reminders
    LinkedList history;// For tracking taken/missed medicines
    
    initHashMap(&medicineMap);// Initialize data structures
    initPriorityQueue(&reminderQueue);
    initLinkedList(&history);
    
    int choice;
    
    printf("\n");
    printf("------------------------------------------\n");
    printf("|        PILLPILOT - DSA PROJECT         |\n");
    printf("|       Medication Tracker using C       |\n");
    printf("------------------------------------------\n");
    
    while (1) {// Main menu loop
        printf("\n========== MAIN MENU ==========\n");
        printf("1. Add New Medicine\n");
        printf("2. View All Medicines (HashMap Demo)\n");
        printf("3. View Today's Schedule (Priority Queue Demo)\n");
        printf("4. Mark Medicine as Taken\n");
        printf("5. Mark Medicine as Missed\n");
        printf("6. View Medication History (Linked List Demo)\n");
        printf("7. View Today's Statistics\n");
        printf("8. Search Medicine by ID (HashMap Lookup)\n");
        printf("9. Delete Medicine\n");
        printf("0. Exit Program\n");
        printf("===============================\n");
        printf("Enter your choice (0-9): ");
        scanf("%d", &choice);
        getchar(); // Consume newline after scanf
        
        switch (choice) {
            case 1: {
                Medicine med;
                med.id = generateId();// Generate unique ID
                printf("\n--- Adding New Medicine ---\n");
                printf("\nEnter medicine name: ");
                fgets(med.name, MAX_NAME, stdin);
                med.name[strcspn(med.name, "\n")] = 0;// Remove newline char from fgets
                
                printf("Enter dosage (e.g., 500mg, 1 tablet): ");
                fgets(med.dosage, MAX_DOSAGE, stdin);
                med.dosage[strcspn(med.dosage, "\n")] = 0;
                
                printf("Enter time (HH:MM format,e.g.,08:30): ");
                fgets(med.time, 6, stdin);
                med.time[strcspn(med.time, "\n")] = 0;
                getchar();
                
                printf("Enter instructions: ");
                fgets(med.instructions, MAX_INSTRUCTIONS, stdin);
                med.instructions[strcspn(med.instructions, "\n")] = 0;
                
                med.priority = calculatePriority(med.time);
                // Insert into HashMap and Priority Queue
                if (insertMedicine(&medicineMap, med)) {
                    enqueuePQ(&reminderQueue, med);
                    printf("\n✓ Medicine added successfully! ID: %d\n", med.id);
                } else {
                    printf("\n✗ Failed to add medicine.\n");
                }
                break;
            }
            
            case 2:
                displayAllMedicines(&medicineMap);
                break;
            
            case 3:
                displayPriorityQueue(&reminderQueue);
                break;
            
            case 4: {
                int id;
                printf("\nEnter medicine ID: ");
                scanf("%d", &id);
                
                Medicine* med = searchMedicine(&medicineMap, id);  // Lookup in HashMap
                if (med != NULL) {
                    insertAtHead(&history, med->id, med->name, "taken");
                    printf("\n✓ Marked as taken!\n");
                } else {
                    printf("\n✗ Medicine not found!\n");
                }
                break;
            }
            
            case 5: {
                int id;
                printf("\nEnter medicine ID to mark as missed: ");
                scanf("%d", &id);
                
                Medicine* med = searchMedicine(&medicineMap, id);
                if (med != NULL) {
                    insertAtHead(&history, med->id, med->name, "missed");
                    printf("\n✓ Marked as missed!\n");
                } else {
                    printf("\n✗ Medicine not found!\n");
                }
                break;
            }
            
            case 6:
                displayHistory(&history);
                break;
            
            case 7: {
                int taken, missed;
                getStatistics(&history, &taken, &missed);
                printf("\n=== TODAY'S MEDICATION STATISTICS ===\n");
                printf("Total Medicines: %d\n", medicineMap.count);
                printf("Medicines Taken Today: %d\n", taken);
                printf("Medicines Missed Today: %d\n", missed);
                if (taken + missed > 0) {
                    float adherence = (float)taken / (taken + missed) * 100;    
                    printf("Today's Adherence Rate: %.1f%%\n", adherence);
                    if (adherence == 100) {
                        printf("Keep it up!\n");
                    } else if (adherence >= 80) {
                        printf("You're doing well!\n");
                    } else if (adherence >= 50) {
                        printf("Good effort! Let's try to improve!\n");
                    } else {
                        printf("⚠ Room for improvement. Set reminders!\n");
                    }
                }else {
                    printf("No medication taken or missed today yet.\n");
                }
                break;
            }
            
            case 8: {
                int id;
                printf("\nEnter medicine ID to search: ");
                scanf("%d", &id);
                
                Medicine* med = searchMedicine(&medicineMap, id);
                if (med != NULL) {
                    printf("\n=== MEDICINE FOUND ===\n");
                    printf("ID: %d\n", med->id);
                    printf("Name: %s\n", med->name);
                    printf("Dosage: %s\n", med->dosage);
                    printf("Scheduled Time: %s\n", med->time);
                    printf("Instructions: %s\n", med->instructions);
                    printf("Priority Value: %d minutes from midnight\n",med->priority);
                } else {
                    printf("\n✗ No Medicine found with ID %d\n",id);
                    printf("  Double-check the ID or use option 2 to list all medicines.\n");
                }
                break;
            }
            
            case 9: {
                int id;
                printf("\nEnter medicine ID to delete: ");
                scanf("%d", &id);
                //confirm deletion
                Medicine* med = searchMedicine(&medicineMap, id);
                if (med != NULL) {
                    printf("Found medicine: %s (%s)\n", med->name, med->dosage);
                    printf("Are you sure you want to delete this? (y/n): ");
                    char confirm;
                    scanf(" %c", &confirm);
                    if (confirm == 'y' || confirm == 'Y') {
                        if (deleteMedicine(&medicineMap, id)) {
                            printf("\n✓ Medicine ID %d deleted successfully!\n", id);
                        } else {
                            printf("\n✗ Failed to delete medicine ID %d\n", id);
                        }
                    } else {
                        printf("\n✗ Deletion cancelled.\n");
                    }
                } else {
                    printf("\n✗ Medicine with ID %d not found!\n",id);
                }
                break;
            }
            
            case 0:
                printf("\nThank you for using PillPilot!\n");
                printf("Stay healthy and remember to take your meds on time!\n");
                printf("Project by: Team *\n");
                printf("Course: Data Structures & Applications\n");
                printf("Structures Used: HashMap, Priority Queue, Linked List\n\n");
                printf("Goodbye!\n");
                return 0;
            
            default:
                printf("\n✗ Invalid choice! Please enter a number from 0-9 and Try again.\n");
                printf("Tip: Read the menu carefully and try again.\n");
                printf("If the issue persists, restart the program.\n");
        }
        printf("\nPress Enter to continue...");
        getchar();
    }
    
    return 0;
}
