#include<iostream>
#include<vector>
#include<string>
#include<map>
#include<algorithm>
using namespace std;

vector<string> ret;
map<string,vector<string> > cantCoExist;
int n;

char baseToChar(int b){
    switch(b){
        case 0: return 'A';
        case 1: return 'T';
        case 2: return 'C';
        case 3: return 'G';
        default : return 'X';
    }
}

bool canAppend(int &base, string &s){
    if(s.length() < 2) return 1;

    string check("XY");
    check[0] = s[s.length()-1];
    check[1] = baseToChar(base);
    string read;
    vector<string> complements = cantCoExist[check];
    /*
    cout << "#" << check << "#, complement = "  
        << complements[0]
        << ", string: " << s << endl;
    cout << s.length() << endl;
    int condition = s.length() - 6;
    */

    int condition = s.length() - 6;
    for(int i=0;i<condition;i++){
        read = s.substr(i,2);
        for(int j=0;j<complements.size();j++){
            if(complements[j] == read){
                //cout << "cannot insert " << check << " because of " << 
                //    read << " at position " << i << endl; 
                return 0;
            }
        }
    }
    return 1;
}

int createseq(string &seqSoFar){
    if(seqSoFar.length() >= n){ 
        cout << seqSoFar << endl;
        ret.push_back(seqSoFar);
        return 0;
    }
    string curr = "";

    for (int i=0;i<4;i++){
        if(canAppend(i,seqSoFar)){
            string passOn;
            passOn = seqSoFar + baseToChar(i);
            createseq(passOn);
        }
    }
}

string revComp(string s){
    reverse(s.begin(), s.end());
    for(int i=0;i<s.length();i++){
        if(s[i] == 'A') s[i] = 'T';
        else if(s[i] == 'T') s[i] = 'A';

        if(s[i] == 'G') s[i] = 'C';
        else if(s[i] == 'C') s[i] = 'G';
    }
    return s;
}

void init(){
    string nt2("XY"), revNt2;
    for (int i=0;i<4;i++)
    for(int j=0;j<4;j++){
        nt2[0] = baseToChar(i);
        nt2[1] = baseToChar(j);
        revNt2 = revComp(nt2);
        cantCoExist[nt2].push_back(revNt2);
    }
}

int main(int argc, char **argv){
    //This function initializes all the 2-nt sequences
    //such that their reverse complements cannot exist.
    init();
    //n is the length of the sequence you want.
    n = atoi(argv[1]);
    cout << n << endl;
    string seq = "";
    createseq(seq);
    cout << ret.size() << endl;
    //for(int i=0;i<ret.size();i++)
      //  cout << ret[i] << endl;
}
