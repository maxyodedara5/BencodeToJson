#include <iostream>
#include <string>
#include <fstream>


void AddToJSON(std::string toAdd , std::string jsonString , bool& isKey) {
    
    if (isKey == true) {
        jsonString += toAdd; 
        jsonString += ":"; 
        isKey = false;
        return;
    }
    else
    {
        jsonString += toAdd;
        jsonString += ",";
        isKey = true;
        return;
    }
}


int main() {

	std::cout << "Enter the file name :" << std::endl;
	std::string fileName = "sample.torrent"; 
	//std::cin >> fileName; 
    std::cout << fileName << std::endl;
   
    std::ifstream inf{ fileName };
    
    // If we couldn't open the output file stream for reading
    if (!inf)
    {
        // Print an error and exit
        std::cerr << "Error Input file cannot be read" << std::endl;
        return 1;
    }

    // While there's still stuff left to read
    std::string strInput;
    while (inf)
    {
        // read stuff from the file into a string and print it
        
        std::getline(inf, strInput);
        std::cout << strInput << '\n';
    }


    int inputLen = strInput.length();
    std::string jsonInput; 
    int current = 0; 
    std::string number; 
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
                jsonInput += word;
                jsonInput += "\"";
                current += numberOfLetters + 1; 
            }
               
            continue;
        }

        if (strInput[current] == 'i') {
            current++; //skipping i 
            number = ""; 
            while (strInput[current] != 'e') {
                number += strInput[current];
                current++;
            }
            
            jsonInput += number; 

            current++; //skipping e 
            continue;
        }



        if (strInput[current] == 'd') {
            jsonInput += "{";
            current++;
            continue;
        }
        if (strInput[current] == 'e') {
            jsonInput += "}";
            current++;
            continue;
        }


        current++;
    }

  
    // Write to created JSON file 

    // ofstream is used for writing files

    std::string outputFilename;
    
    std::ofstream outf{ "Torrent.json" };

    // If we couldn't open the output file stream for writing
    if (!outf)
    {
        // Print an error and exit
        std::cerr << "Error in writing file" << std::endl;
        return 1;
    }

    // We'll write two lines into this file
    outf << jsonInput << '\n';
	return 0;
}