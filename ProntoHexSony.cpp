#include "ProntoHexSony.h"

ProntoHexSony::ProntoHexSony(/* args */)
{
}

ProntoHexSony::ProntoHexSony(ProntoHex pronto)
{
    m_filename = pronto.GetFilename();
    m_original_file_contents = pronto.GetOriginalData();
    m_cleaned_file_contents = pronto.GetCleanedData();

    raw_bytes.clear();
    for(int i = 0; i < pronto.raw_bytes.size(); i++){
        raw_bytes.push_back(pronto.raw_bytes[i]);
    }
}

ProntoHexSony::~ProntoHexSony()
{
}

std::string ProntoHexSony::GetPreamble(){
    if(raw_bytes.size() < 8){
        throw runtime_error("Preamble Invalid, size less than 8");
    }
    if(raw_bytes[0] != 0 || raw_bytes[1] != 0){
        throw runtime_error("Preamble Invalid, first 2 bytes not 0000.");
    }
    char hexstr[20];
    int i;
    for (i=0; i<4; i++) {
        sprintf(hexstr+i*5, "%02X%02X ", raw_bytes[i*2], raw_bytes[i*2+1]);
    }
    hexstr[19] = 0; 
    return string(hexstr);
}



string ProntoHexSony::GetSonySequenceLeadInPair(int sequence){

    if(raw_bytes.size() < 8){
        throw runtime_error("Preamble Invalid, size less than 8");
    }
    if(sequence != 1 && sequence != 2){
        throw runtime_error("Cannot request sequence " + to_string(sequence));
    }
    int start_index = GetSequenceStartIndex(sequence);

    char hexstr[10];
    sprintf(hexstr,"%02X%02X %02X%02X",raw_bytes[start_index],raw_bytes[start_index+1],raw_bytes[start_index+2],raw_bytes[start_index+3]);
    hexstr[9] = 0;
    return string(hexstr);
}

string ProntoHexSony::GetSonySequenceLeadOutPair(int sequence){
    
    if(raw_bytes.size() < 8){
        throw runtime_error("Preamble Invalid, size less than 8");
    }
    if(sequence != 1 && sequence != 2){
        throw runtime_error("Cannot request sequence " + to_string(sequence));
    }
    int start_index = GetSequenceStartIndex(sequence);
    // printf("start index: %d\n", start_index);
    int end_index = start_index + (GetSequenceLength(sequence)-1)*4;
    // printf("end index: %d\n", end_index);
    char hexstr[10];
    sprintf(hexstr,"%02X%02X %02X%02X",raw_bytes[end_index],raw_bytes[end_index+1],raw_bytes[end_index+2],raw_bytes[end_index+3]);
    hexstr[9] = 0;
    return string(hexstr);
}




string ProntoHexSony::GetSonySequenceString(int sequence, bool overridecache){
    if(raw_bytes.size() < 8){
        throw runtime_error("Preamble Invalid, size less than 8");
    }
    if(sequence != 1 && sequence != 2){
        throw runtime_error("Cannot request sequence " + to_string(sequence));
    }
    int start_index = GetSequenceStartIndex(sequence) + 4;
    // printf("start index: %d\n", start_index);
    int size = (GetSequenceLength(sequence)-2);
    // printf("size: %d\n", size);
    
    if(m_sequence_strings[sequence-1] != "" && !overridecache){
        // printf("Loading from cache\n");
        return m_sequence_strings[sequence-1];
    }
    // printf("Calculating Sequence String\n");
    char hexstr[size+2];

    for (int i=0; i<size; i++) {
        // printf("%d %d %d %d \n", start_index+i*4, start_index+i*4+1,start_index+i*4+2, start_index+i*4+3);
        // printf("%02X %02X %02X %02X \n", raw_bytes[start_index+i*4], raw_bytes[start_index+i*4+1],raw_bytes[start_index+i*4+2], raw_bytes[start_index+i*4+3]);
        if(raw_bytes[start_index+i*4] == 0x00 && raw_bytes[start_index+i*4+1] == 0x18 && raw_bytes[start_index+i*4+2] == 0x00 && raw_bytes[start_index+i*4+3] == 0x18){
            //printf("0\n");  
            sprintf(hexstr+i, "0");

        }else if(raw_bytes[start_index+i*4] == 0x00 && raw_bytes[start_index+i*4+1] == 0x30 && raw_bytes[start_index+i*4+2] == 0x00 && raw_bytes[start_index+i*4+3] == 0x18){
            // printf("1\n");    
            sprintf(hexstr+i, "1");
        }
        // printf("\n");        
    }

    sprintf(hexstr+size,"0");
    hexstr[size+1]=0;
    m_sequence_strings[sequence-1] = string(hexstr);
    return m_sequence_strings[sequence-1];
}


int ProntoHexSony::GetSonySequenceKeycode(int sequence, bool overridecache){
    if(raw_bytes.size() < 8){
        throw runtime_error("Preamble Invalid, size less than 8");
    }
    if(sequence != 1 && sequence != 2){
        throw runtime_error("Cannot request sequence " + to_string(sequence));
    }

    if(m_keycodes[sequence-1] != -1 && !overridecache){
        // printf("Loading from cache\n");
        return m_keycodes[sequence-1];
    }

    string sequencestring = GetSonySequenceString(sequence);
    // printf("%s\n", sequencestring.c_str());
    string keystring = sequencestring.substr(0,7);
    // printf("%s\n", keystring.c_str());
    reverse(keystring.begin(), keystring.end());
    // printf("%s\n", keystring.c_str());
    
	m_keycodes[sequence-1] = stoi(keystring, 0, 2);
    return m_keycodes[sequence-1];
}


int ProntoHexSony::GetSonySequenceDevicecode(int sequence, bool overridecache){
    if(raw_bytes.size() < 8){
        throw runtime_error("Preamble Invalid, size less than 8");
    }
    if(sequence != 1 && sequence != 2){
        throw runtime_error("Cannot request sequence " + to_string(sequence));
    }

    if(m_devicecodes[sequence-1] != -1 && !overridecache){
        // printf("Loading from cache\n");
        return m_keycodes[sequence-1];
    }

    string sequencestring = GetSonySequenceString(sequence);
    //  printf("%s\n", sequencestring.c_str());
    string keystring = sequencestring.substr(7,GetSequenceLength(sequence)-8);
    //  printf("%s\n", keystring.c_str());
    reverse(keystring.begin(), keystring.end());
    //  printf("%s\n", keystring.c_str());
    
	m_devicecodes[sequence-1] = stoi(keystring, 0, 2);
    return m_devicecodes[sequence-1];
}