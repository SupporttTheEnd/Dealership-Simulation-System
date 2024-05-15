// UMBC - CMSC 341 - Fall 2023 - Proj4
// Title: dealer.cpp
// Author: Andrew Tang
// Date: 11/27/2023
// Description: This is the implementation file for driver.h

#include "dealer.h"

// Name - CarDB::CarDB(int size, hash_fn hash, prob_t probing = DEFPOLCY)
// Desc - The constructor takes size to specify the length of the current hash table, and 
// hash is a function pointer to a hash function. The type of hash is defined in CarDB.h.
// The table size must be a prime number between MINPRIME and MAXPRIME. If the user passes a size 
// less than MINPRIME, the capacity must be set to MINPRIME. If the user passes a size larger than MAXPRIME, 
// the capacity must be set to MAXPRIME. If the user passes a non-prime number the capacity must be set to the 
// smallest prime number greater than user's value. The probing parameter specifies the type 
// of collision handling policy for the current hash table. Moreover, the constructor creates memory 
// for the current table and initializes all member variables.
CarDB::CarDB(int size, hash_fn hash, prob_t probing = DEFPOLCY){
    // set member variables
    m_hash = hash;
    m_currProbing = probing;
    m_currentSize = 0;
    m_currNumDeleted = 0;
    m_newPolicy = m_currProbing;

    // call find next prime function
    
    m_currentCap = getTableSize(size);
    
    // dynamically allocate array and initialize it
    m_currentTable = new Car[m_currentCap];
    for (int i = 0; i < m_currentCap; i++) {
        m_currentTable[i] = EMPTY;
    }

    // set old table member variables
    m_oldTable = nullptr;
    m_oldCap = 0;
    m_oldSize = 0;
    m_oldNumDeleted = 0;
    m_oldProbing = NONE;
}

// Name - CarDB::~CarDB()
// Desc - Destructor deallocates the memory.
CarDB::~CarDB(){
    // free current table
    if (m_currentTable != nullptr) {
        delete[] m_currentTable;
        m_currentTable = nullptr;
    }
    // free old table
    if (m_oldTable != nullptr) {
        delete[] m_oldTable;
        m_oldTable = nullptr;
    }
}

// Name - void changeProbPolicy(prob_t policy)
// Desc - The user can change the collision handling policy of the hash table at the runtime. 
// If a change request has been submitted by a user, the new policy will be stored in the variable m_newPolicy. 
// Once the next rehash operation is initiated the new policy will be used for the newly created hash table.
void CarDB::changeProbPolicy(prob_t policy){
    m_newPolicy = policy;
}

// Name - bool CarDB::insert(Car car)
// Desc - This function inserts an object into the current hash table. The insertion index is 
// determined by applying the hash function m_hash that is set in the CarDB constructor and 
// then reducing the output of the hash function modulo the table size. A hash function is 
// provided in the sample driver.cpp file to be used in this project.
// Hash collisions should be resolved using the probing policy specified in the m_currProbing variable. 
// We insert into the table indicated by m_currentTable. After every insertion we need to check for 
// the proper criteria, and if it is required, we need to rehash the entire table incrementally into a 
// new table. The incremental transfer proceeds with 25% of the nodes at a time. Once we transferred 25% 
// of the nodes for the first time, the second 25% will be transferred at the next operation 
// (insertion or removal). Once all data is transferred to the new table, the old table will be removed, 
// and its memory will be deallocated. If the Car object is inserted, the function returns true, 
// otherwise it returns false. A Car object can only be inserted once. The hash table does not 
// contain duplicate objects. Moreover, the car's ID value should be a valid one falling in the
// range [MINID-MAXID]. Every Car object is a unique object carrying the Car's model and the 
// dealer's ID. The Car's model is the key which is used for hashing.
bool CarDB::insert(Car car){
    // make sure that dealer is between the min and max id
    if (car.getDealer() < MINID || car.getDealer() > MAXID) {
        rehash();
        return false;
    }

    // Car already exists, return false
    if (!(getCar(car.getModel(), car.getDealer()) == EMPTY)) {
        rehash();
        return false; 
    }

    // find initial place to look
    int index = m_hash(car.getModel()) % m_currentCap;
    int i = 1;

    // keep looping if the entry is filled and not deleted
    while (!(m_currentTable[index] == EMPTY) && m_currentTable[index].getUsed()) { 
        // divergant cases for different probing 
        if (m_currProbing == QUADRATIC) {
            index = ((m_hash(car.getModel()) % m_currentCap) + (i * i)) % m_currentCap;
        } 
        else if (m_currProbing == DOUBLEHASH) {
            index = ((m_hash(car.getModel()) % m_currentCap) + i * (11 - (m_hash(car.getModel()) % 11))) % m_currentCap;
        }
        else {
            rehash();
            return false; 
        }

        i++;
    }
    // check if we are adding or just replacing
    if (m_currentTable[index] == EMPTY){
        m_currentSize++;
    }
    else{
        m_currNumDeleted--;
    }

    // define the car as used 
    car.setUsed(true);

    // insert the car 
    m_currentTable[index] = car;
    
    rehash();
    return true; 
}

