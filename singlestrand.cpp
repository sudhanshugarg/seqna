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

/*
void printarray(int **a, int n, int m){
    for(int i=0;i<n;i++){
        for(int j=0;j<m;j++)
            cout << a[i][j] << ",";
        cout << endl;
    }
}
*/


//Longest Common substring s1, s2
//This function returns the longest common
//substring between two dna strands.
//pos1 contains where in s1 is the lcss found,
//and likewise for pos2.
//It doesn't matter what the initial values of pos1
//and pos2 are.
string lcss(string s1, string s2, pair<int,int> &pos1, pair <int,int> &pos2){
    //dp
    //lcs[i,j] = 0 s1[i] != s2[j], else 1+lcs[i-1][j-1]
    //store max at each level, and i,j
    //cout << "#" << s1 << "#" << endl;
    //cout << "#" << s2 << "#" << endl;

    pos1.first = pos1.second = 0;
    pos2.first = pos2.second = 0;

    int n,m;
    n = s1.size(); m = s2.size();
    int lcs[n+1][m+1];
    int maxlcs = -1;
    for(int i=0;i<=m;i++)
        lcs[0][i]=0;
    for(int i=0;i<=n;i++)
        lcs[i][0]=0;

    for(int i=1;i<=n;i++)
        for(int j=1;j<=m;j++){
            if(s1[i-1] == s2[j-1]){
          //      cout << s1[i-1] << ", equal at s1:" << i-1 << ",s2:" << j-1 << endl;
                lcs[i][j] = 1+lcs[i-1][j-1];
                if(lcs[i][j] > maxlcs){
                    maxlcs = lcs[i][j];
                    pos1.second = i;
                    pos2.second = j;
                    pos1.first = pos1.second-maxlcs;
                    pos2.first = pos2.second-maxlcs;
                }
            }
            else
                lcs[i][j] = 0;
        }

    /*
    for(int i=0;i<=n;i++){
        for(int j=0;j<=m;j++)
            cout << lcs[i][j] << ",";
        cout << endl;
    }
    cout << maxlcs << endl;
    cout << pos1.first << "," << pos1.second << ":"
         << pos2.first << "," << pos2.second << endl;
         */
    return s1.substr(pos1.first, pos1.second-pos1.first);
}


int generateSequences(int argc, char **argv){
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

typedef class subStrand{
    private:
    public:
        string *parent;
        int start;
        int end;
        subStrand(string &s1, int startpos, int endpos){
            parent = &s1;
            start = startpos;
            end = endpos;
        }
        subStrand(string &s1){
            parent = &s1;
            start = 0;
            end = s1.size();
        }
        string getStrand(void){
            if(parent != NULL)
                return parent->substr(start, end-start);

            return "";
        }
        subStrand(){
            parent = NULL;
            start = end = -1;
        }
        void set(string &s1, int startpos, int endpos){
            parent = &s1;
            start = startpos;
            end = endpos;
        }
        bool isUnset(void){
            return (parent == NULL);
        }

}subStrand;

int main(int argc, char **argv){
    //generateSequences(argc, argv);
    string s[10];
    string common = "HELLO";//just initialize to greater than 3 in length
    //3 is arbitrary, just neglect substrings <= 3 that are common
    vector<subStrand> domains;
    
    int incr = 0;
    while(cin >> s[incr]){
        subStrand x(s[incr]);
        domains.push_back(x);
        incr++;
    }

    cout << "Input:" << endl;
    for(int i=0;i<incr;i++){
        cout << s[i] << endl;
    }

    subStrand a,b;
    subStrand a1, a2, b1, b2, c;
    pair<int,int> p1, p2;
    bool reset = true;

    while(reset){
        reset = false;
        cout << "current domain list is:" << endl;
        for(int i=0;i<domains.size();i++)
            cout << domains[i].getStrand() << ".";
        cout << endl;
        for(int i=0; i<domains.size();i++){
        for(int j=i+1; j<domains.size();j++){

            a = domains[i];
            b = domains[j];
            //Note that domains will always have atleast
            //2 elements.
            if(a.getStrand() == b.getStrand()) continue;
            cout << a.getStrand() << "::" << b.getStrand() << endl;
            cout << a.start << ",," << a.end << ":::" << b.start << ",," << b.end << endl;

            //find the lcss
            common = lcss(a.getStrand(), b.getStrand(), p1, p2);
            cout << "This is common:" << common << endl;
            if(common.size() < 4) continue;

            //NOTE: p1 and p2 are relative to the two domains
            //a and b that have just been passed in.

            //delete domains i and j: first j and then i
            //to be able to delete properly.
        cout << "before erase current domain list is:" << endl;
        for(int k=0;k<domains.size();k++)
            cout << domains[k].getStrand() << ".";
        cout << endl;
            domains.erase(domains.begin()+j);
            domains.erase(domains.begin()+i);
        cout << "after erase current domain list is:" << endl;
        for(int k=0;k<domains.size();k++)
            cout << domains[k].getStrand() << ".";
        cout << endl;
            //figure out what got set in p1 and p2.
            cout << "p1:" << p1.first << "," << p1.second << endl;
            cout << "p2:" << p2.first << "," << p2.second << endl;
            //push the common lcss into the domains.
            c.set(*(a.parent), p1.first+a.start, p1.second+a.start);
            domains.push_back(c);
            //create 1-2 new strands a1,2 from a, and b1,2 from b.
            if(p1.first != 0){
                a1.set(*(a.parent), 0+a.start, p1.first+a.start);
                domains.push_back(a1);
            }
            if(p1.second != (a.end-a.start)){
                a2.set(*(a.parent), p1.second+a.start, a.end);
                domains.push_back(a2);
            }

            if(p2.first != 0){
                b1.set(*(b.parent), 0+b.start, p2.first+b.start);
                domains.push_back(b1);
            }
            if(p2.second != (b.end-b.start)){
                b2.set(*(b.parent), p2.second+b.start, b.end);
                domains.push_back(b2);
                cout << b2.start << "::" << b2.end << endl;
                cout << b2.getStrand() << "hahaha" << endl;
            }
        cout << "after INS current domain list is:" << endl;
        for(int k=0;k<domains.size();k++)
            cout << domains[k].getStrand() << ".";
        cout << endl;

            //reset
            reset = true;
            break;
        }

        if(reset) break;
        }
    }

    for(int j=0;j<incr;j++){
        cout << "s[" << j << "] strings: ";
        for(int i=0;i<domains.size();i++){
            if(domains[i].parent == &s[j])
                cout << domains[i].getStrand() << ",";
        }
        cout << endl;
    }

}

