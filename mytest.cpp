// UMBC - CMSC 341 - Fall 2023 - Proj4
// Title: mytest.cpp
// Author: Andrew Tang
// Date: 11/27/2023
// Description: This is the test file for dealer.cpp

// CMSC 341 - Fall 2023 - Project 4
#include "dealer.h"
#include <random>
#include <vector>
#include <algorithm>

enum RANDOM {UNIFORMINT, UNIFORMREAL, NORMAL, SHUFFLE};
class Random {
public:
    Random(int min, int max, RANDOM type=UNIFORMINT, int mean=50, int stdev=20) : m_min(min), m_max(max), m_type(type)
    {
        if (type == NORMAL){
            //the case of NORMAL to generate integer numbers with normal distribution
            m_generator = std::mt19937(m_device());
            //the data set will have the mean of 50 (default) and standard deviation of 20 (default)
            //the mean and standard deviation can change by passing new values to constructor 
            m_normdist = std::normal_distribution<>(mean,stdev);
        }
        else if (type == UNIFORMINT) {
            //the case of UNIFORMINT to generate integer numbers
            // Using a fixed seed value generates always the same sequence
            // of pseudorandom numbers, e.g. reproducing scientific experiments
            // here it helps us with testing since the same sequence repeats
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_unidist = std::uniform_int_distribution<>(min,max);
        }
        else if (type == UNIFORMREAL) { //the case of UNIFORMREAL to generate real numbers
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_uniReal = std::uniform_real_distribution<double>((double)min,(double)max);
        }
        else { //the case of SHUFFLE to generate every number only once
            m_generator = std::mt19937(m_device());
        }
    }
    void setSeed(int seedNum){
        // we have set a default value for seed in constructor
        // we can change the seed by calling this function after constructor call
        // this gives us more randomness
        m_generator = std::mt19937(seedNum);
    }

    void getShuffle(vector<int> & array){
        // the user program creates the vector param and passes here
        // here we populate the vector using m_min and m_max
        for (int i = m_min; i<=m_max; i++){
            array.push_back(i);
        }
        shuffle(array.begin(),array.end(),m_generator);
    }

    void getShuffle(int array[]){
        // the param array must be of the size (m_max-m_min+1)
        // the user program creates the array and pass it here
        vector<int> temp;
        for (int i = m_min; i<=m_max; i++){
            temp.push_back(i);
        }
        std::shuffle(temp.begin(), temp.end(), m_generator);
        vector<int>::iterator it;
        int i = 0;
        for (it=temp.begin(); it != temp.end(); it++){
            array[i] = *it;
            i++;
        }
    }

    int getRandNum(){
        // this function returns integer numbers
        // the object must have been initialized to generate integers
        int result = 0;
        if(m_type == NORMAL){
            //returns a random number in a set with normal distribution
            //we limit random numbers by the min and max values
            result = m_min - 1;
            while(result < m_min || result > m_max)
                result = m_normdist(m_generator);
        }
        else if (m_type == UNIFORMINT){
            //this will generate a random number between min and max values
            result = m_unidist(m_generator);
        }
        return result;
    }

    double getRealRandNum(){
        // this function returns real numbers
        // the object must have been initialized to generate real numbers
        double result = m_uniReal(m_generator);
        // a trick to return numbers only with two deciaml points
        // for example if result is 15.0378, function returns 15.03
        // to round up we can use ceil function instead of floor
        result = std::floor(result*100.0)/100.0;
        return result;
    }
    
    private:
    int m_min;
    int m_max;
    RANDOM m_type;
    std::random_device m_device;
    std::mt19937 m_generator;
    std::normal_distribution<> m_normdist;//normal distribution
    std::uniform_int_distribution<> m_unidist;//integer uniform distribution
    std::uniform_real_distribution<double> m_uniReal;//real uniform distribution

};

unsigned int hashCode(const string str);

unsigned int hashCode(const string str) {
    unsigned int val = 0 ;
    const unsigned int thirtyThree = 33 ;  // magic number from textbook
    for (unsigned int i = 0 ; i < str.length(); i++)
        val = val * thirtyThree + str[i] ;
    return val ;
}

string carModels[5] = {"challenger", "stratos", "gt500", "miura", "x101"};
string dealers[5] = {"super car", "mega car", "car world", "car joint", "shack of cars"};

class Tester{
    public:

    /******************************************************************************************************************************************
                                THESE TESTS ALL USE THE DOUBLE HASH PROBING POLICY
    ******************************************************************************************************************************************/
    // Test the insertion operation in the hash table.
    bool insertNormal() {
        cout << "TEST 1 RESULTS:" << endl;
        Random RndID(MINID,MAXID);
        Random RndCar(0,4);// selects one from the carModels array
        Random RndQuantity(0,50);
        CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);
        
        for (int i = 0; i < 10; i++){
            // generating non colliding data
            Car dataObj = Car(to_string(i), RndQuantity.getRandNum(), RndID.getRandNum(), true);
            // inserting data in to the CarDB object
            cardb.insert(dataObj);

            int index = hashCode(to_string(i)) % cardb.m_currentCap;
            if (!(cardb.m_currentTable[index] == dataObj)) {
                return false;
            }
        }

        if (cardb.m_currentSize != 10){
            return false; 
        }

        for (int i = 0; i < 26; i++){
            // generating non colliding data
            Car dataObj = Car(string(1, 'A' + i), RndQuantity.getRandNum(), RndID.getRandNum(), true);
            // inserting data in to the CarDB object
            cardb.insert(dataObj);

            int index = hashCode(string(1, 'A' + i)) % cardb.m_currentCap;
            if (!(cardb.m_currentTable[index] == dataObj)) {
                return false;
            }
        }

        if (cardb.m_currentSize != 36){
            return false; 
        }

