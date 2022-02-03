#pragma once
#include <vector>
#include <cmath>


class BurstPair
{
public:
    BurstPair();
    BurstPair(int timeon, int timeoff);
    ~BurstPair();
    int TimeOn = 0;
    int TimeOff = 0;
};