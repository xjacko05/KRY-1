#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <utility>
#include <vector>
#include <algorithm>

#define ASCII_OFFSET_UPPERCASE 65
#define ASCII_OFFSET_LOWERCASE 97
#define ALPHABET_LENGTH 26

#define FROM_ASCII(c) (c - ASCII_OFFSET_UPPERCASE)
#define TO_ASCII(c) (((c < 0) ? c + ALPHABET_LENGTH : c) + ASCII_OFFSET_UPPERCASE)

#define ENCODE 0
#define DECODE 1
#define CRACK 2

using namespace std;

#define NO_OF_A_VALUES 12
int aValues[] = {1,3,5,7,9,11,15,17,19,21,23,25};

int getModInverse(int i){

    switch(i){
        case (1):
            return 1;
        case (3):
            return 9;
        case (5):
            return 21;
        case (7):
            return 15;
        case (9):
            return 3;
        case (11):
            return 19;
        case (15):
            return 7;
        case (17):
            return 23;
        case (19):
            return 11;
        case (21):
            return 5;
        case (23):
            return 17;
        case (25):
            return 25;
        default:
            return 0;
    }
}


string affineCipher(string input, int a, int b, int mode){

    string output = "";

    for (int i = 0; i < (int) input.length(); i++){

        if (input[i] == ' '){
            output += " ";
        }else{
            input[i] = toupper(input[i]);

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


map<char, int> monoGrams;
map<string, int> diGrams;
map<string, int> triGrams;

void getMonograms(fstream &input){

    char buffer[4] = "\0\0\0";
    char c;

    for (int i = 0; i < ALPHABET_LENGTH; i++){
        monoGrams[(char) (i + ASCII_OFFSET_LOWERCASE)] = 0;
        //cout << (char) (i + ASCII_OFFSET_LOWERCASE) << occurence[(char) (i + ASCII_OFFSET_LOWERCASE)] << endl;
    }

    while (input >> c){

        buffer[2] = buffer[1];
        buffer[1] = buffer[0];
        buffer[0] = tolower(c);

        //monograms
        if (buffer[0] == ' ' ){
            continue;
        }
        monoGrams[buffer[0]]++;

        //digrams
        if (buffer[1] == ' ' || buffer[1] == '\0'){
            continue;
        }
        char digram[3];
        digram[0] = buffer[0];
        digram[1] = buffer[1];
        digram[2] = '\0';
        if (diGrams.find(digram) == diGrams.end()){
            diGrams[digram] = 1;
        } else {
            diGrams[digram]++;
        }

        //trigrams
        if (buffer[2] == ' ' || buffer[2] == '\0'){
            continue;
        }
        if (triGrams.find(buffer) == triGrams.end()){
            triGrams[buffer] = 1;
        } else {
            triGrams[buffer]++;
        }


        //cout << c << occurence[c] << endl;
    }

    input.clear();
    input.seekg (0, ios::beg);
}


set<pair<int,int>> getKey(int plain, int encoded){

    set<pair<int,int>> keys;

    cout << "getKey()" << endl;

    plain = FROM_ASCII(plain);
    encoded = FROM_ASCII(encoded);

    for (int a : aValues){
        for (int b = 0; b < ALPHABET_LENGTH; b++){
            //cout << a << "\t" << b << endl;
            if ((a * plain + b) % ALPHABET_LENGTH == encoded){
                keys.insert(pair<int,int>(a,b));
                cout << a << "\t" << b << endl;
            }
        }
    }
    cout << endl;
    return keys;

}

void permute(string a, int l, int r){
    // Base case
    if (l == r)
        cout << a << endl;
    else {
        // Permutations made
        for (int i = l; i <= r; i++) {
 
            // Swapping done
            swap(a[l], a[i]);
 
            // Recursion called
            permute(a, l + 1, r);
 
            // backtrack
            swap(a[l], a[i]);
        }
    }
}


set<pair<int,int>> checkKeys(int plain, int encoded, set<pair<int,int>> keys){

    set<pair<int,int>> result;

    //cout << "checkKeys()" << endl;

    plain = FROM_ASCII(plain);
    encoded = FROM_ASCII(encoded);

    for (auto key : keys){
        if ((key.first * plain + key.second) % ALPHABET_LENGTH == encoded){
            result.insert(key);
            //cout << key.first << "\t" << key.second << endl;
        }
    }

    return result;
}


void crack_f(){

    vector<char> plain = { 'E', 'A', 'O', 'I', 'N' };
    vector<char> enc = { 'H', 'R', 'D', 'B', 'C' };

    vector<char> plainBottom = { 'Q', 'X', 'W', 'F', 'G' };
    vector<char> encBottom = { 'A', 'G', 'V', 'X', 'J' };

    int count = 0;

    set<pair<int,int>> res;

    for(char& p0 : plain){
        for(char& e0 : enc){
            set<pair<int,int>> k0 = getKey(p0, e0);
            for(char& p1 : plain){
                for(char& e1 : enc){
                    if (p0 == p1 || e0 == e1){
                        continue;
                    }
                    set<pair<int,int>> k1 = checkKeys(p1, e1, k0);
                    if (k1.empty()){
                        continue;
                    }
                    for(char& p2 : plain){
                        for(char& e2 : enc){
                            if (p0 == p2 || p1 == p2 || e0 == e2 || e1 == e2){
                                continue;
                            }
                            set<pair<int,int>> k2 = checkKeys(p2, e2, k1);
                            if (k2.empty()){
                                continue;
                            }
                            //bottom
                            for(char& p3 : plainBottom){
                                for(char& e3 : encBottom){
                                    set<pair<int,int>> k3 = checkKeys(p3, e3, k2);
                                    if (k3.empty()){
                                        continue;
                                    }
                                    for(char& p4 : plainBottom){
                                        for(char& e4 : encBottom){
                                            if (p3 == p4 || e3 == e4){
                                                continue;
                                            }
                                            set<pair<int,int>> k4 = checkKeys(p4, e4, k3);
                                            if (k4.empty()){
                                                continue;
                                            }
                                            for(char& p5 : plainBottom){
                                                for(char& e5 : encBottom){
                                                    if (p3 == p4 || p4 == p5 || e3 == e4 || e4 == e5){
                                                        continue;
                                                    }
                                                    set<pair<int,int>> k5 = checkKeys(p5, e5, k4);
                                                    if (k5.empty()){
                                                        continue;
                                                    }
                                                    count++;
                                                    //cout << p0 << "\t" << e0 << "\t" << p1 << "\t" << e1 << "\t" << p2 << "\t" << e2 << "\t" << p3 << "\t" << e3 << "\t" << p4 << "\t" << e4 << endl;
                                                    for (auto kk : k5){
                                                        res.insert(kk);
                                                        //cout << kk.first << "\t" << kk.second << endl;
                                                    }
                                                    //cout << endl;
                                                    //exit(1);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    for (auto kk : res){
        cout << kk.first << "\t" << kk.second << endl;
    }
    cout << count << endl;
}


void func(){

    vector<char> top = { 'H', 'R', 'D', 'B', 'C' };
    vector<char> bottom = { 'A', 'G', 'V', 'X', 'J' };

    set<pair<int,int>> keys;

    for(char& t : top){
        set<pair<int,int>> k0 = getKey('E', t);
        for(char& b : bottom){
            set<pair<int,int>> k1 = checkKeys('Q', b, k0);
            if (k1.empty()){
                continue;
            }else{
                for (auto i : k1){
                    keys.insert(i);
                }
            }
        }
    }

    for (auto i : keys){
        cout << i.first << "\t" << i.second << endl;
    }
    
}

auto func1(vector<char> mono){

    set<char> top = { 'H', 'R', 'D', 'B', 'C', 'N', 'U' };
    set<char> bottom = { 'A', 'G', 'V', 'X', 'J' };

    vector<char> etsTop = { 'E', 'A', 'O', 'I', 'N'};
    vector<char> estBottom = { 'Q', 'X', 'W', 'G', 'F'};

    set<pair<int,int>> keys;

    for (int a : aValues){
        for (int b = 0; b < ALPHABET_LENGTH; b++){
            for (auto topchar : etsTop){
                char c = (a * FROM_ASCII(topchar) + b) % ALPHABET_LENGTH;
                //if (top.find(TO_ASCII(c)) == top.end()){
                if (find(mono.begin(), mono.begin() + 7, TO_ASCII(c)) == mono.end()){
                    goto next;
                }
            }
            for (auto bottomchar : estBottom){
                char c = (a * FROM_ASCII(bottomchar) + b) % ALPHABET_LENGTH;
                //if (top.find(TO_ASCII(c)) == bottom.end()){
                if (find(mono.end() - 7, mono.end(), TO_ASCII(c)) == mono.end()){
                    goto next;
                }
            }
            keys.insert(pair<int,int>(a,b));

            next:
            continue;
        }
    }

    for (auto k : keys){
        cout << k.first << "\t" << k.second << endl;
    }

    return keys;
}


bool comparator(const pair<char,int> &a, const pair<char,int> &b){

    return (a.second > b.second);
}


auto gg(fstream &input){

    vector<pair<char,int>> mono;
    char c;

    for (int i = 0; i < ALPHABET_LENGTH; i++){
        mono.push_back(pair<char,int>(TO_ASCII(i), 0));
        //cout << (char) (i + ASCII_OFFSET_LOWERCASE) << occurence[(char) (i + ASCII_OFFSET_LOWERCASE)] << endl;
    }

    while (input >> c){

        if (c == ' ' ){
            continue;
        }
        mono[FROM_ASCII(c)].second++;
    }

    input.clear();
    input.seekg (0, ios::beg);

    sort(mono.begin(), mono.end(), comparator);

    vector<char> result;

    for (auto l : mono){
        result.push_back(l.first);
    }

    return result;
}


int main(int argc, char *argv[]){

    //permute((string) "ABCD", 0, 3);

    //crack_f();
    //return 0;


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
        fstream outFile;
        outFile.open(argv[5], ios::out);

        //getMonograms(inFile);

        func1(gg(inFile));

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