        return true; 
    }
    // Test the find operation (getCar(...) function) for an error case, the Car object does not exist in the database.
    bool getCarError() {
        cout << "TEST 2 RESULTS:" << endl;
        Random RndID(MINID, MAXID);
        Random RndCar(0, 4);  // selects one from the carModels array
        Random RndQuantity(0, 50);
        CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);

        // insert some data
        for (int i = 0; i < 50; i++) {
            // generating random data
            Car dataObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(), RndID.getRandNum(), true);
            // inserting data in to the CarDB object
            cardb.insert(dataObj);
        }

        // make a data object 
        Car dataObj = Car("A", RndQuantity.getRandNum(), RndID.getRandNum(), true);
        // call get car
        Car result = cardb.getCar(dataObj.getModel(), dataObj.getDealer());

        // make sure that getcar returns an empty car
        if (!(result == Car("", 0, 0, false))) {
            return false;
        }

        return true;
    }

    // Test the find operation (getCar(...) function) with several non-colliding keys.
    bool getCarNonColliding() {
        cout << "TEST 3 RESULTS:" << endl;
        vector<Car> dataList;
        Random RndID(MINID, MAXID);
        Random RndCar(0, 4);  // selects one from the carModels array
        Random RndQuantity(0, 50);
        CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);

        // insert some data
        for (int i = 0; i < 26; i++) {
            // generating non-colliding data
            Car dataObj = Car(string(1, 'A' + i), RndQuantity.getRandNum(), RndID.getRandNum(), true);
            // inserting data into the CarDB object
            dataList.push_back(dataObj);
            cardb.insert(dataObj);
        }

        // attempt to find each inserted item
        for (vector<Car>::iterator it = dataList.begin(); it != dataList.end(); it++){
            Car result = cardb.getCar((*it).getModel(), (*it).getDealer());
            if (!(result == *it)) {
                return false;
            }
        }
        return true;
    }

    // Test the find operation (getCar(...) function) with several colliding keys without triggering a rehash. This also tests whether the insertion works correctly with colliding data.
    bool getCarColliding() {
        cout << "TEST 4 RESULTS:" << endl;
        vector<Car> dataList;
        Random RndID(MINID, MAXID);
        Random RndCar(0, 4);  // selects one from the carModels array
        Random RndQuantity(0, 50);
        CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);

        // insert some data
        for (int i = 0; i < 49; i++) {
            // generating colliding data
            Car dataObj = Car(string(1, 'A'), RndQuantity.getRandNum(), RndID.getRandNum(), true);
            // inserting data into the CarDB object
            dataList.push_back(dataObj);
            cardb.insert(dataObj);
        }

        // attempt to find each inserted item
        for (vector<Car>::iterator it = dataList.begin(); it != dataList.end(); it++){
            Car result = cardb.getCar((*it).getModel(), (*it).getDealer());
            if (!(result == *it)) {
                return false;
            }
        }

        return true;
    }

    // Test the remove operation with a few non-colliding keys.
    bool removeNonColliding() {
        cout << "TEST 5 RESULTS:" << endl;
        vector<Car> dataList;
        Random RndID(MINID, MAXID);
        Random RndCar(0, 4);  // selects one from the carModels array
        Random RndQuantity(0, 50);
        CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);

        // insert some data
        for (int i = 0; i < 26; i++) {
                // generating non-colliding data
                Car dataObj = Car(string(1, 'A' + i), RndQuantity.getRandNum(), RndID.getRandNum(), true);
                // inserting data into the CarDB object
                dataList.push_back(dataObj);
                cardb.insert(dataObj);
        }
        // remove each item that was inserted
        for (int i = 0; i < 10; i++) {
            // make sure that the remove operation worked
            if (!cardb.remove(dataList[i])) {
                return false;
            }

            // try to find the removed object; it should not be there
            if (!(cardb.getCar(dataList[i].getModel(), dataList[i].getDealer()) == EMPTY)) {
                return false;
            }
        } 
        // make sure that that ten were indeed deleted
        if (cardb.m_currNumDeleted != 10) {
           return false;
        }

        return true;
    }

    // Test the remove operation with a number of colliding keys without triggering a rehash.
    bool removeColliding() {
        cout << "TEST 6 RESULTS:" << endl;
        vector<Car> dataList;
        Random RndID(MINID, MAXID);
        Random RndCar(0, 4);  // selects one from the carModels array
        Random RndQuantity(0, 50);
        CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);

        // insert some data
        for (int i = 0; i < 26; i++) {
                // generating non-colliding data
                Car dataObj = Car(string(1, 'A'), RndQuantity.getRandNum(), RndID.getRandNum(), true);
                // inserting data into the CarDB object
                dataList.push_back(dataObj);
                cardb.insert(dataObj);
        }
        // remove each item that was inserted
        for (int i = 0; i < 10; i++) {
            // make sure that the remove operation worked
            if (!cardb.remove(dataList[i])) {
                return false;
            }

            // try to find the removed object; it should not be there
            if (!(cardb.getCar(dataList[i].getModel(), dataList[i].getDealer()) == EMPTY)) {
                return false;
            }
        } 
        // make sure that that ten were indeed deleted
        if (cardb.m_currNumDeleted != 10) {
           return false;
        }

        return true;
    }

    // Test the remove operation with a number of colliding keys without triggering a rehash.
    bool insertRehash() {
        cout << "TEST 7 RESULTS:" << endl;
        Random RndID(MINID, MAXID);
        Random RndCar(0, 4);  // selects one from the carModels array
        Random RndQuantity(0, 50);
        CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);

        // insert just enough to start rehashing
        for (int i = 0; i < 51; i++) {
            Car dataObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(), RndID.getRandNum(), true);
            cardb.insert(dataObj);
        }

        // Check if rehashing has started
        if (cardb.m_oldTable == nullptr) {
            return false; 
        }

        return true; 
    }

    // Test the rehash completion after triggering rehash due to load factor, i.e. all live data is transferred to the new table and the old table is removed.
    bool insertRehashComplete() {
        cout << "TEST 8 RESULTS:" << endl;

        vector<Car> dataList;
        Random RndID(MINID, MAXID);
        Random RndCar(0, 4);  // selects one from the carModels array
        Random RndQuantity(0, 50);
        CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);

        // insert just enough to get rehashing started
        for (int i = 0; i < 51; i++) {
            Car dataObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(), RndID.getRandNum(), true);
            cardb.insert(dataObj);
            dataList.push_back(dataObj);
        }

        // rehash until the process has completed
        while (cardb.m_oldTable != nullptr) {
            Car dataObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(), RndID.getRandNum(), true);
            cardb.insert(dataObj);
            dataList.push_back(dataObj);
        }

        // Check if rehashing has deleted the old table
        if (cardb.m_oldTable != nullptr) {
            return false; 
        }

        if (cardb.m_currentSize != 55) { 
            return false; 
        }

        // make sure that all entries can be found 
        for (vector<Car>::iterator it = dataList.begin(); it != dataList.end(); it++){
            Car result = cardb.getCar((*it).getModel(), (*it).getDealer());
            if (!(result == *it)) {
                return false;
            }
        }

        return true; 
    }

    // Test the rehashing is triggered after a descent number of data removal.
    bool removeRehash() {
        cout << "TEST 9 RESULTS:" << endl;

        vector<Car> dataList;
        Random RndID(MINID, MAXID);
        Random RndCar(0, 4);  // selects one from the carModels array
        Random RndQuantity(0, 50);
        CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);

        // Insert a large number of data
        for (int i = 0; i < 49; i++) {
            Car dataObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(), RndID.getRandNum(), true);
            cardb.insert(dataObj);
            dataList.push_back(dataObj);
        }

        // Remove a substantial amount of data
        for (int i = 0; i < 40; i++) {
            if (!cardb.remove(dataList[i])) {
                return false;  // Removal failed
            }
        }

        // Check if rehashing has started
        if (cardb.m_currNumDeleted != 0) {
            return false; 
        }

        return true;     
    }

    // Test the rehash completion after triggering rehash due to delete ratio, i.e. all live data is transferred to the new table and the old table is removed.
    bool removeRehashComplete() {
        cout << "TEST 10 RESULTS:" << endl;

        vector<Car> dataList;
        Random RndID(MINID, MAXID);
        Random RndCar(0, 4);  // selects one from the carModels array
        Random RndQuantity(0, 50);
        CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);

        // Insert a large number of data
        for (int i = 0; i < 49; i++) {
            Car dataObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(), RndID.getRandNum(), true);
            cardb.insert(dataObj);
            dataList.push_back(dataObj);
        }
    
        // Remove a substantial amount of data
        for (int i = 0; i < 40; i++) {
            if (!cardb.remove(dataList[i])) {
                return false;  // Removal failed
                 
            }
        }
        // Check if rehashing has deleted the old table
        if (cardb.m_oldTable != nullptr) {
            return false; 
        }
    
        // Check if rehashing has started
        if (cardb.m_currNumDeleted != 0) { 
            return false; 
        }

        // check if the right number of cars have been transfered over
        if (cardb.m_currentSize != 9) { 
            return false; 
        }

        // make sure that all entries can be found 
        for (vector<Car>::iterator it = dataList.begin() + 40; it != dataList.end(); it++){
            Car result = cardb.getCar((*it).getModel(), (*it).getDealer());
            if (!(result == *it)) {
                
                return false;
            }
        }
        return true;     
    }

    /******************************************************************************************************************************************
                    THE SAME TESTS BUT NOW CHANGED TO QUADRATIC PROBING POLICY INSTEAD OF DOUBLEHASH
    ******************************************************************************************************************************************/

    bool insertNormalQuad() {
        cout << "TEST 11 RESULTS:" << endl;
        Random RndID(MINID,MAXID);
        Random RndCar(0,4);// selects one from the carModels array
        Random RndQuantity(0,50);
        CarDB cardb(MINPRIME, hashCode, QUADRATIC);
        
        for (int i = 0; i < 10; i++){
            // generating non colliding data
            Car dataObj = Car(to_string(i), RndQuantity.getRandNum(), RndID.getRandNum(), true);
            // inserting data in to the CarDB object
            cardb.insert(dataObj);

            int index = hashCode(to_string(i)) % cardb.m_currentCap;
            if (!(cardb.m_currentTable[index] == dataObj)) {
                return false;
            }
        }

        if (cardb.m_currentSize != 10){
            return false; 
        }

        for (int i = 0; i < 26; i++){
            // generating non colliding data
            Car dataObj = Car(string(1, 'A' + i), RndQuantity.getRandNum(), RndID.getRandNum(), true);
            // inserting data in to the CarDB object
            cardb.insert(dataObj);

            int index = hashCode(string(1, 'A' + i)) % cardb.m_currentCap;
            if (!(cardb.m_currentTable[index] == dataObj)) {
                return false;
            }
        }

        if (cardb.m_currentSize != 36){
            return false; 
        }

        return true; 
    }

    bool getCarErrorQuad() {
        cout << "TEST 12 RESULTS:" << endl;
        Random RndID(MINID, MAXID);
        Random RndCar(0, 4);  // selects one from the carModels array
        Random RndQuantity(0, 50);
        CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);

        // insert some data
        for (int i = 0; i < 50; i++) {
            // generating random data
            Car dataObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(), RndID.getRandNum(), true);
            // inserting data in to the CarDB object
            cardb.insert(dataObj);
        }

        // make a data object 
        Car dataObj = Car("A", RndQuantity.getRandNum(), RndID.getRandNum(), true);
        // call get car
        Car result = cardb.getCar(dataObj.getModel(), dataObj.getDealer());

        // make sure that getcar returns an empty car
        if (!(result == Car("", 0, 0, false))) {
            return false;
        }

        return true;
    }

    bool getCarNonCollidingQuad() {
        cout << "TEST 13 RESULTS:" << endl;
        vector<Car> dataList;
        Random RndID(MINID, MAXID);
        Random RndCar(0, 4);  // selects one from the carModels array
        Random RndQuantity(0, 50);
        CarDB cardb(MINPRIME, hashCode, QUADRATIC);

        // insert some data
        for (int i = 0; i < 26; i++) {
            // generating non-colliding data
            Car dataObj = Car(string(1, 'A' + i), RndQuantity.getRandNum(), RndID.getRandNum(), true);
            // inserting data into the CarDB object
            dataList.push_back(dataObj);
            cardb.insert(dataObj);
        }

        // attempt to find each inserted item
        for (vector<Car>::iterator it = dataList.begin(); it != dataList.end(); it++){
            Car result = cardb.getCar((*it).getModel(), (*it).getDealer());
            if (!(result == *it)) {
                return false;
            }
        }
        return true;
    }

    bool getCarCollidingQuad() {
        cout << "TEST 14 RESULTS:" << endl;
        vector<Car> dataList;
        Random RndID(MINID, MAXID);
        Random RndCar(0, 4);  // selects one from the carModels array
        Random RndQuantity(0, 50);
        CarDB cardb(MINPRIME, hashCode, QUADRATIC);

        // insert some data
        for (int i = 0; i < 49; i++) {
            // generating colliding data
            Car dataObj = Car(string(1, 'A'), RndQuantity.getRandNum(), RndID.getRandNum(), true);
            // inserting data into the CarDB object
            dataList.push_back(dataObj);
            cardb.insert(dataObj);
        }

        // attempt to find each inserted item
        for (vector<Car>::iterator it = dataList.begin(); it != dataList.end(); it++){
            Car result = cardb.getCar((*it).getModel(), (*it).getDealer());
            if (!(result == *it)) {
                return false;
            }
        }

        return true;
    }

    bool removeNonCollidingQuad() {
        cout << "TEST 15 RESULTS:" << endl;
        vector<Car> dataList;
        Random RndID(MINID, MAXID);
        Random RndCar(0, 4);  // selects one from the carModels array
        Random RndQuantity(0, 50);
        CarDB cardb(MINPRIME, hashCode, QUADRATIC);

        // insert some data
        for (int i = 0; i < 26; i++) {
                // generating non-colliding data
                Car dataObj = Car(string(1, 'A' + i), RndQuantity.getRandNum(), RndID.getRandNum(), true);
                // inserting data into the CarDB object
                dataList.push_back(dataObj);
                cardb.insert(dataObj);
        }
        // remove each item that was inserted
        for (int i = 0; i < 10; i++) {
            // make sure that the remove operation worked
            if (!cardb.remove(dataList[i])) {
                return false;
            }

            // try to find the removed object; it should not be there
            if (!(cardb.getCar(dataList[i].getModel(), dataList[i].getDealer()) == EMPTY)) {
                return false;
            }
        } 
        // make sure that that ten were indeed deleted
        if (cardb.m_currNumDeleted != 10) {
           return false;
        }

        return true;
    }

    
    bool removeCollidingQuad() {
        cout << "TEST 16 RESULTS:" << endl;
        vector<Car> dataList;
        Random RndID(MINID, MAXID);
        Random RndCar(0, 4);  // selects one from the carModels array
        Random RndQuantity(0, 50);
        CarDB cardb(MINPRIME, hashCode, QUADRATIC);

        // insert some data
        for (int i = 0; i < 26; i++) {
                // generating non-colliding data
                Car dataObj = Car(string(1, 'A'), RndQuantity.getRandNum(), RndID.getRandNum(), true);
                // inserting data into the CarDB object
                dataList.push_back(dataObj);
                cardb.insert(dataObj);
        }
        // remove each item that was inserted
        for (int i = 0; i < 10; i++) {
            // make sure that the remove operation worked
            if (!cardb.remove(dataList[i])) {
                return false;
            }

            // try to find the removed object; it should not be there
            if (!(cardb.getCar(dataList[i].getModel(), dataList[i].getDealer()) == EMPTY)) {
                return false;
            }
        } 
        // make sure that that ten were indeed deleted
        if (cardb.m_currNumDeleted != 10) {
           return false;
        }

        return true;
    }

    bool insertRehashQuad() {
        cout << "TEST 17 RESULTS:" << endl;
        Random RndID(MINID, MAXID);
        Random RndCar(0, 4);  // selects one from the carModels array
        Random RndQuantity(0, 50);
        CarDB cardb(MINPRIME, hashCode, QUADRATIC);

        // insert just enough to start rehashing
        for (int i = 0; i < 51; i++) {
            Car dataObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(), RndID.getRandNum(), true);
            cardb.insert(dataObj);
        }

        // Check if rehashing has started
        if (cardb.m_oldTable == nullptr) {
            return false; 
        }

        return true; 
    }


    bool insertRehashCompleteQuad() {
        cout << "TEST 18 RESULTS:" << endl;

        vector<Car> dataList;
        Random RndID(MINID, MAXID);
        Random RndCar(0, 4);  // selects one from the carModels array
        Random RndQuantity(0, 50);
        CarDB cardb(MINPRIME, hashCode, QUADRATIC);

        // insert just enough to get rehashing started
        for (int i = 0; i < 51; i++) {
            Car dataObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(), RndID.getRandNum(), true);
            cardb.insert(dataObj);
            dataList.push_back(dataObj);
        }

        // rehash until the process has completed
        while (cardb.m_oldTable != nullptr) {
            Car dataObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(), RndID.getRandNum(), true);
            cardb.insert(dataObj);
            dataList.push_back(dataObj);
        }

        // Check if rehashing has deleted the old table
        if (cardb.m_oldTable != nullptr) {
            return false; 
        }

        if (cardb.m_currentSize != 55) { 
            return false; 
        }

        // make sure that all entries can be found 
        for (vector<Car>::iterator it = dataList.begin(); it != dataList.end(); it++){
            Car result = cardb.getCar((*it).getModel(), (*it).getDealer());
            if (!(result == *it)) {
                return false;
            }
        }

        return true; 
    }

    bool removeRehashQuad() {
        cout << "TEST 19 RESULTS:" << endl;

        vector<Car> dataList;
        Random RndID(MINID, MAXID);
        Random RndCar(0, 4);  // selects one from the carModels array
        Random RndQuantity(0, 50);
        CarDB cardb(MINPRIME, hashCode, QUADRATIC);

        // Insert a large number of data
        for (int i = 0; i < 49; i++) {
            Car dataObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(), RndID.getRandNum(), true);
            cardb.insert(dataObj);
            dataList.push_back(dataObj);
        }

        // Remove a substantial amount of data
        for (int i = 0; i < 40; i++) {
            if (!cardb.remove(dataList[i])) {
                return false;  // Removal failed
            }
        }

        // Check if rehashing has started
        if (cardb.m_currNumDeleted != 0) {
            return false; 
        }

        return true;     
    }

    bool removeRehashCompleteQuad() {
        cout << "TEST 20 RESULTS:" << endl;

        vector<Car> dataList;
        Random RndID(MINID, MAXID);
        Random RndCar(0, 4);  // selects one from the carModels array
        Random RndQuantity(0, 50);
        CarDB cardb(MINPRIME, hashCode, QUADRATIC);

        // Insert a large number of data
        for (int i = 0; i < 49; i++) {
            Car dataObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(), RndID.getRandNum(), true);
            cardb.insert(dataObj);
            dataList.push_back(dataObj);
        }
    
        // Remove a substantial amount of data
        for (int i = 0; i < 40; i++) {
            if (!cardb.remove(dataList[i])) {
                return false;  // Removal failed
                 
            }
        }
        // Check if rehashing has deleted the old table
        if (cardb.m_oldTable != nullptr) {
            return false; 
        }
    
        // Check if rehashing has started
        if (cardb.m_currNumDeleted != 0) { 
            return false; 
        }

        // check if the right number of cars have been transfered over
        if (cardb.m_currentSize != 9) { 
            return false; 
        }

        // make sure that all entries can be found 
        for (vector<Car>::iterator it = dataList.begin() + 40; it != dataList.end(); it++){
            Car result = cardb.getCar((*it).getModel(), (*it).getDealer());
            if (!(result == *it)) {
                
                return false;
            }
        }
        return true;     
    }

    /******************************************************************************************************************************************
                                                                MORE TESTS
    ******************************************************************************************************************************************/

    bool insertRehashChange() {
        cout << "TEST 21 RESULTS:" << endl;

        vector<Car> dataList;
        Random RndID(MINID, MAXID);
        Random RndCar(0, 4);  // selects one from the carModels array
        Random RndQuantity(0, 50);
        CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);

        // change policy for rehash
        cardb.changeProbPolicy(QUADRATIC);

        // insert just enough to get rehashing started
        for (int i = 0; i < 51; i++) {
            Car dataObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(), RndID.getRandNum(), true);
            cardb.insert(dataObj);
            dataList.push_back(dataObj);
        }

        // rehash until the process has completed
        while (cardb.m_oldTable != nullptr) {
            Car dataObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(), RndID.getRandNum(), true);
            cardb.insert(dataObj);
            dataList.push_back(dataObj);
        }

        // Check if rehashing has deleted the old table
        if (cardb.m_oldTable != nullptr) {
            return false; 
        }
        
        if (cardb.m_currentSize != 55) { 
            return false; 
        }

        if (cardb.m_currProbing != QUADRATIC) { 
            return false; 
        }

        // make sure that all entries can be found 
        for (vector<Car>::iterator it = dataList.begin(); it != dataList.end(); it++){
            Car result = cardb.getCar((*it).getModel(), (*it).getDealer());
            if (!(result == *it)) {
                return false;
            }
        }
        return true; 
    }

    bool updateQuantityNormal() {
        cout << "TEST 22 RESULTS:" << endl;

        vector<Car> dataList;
        Random RndID(MINID, MAXID);
        Random RndCar(0, 4);  // selects one from the carModels array
        Random RndQuantity(0, 50);
        CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);

        // insert cars with quantity of 5
        for (int i = 0; i < 50; i++) {
            Car dataObj = Car(carModels[RndCar.getRandNum()], 5, RndID.getRandNum(), true);
            cardb.insert(dataObj);
            dataList.push_back(dataObj);
        }

        // update half of them to have a quantity of 10 
        for (int i = 0; i < 25; i++) {
            if (!(cardb.updateQuantity(dataList[i], 10))) {
                return false; 
            }
            dataList[i].setQuantity(10);
        }

        // verify that quantities have been correctly updated
        for (int i = 0; i < 50; i++) {
            Car result = cardb.getCar(dataList[i].getModel(), dataList[i].getDealer());
            if (!(result.getQuantity() == dataList[i].getQuantity())) {
                return false;
            }
        }  

        return true;
    }

    bool updateQuantityEdge() {
        cout << "TEST 23 RESULTS:" << endl;

        vector<Car> dataList;
        Random RndID(MINID, MAXID);
        Random RndCar(0, 4);  // selects one from the carModels array
        Random RndQuantity(0, 50);
        CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);

        // insert cars with quantity of 5
        for (int i = 0; i < 50; i++) {
            Car dataObj = Car(carModels[RndCar.getRandNum()], 5, RndID.getRandNum(), true);
            cardb.insert(dataObj);
            dataList.push_back(dataObj);
        }
        
        // make a car that is not in the database
        Car dataObj = Car("Test", RndQuantity.getRandNum(), RndID.getRandNum(), true);

        // should not be able to update this 
        if (cardb.updateQuantity(dataObj, 10)) {
            return false; 
        }

        return true;
    }

    bool constructorNormal() {
        cout << "TEST 24 RESULTS:" << endl;

        int size = 103;
        hash_fn hash = hashCode;
        prob_t probing = DEFPOLCY;

        // Create CarDB object
        CarDB cardb(size, hash, probing);

        // Checkall the starting states
         if (cardb.m_hash != hash || cardb.m_newPolicy != probing ||
            cardb.m_currentSize != 0 || cardb.m_currNumDeleted != 0 ||
            cardb.m_currentCap != size || cardb.m_currProbing != probing ||
            cardb.m_oldTable != nullptr || cardb.m_oldCap != 0 ||
            cardb.m_oldSize != 0 || cardb.m_oldNumDeleted != 0 ||
            cardb.m_oldProbing != NONE) {
            return false;
        }

        // make sure the current table is right
        for (int i = 0; i < cardb.m_currentCap; i++) {
            if (!(cardb.m_currentTable[i] == EMPTY)) {
                return false;
            }
        }

        return true;
    }

    bool insertColliding() {
        cout << "TEST 25 RESULTS:" << endl;
        vector<Car> dataList;
        Random RndID(MINID,MAXID);
        Random RndCar(0,4);// selects one from the carModels array
        Random RndQuantity(0,50);
        CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);
        
        for (int i = 0; i < 10; i++){
            // generating non colliding data
            Car dataObj = Car(string(1, 'B'), RndQuantity.getRandNum(), RndID.getRandNum(), true);
            // inserting data in to the CarDB object
            cardb.insert(dataObj);
            dataList.push_back(dataObj);
        }

        if (cardb.m_currentSize != 10){
            return false; 
        }

        for (int i = 0; i < 26; i++){
            // generating non colliding data
            Car dataObj = Car(string(1, 'A'), RndQuantity.getRandNum(), RndID.getRandNum(), true);
            // inserting data in to the CarDB object
            cardb.insert(dataObj);
            dataList.push_back(dataObj);
        }

        if (cardb.m_currentSize != 36){
            return false; 
        }

        // attempt to find each inserted item
        for (vector<Car>::iterator it = dataList.begin(); it != dataList.end(); it++){
            Car result = cardb.getCar((*it).getModel(), (*it).getDealer());
            if (!(result == *it)) {
                return false;
            }
        }
        
        return true; 
    }

    bool removeInOld() {
        cout << "TEST 26 RESULTS:" << endl;
        vector<Car> dataList;
        Random RndID(MINID, MAXID);
        Random RndCar(0, 4);  // selects one from the carModels array
        Random RndQuantity(0, 50);
        CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);

        // insert just enough to start rehashing
        for (int i = 0; i < 51; i++) {
            Car dataObj = Car(string(1, 'A'), RndQuantity.getRandNum(), RndID.getRandNum(), true);
            cardb.insert(dataObj);
            dataList.push_back(dataObj);
        }

        // Check if rehashing has started
        if (cardb.m_oldTable == nullptr) {
            return false; 
        }

        // remove a item in the old table
        if (!cardb.remove(dataList[30])){
            return false;
        }

        // 12 + 12 from rehash plus the one we deleted manually
        if (cardb.m_oldNumDeleted != 25){
            return false; 
        }
        // try to find the removed object; it should not be there
        if (!(cardb.getCar(dataList[30].getModel(), dataList[30].getDealer()) == EMPTY)) {
            return false;
        }
        return true; 
    }

    bool getInOld() {
        cout << "TEST 27 RESULTS:" << endl;
        vector<Car> dataList;
        Random RndID(MINID, MAXID);
        Random RndCar(0, 4);  // selects one from the carModels array
        Random RndQuantity(0, 50);
        CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);

        // insert just enough to start rehashing
        for (int i = 0; i < 51; i++) {
            Car dataObj = Car(string(1, 'A'), RndQuantity.getRandNum(), RndID.getRandNum(), true);
            cardb.insert(dataObj);
            dataList.push_back(dataObj);
        }

        // Check if rehashing has started
        if (cardb.m_oldTable == nullptr) {
            return false; 
        }

        // try to find an object in the old array
        if (cardb.getCar(dataList[30].getModel(), dataList[30].getDealer()) == EMPTY) {
            return false;
        }
        return true; 
    }

    bool updateInOld() {
        cout << "TEST 28 RESULTS:" << endl;
        vector<Car> dataList;
        Random RndID(MINID, MAXID);
        Random RndCar(0, 4);  // selects one from the carModels array
        Random RndQuantity(0, 50);
        CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);

        // insert just enough to start rehashing
        for (int i = 0; i < 51; i++) {
            Car dataObj = Car(string(1, 'A'), RndQuantity.getRandNum(), RndID.getRandNum(), true);
            cardb.insert(dataObj);
            dataList.push_back(dataObj);
        }

        // Check if rehashing has started
        if (cardb.m_oldTable == nullptr) {
            return false; 
        }
        
    
        // try to update an object in the old array
        if (!(cardb.updateQuantity(dataList[30], 10))) {
            return false; 
        }

        dataList[30].setQuantity(10);

        // make sure that the quantity was updated correctly
        Car result = cardb.getCar(dataList[30].getModel(), dataList[30].getDealer());

        if (!(result.getQuantity() == dataList[30].getQuantity())) {
            return false;
        }

        return true; 
    }

    bool replace() {
        cout << "TEST 29 RESULTS:" << endl;
        vector<Car> dataList;
        Random RndID(MINID, MAXID);
        Random RndCar(0, 4);  // selects one from the carModels array
        Random RndQuantity(0, 50);
        CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);

        for (int i = 0; i < 10; i++){
            // generating non colliding data
            Car dataObj = Car(to_string(i), RndQuantity.getRandNum(), RndID.getRandNum(), true);
            // inserting data in to the CarDB object
            cardb.insert(dataObj);
        }

        // insert an item
        Car dataObj = Car(string(1, 'A'), RndQuantity.getRandNum(), RndID.getRandNum(), true);
        cardb.insert(dataObj);
        dataList.push_back(dataObj);

        // remove the inserted item
        if (!cardb.remove(dataObj)) {
            return false;
        }

        if (cardb.m_currNumDeleted != 1){
            return false;
        }

        // reinsert the item
        cardb.insert(dataObj);

        if (cardb.m_currNumDeleted != 0){
            return false;
        }

        if (cardb.m_currentSize!= 11){
            return false;
        }

        return true; 
    }

    bool getCarEdge() {
        cout << "TEST 30 RESULTS:" << endl;
        vector<Car> dataList;
        Random RndID(MINID, MAXID);
        Random RndCar(0, 4);  // selects one from the carModels array
        Random RndQuantity(0, 50);
        CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);

        // insert some data
        for (int i = 0; i < 26; i++) {
            // generating non-colliding data
            Car dataObj = Car(string(1, 'A' + i), RndQuantity.getRandNum(), RndID.getRandNum(), true);
            // inserting data into the CarDB object
            dataList.push_back(dataObj);
            cardb.insert(dataObj);
        }

        // remove this car object
        cardb.remove(dataList[10]);
        // make sure getcar returns false
        if (!(cardb.getCar(dataList[10].getModel(), dataList[10].getDealer()) == EMPTY)) {
            return false;
        }

        return true;
    }

    /******************************************************************************************************************************************
                    THE SAME TESTS BUT NOW CHANGED TO NONE PROBING POLICY INSTEAD OF DOUBLEHASH
    ******************************************************************************************************************************************/

    bool insertNormalNone() {
        cout << "TEST 31 RESULTS:" << endl;
        Random RndID(MINID,MAXID);
        Random RndCar(0,4);// selects one from the carModels array
        Random RndQuantity(0,50);
        CarDB cardb(MINPRIME, hashCode, NONE);
        
        for (int i = 0; i < 10; i++){
            // generating non colliding data
            Car dataObj = Car(to_string(i), RndQuantity.getRandNum(), RndID.getRandNum(), true);
            // inserting data in to the CarDB object
            cardb.insert(dataObj);

            int index = hashCode(to_string(i)) % cardb.m_currentCap;
            if (!(cardb.m_currentTable[index] == dataObj)) {
                return false;
            }
        }

        if (cardb.m_currentSize != 10){
            return false; 
        }

        for (int i = 0; i < 26; i++){
            // generating non colliding data
            Car dataObj = Car(string(1, 'A' + i), RndQuantity.getRandNum(), RndID.getRandNum(), true);
            // inserting data in to the CarDB object
            cardb.insert(dataObj);

            int index = hashCode(string(1, 'A' + i)) % cardb.m_currentCap;
            if (!(cardb.m_currentTable[index] == dataObj)) {
                return false;
            }
        }

        if (cardb.m_currentSize != 36){
            return false; 
        }

        return true; 
    }

    bool insertCollidingNone() {
        cout << "TEST 32 RESULTS:" << endl;
        vector<Car> dataList;
        Random RndID(MINID,MAXID);
        Random RndCar(0,4);// selects one from the carModels array
        Random RndQuantity(0,50);
        CarDB cardb(MINPRIME, hashCode, NONE);
        
        // insert one instance
        Car dataObj1 = Car(string(1, 'B'), RndQuantity.getRandNum(), RndID.getRandNum(), true);
        cardb.insert(dataObj1);
        dataList.push_back(dataObj1);

        for (int i = 0; i < 10; i++){
            // generating non colliding data
            Car dataObj = Car(string(1, 'B'), RndQuantity.getRandNum(), RndID.getRandNum(), true);
            // inserting data in to the CarDB object
            if (cardb.insert(dataObj)){
                return false; 
            }
        }

        if (cardb.m_currentSize != 1){
            return false; 
        }

        // insert another instance
        Car dataObj2 = Car(string(1, 'A'), RndQuantity.getRandNum(), RndID.getRandNum(), true);
        cardb.insert(dataObj2);
        dataList.push_back(dataObj2);

        for (int i = 0; i < 26; i++){
            // generating non colliding data
            Car dataObj = Car(string(1, 'A'), RndQuantity.getRandNum(), RndID.getRandNum(), true);
            // inserting data in to the CarDB object
            cardb.insert(dataObj);
            if (cardb.insert(dataObj)){
                return false; 
            }
        }

        if (cardb.m_currentSize != 2){
            return false; 
        }

        // attempt to find each inserted item
        for (vector<Car>::iterator it = dataList.begin(); it != dataList.end(); it++){
            Car result = cardb.getCar((*it).getModel(), (*it).getDealer());
            if (!(result == *it)) {
                return false;
            }
        }
        
        return true; 
    }

    private:
    
    /**********************************************
    * If we need helper functions to be reused in *
    *   test functions they can be declared here!
    **********************************************/
};

