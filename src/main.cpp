#include <iostream>
#include "BpTree.hpp"
#include "vector.hpp"
#include "map.hpp"
#include "User.hpp"
#include "Train.hpp"
#include "Run.hpp"
#include "Date.hpp"
using namespace std;
//using namespace sjtu;

USER_ALL user_all;
TRAIN_ALL trainAll;

int main(){
    std::ios::sync_with_stdio(false);
    while(run_program()) {}
    return 0;
}