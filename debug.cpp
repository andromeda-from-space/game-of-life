#include <iostream>

using namespace std;

// Super basic testing for low level coding issues
int main(int argc, char* argv[]){
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

    return 0;
}