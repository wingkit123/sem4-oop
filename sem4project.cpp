#include <iostream>
#include <string>
#include <iomanip> // For formatting output (setw, left)

using namespace std;

// --- DATA STRUCTURE DEFINITIONS (Structs and Classes) ---
// All concepts below are covered in your course syllabus.

// Struct for a food item node in the main menu's linked list. (Covered in Chapter 4)
struct FoodItem
{
  int id;
  string name;
  double price;
  FoodItem *next;
};

// Struct for an order node in the delivery queue. (Covered in Chapter 5)
struct OrderNode
{
  FoodItem *item;  // Points to the food item that was ordered
  OrderNode *next; // Pointer for the linked queue
};

// Struct for a hash table entry. (Covered in Chapter 7)
// This will be used for fast O(1) lookups.
struct HashNode
{
  int key;         // The food ID
  FoodItem *value; // A pointer to the FoodItem node in the linked list
  HashNode *next;  // For handling collisions (chaining)
};

// A constant for the size of the hash table.
// A prime number is often chosen to help reduce collisions.
const int HASH_TABLE_SIZE = 47;

// --- CLASS IMPLEMENTATIONS ---

// A Hash Table class for fast lookups, edits, and deletions. (Covered in Chapter 7)
// This table stores pointers to the nodes in the main menu linked list.
class HashTable
{
private:
  HashNode *table[HASH_TABLE_SIZE]; // Fixed-size array of HashNode pointers

public:
  HashTable()
  {
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
      table[i] = nullptr;
    }
  }

  // Hash function using modular arithmetic, as described in your notes.
  int hashFunction(int key)
  {
    return key % HASH_TABLE_SIZE;
  }

  // Insert a key-value pair (ID and a pointer to the FoodItem)
  void insert(int key, FoodItem *food)
  {
    int index = hashFunction(key);
    HashNode *newNode = new HashNode{key, food, nullptr};

    // If the slot is empty, insert it there.
    if (table[index] == nullptr)
    {
      table[index] = newNode;
    }
    else
    {
      // Otherwise, handle collision by adding to the front of the chain.
      newNode->next = table[index];
      table[index] = newNode;
    }
  }

  // Search for a FoodItem pointer by its ID. This is very fast.
  FoodItem *search(int key)
  {
    int index = hashFunction(key);
    HashNode *entry = table[index];
    while (entry != nullptr)
    {
      if (entry->key == key)
      {
        return entry->value;
      }
      entry = entry->next;
    }
    return nullptr; // Return null if not found
  }

  // Remove an entry from the hash table.
  void remove(int key)
  {
    int index = hashFunction(key);
    HashNode *entry = table[index];
    HashNode *prev = nullptr;

    while (entry != nullptr && entry->key != key)
    {
      prev = entry;
      entry = entry->next;
    }

    if (entry == nullptr)
      return; // Key not found

    if (prev == nullptr)
    { // It was the first node in the slot
      table[index] = entry->next;
    }
    else
    {
      prev->next = entry->next;
    }
    delete entry;
  }
  // Destructor to clean up memory
  ~HashTable()
  {
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
      HashNode *entry = table[i];
      while (entry != nullptr)
      {
        HashNode *prev = entry;
        entry = entry->next;
        delete prev;
      }
    }
    // No need to delete[] table since it's a fixed-size array
  }
};

// A Linked Queue class to manage pending orders. (Covered in Chapter 5)
class OrderQueue
{
private:
  OrderNode *front, *rear;

public:
  OrderQueue()
  {
    front = rear = nullptr;
  }

  // Enqueue: Add an order to the rear of the queue.
  void addOrder(FoodItem *item)
  {
    if (!item)
    {
      cout << "\nError: Food ID does not exist. Cannot place order.\n";
      return;
    }
    OrderNode *newOrder = new OrderNode{item, nullptr};
    if (rear == nullptr)
    { // If the queue is empty
      front = rear = newOrder;
    }
    else
    {
      rear->next = newOrder;
      rear = newOrder;
    }
    cout << "\nOrder for \"" << item->name << "\" has been placed in the queue.\n";
  }

