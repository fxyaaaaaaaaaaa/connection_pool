#pragma once
#include<iostream>
using namespace std;
#define CLOG(str) \
        cout<<__FILE__<<":"<<__LINE__<<" "<<\
        __TIMESTAMP__<<" : "<<str<<endl;