// Name - bool CarDB::remove(Car car)
// Desc - This function removes a data point from either the current hash table or the old 
// hash table where the object is stored. In a hash table we do not empty the bucket, we only 
// tag it as deleted. To tag a removed bucket we can use the member variable m_used in the Car class. 
// To find the bucket of the object we should use the proper probing policy for the table.
// After every deletion we need to check for the proper criteria, and if it is required, we need 
// to rehash the entire table incrementally into the current table. The incremental transfer 
// proceeds with 25% of the nodes at a time. Once we transferred 25% of the nodes for the first time, 
// the second 25% will be transferred at the next operation (insertion or removal). Once all data is 
// transferred to the new table, the old table will be removed, and its memory will be deallocated.
// If the Car object is found and is deleted, the function returns true, otherwise it returns false.
bool CarDB::remove(Car car){
    // find initial place to look
    int index = m_hash(car.getModel()) % m_currentCap;
    int i = 1;

    // keep looping if the entry is occupied
    while (!(m_currentTable[index] == EMPTY)) {
        // if the entry matches the car and is not deleted
        if (m_currentTable[index] == car && m_currentTable[index].getUsed()) {
            // set deleted
            m_currentTable[index].setUsed(false);
            m_currNumDeleted++;

            rehash();
            return true;
        }

        // divergant cases for different probing 
        if (m_currProbing == QUADRATIC) {
            index = ((m_hash(car.getModel()) % m_currentCap) + (i * i)) % m_currentCap;
        } 
        else if (m_currProbing == DOUBLEHASH) {
            index = ((m_hash(car.getModel()) % m_currentCap) + i * (11 - (m_hash(car.getModel()) % 11))) % m_currentCap;
        }
        else {
            rehash();
            return false; 
        }

        i++;
    }
    // also check for entry in old table
    if (m_oldTable != nullptr) {
        // find initial place to look
        index = m_hash(car.getModel()) % m_oldCap;
        i = 1;

        // keep looping if the entry is occupied
        while (!(m_oldTable[index] == EMPTY)) {
            // if the entry matches the car and is not deleted
            if (m_oldTable[index] == car && m_oldTable[index].getUsed()) {
                m_oldTable[index].setUsed(false);
                m_oldNumDeleted++;

                // check conditions for rehash to start
                rehash();
                return true;
            }

            // divergant cases for different probing 
            if (m_oldProbing == QUADRATIC) {
                index = ((m_hash(car.getModel()) % m_oldCap) + (i * i)) % m_oldCap;
            } 
            else if (m_oldProbing == DOUBLEHASH) {
                index = ((m_hash(car.getModel()) % m_oldCap) + i * (11 - (m_hash(car.getModel()) % 11))) % m_oldCap;
            }
            else {
                rehash();
                return false; 
            }

            i++;
        }
    }
    // entry not found
    rehash();
    return false;
}

// Name - Car CarDB::getCar(string model, int dealer) const
// Desc - This function looks for the Car object with the model and the dealer id in the hash table, 
// if the object is found the function returns it, otherwise the function returns empty object. 
// If there are two hash tables at the time, the function needs to search both tables.
Car CarDB::getCar(string model, int dealer) const{
    // find initial place to look
    int index = m_hash(model) % m_currentCap;
    int i = 1;

    // keep looping if the entry is occupied
    while (!(m_currentTable[index] == EMPTY)) {
        // if the entry matches the car and is not deleted
        if (m_currentTable[index].getModel() == model &&
            m_currentTable[index].getDealer() == dealer &&
            m_currentTable[index].getUsed()) {
            return m_currentTable[index];
        }
        
        // divergant cases for different probing 
        if (m_currProbing == QUADRATIC) {
            index = ((m_hash(model) % m_currentCap) + (i * i)) % m_currentCap;
        } 
        else if (m_currProbing == DOUBLEHASH) {
            index = ((m_hash(model) % m_currentCap) + i * (11 - (m_hash(model) % 11))) % m_currentCap;
        }
        else{
            return EMPTY; 
        }
        i++;
    }
    // also check for entry in old table
    if (m_oldTable != nullptr) {
        // find initial place to look
        index = m_hash(model) % m_oldCap;
        i = 1;

        // keep looping if the entry is occupied
        while (!(m_oldTable[index] == EMPTY)) {
            // if the entry matches the car and is not deleted
            if (m_oldTable[index].getModel() == model &&
                m_oldTable[index].getDealer() == dealer &&
                m_oldTable[index].getUsed()) {
                return m_oldTable[index];
            }

            // divergant cases for different probing 
            if (m_oldProbing == QUADRATIC) {
                index = ((m_hash(model) % m_oldCap) + (i * i)) % m_oldCap;
            } 
            else if (m_oldProbing == DOUBLEHASH) {
                index = ((m_hash(model) % m_oldCap) + i * (11 - (m_hash(model) % 11))) % m_oldCap;
            }
            else{
                return EMPTY; 
            }
            i++;
        }
    }
    // return empty car if not found 
    return EMPTY;
}

