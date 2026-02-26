#include<bits/stdc++.h>
using namespace std;

unordered_map<string,int> registers;
unordered_map<string,int> operations;
unordered_map<string,int> labels;
int memory[1<<10][1<<8] = {0};
vector<string> instructions;
int itr = 0;
int throwError = 0;
int clockCycles=0;
int alt_clock=0;
int ROW_ACCESS_DELAY;
int COL_ACCESS_DELAY;
int currentRow = -1;
int bufferUpdates = 0;
int ROW_BUFFER[1<<8] = {0};
unordered_map<string,queue<int>> engagedRegs;
queue<pair<string,int>> queuedRegs;

void printData(){
	for(int i=0;i<1024;i++){
		for(int j=0;j<256;j++){
			if(memory[i][j]!=0){
				cout<<i*1024+j*4<<"="<<memory[i][j]<<endl;
			}
		}
	}
}

bool check_number(string str){
	if(str.length()==0)
		return true;
	else{
   		if(!isdigit(str[0])){
	   		if(str[0]!='-' && str[0]!='+'){
		   		return false;
	   		}
   		}else{
			for (int i = 1;i < str.length(); i++)
   				if (isdigit(str[i]) == false)
      				return false;
		}
	}
    return true;
}

bool checkAddress(string reg){
	int n=reg.length();
	if(!check_number(reg)){
		if (n>=7 && reg.substr(n-7)=="($zero)"){
			return true;
		}else if(n<4){
		 	return false;
		}else if (reg[n-4]=='(' && reg[n-1]==')'){
			string s = reg.substr(n-3,2);
			if(registers.find(s)!=registers.end()){
				return true;
			}else{
				return false;
			}
		}else if(n<5){
			return false;
		}else if(reg[n-5]=='(' && reg[n-1]==')'){
			string s = reg.substr(n-4,3);
			if(registers.find(s)!=registers.end()){
				return true;
			}else{
				return false;
			}
		}else{
			return false;
		}
	}else{
		return true;
	}
}

int locateAddress(string reg){
	int addr;
	int n=reg.length();
	if(check_number(reg)){
		addr=stoi(reg);
	}else{
		int num=0;
		string first = reg.substr(0,n-5);
		string second = reg.substr(n-4,3);
		if (n>=7 && reg.substr(n-7)=="($zero)"){
			first = reg.substr(0,n-7);
			second = reg.substr(n-6,5);
		}else if (reg[n-4]=='(' && reg[n-1]==')'){
			first = reg.substr(0,n-4);
			second = reg.substr(n-3,2);
		}else{
			first = reg.substr(0,n-5);
			second = reg.substr(n-4,3);
		}
		if(first!=""){
			num = stoi(first);
		}
		addr=(num+registers[second]);
	}
	if(addr>=(1<<18)){
		addr=-2;
	}
	if(addr%4!=0){
		addr=-1;
	}
	return addr;
}

void printOperations(){
	for (auto i: operations){
		cout<<i.first<<": "<<i.second<<"\n";
	}
}

void fillRegs(){
	registers["$r0"]=0;
	registers["$at"]=0;
	registers["$v0"]=0;
	registers["$v1"]=0;
	registers["$zero"] =0;
	
	string c;
	int m;
	for(int i=0;i<3;i++){
		switch(i)
		{
			case 0: {c="$a";m=4;break;}
			case 1: {c="$t";m=10;break;}
			case 2: {c="$s";m=9;break;}
		}
		for(int j=0;j<m;j++){
			registers[c+to_string(j)]=0;
		}
	}
	for (int i=0;i<=32;i++){
		c="$"+to_string(i);
		registers[c]=0;
	}
	registers["$k0"]=0;
	registers["$k1"]=0;
	registers["$gp"]=0;
	registers["$sp"]=2147479548;
	registers["$ra"]=0;
}

void store(){
	for(int i=0;i<256;i++){
		memory[currentRow][i]=ROW_BUFFER[i];
	}
}

void load(){
	for(int i=0;i<256;i++){
		ROW_BUFFER[i]=memory[currentRow][i];
	}
}