  // Dequeue: Process and remove the order from the front.
  void dispatchOrder()
  {
    if (front == nullptr)
    {
      cout << "\nNo orders in the queue to dispatch.\n";
      return;
    }
    OrderNode *temp = front;
    cout << "\nDispatched order for \"" << temp->item->name << "\".\n";
    front = front->next;
    if (front == nullptr)
    {
      rear = nullptr; // The queue is now empty
    }
    delete temp;
  }

  // Display all orders currently in the queue.
  void displayOrders()
  {
    if (front == nullptr)
    {
      cout << "\nThe order queue is currently empty.\n";
      return;
    }
    cout << "\n--- Pending Delivery Queue ---\n";
    OrderNode *temp = front;
    int i = 1;
    while (temp)
    {
      cout << i << ". ID: " << temp->item->id << ", Name: " << temp->item->name << endl;
      temp = temp->next;
      i++;
    }
    cout << "------------------------------\n";
  }

  ~OrderQueue()
  {
    while (front != nullptr)
    {
      OrderNode *temp = front;
      front = front->next;
      delete temp;
    }
  }
};

// The main class for the system, managing the menu as a Linked List.
class FoodDeliverySystem
{
private:
  FoodItem *head;
  HashTable itemHashTable; // Contains a hash table for fast lookups.
  int nextId;              // Auto-incrementing ID counter

  // --- Merge Sort for Linked List (Covered in Chapter 8) ---
  // This is an efficient sorting algorithm suitable for a linked list.

  // Helper function to split the list into two halves.
  void splitList(FoodItem *source, FoodItem **frontRef, FoodItem **backRef)
  {
    if (source == nullptr || source->next == nullptr)
    {
      *frontRef = source;
      *backRef = nullptr;
      return;
    }
    FoodItem *slow = source;
    FoodItem *fast = source->next;
    while (fast != nullptr)
    {
      fast = fast->next;
      if (fast != nullptr)
      {
        slow = slow->next;
        fast = fast->next;
      }
    }
    *frontRef = source;
    *backRef = slow->next;
    slow->next = nullptr;
  }

  // Helper function to merge two sorted lists.
  FoodItem *mergeSortedLists(FoodItem *a, FoodItem *b)
  {
    if (a == nullptr)
      return b;
    if (b == nullptr)
      return a;
    FoodItem *result = nullptr;
    if (a->price <= b->price)
    {
      result = a;
      result->next = mergeSortedLists(a->next, b);
    }
    else
    {
      result = b;
      result->next = mergeSortedLists(a, b->next);
    }
    return result;
  }

  // The main recursive merge sort function.
  void mergeSort(FoodItem **headRef)
  {
    FoodItem *current = *headRef;
    if (current == nullptr || current->next == nullptr)
      return;
    FoodItem *a, *b;
    splitList(current, &a, &b);
    mergeSort(&a);
    mergeSort(&b);
    *headRef = mergeSortedLists(a, b);
  }
  // --- End of Merge Sort ---

public:
  FoodDeliverySystem()
  {
    head = nullptr;
    nextId = 1; // Initialize auto-incrementing ID counter
  } // Add a food item with auto-incrementing ID.
  void addFood()
  {
    string name;
    double price;

    int id = nextId++; // Auto-assign and increment ID
    cout << "Food ID " << id << " assigned automatically.\n";

    cout << "Enter Food Name: ";
    cin.ignore();
    getline(cin, name);
    cout << "Enter Price (RM): ";
    cin >> price;
    while (cin.fail())
    {
      cin.clear();
      cin.ignore(); 
      cout << "Invalid input. Please enter a numeric price: ";
      cin >> price;
    }

    FoodItem *newFood = new FoodItem{id, name, price, nullptr};

    // Add to the head of the linked list for simplicity
    newFood->next = head;
    head = newFood;

    // Also add the item's pointer to the hash table for fast access
    itemHashTable.insert(id, newFood);
    cout << "\nFood item \"" << name << "\" added successfully with ID " << id << "!\n";
  }

