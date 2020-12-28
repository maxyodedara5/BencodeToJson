#include <iostream>
#include <string>
#include <fstream>
#include <stack>


std::string InputFile() {
    std::cout << "Enter the file name :" << std::endl;
    std::string fileName = "bbb_sunflower.torrent";
    //std::cin >> fileName; 
    //std::cout << fileName << std::endl;

    std::ifstream inf{ fileName };

    // If we couldn't open the output file stream for reading
    if (!inf)
    {
        // Print an error and exit
        std::cerr << "Error Input file cannot be read" << std::endl;
        return "Error\n";
    }

    // While there's still stuff left to read
    std::string strInput;
    while (inf)
    {
        // read stuff from the file into a string and print it

        std::getline(inf, strInput);
        //std::cout << strInput << '\n';
    }
    return strInput;
}

std::string OutputFile(std::string jsonInput) {
    // Write to created JSON file 

    // ofstream is used for writing files

    std::string outputFilename;

    std::ofstream outf{ "Torrent.json" };

    // If we couldn't open the output file stream for writing
    if (!outf)
    {
        // Print an error and exit
        std::cerr << "Error in writing file" << std::endl;
        return "Error";
    }

    // We'll write two lines into this file
    outf << jsonInput << '\n';
    return "Success";
}

//function to add text to json string
/*
string toAdd        - is the word to be appended to jsonString
string jsonString   - is current json file created
bool& isKey         - is to determine if the word is key or value, if key then : needs to be appended 
                      after quotes and if its value then , needs to be appended
*/
void AddToJSON(std::string toAdd , std::string& jsonString , bool& isKey , bool arrayStart) {
    
    // if its inside array only "," needs to added 
    if (arrayStart == true) {
        jsonString += toAdd;
        jsonString += "\"";
        jsonString += ",";
        isKey = true;
        return;
    }
    else
    {
        //inside json key value pairs so "," and ":" both need to be managed 
        if (isKey == true) {
            jsonString += toAdd;
            jsonString += "\"";
            jsonString += ":";

            isKey = false;
            return;
        }
        else
        {
            jsonString += toAdd;
            jsonString += "\"";
            jsonString += ",";
            isKey = true;
            return;
        }
    }

   
}

//function to add text(number) to json string
/*
* 
*  TODO - Negative numbers 
* 
string toAdd        - is the word to be appended to jsonString
string jsonString   - is current json file created
bool& isKey         - is to determine if the word is key or value, if key then : needs to be appended
                      after quotes and if its value then , needs to be appended
bool number         - to confirm if word is number or word , if its number then quotes wont be needed 
*/
void AddToJSON(std::string toAdd, std::string& jsonString, bool& isKey ) {

   
        jsonString += toAdd;
        jsonString += ",";
        isKey = true;
        return;
    
}

int main() {

	

    std::string strInput = InputFile();

    int inputLen = strInput.length();
    std::string jsonInput; 
    int current = 0; 
    std::string number; 
    bool iskey = true;
    std::stack<std::string> containerTrack;
    bool arrayStart = false; 
    while (current != inputLen)
    {

        if (isdigit(strInput[current])) {
            number = "";
            while (strInput[current] != ':') {
                number += strInput[current];
                current++;
            }
            if (strInput[current] == ':') {
                int numberOfLetters = stoi(number);
                std::string word = strInput.substr(current + 1, numberOfLetters);
                jsonInput += "\""; 
                
                AddToJSON(word, jsonInput, iskey , arrayStart);
                //jsonInput += word;
               
                current += numberOfLetters + 1; 
            }
               
            continue;
        }

        //bencode conversion for numbers 
        // todo negative numbers 
        if (strInput[current] == 'i') {
            current++; //skipping i 
            number = ""; 
            while (strInput[current] != 'e') {
                number += strInput[current];
                current++;
            }
            
            AddToJSON(number, jsonInput, iskey );
            //jsonInput += number; 

            current++; //skipping e 
            continue;
        }



        if (strInput[current] == 'd') {
            jsonInput += "{";
            containerTrack.push("{");
            current++;
            continue;
        }

        if (strInput[current] == 'l') {
            containerTrack.push("[");
            arrayStart = true;
            jsonInput += "[";
            current++;
            continue;
        }

        if (strInput[current] == 'e') {
            if (containerTrack.top() == "[") {
                if (*(jsonInput.end()-1) == ',') {
                    jsonInput.pop_back();
                }
                jsonInput += "]";
                arrayStart = false; 
                containerTrack.pop();
            }
            else
            {
                if (containerTrack.top() == "{") {
                    if (*(jsonInput.end() - 1) == ',') {
                        jsonInput.pop_back();
                    }
                    jsonInput += "}";
                    arrayStart = false;
                    containerTrack.pop();
                }
            }
            
            current++;
            continue;
        }


        current++;
    }

    std::string outfileStatus = OutputFile(jsonInput);
    
	return 0;
}