void fillOpers()
{
	operations["add"]=0;
	operations["sub"]=0;
	operations["mul"]=0;
	operations["beq"]=0;
	operations["bne"]=0;
	operations["slt"]=0;
	operations["j"]=0;
	operations["li"]=0; 
	operations["lw"]=0;
	operations["sw"]=0;
	operations["addi"]=0;
}

vector<string> lexer(string line){
	int n=line.length();
	vector<string> v;
	bool first=false;
	bool second=false;
	string s="";
	int i=0;
	while(i<n){
		if(first){
			if(second){
				if(line[i]==','){
					v.push_back(s);
					s="";
					i++;
					while(line[i]==' ' || line[i]=='\t'){
						if(i<n){
							i++;
						}else{
							break;
						}
					}
				}else{
					if (line[i]!=' ' && line[i]!='\t')
						s+=line[i];
					i++;
				}
			}else{
				if(line[i]==' '||line[i]=='\t'){
					second=true;
					v.push_back(s);
					s="";
					while(line[i]==' ' || line[i]=='\t'){
						if(i<n){
							i++;
						}
						else{
							break;
						}
					}
				}else{
					s+=line[i];
					i++;    
				}
			}
		}else{
			if(line[i]!=' ' && line[i]!='\t'){
				first=true;
				s+=line[i];
			}
			i++;
		}
	}
	if (s!="")
		v.push_back(s);
	return v;
}

