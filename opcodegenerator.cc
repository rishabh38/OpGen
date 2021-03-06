#include "fileHandler.h"
#include "instructure.h"
#include "word_utility.h"

#include <iostream>
#include <string>

using namespace std;

static instructure instructionRule;



bool checkConfigInstruction (string configInstruction){
  int index = configInstruction.find (":=");
  if (index == -1){
	  cerr << "error: unable to find :=" << endl;
    return false;
  }

  index += 2;
  int oldIndex = index;

  index = configInstruction.find (";");

  if (!checkBitStringFormat (configInstruction.substr (oldIndex, index - oldIndex))){
    cerr << "error: bitStringFormat not valid" << endl;
    return false;
  }

  return true;
}

string getBitString (string configInstruction){
  string bitString;
  if (!checkConfigInstruction (configInstruction)){
    cerr << "error: configInstruction \"" << configInstruction
         << "\"is not valid" << endl;
    return  bitString;
  }

  int index = configInstruction.find (":=");
  index += 2;
  int oldIndex = index;
  index = configInstruction.find (";");
  return configInstruction.substr (oldIndex, index - oldIndex);

}

vector<string> configInstruction2VecString (string configInstruction){
  vector<string> configInstructionVector;
  string instructionPart = configInstruction.substr (0, configInstruction.find (":=") + 2);
  configInstructionVector = stringtostringvec (instructionPart);
  configInstructionVector.push_back (getBitString (configInstruction));

  return configInstructionVector;
}

string makeInstructionNice (string configurationString){
  int partingIndex =  configurationString.find (":=");
  int index;
  while ((index = configurationString.find ("\n")) >= 0 &&
          index < partingIndex){
    configurationString.erase (index, 1);
    partingIndex--;
  }

  while ((index = configurationString.find ("\n")) >= 0){
    configurationString.replace (index, 1, "|");
  }

  while ((index = configurationString.find ("\t")) >= 0){
    configurationString.replace (index, 1, " ");
  }

  return configurationString;
}



bool writetoInstructure (ifstream &configStream){
  string configInstruction;
  while (getline (configStream, configInstruction, ';') &&
         !configStream.eof ()){
    if (!checkConfigInstruction (configInstruction)){
      return false;
    }

    configInstruction = makeInstructionNice (configInstruction);
    vector<string> temp = configInstruction2VecString (configInstruction);
    instructionRule.addInstruction (temp);
  }

  return true;
}

bool addConfiguration (string fileName){
  if (!doesFileExist (fileName)){
    cerr << "error: file doesn't exist" << endl;
    return false;
  }

  ifstream configStream = openInFile (fileName);
  if (!writetoInstructure (configStream)){
    cerr << "error: unable to read configuration file" << endl;
    return false;
  }

  return true;
}

bool generateOpCode (string instFileName, string opCodeFile){
  if (!doesFileExist (instFileName)){
    return false;
  }
  ifstream instStream  = openInFile (instFileName);
  ofstream opcodeStream (opCodeFile);

  string instruction;
  while (getline (instStream, instruction) && !instStream.eof()){
    vector<string> temp = stringtostringvec (instruction);
    opcodeStream << instructionRule.checkInstruction (temp) << endl;
  }

  return true;
}

int main(int argc, char* argv[]){
  if (argc < 4){
    cerr << "usage: opcgen <configuration_filename>"
         << " <instruction_filename> <output_filename>" << endl;
    return 1;
  }

  string confFileName = argv[1];
  string instFileName = argv[2];
  string opcodeFileName = argv[3];

  addConfiguration (confFileName);
  generateOpCode (instFileName, opcodeFileName);
}
