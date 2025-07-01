#include <iostream>

using namespace std;

class Base1 {
    public:
        Base1();
        Base1(int base1Int);
        Base1(const Base1 & other);
        Base1& operator=(Base1 & other);
        virtual ~Base1();
        void printVal1();
    private:
        int val1;
};

Base1::Base1() : val1(0){
    cout << "Base1() constructor called.\n";
}

Base1::Base1(int base1Int) : val1(base1Int){
    cout << "Base1(int) constructor called with base1Int = " << base1Int << "\n";
}

Base1::Base1(const Base1 & other) : val1(other.val1) {
    cout << "Base1 copy constructor\n";
}

Base1& Base1::operator=(Base1 & other){
    if(this != &other){
        val1 = other.val1;
        cout << "Base1 assignment operator called without self-assignment\n";
    } else {
        cout << "Base1 assignment operator called with self-assignment\n";
    }
    return *this;
}

Base1::~Base1(){
    cout << "Base1 destructor called.\n";
}

void Base1::printVal1(){
    cout << "Printing val1 in Base1: " << val1 << "\n";
}

class Base2 {
    public:
        Base2();
        Base2(char base2Char);
        Base2(const Base2 & other);
        Base2& operator=(Base2 & other);
        virtual ~Base2();
        void printVal2();
    private:
        char val2;
};

Base2::Base2() : val2('a') {
    cout << "Base2() constructor called.\n";
}

Base2::Base2(char base2Char) : val2(base2Char) {
    cout << "Base2(char) constructor called with base2Char = " << base2Char << "\n";
}

Base2::Base2(const Base2 & other) : val2(other.val2) {
    cout << "Base2 copy constructor\n";
}

Base2& Base2::operator=(Base2 & other){
    if(this != &other){
        val2 = other.val2;
        cout << "Base2 assignment operator called without self-assignment\n";
    } else {
        cout << "Base2 assignment operator called with self-assignment\n";
    }
    return *this;
}

Base2::~Base2(){
    cout << "Base2 destructor called.\n";
}

void Base2::printVal2(){
    cout << "Printing val2 in Base2: " << val2 << "\n";
}

class Derived : public Base1, public Base2 {
    public:
        Derived();
        Derived(int derInt);
        Derived(int derInt, char derChar);
        Derived(const Derived & other);
        Derived& operator=(Derived & other);
        ~Derived();
        void printDer();
};

Derived::Derived(){
    cout << "Derived() constructor called.\n";
}

// Calls Base1() and Base2()
Derived::Derived(int derInt){
    cout << "Derived(int) constructor called with derInt = " << derInt << "\n";
}

/*
// Explicit call uses the constructor
Derived::Derived(int derInt) : Base1(derInt) {
    cout << "Derived(int) constructor called with derInt = " << derInt << "\n";
}
*/

/*
// Cannot call out of order without warnings and it doesn't call it in the order here
Derived::Derived(int derInt) : Base2(), Base1(0) {
    cout << "Derived(int) constructor called with derInt = " << derInt << "\n";
}
*/

/*
// Calls Base1() and Base2()
Derived::Derived(int derInt, char derChar){
    cout << "Derived(int, char) constructor called with derInt = " << derInt << " and derChar = " << derChar << "\n";
}
*/

// Calls as expected
Derived::Derived(int derInt, char derChar) : Base1(derInt), Base2(derChar){
    cout << "Derived(int, char) constructor called with derInt = " << derInt << " and derChar = " << derChar << "\n";
}

/*
// Order is enforced, but calls are correct
Derived::Derived(int derInt, char derChar) : Base2(derChar), Base1(derInt){
    cout << "Derived(int, char) constructor called with derInt = " << derInt << " and derChar = " << derChar << "\n";
}
*/

Derived::Derived(const Derived & other) : Base1(other), Base2(other){
    cout << "Derived copy constructor\n";
}

Derived& Derived::operator=(Derived & other){
    if(this != &other){
        this->Base1::operator=(other);
        this->Base2::operator=(other);
        cout << "Derived assignment operator called without self-assignment\n";
    } else {
        cout << "Derived assignment operator called with self-assignment\n";
    }
    return *this;
}

Derived::~Derived(){
    cout << "Derived destructor called.\n";
}

void Derived::printDer(){
    printVal1();
    printVal2();
    cout << "Doing printDer()\n";
}

// Super basic testing for low level coding issues
int main(int argc, char* argv[]){
    /*
    // Testing using modspace enforce integer wrapping to simplify generalized periodic boundaries
    int val = 5;
    int max = 7;

    val = (val + 5) % max;
    cout << "Actual: " << val << " Expected: 3\n";
    
    val = 3;
    val = (val - 5) % max;
    cout << "Actual: " << val << " Expected: 5\n";
    
    val = 5;
    val = (val + 7) % max;
    cout << "Actual: " << val << " Expected: 5\n";

    val = 5;
    val = (val - 7) % max;
    cout << "Actual: " << val << " Expected: 5\n";

    val = 5;
    val = (val - 14) % max;
    cout << "Actual: " << val << " Expected: 5\n";
    */

    cout << "--------------------\n";
    cout << "Constructor test\n";
    cout << "--------------------\n";
    Derived test1 = Derived(5, 'c');
    cout << "\n\n";

    cout << "--------------------\n";
    cout << "Copy Constructor test\n";
    cout << "--------------------\n";
    Derived test2 = Derived(test1);
    cout << "\n\n";
    
    Derived test3;
    cout << "--------------------\n";
    cout << "Assignment operator test\n";
    cout << "--------------------\n";
    test3 = test1;
    test3.printDer();
    cout << "\n\n";

    cout << "--------------------\n";
    Base1* base1Ptr = new Derived(5, 'c');
    cout << "--------------------\n";
    cout << "Base1 pointer test\n";
    cout << "--------------------\n";
    delete(base1Ptr);
    base1Ptr = nullptr;
    
    cout << "--------------------\n";
    Base2* base2Ptr = new Derived(5, 'c');
    cout << "--------------------\n";
    cout << "Base2 pointer test\n";
    cout << "--------------------\n";
    delete(base2Ptr);
    base2Ptr = nullptr;
    cout << "--------------------\n";
    return 0;
}