#include<iostream>
#include<vector>
#include<string>
#include<map>
#include<algorithm>
#include<fstream>
#include<sstream>
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

int createSingleSeq(string &seq, vector<string> &ret){
    string passOn;
    for (int i=0;i<4;i++){
        if(canAppend(i,seq)){
            passOn = seq + baseToChar(i);
            ret.push_back(passOn);
        }
    }
}

int createseq(string &seqSoFar){
    if(seqSoFar.length() >= n){ 
        cout << seqSoFar << endl;
        ret.push_back(seqSoFar);
        return 0;
    }

    string passOn;
    for (int i=0;i<4;i++){
        if(canAppend(i,seqSoFar)){
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

    //2 strategies.
    //a) Design all sequences from scratch.
    //b) For each sequence of length n, we can only
    //consider sequences of length n-1, and add each
    //a base to it. This technique, also known 
    //as memoization, will remove a lot of duplicate
    //computation.


    /*
    //a) Strategy 1.
    string seq = "";
    createseq(seq);
    cout << ret.size() << endl;
    //for(int i=0;i<ret.size();i++)
      //  cout << ret[i] << endl;
    */

    //b) Strategy 2.
    n = 4;
    string seq = "";
    createseq(seq);
    ofstream writeFile;
    writeFile.open("4.seq");
    for(int i=0;i<ret.size();i++){
        writeFile << ret[i] << endl;
    }
    writeFile.close();

    //all sequences of length 4 have been calculated.
    //for length i, only consider those of length i-1.
    //this assumes all sequences of length i are stored
    //in a file named i.seq.
    n = atoi(argv[1]);
    
    ifstream readFile;

    for(int i=5;i <= n; i++){
        ostringstream ossRead, ossWrite;
        ossRead << i-1;
        ossRead << ".seq";
        ossWrite << i;
        ossWrite << ".seq";
        cout << ossRead.str();
        cout << ossWrite.str();

        //check if i.seq exists, if so, move
        //to the next file
        ifstream checkFileExists(ossWrite.str().c_str());
        if(checkFileExists){
            i++;
            continue;
        }
        readFile.open(ossRead.str().c_str());
        writeFile.open(ossWrite.str().c_str());


        string seq;
        vector<string> vseq;
        while(getline(readFile,seq)){
            //cout << seq << "::" << endl;
            createSingleSeq(seq,vseq);
            for (int j=0;j<vseq.size();j++){
                writeFile << vseq[j] << endl;
                //cout << vseq[j] << endl;
            }
            vseq.clear();
        }
        readFile.close();
        writeFile.close();
    }
}
