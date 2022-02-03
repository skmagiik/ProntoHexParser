#include "ProntoHex.h"

ProntoHex::ProntoHex(/* args */)
{
}

ProntoHex::ProntoHex(std::string filename)
{   
    m_filename = filename;
    std::ifstream t(filename);
    if(!t.is_open())
    {
        // error! maybe the file doesn't exist.
        //printf("%s !is_open()\n", filename.c_str());
        m_isCreated = false;
        return;
    }
    std::stringstream buffer;
    buffer << t.rdbuf();
    m_original_file_contents = buffer.str();
    m_cleaned_file_contents = buffer.str();
    // Remove all whitespace from the original file contents
    m_cleaned_file_contents.erase(std::remove_if(m_cleaned_file_contents.begin(), 
                            m_cleaned_file_contents.end(),
                            [](unsigned char x){return std::isspace(x);}),
            m_cleaned_file_contents.end());

    raw_bytes = explode(m_cleaned_file_contents);
    m_isCreated = true;
};

ProntoHex::~ProntoHex()
{
}


bool ProntoHex::IsCreated(){
    return m_isCreated;
}
std::string ProntoHex::GetFilename(){
    return m_filename;
}
std::string ProntoHex::GetOriginalData(){
    return m_original_file_contents;
}
std::string ProntoHex::GetCleanedData(){
    return m_cleaned_file_contents;
}

std::string ProntoHex::GetPreamble(){
    if(raw_bytes.size() < 8){
        throw runtime_error("Preamble Invalid, size less than 8");
    }
    if(raw_bytes[0] != 0 || raw_bytes[1] != 0){
        throw runtime_error("Preamble Invalid, first 2 bytes not 0000.");
    }
    char hexstr[20];
    int i;
    for (i=0; i<2; i++) {
        sprintf(hexstr+i*5, "%02X%02X ", raw_bytes[i*2], raw_bytes[i*2+1]);
    }
    hexstr[19] = 0; 
    return string(hexstr);
}



vector<unsigned char> ProntoHex::explode(std::string input)
{
    //Expects already filtered and no whitespace string
	vector<unsigned char> bytes;
	size_t position = 0;
	string token;
	while (input.size() >= 2 ) {
		token = input.substr(0, 2);
		unsigned char byte = (unsigned char)strtol(token.c_str(), NULL, 16);
		bytes.push_back(byte);
		input.erase(0, 2);
	}
	// unsigned char byte = (unsigned char)strtol(str.c_str(), NULL, 16);
	// bytes.push_back(byte);
	return bytes;
}

void ProntoHex::PrintHex() {

	printf("Raw Hex: \r\n");
	for (uint16_t i = 0; i < raw_bytes.size(); i++) {
		printf("%02X", raw_bytes[i]);
		if ((i+1) % 16 == 0 && i > 0) {
			printf("\r\n");
		}
		else if (i < raw_bytes.size()) {
            if(i%2 == 1){
    			printf(" ");
            }
		}
	}
    printf("\n");
}

double ProntoHex::GetIRFrequency(){
    if(raw_bytes.size() < 8){
        throw runtime_error("Preamble Invalid, size less than 8");
    }
    uint16_t freq = (raw_bytes[2]<<8) + raw_bytes[3];
    if(freq == 0){
        throw runtime_error("Preamble Invalid, frequency = 0");
    }
    return (1000000/(freq * .241246));
}


double ProntoHex::GetIRPulseDuration(){
    return (1000000 / GetIRFrequency());
}

bool ProntoHex::GetAssumeSony(){
    return m_assumeSony;
}
void ProntoHex::SetAssumeSony(bool assume){
    m_assumeSony = assume;
}

int ProntoHex::GetSonyScore(){
    uint16_t burst1[] = {0x30,0x18};
    uint16_t burst0[] = {0x18,0x18};
    uint16_t leadin[] = {0x60,0x18};
    uint16_t leadout1[] = {0x18,0x03f6};
    uint16_t leadout2[] = {0x30,0x03f6};
    int score = 0;
    for(int i = 0; i < raw_bytes.size()/4; i++){
        uint16_t burstref[2];
        burstref[0] = (raw_bytes[i*4]<<8) + raw_bytes[i*4+1];
        burstref[1] = (raw_bytes[i*4+2]<<8) + raw_bytes[i*4+3];
        // printf("%04X %04X", burstref[0], burstref[1]);        
        if(burstref[0] == burst1[0] && burstref[1] == burst1[1]){
            // printf(" = 1\n");
            score += 1;
        }else if(burstref[0] == burst0[0] && burstref[1] == burst0[1]){
            // printf(" = 0\n");
            score += 1;
        }else if(burstref[0] == leadin[0] && burstref[1] == leadin[1]){
            // printf(" = Lead In\n");
            score += 1;
        }else if(burstref[0] == leadout1[0] && burstref[1] == leadout1[1]){
            // printf(" = Lead Out 1\n");
            score += 1;
        }else if(burstref[0] == leadout2[0] && burstref[1] == leadout2[1]){
            // printf(" = Lead Out 2\n");
            score += 1;
        }else{
            // printf(" ?\n");
        }
    }
    return score;
}

