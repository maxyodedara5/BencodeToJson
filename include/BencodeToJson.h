#ifndef BENCODETOJSON
#define BENCODETOJSON
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <stack>

namespace BencodeToJson
{
    typedef unsigned char BYTE;

    //reading data from .torrent file
    std::vector<BYTE> readFile(std::string filename)
    {
        // open the file:
        std::ifstream file(filename, std::ios::binary);

        // to stop eating new lines in binary mode
        file.unsetf(std::ios::skipws);

        // get its size:
       // std::streampos fileSize;
        unsigned int fileSize; 
        file.seekg(0, std::ios::end);
        fileSize = (unsigned int)file.tellg();
        file.seekg(0, std::ios::beg);
       
        // reserve capacity
        std::vector<BYTE> vec;
        vec.reserve(fileSize);

        // read the data:
        vec.insert(vec.begin(),
            std::istream_iterator<BYTE>(file),
            std::istream_iterator<BYTE>());

        return vec;
    }

    //used for conversion of pieces to hex code 
    inline std::string hex_last_2(const char& c) {
        std::stringstream ss;
        ss << std::uppercase << std::setw(2) << std::setfill('0') << std::hex << int{ c };
        return ss.str().substr(ss.str().size() - 2, ss.str().size());
    }

    //function to add text to json input array
    /*
    toAdd           - is the word to be added to json input array
    jsonString      - is current json input array created
    isKey           - is to determine if the word is key or value, if key then : needs to be appended
                    after quotes and if its value then , needs to be appended
    listStart       - to keep track of list
    piecesFlag      - to check if toAdd received is pieces of torrent file
    */
    void AddToJSON(std::vector<BYTE>& toAdd, std::vector<std::string>& jsonString, bool& isKey, bool listStart, bool& piecesFlag) {


        //pieces logic utf8 encoded symbols converted to hex codes 
        if (piecesFlag == true) {
            std::vector<char> bytes(toAdd.begin(), toAdd.end());
            jsonString.push_back("<hex> ");

            for (size_t i = 0; i < bytes.size(); i++)
            {
                jsonString.push_back(hex_last_2(bytes[i]) + " ");
            }
            jsonString.push_back("</hex>");

            piecesFlag = false;
            jsonString.push_back("\"");
            jsonString.push_back(",");
            isKey = true;
            return;
        }




        // if its inside array only "," needs to added 
        if (listStart == true) {
            for (size_t i = 0; i < toAdd.size(); i++)
            {
                std::string toAd; toAd.push_back(toAdd[i]);
                jsonString.push_back(toAd);

            }

            jsonString.push_back("\"");
            jsonString.push_back(",");
            isKey = true;
            return;
        }
        else
        {
            //inside json key value pairs so "," and ":" both need to be managed 
            if (isKey == true) {
                for (size_t i = 0; i < toAdd.size(); i++)
                {
                    std::string toAd; toAd.push_back(toAdd[i]);
                    jsonString.push_back(toAd);

                }
                jsonString.push_back("\"");
                jsonString.push_back(":");

                isKey = false;
                return;
            }
            else
            {
                for (size_t i = 0; i < toAdd.size(); i++)
                {
                    std::string toAd; toAd.push_back(toAdd[i]);
                    jsonString.push_back(toAd);

                }
                jsonString.push_back("\"");
                jsonString.push_back(",");
                isKey = true;
                return;
            }
        }




    }

