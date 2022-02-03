#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <cmath>
#include "BurstPair.h"
using namespace std;

class ProntoHex
{
public:
    ProntoHex();
    ProntoHex(std::string filename);
    ~ProntoHex();
    void PrintHex();
    bool IsCreated();
	vector<unsigned char> raw_bytes = vector<unsigned char>();

    std::string GetFilename();
    std::string GetOriginalData();
    std::string GetCleanedData();
    virtual std::string GetPreamble();
    double GetIRFrequency();
    double GetIRPulseDuration();

    // Force parsing
    void SetAssumeSony(bool assume);
    bool GetAssumeSony();

    int GetSequenceLength(int sequence);
    int GetSequenceStartIndex(int sequence);
    bool IsSequencePresent(int sequence);
    vector<unsigned char> GetSequenceBytes(int sequence);
    vector<int> GetSequenceTiming(int sequence, bool withsigns = false);
    vector<BurstPair> GetSequenceBurstPairs(int sequence, bool verboseOutput = false, int verboseLevel = 5);


    // DO NOT USE!
    int GetSonyScore();
    int GetNECScore();
    
private:
    // DO NOT USE!
    bool m_assumeSony = true;
    
protected:
    bool m_isCreated = false;
    std::string m_filename;
    std::string m_original_file_contents;
    std::string m_cleaned_file_contents;
	vector<unsigned char> explode(const string str);
};