int main(){
    Tester tester;

    cout << endl << "\033[1;31m=============================|Test Cases for the DEALER Class for DOUBLEHASH|==============================\033[0m" << endl;

    cout << "________________________________________________________" << endl; 

    cout << "Test the insertion operation for a normal case in the hash table." << endl; 

    if (tester.insertNormal()) {
        cout << "\033[1;32mSUCCESS\033[0m insertion passed for a normal test"<< endl;
    } 
    else {
        cout << "FAILURE: insertion failed for a normal test" << endl;
    }

    cout << "________________________________________________________" << endl; 

    cout << "Test the find operation (getCar(...) function) for an error case, the Car object does not exist in the database." << endl; 

    if (tester.getCarError()) {
        cout << "\033[1;32mSUCCESS\033[0m getCar passed for a error test"<< endl;
    } 
    else {
        cout << "FAILURE: getCar failed for a error test" << endl;
    }

    cout << "________________________________________________________" << endl; 

    cout << "Test the find operation (getCar(...) function) with several non-colliding keys." << endl; 

    if (tester.getCarNonColliding()) {
        cout << "\033[1;32mSUCCESS\033[0m getCar passed for a normal test"<< endl;
    } 
    else {
        cout << "FAILURE: getCar failed for a normal test" << endl;
    }

    cout << "________________________________________________________" << endl; 

    cout << "Test the find operation (getCar(...) function) with several colliding keys without triggering a rehash. This also tests whether the insertion works correctly with colliding data." << endl; 

    if (tester.getCarColliding()) {
        cout << "\033[1;32mSUCCESS\033[0m getCar passed for a normal test"<< endl;
    } 
    else {
        cout << "FAILURE: getCar failed for a normal test" << endl;
    }

    cout << "________________________________________________________" << endl; 

    cout << "Test the remove operation with a few non-colliding keys." << endl; 

    if (tester.removeNonColliding()) {
        cout << "\033[1;32mSUCCESS\033[0m remove passed for a normal test"<< endl;
    } 
    else {
        cout << "FAILURE: remove failed for a normal test" << endl;
    }

    cout << "________________________________________________________" << endl; 

    cout << "Test the remove operation with a number of colliding keys without triggering a rehash." << endl; 

    if (tester.removeColliding()) {
        cout << "\033[1;32mSUCCESS\033[0m remove passed for a normal test"<< endl;
    } 
    else {
        cout << "FAILURE: remove failed for a normal test" << endl;
    }

    cout << "________________________________________________________" << endl; 

    cout << "Test the rehashing is triggered after a descent number of data insertion." << endl; 

    if (tester.insertRehash()) {
        cout << "\033[1;32mSUCCESS\033[0m rehash passed for a normal test"<< endl;
    } 
    else {
        cout << "FAILURE: rehash failed for a normal test" << endl;
    }

    cout << "________________________________________________________" << endl; 

    cout << "Test the rehash completion after triggering rehash due to load factor, i.e. all live data is transferred to the new table and the old table is removed." << endl; 

    if (tester.insertRehashComplete()) {
        cout << "\033[1;32mSUCCESS\033[0m rehash passed for a normal test"<< endl;
    } 
    else {
        cout << "FAILURE: rehash failed for a normal test" << endl;
    }

    cout << "________________________________________________________" << endl; 

    cout << "Test the rehashing is triggered after a descent number of data removal." << endl; 

    if (tester.removeRehash()) {
        cout << "\033[1;32mSUCCESS\033[0m rehash passed for a normal test"<< endl;
    } 
    else {
        cout << "FAILURE: rehash failed for a normal test" << endl;
    }
    
    cout << "________________________________________________________" << endl; 

    cout << "Test the rehash completion after triggering rehash due to delete ratio, i.e. all live data is transferred to the new table and the old table is removed." << endl; 

    if (tester.removeRehashComplete()) {
        cout << "\033[1;32mSUCCESS\033[0m rehash passed for a normal test"<< endl;
    } 
    else {
        cout << "FAILURE: rehash failed for a normal test" << endl;
    }
    
    cout << endl << "\033[1;34m=============================|Test Cases for the DEALER Class for QUADRATIC|==============================\033[0m" << endl;

    cout << "________________________________________________________" << endl; 

    cout << "Test the insertion operation for a normal case in the hash table." << endl; 

    if (tester.insertNormalQuad()) {
        cout << "\033[1;32mSUCCESS\033[0m insertion passed for a normal test"<< endl;
    } 
    else {
        cout << "FAILURE: insertion failed for a normal test" << endl;
    }

    cout << "________________________________________________________" << endl; 

    cout << "Test the find operation (getCar(...) function) for an error case, the Car object does not exist in the database." << endl; 

    if (tester.getCarErrorQuad()) {
        cout << "\033[1;32mSUCCESS\033[0m getCar passed for a error test"<< endl;
    } 
    else {
        cout << "FAILURE: getCar failed for a error test" << endl;
    }

    cout << "________________________________________________________" << endl; 

    cout << "Test the find operation (getCar(...) function) with several non-colliding keys." << endl; 

    if (tester.getCarNonCollidingQuad()) {
        cout << "\033[1;32mSUCCESS\033[0m getCar passed for a normal test"<< endl;
    } 
    else {
        cout << "FAILURE: getCar failed for a normal test" << endl;
    }

    cout << "________________________________________________________" << endl; 

    cout << "Test the find operation (getCar(...) function) with several colliding keys without triggering a rehash. This also tests whether the insertion works correctly with colliding data." << endl; 

    if (tester.getCarCollidingQuad()) {
        cout << "\033[1;32mSUCCESS\033[0m getCar passed for a normal test"<< endl;
    } 
    else {
        cout << "FAILURE: getCar failed for a normal test" << endl;
    }

    cout << "________________________________________________________" << endl; 

    cout << "Test the remove operation with a few non-colliding keys." << endl; 

    if (tester.removeNonCollidingQuad()) {
        cout << "\033[1;32mSUCCESS\033[0m remove passed for a normal test"<< endl;
    } 
    else {
        cout << "FAILURE: remove failed for a normal test" << endl;
    }

    cout << "________________________________________________________" << endl; 

    cout << "Test the remove operation with a number of colliding keys without triggering a rehash." << endl; 

    if (tester.removeCollidingQuad()) {
        cout << "\033[1;32mSUCCESS\033[0m remove passed for a normal test"<< endl;
    } 
    else {
        cout << "FAILURE: remove failed for a normal test" << endl;
    }

    cout << "________________________________________________________" << endl; 

    cout << "Test the rehashing is triggered after a descent number of data insertion." << endl; 

    if (tester.insertRehashQuad()) {
        cout << "\033[1;32mSUCCESS\033[0m rehash passed for a normal test"<< endl;
    } 
    else {
        cout << "FAILURE: rehash failed for a normal test" << endl;
    }

    cout << "________________________________________________________" << endl; 

    cout << "Test the rehash completion after triggering rehash due to load factor, i.e. all live data is transferred to the new table and the old table is removed." << endl; 

    if (tester.insertRehashCompleteQuad()) {
        cout << "\033[1;32mSUCCESS\033[0m rehash passed for a normal test"<< endl;
    } 
    else {
        cout << "FAILURE: rehash failed for a normal test" << endl;
    }

    cout << "________________________________________________________" << endl; 

    cout << "Test the rehashing is triggered after a descent number of data removal." << endl; 

    if (tester.removeRehashQuad()) {
        cout << "\033[1;32mSUCCESS\033[0m rehash passed for a normal test"<< endl;
    } 
    else {
        cout << "FAILURE: rehash failed for a normal test" << endl;
    }
    
    cout << "________________________________________________________" << endl; 

    cout << "Test the rehash completion after triggering rehash due to delete ratio, i.e. all live data is transferred to the new table and the old table is removed." << endl; 

    if (tester.removeRehashCompleteQuad()) {
        cout << "\033[1;32mSUCCESS\033[0m rehash passed for a normal test"<< endl;
    } 
    else {
        cout << "FAILURE: rehash failed for a normal test" << endl;
    }
    
    cout << endl << "\033[1;33m=============================|Additional Test Cases for the DEALER Class|==============================\033[0m" << endl;
    
    cout << "________________________________________________________" << endl; 

    cout << "Test that the changeProbPolicy function is working" << endl; 

    if (tester.insertRehashChange()) {
        cout << "\033[1;32mSUCCESS\033[0m changeProbPolicy passed for a normal test"<< endl;
    } 
    else {
        cout << "FAILURE: changeProbPolicy failed for a normal test" << endl;
    }

    cout << "________________________________________________________" << endl; 

    cout << "Test that the updateQuantity function is working for a normal test" << endl; 

    if (tester.updateQuantityNormal()) {
        cout << "\033[1;32mSUCCESS\033[0m updateQuantity passed for a normal test"<< endl;
    } 
    else {
        cout << "FAILURE: updateQuantity failed for a normal test" << endl;
    }

    cout << "________________________________________________________" << endl; 

    cout << "Test that the updateQuantity function is working for a edge test where it tries to update a item not in the database" << endl; 

    if (tester.updateQuantityEdge()) {
        cout << "\033[1;32mSUCCESS\033[0m updateQuantity passed for a edge test"<< endl;
    } 
    else {
        cout << "FAILURE: updateQuantity failed for a edge test" << endl;
    }

    cout << "________________________________________________________" << endl; 

    cout << "Test that the constructor function is working for a normal case" << endl; 

    if (tester.constructorNormal()) {
        cout << "\033[1;32mSUCCESS\033[0m constructor passed for a normal test"<< endl;
    } 
    else {
        cout << "FAILURE: constructor failed for a normal test" << endl;
    }

    cout << "________________________________________________________" << endl; 

    cout << "Test the insertion operation for a normal case in the hash table where data collides." << endl; 

    if (tester.insertColliding()) {
        cout << "\033[1;32mSUCCESS\033[0m insertion passed for a normal test"<< endl;
    } 
    else {
        cout << "FAILURE: insertion failed for a normal test" << endl;
    }

    cout << "________________________________________________________" << endl; 

    cout << "Test the removal operation for a case where the item is in the old table" << endl; 

    if (tester.removeInOld()) {
        cout << "\033[1;32mSUCCESS\033[0m remove passed for a normal test"<< endl;
    } 
    else {
        cout << "FAILURE: remove failed for a normal test" << endl;
    }

    cout << "________________________________________________________" << endl; 

    cout << "Test the find operation (getCar(...) function) for a case where the item is in the old table" << endl; 

    if (tester.getInOld()) {
        cout << "\033[1;32mSUCCESS\033[0m getCar passed for a normal test"<< endl;
    } 
    else {
        cout << "FAILURE: getCar failed for a normal test" << endl;
    }

    cout << "________________________________________________________" << endl; 

    cout << "Test the updateQuantity for a case where the item is in the old table" << endl; 

    if (tester.updateInOld()) {
        cout << "\033[1;32mSUCCESS\033[0m updateQuantity passed for a normal test"<< endl;
    } 
    else {
        cout << "FAILURE: updateQuantity failed for a normal test" << endl;
    }

    cout << "________________________________________________________" << endl; 

    cout << "Test the insert/remove for a case where the item is replaced" << endl; 

    if (tester.replace()) {
        cout << "\033[1;32mSUCCESS\033[0m insert/remove passed for a normal test"<< endl;
    } 
    else {
        cout << "FAILURE: insert/remove failed for a normal test" << endl;
    }

    cout << "________________________________________________________" << endl; 

    cout << "Test the getCar function for a case where the item is removed and an attempt is made to find it" << endl; 

    if (tester.getCarEdge()) {
        cout << "\033[1;32mSUCCESS\033[0m getCar passed for a edge test"<< endl;
    } 
    else {
        cout << "FAILURE: getCar failed for a egde test" << endl;
    }


    cout << endl << "\033[1;34m=============================|Test Cases for the DEALER Class for NONE|==============================\033[0m" << endl;

    cout << "________________________________________________________" << endl; 

    cout << "Test the insertion operation for a normal case in the hash table." << endl; 

    if (tester.insertNormalNone()) {
        cout << "\033[1;32mSUCCESS\033[0m insertion passed for a normal test"<< endl;
    } 
    else {
        cout << "FAILURE: insertion failed for a normal test" << endl;
    }

    cout << "________________________________________________________" << endl; 

    cout << "Test the insertion operation for a normal case in the hash table where data collides." << endl; 

    if (tester.insertCollidingNone()) {
        cout << "\033[1;32mSUCCESS\033[0m insertion passed for a normal test"<< endl;
    } 
    else {
        cout << "FAILURE: insertion failed for a normal test" << endl;
    }
}