// Name - float CarDB::lambda() const
// Desc - This function returns the load factor of the current hash table. The load factor 
// is the ratio of occupied buckets to the table capacity. The number of occupied buckets 
// is the total of available buckets and deleted buckets. An occupied bucket is a bucket 
// which can contain either a live data node (available to be used) or a deleted node.
float CarDB::lambda() const {
    // just the current size over the cap
    return float(m_currentSize) / m_currentCap;
}

// Name - float CarDB::deletedRatio() const
// Desc - This function returns the ratio of the deleted buckets to the total number of occupied buckets 
float CarDB::deletedRatio() const {
    // protect against 0 div
    if (m_currentSize == 0) {
        return 0.0; 
    }

    // just the num deleted over the current size
    return float(m_currNumDeleted) / (m_currentSize);
}

// Name - void CarDB::dump() const
// Desc - This function dumps the contents of the current hash table and the old hash table if it exists. 
// It prints the contents of the hash table in array-index order. Note: The implementation of this 
// function is provided. The function is provided to facilitate debugging.
void CarDB::dump() const {
    cout << "Dump for the current table: " << endl;
    if (m_currentTable != nullptr)
        for (int i = 0; i < m_currentCap; i++) {
            cout << "[" << i << "] : " << m_currentTable[i] << endl;
        }
    cout << "Dump for the old table: " << endl;
    if (m_oldTable != nullptr)
        for (int i = 0; i < m_oldCap; i++) {
            cout << "[" << i << "] : " << m_oldTable[i] << endl;
        }
}

// Name - bool CarDB::updateQuantity(Car car, int quantity)
// Desc - This function looks for the Car object in the hash table, if the object is found the function updates its quantity and returns 
// true, otherwise the function returns false. If there are two hash tables at the time, the function needs to search both tables for the search operation.
bool CarDB::updateQuantity(Car car, int quantity){
    // find initial place to look
    int index = m_hash(car.getModel()) % m_currentCap;
    int i = 1;
    
    // keep looping if the entry is occupied
    while (!(m_currentTable[index] == EMPTY)) {
        if (m_currentTable[index] == car && m_currentTable[index].getUsed()) {
            // update the quantity and return true
            m_currentTable[index].setQuantity(quantity);
            return true;
        }

        // divergant cases for different probing 
        if (m_currProbing == QUADRATIC) {
            index = ((m_hash(car.getModel()) % m_currentCap) + (i * i)) % m_currentCap;
        } 
        else if (m_currProbing == DOUBLEHASH) {
            index = ((m_hash(car.getModel()) % m_currentCap) + i * (11 - (m_hash(car.getModel()) % 11))) % m_currentCap;
        }
        else{
            return false; 
        }

        i++;
    }

    // also check for entry in old table
    if (m_oldTable != nullptr) {
        // find initial place to look
        index = m_hash(car.getModel()) % m_oldCap;
        i = 1;

        // keep looping if the entry is occupied
        while (!(m_oldTable[index] == EMPTY)) {
            if (m_oldTable[index] == car && m_oldTable[index].getUsed()) {
                // update the quantity and return true
                m_oldTable[index].setQuantity(quantity);
                return true;
            }

            // divergant cases for different probing
            if (m_oldProbing == QUADRATIC) {
                index = ((m_hash(car.getModel()) % m_oldCap) + (i * i)) % m_oldCap;
            } 
            else if (m_oldProbing == DOUBLEHASH) {
                index = ((m_hash(car.getModel()) % m_oldCap) + i * (11 - (m_hash(car.getModel()) % 11))) % m_oldCap;
            }
            else{
                return false; 
            }

            i++;
        }
    }

    // return false if not found 
    return false;
}

// Name - bool CarDB::isPrime(int number)
// Desc - This function returns true if the passed argument "number" is a prime number, 
// otherwise it returns false. Note: The implementation of this function is provided.
bool CarDB::isPrime(int number){
    bool result = true;
    for (int i = 2; i <= number / 2; ++i) {
        if (number % i == 0) {
            result = false;
            break;
        }
    }
    return result;
}