  // Edit a food item using its ID.
  void editFood(int id)
  {
    FoodItem *itemToEdit = itemHashTable.search(id); // O(1) average lookup
    if (itemToEdit)
    {
      cout << "Current Name: " << itemToEdit->name << ". Enter new name: ";
      cin.ignore();
      getline(cin, itemToEdit->name);
      cout << "Current Price: " << itemToEdit->price << ". Enter new price: ";
      cin >> itemToEdit->price;
      cout << "\nFood item updated successfully!\n";
    }
    else
    {
      cout << "\nError: Food item with ID " << id << " not found.\n";
    }
  }

  // Delete a food item using its ID.
  void deleteFood(int id)
  {
    if (!head)
    {
      cout << "\nMenu is empty. Cannot delete.\n";
      return;
    }

    FoodItem *toDelete = nullptr;
    FoodItem *current = head;
    FoodItem *prev = nullptr;

    // Find the node to delete
    while (current != nullptr && current->id != id)
    {
      prev = current;
      current = current->next;
    }

    if (current == nullptr)
    { // We went through the whole list
      cout << "\nError: Food item with ID " << id << " not found.\n";
      return;
    }

    toDelete = current;

    if (prev == nullptr)
    { // Deleting the head node
      head = toDelete->next;
    }
    else
    {
      prev->next = toDelete->next;
    }

    itemHashTable.remove(id); // Remove from hash table
    delete toDelete;          // Free the memory
    cout << "\nFood item with ID " << id << " deleted successfully.\n";
  }

  // Display the menu with professional formatting.
  void displayMenu()
  {
    if (!head)
    {
      cout << "\nThe menu is currently empty.\n";
      return;
    }
    cout << "\n---------------------------------------------\n";
    cout << "                 FULL FOOD MENU\n";
    cout << "---------------------------------------------\n";
    cout << left << setw(8) << "ID" << setw(25) << "Name" << "Price (RM)" << endl;
    cout << "---------------------------------------------\n";

    FoodItem *temp = head;
    while (temp)
    {
      cout << left << setw(8) << temp->id
           << setw(25) << temp->name
           << fixed << setprecision(2) << temp->price << endl;
      temp = temp->next;
    }
    cout << "---------------------------------------------\n";
  }

  // A simple linear search by name. (Covered in Chapter 6)
  void searchFoodByName(const string &key)
  {
    FoodItem *temp = head;
    bool found = false;
    cout << "\n--- Search Results for \"" << key << "\" ---\n";
    while (temp)
    {
      // A simple substring search (case-sensitive)
      if (temp->name.find(key) != string::npos)
      {
        cout << "Found -> ID: " << temp->id << ", Name: " << temp->name << ", Price: RM" << temp->price << endl;
        found = true;
      }
      temp = temp->next;
    }
    if (!found)
    {
      cout << "No food items found matching your search term.\n";
    }
  }

  // Public method to call the merge sort.
  void sortMenuByPrice()
  {
    if (!head || !head->next)
    {
      cout << "\nMenu is already sorted or has too few items to sort.\n";
      return;
    }
    mergeSort(&head);
    cout << "\nMenu has been sorted by price using Merge Sort.\n";
  }

  // Public method to get a food item's pointer using its ID.
  FoodItem *findFoodById(int id)
  {
    return itemHashTable.search(id);
  }

  // Destructor to clean up all nodes in the linked list
  ~FoodDeliverySystem()
  {
    FoodItem *current = head;
    while (current != nullptr)
    {
      FoodItem *next = current->next;
      delete current;
      current = next;
    }
  }
};

// --- MAIN PROGRAM LOGIC ---

// Function to display program banner
void showProgramBanner()
{
  cout << "\n";
  cout << "********************************************************\n";
  cout << "*                                                      *\n";
  cout << "*          FOODIE EXPRESS DELIVERY SYSTEM              *\n";
  cout << "*                                                      *\n";
  cout << "********************************************************\n";
  cout << "\nWelcome to the Food Delivery Management System!\n";
}

