/** 
 * KRY 2023
 * Project 1
 * Implementation and Frequency analysis of Affine cipher
 * by Martin Jacko <xjacko05@stud.fit.vutbr.cz>
 **/

//#include <cstdio>
//#include <cstdlib>
//#include <unistd.h>
//#include <cctype>
//#include <utility>
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#define ASCII_OFFSET_UPPERCASE 65
#define ASCII_OFFSET_LOWERCASE 97
#define ALPHABET_LENGTH 26

#define FROM_ASCII(c) ((c < ASCII_OFFSET_LOWERCASE) ? (c - ASCII_OFFSET_UPPERCASE) : (c - ASCII_OFFSET_LOWERCASE))
#define TO_ASCII(c) (((c < 0) ? c + ALPHABET_LENGTH : c) + ASCII_OFFSET_UPPERCASE)

#define ENCODE 0
#define DECODE 1
#define CRACK 2

#define BUFFER_SIZE 4096

using namespace std;

int aValues[] = {1,3,5,7,9,11,15,17,19,21,23,25};


/* decodes/encodes given string using provided key */
string affineCipher(string input, int a, int b, int mode){

    string output = "";

    //modular inverse calculation
    int modInverse = 0;
    if (mode == DECODE){
        for (int i = 0; i < ALPHABET_LENGTH; i++){
            if ((i * a) % ALPHABET_LENGTH == 1){
                modInverse = i;
                break;
            }
        }
        if (!modInverse){
            cerr << "Forbidden a value provided" << endl;
            return output;
        }
    }

    //en/decoding
    for (int i = 0; i < (int) input.length(); i++){

        if (!isalpha(input[i])){
            output += input[i];
        }else{
            int plainCharValue = FROM_ASCII((int) input[i]);
            int encodedCharValue;

            if (mode == ENCODE){
                encodedCharValue = (a * plainCharValue + b) % ALPHABET_LENGTH;
            }else if (mode == DECODE){
                encodedCharValue = modInverse * (plainCharValue - b) % ALPHABET_LENGTH;
            }
            //cout << input[i] << "\t" << (int) input[i] << "\t" << plainCharValue << "\t" << encodedCharValue << "\t" << (char) TO_ASCII(encodedCharValue) << endl;
            output += (char) TO_ASCII(encodedCharValue);
        }
    }

    return output;
}


