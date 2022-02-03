#include "InputParser.h"
#include "BurstPair.h"
#include "ProntoHex.h"
#include <fstream>
#include <sstream>

ProntoHex pronto;
enum OutputType{ NONE=0, IR_SCRUTINIZER, C_ARRAY};

int main(int argc, char **argv)
{
    OutputType outputType = OutputType::NONE;
    bool printSigns = true;
    bool skipSizeEval = false;
    static bool verboseOutput = false;
    static int verboseLevel = 5; 


    InputParser input(argc,argv);
    if(input.cmdOptionExists("-h")){
        printf("ProntoHex Help:\n");
        printf("  -f                       Filename with prontohex data to parse\n");
        printf("  -s, --skip-number-signs  Do not print positive/negative signs on timing information\n");
        printf("  -o FORMAT                Output at end with format\n");
        printf("                           FORMATS: irscrutinizer, c\n");
        printf("  --skip-size-eval         Do not error on invalid sizing of the prontohex data\n");
        printf("  --skip-all-errors        Do not halt the program when encountering errors in pronto hex parsing\n");
        printf("  -v LEVEL                 Enable verbose output. LEVEL is optional: 1 (most) to 5 (least, default)\n");
        return 0;
    }
    if(!input.cmdOptionExists("-f")){
        printf("Filename must be included with -f. See help via -h for more options.\n");
    }
    if(input.cmdOptionExists("-v")){
        if(input.getCmdOption("-v") == "1"){
            verboseLevel = 1;
        }else if(input.getCmdOption("-v") == "2"){
            verboseLevel = 2;
        }else if(input.getCmdOption("-v") == "3"){
            verboseLevel = 3;
        }else if(input.getCmdOption("-v") == "4"){
            verboseLevel = 4;
        }
        verboseOutput = true;
    }
    if(input.cmdOptionExists("-s") || input.cmdOptionExists("--skip-number-signs")){
        if(input.getCmdOption("-s") == "0"){
            printSigns = true;
        }else{
            printSigns = false;
        }
    }

    if(input.cmdOptionExists("-o")){
        if(input.getCmdOption("-o") == "irscrutinizer" || input.getCmdOption("-o") == "irs" || input.getCmdOption("-o") == "IRSCRUTINIZER"){
            outputType = OutputType::IR_SCRUTINIZER;
        }else if(input.getCmdOption("-o") == "c" || input.getCmdOption("-o") == "C" || input.getCmdOption("-o") == "arduino" || input.getCmdOption("-o") == "ARDUINO"){
            outputType = OutputType::C_ARRAY;
        }
    }

    if(input.cmdOptionExists("--skip-size-eval")){
        skipSizeEval = true;
    }
    if(input.cmdOptionExists("--skip-all-errors")){
        skipSizeEval = true;
    }

    const std::string &filename = input.getCmdOption("-f");
    if (!filename.empty()){
        pronto = ProntoHex(filename);
    }

    if(pronto.IsCreated()){
        printf("Filename: %s\n\n",pronto.GetFilename().c_str());
        printf("Original Data:\n%s\n",pronto.GetOriginalData().c_str());
        if(verboseOutput && verboseLevel <= 1){
            printf("\nCleaned Data:\n%s\n",pronto.GetCleanedData().c_str());
        }
        if(verboseOutput && verboseLevel <= 3){
            printf("\n");
            pronto.PrintHex();
        }
        printf("\n");
        int sequence1size = pronto.GetSequenceLength(1);
        int sequence2size = pronto.GetSequenceLength(2);
        int expectedsize = 8 + 4 * (sequence1size + sequence2size);
        int actualsize = (int)pronto.raw_bytes.size();
        
        if(verboseOutput){
            printf("Sequence 1 Size: %d\n", sequence1size);
            printf("Sequence 2 Size: %d\n", sequence2size);
            printf("Expected Size: %d\n", expectedsize);
            printf("Actual Size: %d\n", actualsize);
        }
        if(actualsize != expectedsize){
            printf("%s: Actual size does not match expected size\n",skipSizeEval ? "Warning" : "Error");
            if(!skipSizeEval){
                return -1;
            }
        }

        printf("\n");
        printf("Frequency: %0.2f Hz\n",pronto.GetIRFrequency());
        printf("Time per Pulse: %0.3f us\n",pronto.GetIRPulseDuration());
        printf("\n");

        for(int i = 1; i < 3; i++){
            if(pronto.IsSequencePresent(i)){
                printf("Sequence %d is present.\n", i);
                if(verboseOutput && verboseLevel <= 4){

                    printf("Sequence %d Burst Pairs: %d\n",i,pronto.GetSequenceLength(i));
                    vector<BurstPair> sequencePairs = pronto.GetSequenceBurstPairs(i, verboseOutput, verboseLevel);
                    for(int j = 0; j < sequencePairs.size(); j++){
                        printf("Burst pair %d: %d, %d\n",j, sequencePairs[j].TimeOn, sequencePairs[j].TimeOff);
                    }
                    printf("\n\n");
                }


                printf("Sequence %d Timings: %d\n",i,pronto.GetSequenceLength(i));
                vector<int> sequenceData = pronto.GetSequenceTiming(i,printSigns);
                for(int j = 0; j < sequenceData.size(); j++){
                    printf("%s%d",printSigns ? sequenceData[j]>=0 ? "+" : "" : "", sequenceData[j]);
                    if(j+1 < sequenceData.size()){
                        printf(",");
                    }
                }
                printf("\n\n");

                
            }else{
                printf("Sequence %d is not present.\n\n", i);
            }
        }

        if(outputType == OutputType::IR_SCRUTINIZER){
            printf("Freq=%dHz[",(int)round(pronto.GetIRFrequency()));
            vector<int> sequenceData = pronto.GetSequenceTiming(1,true);
            for(int j = 0; j < sequenceData.size(); j++){
                printf("%s%d",sequenceData[j]>=0 ? "+" : "", sequenceData[j]);
                if(j+1 < sequenceData.size()){
                    printf(",");
                }
            }
            printf("][");
            sequenceData.clear();
            sequenceData = pronto.GetSequenceTiming(2,true);
            for(int j = 0; j < sequenceData.size(); j++){
                printf("%s%d",sequenceData[j]>=0 ? "+" : "", sequenceData[j]);
                if(j+1 < sequenceData.size()){
                    printf(",");
                }
            }
            printf("][]\n\n");
        }

    }else{
        printf("Error loading file or creating Pronto object from file contents.\nVerify the filename and the file contents are ASCII text\n\n");
        return -1;
    }
    return 0;
}