    //function to add text(number) to json string
    /*
    toAdd           - is the word to be added to json input array
    jsonString      - is current json input array created
    isKey           - is to determine if the word is key or value, if key then : needs to be appended
                    after quotes and if its value then , needs to be appended
    */
    void AddToJSON(std::vector<BYTE>& toAdd, std::vector<std::string>& jsonString, bool& isKey) {


        for (size_t i = 0; i < toAdd.size(); i++)
        {
            std::string toAd; toAd.push_back(toAdd[i]);
            jsonString.push_back(toAd);

        }
        jsonString.push_back(",");
        isKey = true;
        return;

    }

    
    int BencodeToJSON(std::string fileName) {
        try
        {

        std::string file = fileName;
        
        //reading file into vector array 
        std::vector<BYTE> filedata = readFile(file);
        int inputLen = filedata.size();

        //array for writing to json
        std::vector<std::string> jsonInput;

        std::vector<BYTE>::iterator current = filedata.begin(); //track of current pos
        std::string number;
        std::stack<std::string> containerTrack;
        bool iskey = true;
        bool listStart = false;
        bool piecesFlag = false;

        while (current != filedata.end())
        {

            //words logic 
            if (isdigit(*current)) {
                number = "";
                while (*current != ':') {
                    number += *current;
                    current++;
                }
                if (*current == ':') {
                    int numberOfLetters = stoi(number);
                    std::vector<BYTE> word;
                    word.assign(current + 1, current + 1 + numberOfLetters);

                    // handling ],[ in json
                    if (!jsonInput.empty() && *(jsonInput.end() - 1) == "]") {
                        jsonInput.push_back(",");
                    }

                    //handling },{ in json
                    if (!jsonInput.empty() && *(jsonInput.end() - 1) == "}") {
                        jsonInput.push_back(",");
                    }

                    jsonInput.push_back("\"");

                    //add word to json
                    AddToJSON(word, jsonInput, iskey, listStart, piecesFlag);
                     

                    //pieces logic
                    std::vector<BYTE> pieces{ 'p','i','e','c','e','s' };
                    if (word.size() == 6 && word == pieces) {

                        piecesFlag = true;
                    }

                    current += numberOfLetters + 1;
                }

                continue;
            }

            //bencode conversion for numbers 
            // todo negative numbers 
            if (*current == 'i') {
                current++; //skipping i 
                std::vector<BYTE> number;
                //number = "";
                while (*current != 'e') {
                    number.push_back(*current);
                    current++;
                }

                //adding numbers to json
                AddToJSON(number, jsonInput, iskey);
                 

                current++; //skipping e 
                continue;
            }



            if (*current == 'd') {

                // }{ handling the , between braces if inside list

                if (!jsonInput.empty() && *(jsonInput.end() - 1) == "}") {
                    jsonInput.push_back(",");
                }


                jsonInput.push_back("{");
                iskey = true;
                listStart = false;
                containerTrack.push("{");
                current++;
                continue;
            }

            if (*current == 'l') {

                if (!jsonInput.empty() && *(jsonInput.end() - 1) == "]") {
                    jsonInput.push_back(",");
                }
                containerTrack.push("[");
                listStart = true;

                jsonInput.push_back("[");
                current++;
                continue;
            }

            //closing lists and dictionaries 
            //stack used to keep track of opening and closing braces 
            if (*current == 'e') {
                if (containerTrack.top() == "[") {
                    if (*(jsonInput.end() - 1) == ",") {
                        jsonInput.pop_back();
                    }
                    jsonInput.push_back("]");

                    listStart = false;
                    containerTrack.pop();
                }
                else
                {
                    if (containerTrack.top() == "{") {

                        if (*(jsonInput.end() - 1) == ",") {
                            jsonInput.pop_back();
                        }
                        jsonInput.push_back("}");
                        listStart = false;
                        containerTrack.pop();
                    }
                }

                current++;
                continue;
            }


            current++;
        }


        //adding json extension to tobe created filename
        std::size_t pos = fileName.find(".");
        fileName = fileName.substr(0, pos);
        std::string fileoutput = fileName + ".json";
        
        //Writing created vector to file 
        std::ofstream outf{ fileoutput };
        for (size_t i = 0; i < jsonInput.size(); i++)
        {
            outf << jsonInput[i];
        }


        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            return -1;
        }
        return 0;
    }


}

#endif