/* attempts to find key by executing frequency analysis */
auto crack(vector<char> monoGrams, int size){

    vector<char> most = { 'E', 'A', 'O', 'I', 'N'};
    vector<char> least = { 'Q', 'X', 'W', 'G', 'F'};

    //tolerance selection based on ciphertext length
    int tolerance;
    if (size > 2000){
        tolerance = 8;
    }else if (size > 500){
        tolerance = 12 - size / 500;
    }else{
        tolerance = 21 - size / 50;
    }
    
    vector<tuple<int,int,int>> keys;

    int cahrEncode = 0;

    rerun:

    //loops over keys
    for (int a : aValues){
        for (int b = 0; b < ALPHABET_LENGTH; b++){
            int score = 0;
            //testing 5 most occuring letters
            for (int i = 0; i < (int) most.size(); i++){
                char c = (a * FROM_ASCII(most[i]) + b) % ALPHABET_LENGTH;
                auto pos = find(monoGrams.begin(), monoGrams.begin() + tolerance, TO_ASCII(c));
                cahrEncode++;
                if (pos == monoGrams.begin() + tolerance){
                    goto next;
                }else{
                    score += abs(i - distance(monoGrams.begin(), pos));
                    //cout << most[i] << "\t" << (char) TO_ASCII(c) << "\t" << abs(i - distance(monoGrams.begin(), pos)) << endl; 
                }
            }
            //testing 5 least occuring letters
            for (int i = 0; i < (int) least.size(); i++){
                char c = (a * FROM_ASCII(least[i]) + b) % ALPHABET_LENGTH;
                cahrEncode++;
                auto pos = find(monoGrams.end() - tolerance, monoGrams.end(), TO_ASCII(c));
                if (pos == monoGrams.end()){
                    goto next;
                }else{
                    score += abs(i - distance(pos, monoGrams.end() - 1));
                    //cout << least[i] << "\t" << (char) TO_ASCII(c) << "\t" << abs(i - distance(pos, monoGrams.end() - 1)) << endl; 
                }
            }
            //cout << a << "\t" << b << "\t" << score << endl;
            keys.push_back(tuple<int,int,int>(a,b,score));

            next:
            continue;
        }
    }

    //reruns if no key wa found
    if (keys.size() == 0 && tolerance <= ALPHABET_LENGTH){
        tolerance++;
        //cout << "COEFF CHANGE" << endl;
        goto rerun;
    //returns key with best(lowest score)
    }else if (keys.size() > 1){
        //cout << "FOUND MORE THAN ONE KEY" << endl;
        
        int minscore = 0;
        for (int i = 1; i < (int) keys.size(); i++){
            //cout << get<0>(keys[i]) << "\t" << get<1>(keys[i]) << "\t" << get<2>(keys[i]) << endl;
            if (get<2>(keys[i]) < get<2>(keys[minscore])){
                minscore = i;
            }
        }
        cout << "NO. of cycles\t" << cahrEncode << endl;
        return pair<int,int>(get<0>(keys[minscore]), get<1>(keys[minscore]));
    }
    cout << "NO. of cycles\t" << cahrEncode << endl;
    return pair<int,int>(get<0>(keys[0]), get<1>(keys[0]));
}


/* helper function for sorting algorithm */
bool comparator(const pair<char,int> &a, const pair<char,int> &b){

    return (a.second > b.second);
}


/* obtains frequency of monograms from input file */
auto getMonoGrams(fstream &input){

    vector<pair<char,int>> data;

    for (int i = 0; i < ALPHABET_LENGTH; i++){
        data.push_back(pair<char,int>(TO_ASCII(i), 0));
    }

    char c;

    while (input >> c){

        if (!isalpha(c) ){
            continue;
        }
        data[FROM_ASCII(c)].second++;
    }

    input.clear();
    input.seekg (0, ios::beg);

    //sorts pairs accoring to character frequency
    sort(data.begin(), data.end(), comparator);

    vector<char> monoGrams;

    for (auto letter : data){
        monoGrams.push_back(letter.first);
        //cout << letter.first << endl;
    }

    return monoGrams;
}


/* handles profram flow and argument parsing */
int main(int argc, char *argv[]){

    if (argc < 6){
        return 0;
    }

    if ((string) argv[1] == "-e"){
        cout << affineCipher(argv[6], atoi(argv[3]), atoi(argv[5]), ENCODE) << endl;
    }else if ((string) argv[1] == "-d"){
        cout << affineCipher(argv[6], atoi(argv[3]), atoi(argv[5]), DECODE) << endl;
    }else{
        fstream inFile;
        inFile.open(argv[3], ios::in);

        inFile.seekg(0, ios::end);
        int fileSize = inFile.tellg();
        inFile.clear();
        inFile.seekg (0, ios::beg);

        auto key = crack(getMonoGrams(inFile), fileSize);

        cout << "a=" << key.first << ",b=" << key.second << endl;

        fstream outFile;
        outFile.open(argv[5], ios::out);

        char* buffer = new char [BUFFER_SIZE];

        //file read at 4096 characters at the time to avoid loading whole file into memory
        while (inFile.peek() != EOF){
            memset(buffer, 0, BUFFER_SIZE);
            inFile.read(buffer, BUFFER_SIZE);
            outFile << affineCipher((string) buffer, key.first, key.second, DECODE);
        }

        delete[] buffer;
        inFile.close();
        outFile.close();
    }

    return 0;
}