void parser(vector<string> tokens){
	cout<<"Current instruction: "<<instructions[itr]<<endl;
	int m=tokens.size();
	string s0=tokens[0];
	if(m>4){
		cout<<"Syntax Error on line "<<(++itr)<<endl;
		throwError =1;
		return;
	}else if(m!=1){
		if(operations.find(s0)!=operations.end()){
			if(m==2){
				string s1=tokens[1];
				if (s0!="j"){
					cout <<"Invalid instruction on line "<<(++itr)<<endl;
					throwError =1;
					return;
				}else if(labels.find(s1)!=labels.end()){
					itr=labels[s1];
					if(alt_clock<clockCycles){
						cout<<"cycle "<<++alt_clock<<": Branching to "<<s1<<endl;
					}else{
						cout<<"cycle "<<++clockCycles<<": Branching to "<<s1<<endl;
						alt_clock++;
					}
				}else{
					cout<<"Such a label doesn't exist, error on line "<<(++itr)<<endl;
					throwError=1;
					return;
				}
			}else if(m==3){
				string s1=tokens[1];
				string s2=tokens[2];
				if(registers.find(s1)==registers.end()){
					cout<<"Invalid register\n";
					throwError=1;
					return;
				}else if(s1=="$zero" && s0!="sw"){
					cout << "Value of $zero cannot be changed on line "<<(++itr)<<endl;
					throwError=1;
					return;
				}
				if(s0=="li"){
					if(!check_number(s2)){
						cout<<"Please provide a number as a second argument in li on line "<<(++itr)<<endl;
						throwError=1;
						return;
					}
					if(alt_clock<clockCycles){
						while(!queuedRegs.empty()){
							pair<string,int> p = queuedRegs.front();
							if(engagedRegs.find(s1)!=engagedRegs.end()){
								if(!engagedRegs[s1].empty()){
									int x = engagedRegs[s1].front();
									if(x<0){
										break;
									}else{
										alt_clock = p.second;
										queuedRegs.pop();
										engagedRegs[p.first].pop();
									}
								}else{
									break;
								}
							}else{
								break;
							}
						}
						registers[s1]=stoi(s2);
						cout<<"cycle "<<++alt_clock<<": "<<s1<<" = "<<s2<<endl;	
					}else{
						registers[s1]=stoi(s2);
						cout<<"cycle "<<++clockCycles<<": "<<s1<<" = "<<s2<<endl;
						++alt_clock;
					}
				}else{
					if(!checkAddress(s2)){
						cout<<"Invalid format of memory address on line "<<(++itr)<<endl;
						throwError=1;
						return;
					}
					int address=locateAddress(s2);
					int ROW_ADDRESS = address/1024;
					int COL_ADDRESS = (address%1024)/4;
					if(s0=="lw"){
						if(address==-2){
							cout <<"Only 2^20 Bytes of memory could be used on line "<<(++itr)<<endl;
							throwError=1;
							return;
						}
						if(address==-1){
							cout<<"Unaligned memory address on line "<<(++itr)<<endl;
							throwError=1;
							return;
						}
						if(clockCycles==alt_clock){
							clockCycles++;
						}
						cout<<"cycle "<<++alt_clock<<": DRAM request issued"<<endl;
						if(ROW_ADDRESS!=currentRow){
							cout<<"cycle "<<++clockCycles<<"-";
							if(currentRow!=-1){
								store();
								clockCycles+=ROW_ACCESS_DELAY;
							}
							currentRow = ROW_ADDRESS;
							load();
							clockCycles+=ROW_ACCESS_DELAY;
							bufferUpdates++;
						}else{
							cout<<"cycle "<<++clockCycles<<"-";
						}
						clockCycles+=(COL_ACCESS_DELAY-1);
						registers[s1]=ROW_BUFFER[COL_ADDRESS];
						queuedRegs.push(make_pair(s1,clockCycles));
						engagedRegs[s1].push(clockCycles);
						cout<<clockCycles<<": "<<s1<<" = "<<registers[s1]<<endl;
					}else if(s0=="sw"){
						while(!queuedRegs.empty()){
							pair<string,int> p = queuedRegs.front();
							if(engagedRegs.find(s1)!=engagedRegs.end()){
								if(!engagedRegs[s1].empty()){
									int x = engagedRegs[s1].front();
									if(x<0){
										break;
									}else{
										alt_clock = p.second;
										queuedRegs.pop();
										engagedRegs[p.first].pop();
									}
								}else{
									break;
								}
							}else{
								break;
							}
						}
						if (address==-2){
							cout <<"Only 2^20 Bytes of memory could be used on line "<<(++itr)<<endl;
							throwError=1;
							return;
						}
						if(address==-1){
							cout<<"Unaligned memory address on line "<<(++itr)<<endl;
							throwError=1;
							return;
						}
						if(clockCycles==alt_clock){
							clockCycles++;
						}
						cout<<"cycle "<<++alt_clock<<": DRAM request issued"<<endl;
						if(ROW_ADDRESS!=currentRow){
							cout<<"cycle "<<++clockCycles<<"-";
							if(currentRow!=-1){
								store();
								clockCycles+=ROW_ACCESS_DELAY;
							}
							currentRow = ROW_ADDRESS;
							load();
							clockCycles+=ROW_ACCESS_DELAY;
							bufferUpdates++;
						}else{
							cout<<"cycle "<<++clockCycles<<"-";
						}
						clockCycles+=(COL_ACCESS_DELAY-1);
						ROW_BUFFER[COL_ADDRESS]=registers[s1];
						bufferUpdates++;
						queuedRegs.push(make_pair(s1,clockCycles));
						engagedRegs[s1].push((-1)*clockCycles);
						cout<<clockCycles<<": Memory address "<<address<<"-"<<address+3<<"="<<registers[s1]<<endl;	
					}else{
						cout<<"Invalid instruction on line "<<(++itr)<<endl;
						throwError=1;
						return;
					}
				}
			}else if(m==4){
				string s1=tokens[1];
				string s2=tokens[2];
				string s3=tokens[3];
				if(registers.find(s1)!=registers.end() && registers.find(s2)!=registers.end()){
					while(!queuedRegs.empty()){
						int k = 0;
						pair<string,int> p = queuedRegs.front();
						if(engagedRegs.find(s1)!=engagedRegs.end()){
							if(!engagedRegs[s1].empty()){
								int x = engagedRegs[s1].front();
								if(x>0){
									k++;
								}
							}
						}
						if(engagedRegs.find(s2)!=engagedRegs.end()){
							if(!engagedRegs[s2].empty()){
								int x = engagedRegs[s2].front();
								if(x>0){
									k++;
								}
							}
						}
						if(engagedRegs.find(s3)!=engagedRegs.end()){
							if(!engagedRegs[s3].empty()){
								int x = engagedRegs[s3].front();
								if(x>0){
									k++;
								}
							}
						}
						if(k==0){
							break;
						}else{
							alt_clock = p.second;
							queuedRegs.pop();
							engagedRegs[p.first].pop();
						}
					}
					if(s0=="beq"){
						if(registers[s1]==registers[s2]){
							if(labels.find(s3)!=labels.end()){
								itr=labels[s3];
								if(alt_clock<clockCycles){
									cout<<"cycle "<<++alt_clock<<": Branching to "<<s3<<endl;
								}else{
									cout<<"cycle "<<++clockCycles<<": Branching to "<<s3<<endl;
									++alt_clock;
								}
							}else{
								cout<<"Invalid label on line "<<(++itr)<<endl;
								throwError=1;
								return;
							}
						}else{
							if(alt_clock<clockCycles){
								cout<<"cycle "<<++alt_clock<<": Branching not executed to "<<s3<<endl;
							}else{
								cout<<"cycle "<<++clockCycles<<": Branching not executed to "<<s3<<endl;
								++alt_clock;
							}
						}
					}
					else if(s0=="bne"){
						if(registers[s1]!=registers[s2]){
							if(labels.find(s3)!=labels.end()){
								itr=labels[s3];
								if(alt_clock<clockCycles){
									cout<<"cycle "<<++alt_clock<<": Branching to "<<s3<<endl;
								}else{
									cout<<"cycle "<<++clockCycles<<": Branching to "<<s3<<endl;
									++alt_clock;
								}
							}else{
								cout<<"Invalid label on line "<<(++itr)<<endl;
								throwError=1;
								return;
							}
						}else{
							if(alt_clock<clockCycles){
								cout<<"cycle "<<++alt_clock<<": Branching not executed to "<<s3<<endl;
							}else{
								cout<<"cycle "<<++clockCycles<<": Branching not executed to "<<s3<<endl;
								++alt_clock;
							}
						}
					}
					else{
						if (s1=="$zero"){
							cout << "value of $zero cannot be changed on line "<<(++itr)<<endl;
							throwError=1;
							return;
						}else if(s0=="add"){
							if(registers.find(s3)!=registers.end()){
								registers[s1]=registers[s2]+registers[s3];
								if(alt_clock<clockCycles){
									cout<<"cycle "<<++alt_clock<<": "<<s1<<" = "<<registers[s1]<<endl;
								}else{
									cout<<"cycle "<<++clockCycles<<": "<<s1<<" = "<<registers[s1]<<endl;
									++alt_clock;
								}	
							}else{
								cout<<"Invalid register on line "<<(++itr)<<endl;
								throwError=1;
								return;
							}
						}else if(s0=="sub"){
							if(registers.find(s3)!=registers.end()){
								registers[s1]=registers[s2]-registers[s3];
								if(alt_clock<clockCycles){
									cout<<"cycle "<<++alt_clock<<": "<<s1<<" = "<<registers[s1]<<endl;
								}else{
									cout<<"cycle "<<++clockCycles<<": "<<s1<<" = "<<registers[s1]<<endl;
									++alt_clock;
								}	
							}else{
								cout<<"Invalid register on line "<<(++itr)<<endl;
								throwError=1;
								return;
							}
						}
						else if(s0=="mul"){
							if(registers.find(s3)!=registers.end()){
								registers[s1]=registers[s2]*registers[s3];
								if(alt_clock<clockCycles){
									cout<<"cycle "<<++alt_clock<<": "<<s1<<" = "<<registers[s1]<<endl;
								}else{
									cout<<"cycle "<<++clockCycles<<": "<<s1<<" = "<<registers[s1]<<endl;
									++alt_clock;
								}
							}else{
								cout<<"Invalid register on line "<<(++itr)<<endl;
								throwError=1;
								return;
							}
						}else if(s0=="slt"){
							if(registers.find(s3)!=registers.end()){
								if(registers[s2]<registers[s3]){
									registers[s1]=1;
								}
								else{
									registers[s1]=0;
								}
								if(alt_clock<clockCycles){
									cout<<"cycle "<<++alt_clock<<": "<<s1<<" = "<<registers[s1]<<endl;
								}else{
									cout<<"cycle "<<++clockCycles<<": "<<s1<<" = "<<registers[s1]<<endl;
									++alt_clock;
								}
							}else{
								cout<<"Invalid register on line "<<(++itr)<<endl;
								throwError=1;
								return;
							}
						}else{	
							if(check_number(s3)){
								registers[s1]=registers[s2]+stoi(s3);
								if(alt_clock<clockCycles){
									cout<<"cycle "<<++alt_clock<<": "<<s1<<" = "<<registers[s1]<<endl;
								}else{
									cout<<"cycle "<<++clockCycles<<": "<<s1<<" = "<<registers[s1]<<endl;
									++alt_clock;
								}
							}else{
								cout<<"Immediate value is not an integer on line "<<(++itr)<<endl;
								throwError=1;
								return;
							}
						}
					}					
				}else{
					cout<<"Invalid register on line "<<(++itr)<<endl;
					throwError=1;
					return;
				}
			}
			itr++;
			if(m!=1){
				operations[s0]++;
			}
			while(!queuedRegs.empty()){
				pair<string,int> p = queuedRegs.front();
				if(p.second<=alt_clock){
					queuedRegs.pop();
					engagedRegs[p.first].pop();
				}else{
					break;
				}
			}
		}
		else{
			cout<<"Invalid Instruction on line "<<(++itr)<<endl;
			throwError=1;
			return;
		}
	}else{
		itr++;
	}	
}