int ProntoHex::GetNECScore(){
    uint16_t burst1[] = {0x16,0x60};
    uint16_t burst0[] = {0x16,0x16};
    uint16_t leadin[] = {0x156,0xab};
    uint16_t leadout[] = {0x16,0x0593};
    int score = 0;
    for(int i = 0; i < raw_bytes.size()/4; i++){
        uint16_t burstref[2];
        burstref[0] = (raw_bytes[i*4]<<8) + raw_bytes[i*4+1];
        burstref[1] = (raw_bytes[i*4+2]<<8) + raw_bytes[i*4+3];
        // printf("%04X %04X", burstref[0], burstref[1]);
        if(burstref[0] == burst1[0] && burstref[1] == burst1[1]){
            // printf(" = 1\n");
            score += 1;
        }else if(burstref[0] == burst0[0] && burstref[1] == burst0[1]){
            // printf(" = 0\n");
            score += 1;
        }else if(burstref[0] == leadin[0] && burstref[1] == leadin[1]){
            // printf(" = Lead In\n");
            score += 1;
        }else if(burstref[0] == leadout[0] && burstref[1] == leadout[1]){
            // printf(" = Lead Out 1\n");
            score += 1;
        }else{
            // printf(" ?\n");
        }
    }
    return score;
}

bool ProntoHex::IsSequencePresent(int sequence){
    if(raw_bytes.size() < 8){
        throw runtime_error("Preamble Invalid, size less than 8");
    }
    if(sequence != 1 && sequence != 2){
        throw runtime_error("Cannot request sequence " + to_string(sequence));
    }
    return raw_bytes[2+sequence*2] > 0 || raw_bytes[3+sequence*2] > 0;
}


int ProntoHex::GetSequenceLength(int sequence){
    if(raw_bytes.size() < 8){
        throw runtime_error("Preamble Invalid, size less than 8");
    }
    if(sequence != 1 && sequence != 2){
        throw runtime_error("Cannot request sequence " + to_string(sequence));
    }
    uint16_t size = (raw_bytes[2+sequence*2]<<8) + raw_bytes[3+sequence*2];
    return size;
}



int ProntoHex::GetSequenceStartIndex(int sequence){

    if(raw_bytes.size() < 8){
        throw runtime_error("Preamble Invalid, size less than 8");
    }
    if(sequence != 1 && sequence != 2){
        throw runtime_error("Cannot request sequence " + to_string(sequence));
    }
    if(!IsSequencePresent(sequence)){
        throw runtime_error("Sequence " + to_string(sequence) + " is not found in the pronto hex data");
    }
    int start_index = 8;
    if(IsSequencePresent(1) && sequence == 2){
        start_index += GetSequenceLength(1)*4;
    }
    return start_index;
}


vector<unsigned char> ProntoHex::GetSequenceBytes(int sequence){
    if(sequence != 1 && sequence != 2){
        throw runtime_error("Cannot request sequence " + to_string(sequence));
    }
    if(!IsSequencePresent(sequence)){
        throw runtime_error("Sequence " + to_string(sequence) + " is not found in the pronto hex data");
    }

    int index = GetSequenceStartIndex(sequence);
    int length = GetSequenceLength(sequence);

    vector<unsigned char> results;
    for(int i = 0; i < length; i++){
        results.push_back(raw_bytes[index + i*4]);
        results.push_back(raw_bytes[index + i*4 + 1]);
        results.push_back(raw_bytes[index + i*4 + 2]);
        results.push_back(raw_bytes[index + i*4 + 3]);
    }
    return results;
}


vector<int> ProntoHex::GetSequenceTiming(int sequence, bool withsigns){
    if(sequence != 1 && sequence != 2){
        throw runtime_error("Cannot request sequence " + to_string(sequence));
    }
    if(!IsSequencePresent(sequence)){
        throw runtime_error("Sequence " + to_string(sequence) + " is not found in the pronto hex data");
    }

    vector<int> results;

    vector<BurstPair> sequencePairs = GetSequenceBurstPairs(sequence);
    for(int j = 0; j < sequencePairs.size(); j++){
        // printf("Burst pair %d: %d, %d\n",j, sequencePairs[j].TimeOn, sequencePairs[j].TimeOff);
        results.push_back(sequencePairs[j].TimeOn);
        results.push_back((withsigns ? -1 : 1) * sequencePairs[j].TimeOff);
    }

    return results;
}


vector<BurstPair> ProntoHex::GetSequenceBurstPairs(int sequence){
    if(sequence != 1 && sequence != 2){
        throw runtime_error("Cannot request sequence " + to_string(sequence));
    }
    if(!IsSequencePresent(sequence)){
        throw runtime_error("Sequence " + to_string(sequence) + " is not found in the pronto hex data");
    }

    int index = GetSequenceStartIndex(sequence);
    vector<unsigned char> bytes = GetSequenceBytes(sequence);

    vector<BurstPair> results;
    for(int i = 0; i < bytes.size(); i+= 4){
        int onpulses = (bytes[i]<<8) + bytes[i+1];
        int onvalue = round(onpulses * GetIRPulseDuration());
        int offpulses = (bytes[i+2]<<8) + bytes[i+3];
        int offvalue = round(offpulses * GetIRPulseDuration());
        BurstPair pair(onvalue, offvalue);
        results.push_back(pair);
    }
    return results;
}