// Name - int CarDB::findNextPrime(int current)
// Desc - This function returns the smallest prime number greater than the argument "current". 
// If "current" is less than or equal to MINPRIME, the function returns MINPRIME. If "current" 
// is greater than or equal to MAXPRIME, the function returns MAXPRIME. In a hash table we'd 
// like to use a table with prime size. Then, every time we need to determine the size for a 
// new table, we use this function. Note: The implementation of this function is provided.
int CarDB::findNextPrime(int current){
    //we always stay within the range [MINPRIME-MAXPRIME]
    //the smallest prime starts at MINPRIME
    if (current < MINPRIME) current = MINPRIME-1;
    for (int i=current; i<MAXPRIME; i++) { 
        for (int j=2; j*j<=i; j++) {
            if (i % j == 0) 
                break;
            else if (j+1 > sqrt(i) && i != current) {
                return i;
            }
        }
    }
    //if a user tries to go over MAXPRIME
    return MAXPRIME;
}

// Name - ostream& operator<<(ostream& sout, const Car &car )
// Desc - overloaded extraction for car
ostream& operator<<(ostream& sout, const Car &car ) {
    if (!car.m_model.empty())
        sout << car.m_model << " (" << car.m_dealer << "," << car.m_quantity<< ")";
    else
        sout << "";
  return sout;
}

// Name - bool operator==(const Car& lhs, const Car& rhs)
// Desc - overloaded == for car
bool operator==(const Car& lhs, const Car& rhs){
    // since the uniqueness of an object is defined by model and delaer
    // the equality operator considers only those two criteria
    return ((lhs.m_model == rhs.m_model) && (lhs.m_dealer == rhs.m_dealer));
}

// Name - void CarDB::rehash()
// Desc - implementation of the rehash function 
void CarDB::rehash(){
    if (lambda() <= 0.5f && deletedRatio() <= 0.8f && m_oldTable == nullptr) {
        return;
    }
   
    if (m_oldTable == nullptr){
        // Set the old table to the current table
        m_oldTable = m_currentTable;
        m_oldCap = m_currentCap;
        m_oldSize = m_currentSize;
        m_oldNumDeleted = m_currNumDeleted;
        m_oldProbing = m_currProbing;

        // reconstruct the new table
        
        m_currentCap = getTableSize((m_currentSize - m_currNumDeleted) * 4);
    

        m_currentTable = new Car[m_currentCap];
        for (int i = 0; i < m_currentCap; i++) {
            m_currentTable[i] = EMPTY;
        }

        m_currentSize = 0;
        m_currNumDeleted = 0;
        m_currProbing = m_newPolicy; 
    }

    int transferred = 0;

    // transfer entries from the old to the new
    for (int i = 0; i < m_oldCap && transferred < (m_oldSize / 4); i++) {
        // transfer if filled and used
        
        if (!(m_oldTable[i] == EMPTY) && m_oldTable[i].getUsed()) {
            rehashInsert(m_oldTable[i]);

            m_oldTable[i].setUsed(false);
            m_oldNumDeleted++;

            transferred++;
        }
    }
    
    // check if incremental rehashing is completed
    if (m_oldSize == m_oldNumDeleted) {
        delete[] m_oldTable;
        m_oldTable = nullptr;
        m_oldCap = 0;
        m_oldSize = 0;
        m_oldNumDeleted = 0;
    }
}

// Name - bool CarDB::rehashInsert(Car car)
// Desc - implementation of the insert helper for the rehash function 
bool CarDB::rehashInsert(Car car){
    // find initial place to look
    int index = m_hash(car.getModel()) % m_currentCap;
    int i = 1;

    // keep looping if the entry is filled and not deleted
    while (!(m_currentTable[index] == EMPTY) && m_currentTable[index].getUsed()) { 
        // if the car is already present
        if (m_currentTable[index] == car) {
            return false;
        }

        // divergant cases for different probing 
        if (m_currProbing == QUADRATIC) {
            index = ((m_hash(car.getModel()) % m_currentCap) + (i * i)) % m_currentCap;
        } 
        else if (m_currProbing == DOUBLEHASH) {
            index = ((m_hash(car.getModel()) % m_currentCap) + i * (11 - (m_hash(car.getModel()) % 11))) % m_currentCap;
        }
        else {
            return false; 
        }

        i++;
    }
    // check if we are adding or just replacing
    if (m_currentTable[index] == EMPTY){
        m_currentSize++;
    }
    else{
        m_currNumDeleted--;
    }
    // insert the car 
    m_currentTable[index] = car;
    
    return true; 
}

// Name - int getTableSize(int size) 
// Desc - implementation of the helper to resize an array
int CarDB::getTableSize(int size) {
    // Ensure the size is within bounds
    if (size < MINPRIME) {
        return MINPRIME;
    } else if (size > MAXPRIME) {
        return MAXPRIME;
    }

    if (isPrime(size)){
        return size; 
    }
    else {
        return findNextPrime(size);
    }
}