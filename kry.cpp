#include <stdio.h>
#include <iostream>
#include <string>

#define ASCII_OFFSET_UPPERCASE 65
#define ALPHABET_LENGTH 26

#define FROM_ASCII(c) (c - ASCII_OFFSET_UPPERCASE)
#define TO_ASCII(c) (((c < 0) ? c + ALPHABET_LENGTH : c) + ASCII_OFFSET_UPPERCASE)

#define ENCODE true
#define DECODE false

using namespace std;

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


string affineCipher(string input, int a, int b, bool mode){

    string output = "";

    for (int i = 0; i < (int) input.length(); i++){

        if (input[i] == ' '){
            output += " ";
        }else{
            int plainCharValue = FROM_ASCII((int) input[i]);
            int encodedCharValue;

            if (mode == ENCODE){
                encodedCharValue = (a * plainCharValue + b) % ALPHABET_LENGTH;
            }else if (mode == DECODE){
                encodedCharValue = getModInverse(a) * (plainCharValue - b) % ALPHABET_LENGTH;
            }
            //cout << input[i] << "\t" << (int) input[i] << "\t" << plainCharValue << "\t" << encodedCharValue << "\t" << (char) TO_ASCII(encodedCharValue) << "\n";
            output += (char) TO_ASCII(encodedCharValue);
        }
    }

    return output;
}

int main(int argc, char *argv[]){

    //cout << affineCipher("TOTO JE TAJNA ZPRAVA", 3, 7, ENCODE) << "\n";
    //cout << affineCipher("MXMX IT MHIUH EAGHSH", 3, 7, DECODE) << "\n";

    string msg = "FAKUJEM ICH ULICA";
    int a = 15;
    int b = 3575555;
    string encoded = affineCipher(msg, a, b, ENCODE);
    string decoded = affineCipher(encoded, a, b, DECODE);
    cout << msg << "\t" << encoded << "\t" << decoded << "\t" << ((msg == decoded) ? "OK" : "ERROR") << "\n";
    


    
    return 0;
}