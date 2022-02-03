#pragma once
#include "ProntoHex.h"
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>

class ProntoHexSony : public ProntoHex
{
private:
    /* data */
    int GetSonySequenceStartIndex(int sequence);
    string m_sequence_strings[2] = {"",""};
    int m_keycodes[2] = {-1,-1};
    int m_devicecodes[2] = {-1,-1};

public:
    ProntoHexSony(/* args */);
    ProntoHexSony(ProntoHex pronto);
    ~ProntoHexSony();
    std::string GetPreamble();

    //Sony Specific
    bool IsSonySequencePresent(int sequence);
    string GetSonySequenceLeadInPair(int sequence);
    string GetSonySequenceString(int sequence, bool overridecache = false);
    string GetSonySequenceLeadOutPair(int sequence);
    int GetSonySequenceKeycode(int sequence, bool overridecache = false);
    int GetSonySequenceDevicecode(int sequence, bool overridecache = false);

};
