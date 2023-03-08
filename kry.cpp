#include <stdio.h>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <cctype>
#include <iostream>
#include <fstream>
#include <string>
#include <utility>
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


string affineCipher(string input, int a, int b, int mode){

    string output = "";

    for (int i = 0; i < (int) input.length(); i++){

        if (!isalpha(input[i])){
            output += input[i];
        }else{
            //input[i] = toupper(input[i]);

            int plainCharValue = FROM_ASCII((int) input[i]);
            int encodedCharValue;

            if (mode == ENCODE){
                encodedCharValue = (a * plainCharValue + b) % ALPHABET_LENGTH;
            }else if (mode == DECODE){
                int aModInverse = 0;
                for (int i = 0; i < ALPHABET_LENGTH; i++){
                    if ((i * a) % ALPHABET_LENGTH == 1){
                        aModInverse = i;
                    }
                }
                encodedCharValue = aModInverse * (plainCharValue - b) % ALPHABET_LENGTH;
            }
            //cout << input[i] << "\t" << (int) input[i] << "\t" << plainCharValue << "\t" << encodedCharValue << "\t" << (char) TO_ASCII(encodedCharValue) << endl;
            output += (char) TO_ASCII(encodedCharValue);
        }
    }

    return output;
}


auto crack(vector<char> monoGrams){

    vector<char> most = { 'E', 'A', 'O', 'I', 'N'};
    vector<char> least = { 'Q', 'X', 'W', 'G', 'F'};

    int coefficient = 25;

    //vector<pair<int,int>> keys;
    vector<tuple<int,int,int>> keys;

    rerun:

    for (int a : aValues){
        for (int b = 0; b < ALPHABET_LENGTH; b++){
            int score = 0;
            for (int i = 0; i < (int) most.size(); i++){
                char c = (a * FROM_ASCII(most[i]) + b) % ALPHABET_LENGTH;
                auto pos = find(monoGrams.begin(), monoGrams.begin() + coefficient, TO_ASCII(c));
                if (pos == monoGrams.begin() + coefficient){
                    goto next;
                }else{
                    score += abs(i - distance(monoGrams.begin(), pos));
                    //cout << most[i] << "\t" << (char) TO_ASCII(c) << "\t" << abs(i - distance(monoGrams.begin(), pos)) << endl; 
                }
            }
            for (int i = 0; i < (int) least.size(); i++){
                char c = (a * FROM_ASCII(least[i]) + b) % ALPHABET_LENGTH;
                auto pos = find(monoGrams.end() - coefficient, monoGrams.end(), TO_ASCII(c));
                if (pos == monoGrams.end()){
                    goto next;
                }else{
                    score += abs(i - distance(pos, monoGrams.end() - 1));
                    //cout << least[i] << "\t" << (char) TO_ASCII(c) << "\t" << abs(i - distance(pos, monoGrams.end() - 1)) << endl; 
                }
            }
            cout << a << "\t" << b << "\t" << score << endl;
            //keys.push_back(pair<int,int>(a,b));
            keys.push_back(tuple<int,int,int>(a,b,score));

            next:
            continue;
        }
    }

    if (keys.size() == 0 && coefficient <= ALPHABET_LENGTH / 2){
        coefficient++;
        cout << "COEFF CHANGE" << endl;
        goto rerun;
    }

    //cout << distance(monoGrams.begin(), monoGrams.end()) << monoGrams[monoGrams.end()];

    if (keys.size() > 1){
        cout << "FOUND MORE THAN ONE KEY" << endl;
        for (auto k : keys){
            cout << get<0>(k) << "\t" << get<1>(k) << "\t" << get<2>(k) << endl;
        }
        
        int minscore = 0;
        for (int i = 1; i < (int) keys.size(); i++){
            if (get<2>(keys[i]) < get<2>(keys[minscore])){
                minscore = i;
            }
        }

        return pair<int,int>(get<0>(keys[minscore]), get<1>(keys[minscore]));
        
        /*vector<int> score;

        for (auto k : keys){
            int s = 0;
            for (int i = 0; i < (int) most.size(); i++){
                char c = (k.first * FROM_ASCII(most[i]) + k.second) % ALPHABET_LENGTH;
                int index = distance(monoGrams.begin(), find(monoGrams.begin(), monoGrams.begin() + coefficient, TO_ASCII(c)));
                s += abs(i - index);
            }
            for (int i = 0; i < (int) least.size(); i++){
                char c = (k.first * FROM_ASCII(least[i]) + k.second) % ALPHABET_LENGTH;
                int index = distance(monoGrams.end(), find(monoGrams.end() - coefficient, monoGrams.end(), TO_ASCII(c)));
                s += abs(i - index);
            }
        }*/
    }

    //return keys[0];
    return pair<int,int>(get<0>(keys[0]), get<1>(keys[0]));
}


bool comparator(const pair<char,int> &a, const pair<char,int> &b){

    return (a.second > b.second);
}


auto getMonoGrams(fstream &input){

    vector<pair<char,int>> data;
    char c;

    for (int i = 0; i < ALPHABET_LENGTH; i++){
        data.push_back(pair<char,int>(TO_ASCII(i), 0));
    }

    while (input >> c){

        if (!isalpha(c) ){
            continue;
        }
        data[FROM_ASCII(c)].second++;
    }

    input.clear();
    input.seekg (0, ios::beg);

    sort(data.begin(), data.end(), comparator);

    vector<char> monoGrams;

    for (auto letter : data){
        monoGrams.push_back(letter.first);
        //cout << letter.first << endl;
    }

    return monoGrams;
}


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

        pair<int,int> key = crack(getMonoGrams(inFile));

        cout << "a=" << key.first << ",b=" << key.second << endl;

        
        fstream outFile;
        outFile.open(argv[5], ios::out);

        char* buffer = new char [BUFFER_SIZE];

        while (inFile.peek() != EOF){
            memset(buffer, 0, BUFFER_SIZE);
            inFile.read(buffer, BUFFER_SIZE);
            outFile << affineCipher((string) buffer, key.first, key.second, DECODE);
        }

        delete buffer;

        /*
        for (auto const& [key, val] : o)
        {
            cout << key << ':' << val << endl;
        }*/

        inFile.close();
        outFile.close();
    }

    /*
    string msg = "TOTO JE TAJNA ZPRAVA";
    int a = 15;
    int b = 3575555;
    string encoded = affineCipher(msg, a, b, ENCODE);
    string decoded = affineCipher(encoded, a, b, DECODE);
    cout << msg << "\t" << encoded << "\t" << decoded << "\t" << ((msg == decoded) ? "OK" : "ERROR") << endl;
    */

    return 0;
}