// Function to clear screen (cross-platform)
void clearScreen()
{
#ifdef _WIN32
  system("cls");
#else
  system("clear");
#endif
}

// Function to display the main menu
void showMainMenu()
{
  cout << "\n=========== FOOD DELIVERY MANAGEMENT SYSTEM ===========\n";
  cout << "------------------ Menu Administration ----------------\n";
  cout << " 1. Add New Food Item\n";
  cout << " 2. Edit Food Item by ID\n";
  cout << " 3. Delete Food Item by ID\n";
  cout << " 4. Display Full Menu\n";
  cout << " 5. Sort Menu by Price (using Merge Sort)\n";
  cout << " 6. Search Food by Name (using Linear Search)\n";
  cout << "-------------------- Order System --------------------\n";
  cout << " 7. Place New Order into Queue\n";
  cout << " 8. View All Pending Orders\n";
  cout << " 9. Dispatch Next Order from Queue\n";
  cout << "------------------------------------------------------\n";
  cout << " 10. Exit\n";
  cout << "=====================================================\n";
  cout << "\nEnter your choice: ";
}

int main()
{
  // Display program banner
  showProgramBanner();

  FoodDeliverySystem fds;
  OrderQueue orderQueue;
  int choice;

  // Prompt user for initial setup
  int numItems;
  cout << "\n--------------------------------------------------\n";
  cout << "How many food items would you like to add initially? ";
  cin >> numItems;
  while (cin.fail() || numItems < 0)
  {
    cin.clear();
    // cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.ignore();
    cout << "Invalid input. Please enter a non-negative number: ";
    cin >> numItems;
  }

  // Add the specified number of items with auto-incrementing IDs
  cout << "\nAdding " << numItems << " food items with auto-assigned IDs (1, 2, 3, ...):\n";
  for (int i = 0; i < numItems; i++)
  {
    cout << "\n--- Adding Food Item " << (i + 1) << " ---\n";
    fds.addFood();
  }

  if (numItems > 0)
  {
    cout << "\nInitial setup complete! " << numItems << " food items added.\n";
    cout << "Press Enter to continue...";
    cin.ignore();
    cin.get();
  }

  while (true)
  {
    clearScreen(); // Clear screen for better readability
    showMainMenu();
    cin >> choice;

    if (cin.fail())
    {
      cin.clear();
      // cin.ignore(numeric_limits<streamsize>::max(), '\n');
      cin.ignore();
      cout << "\nInvalid input. Please enter a number.\n";
      cout << "Press Enter to continue...";
      cin.get();
      continue;
    }

    if (choice == 10)
    {
      cout << "\nThank you for using Foodie Express Delivery System!\n";
      cout << "Exiting system. Goodbye!\n";
      break;
    }

    int id;
    string name;

    switch (choice)
    {
    case 1:
      fds.addFood();
      break;
    case 2:
      cout << "\nEnter Food ID to edit: ";
      cin >> id;
      fds.editFood(id);
      break;
    case 3:
      cout << "\nEnter Food ID to delete: ";
      cin >> id;
      fds.deleteFood(id);
      break;
    case 4:
      fds.displayMenu();
      break;
    case 5:
      fds.sortMenuByPrice();
      break;
    case 6:
      cout << "\nEnter food name to search for: ";
      cin.ignore();
      getline(cin, name);
      fds.searchFoodByName(name);
      break;
    case 7:
      cout << "\nEnter Food ID to place an order: ";
      cin >> id;
      orderQueue.addOrder(fds.findFoodById(id));
      break;
    case 8:
      orderQueue.displayOrders();
      break;
    case 9:
      orderQueue.dispatchOrder();
      break;
    default:
      cout << "\nInvalid choice. Please select an option from 1 to 10.\n";
    }

    // Pause after each operation for better user experience
    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
  }

  return 0;
}