int main(int argc, char** argv)
{
	string fileName = argv[1];
	ifstream myFile(fileName);
	string line;
	while(getline(myFile,line)){
		vector <string> strings;
		strings = lexer(line);
		if(strings.size()!=0){
			int x = line.find(':');
			if (x!=string::npos){
				instructions.push_back(line.substr(0,x+1));
				line = line.substr(x+1);
				strings = lexer(line);
			}
			if (strings.size()!=0)
				instructions.push_back(line);
		}
	}
	int n=instructions.size();

	if(argc>2){
		if(!check_number(argv[2]) || !check_number(argv[3])){
			cout<<"Invalid values of row and/or col access delays."<<endl;
			throwError = 1;
			return 0;
		}else{
			ROW_ACCESS_DELAY = stoi(argv[2]);
			COL_ACCESS_DELAY = stoi(argv[3]);
		}
	}
	fillRegs();
	fillOpers();

	for(int i=0;i<n;i++){
		string currentLine = instructions[i];
		vector<string> strings;
		strings=lexer(currentLine);

		if(strings.size()==1){
			int l=strings[0].size();
			if(strings[0][l-1]==':'){
				if (operations.find(strings[0].substr(0,l-1))!=operations.end() || registers.find(strings[0].substr(0,l-1))!=registers.end()){
					cout << "A label name can't be reserved keyword on line "<<(++i)<<endl;
					throwError = 1;
					return 0;
				}
				if (labels.find(strings[0].substr(0,l-1))!=labels.end()){
					cout << "You cannot provide more than 1 set of instructions for same label on line "<<(++i)<<endl;
					throwError = 1;
					return 0;
				}
				labels[strings[0].substr(0,l-1)]=i;
			}else{
				cout<<"Colon required at the end of label on line "<<(++i)<<endl;
				throwError=1;
				return 0;
			}
		}

		if(strings.size()==2 && strings[1]==":"){
			if (operations.find(strings[0])!=operations.end() || registers.find(strings[0])!=registers.end()){
				cout << "A label name can't be reserved keyword on line "<<(++i)<<endl;
				throwError =1;
				return 0;
			}
			if (labels.find(strings[0])!=labels.end()){
				cout << "You cannot provide more than 1 set of instructions for same label on line "<<(++i)<<endl;
				throwError =1;
				return 0;
			}
			labels[strings[0]]=i;
			instructions[i]=strings[0]+":";
		}
	}

	while(itr<n){
		string currentLine = instructions[itr];
		vector<string> strings;

		strings=lexer(currentLine);
		parser(strings);
		if(throwError==1){
			return 0;
		}
	}

	cout<<"\nTotal number of clock cycles before the last store operation: "<<clockCycles++<<endl;
	store();
	cout<<"cycle "<<clockCycles<<"-";
	clockCycles+=(ROW_ACCESS_DELAY-1);
	cout<<clockCycles<<": "<<"Final store operation from row buffer to main memory."<<endl;
	cout<<"Total number of clock cycles: "<<clockCycles<<endl;

	cout<<"\nTotal number of buffer updates: "<<bufferUpdates<<endl;
	cout<<"\nTotal number of operations:"<<endl;
	printOperations();

	cout<<"\n\n";

	cout<<"Modified memory cells(non-zero)"<<endl;
	printData();

	return 0;
}