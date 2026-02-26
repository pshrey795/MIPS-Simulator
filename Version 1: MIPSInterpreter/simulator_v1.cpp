#include<bits/stdc++.h>
using namespace std;

unordered_map<string,int> registers;
unordered_map<string,int> operations;
unordered_map<string,int> labels;
vector <string> regPrint;
int memory[1<<18] = {0};
vector<string> instructions;
int itr = 0;
int throwError = 0;
int clockCycles=0;

// For converting the decimal values into hexadecimal values
string int_to_hex(int n){
	string binary;	// to first convert the decimal into 32-bit signed number.
	int np = abs(n);
	for (int i=0;i<32;i++){
		binary += '0';
	}
	int i=31;
	while(np!=0 && i>=0){
		if (np%2==1) binary[i] = '1';
		np/=2;
		i--;
	}
	i=31;
	while(n<0 && i>=0){
		if (binary[i]=='0') binary[i] = '1';
		else binary[i] = '0';
		i--;
	}
	i=31;
	while(n<0 && i>=0){
		if (binary[i]=='0'){
			binary[i]='1';
			break;
		}
		else{
			binary[i] = '0';
			i--;
		}
	}
	string hex;		// For converting the binary into hexadecimal
	for (int i=0;i<8;i++){
		int val=0;
		val = (binary[31-4*i]-'0')+(binary[31-4*i-1]-'0')*2+(binary[31-4*i-2]-'0')*4+(binary[31-4*i-3]-'0')*8;
		if (val>=0 && val<=9) hex = char('0'+val) + hex;
		else{
			hex = char('a'+val-10) + hex;
		}
	}

	return hex;
}

// For showing the contents of a register
void show(string a){
	cout <<a.substr(1)<<": "<<int_to_hex(registers[a])<<"\n";
}

// To check if a string denotes a integer or not
bool check_number(string str){
	if(str.length()==0)
		return true;
   	if(!isdigit(str[0])){
	   	if(str[0]!='-' && str[0]!='+'){
		   return false;
	   	}
   	}
   	for (int i = 1;i < str.length(); i++)
   		if (isdigit(str[i]) == false)
      		return false;
    return true;
}

// To check whether somethig is a valid memory 
bool checkAddress(string reg){
	int n=reg.length();
	if(!check_number(reg)){
		if (n>=7 && reg.substr(n-7)=="($zero)") return true;
		
		if(n<4) return false;

		if (reg[n-4]=='(' && reg[n-1]==')'){
			string s = reg.substr(n-3,2);
			if(registers.find(s)!=registers.end()){
				return true;
			}
			else{
				return false;
			}
		}
		else{
			if (n<5) return false;
			if(reg[n-5]=='(' && reg[n-1]==')'){
				string s = reg.substr(n-4,3);
				if(registers.find(s)!=registers.end()){
					return true;
				}
				else{
					return false;
				}
			}
			else{
				return false;
			}
		}
	}
	else{
		// it is a number and hence can be directly used as a address
		return true;
	}
}

// To get the memory address from a string
int locateAddress(string reg){
	int addr;
	int n=reg.length();
	if(check_number(reg)){
		addr=stoi(reg);
	}
	else{
		int num=0;
		string first = reg.substr(0,n-5);
		string second = reg.substr(n-4,3);
		if (n>=7 && reg.substr(n-7)=="($zero)"){
			first = reg.substr(0,n-7);
			second = reg.substr(n-6,5);
		}
		else if (reg[n-4]=='(' && reg[n-1]==')'){
			first = reg.substr(0,n-4);
			second = reg.substr(n-3,2);
		}
		if(first!=""){
			num = stoi(first);
		}
		addr=(num+registers[second]);
	}
	if(addr%4==0){
		addr=addr/4;
	}
	else{
		// because we are handling just lw and sw, in them it must be multiple of 4, in lb it could be anything
		addr=-1;
	}
	if(addr>=(1<<18)){
		// Memory out of limits must be thrown
		addr=-2;
	}
	return addr;
}

// Printing the registers.
void printRegisters(){
	for (auto i: regPrint) show(i);
	cout<<"\n";
}

// Print the number of times each operation is executed.
void printOperations(){
	for (auto i: operations){
		cout<<i.first<<": "<<i.second<<"\n";
		clockCycles += i.second;
	}
	cout<<"\n";
}

// Filling the 32 registers and $zero in the registrs map.
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
		c= "$"+to_string(i);
		registers[c] =0;
	}
	registers["$k0"]=0;
	registers["$k1"]=0;
	registers["$gp"]=0;
	registers["$sp"]=2147479548;
	registers["$ra"]=0;
}

// To print the registers in a defined order.
void fillregPrint(){
	regPrint.push_back("$r0");
	regPrint.push_back("$at");
	regPrint.push_back("$v0");
	regPrint.push_back("$v1");
	regPrint.push_back("$a0");
	regPrint.push_back("$a1");
	regPrint.push_back("$a2");
	regPrint.push_back("$a3");
	regPrint.push_back("$t0");
	regPrint.push_back("$t1");
	regPrint.push_back("$t2");
	regPrint.push_back("$t3");
	regPrint.push_back("$t4");
	regPrint.push_back("$t5");
	regPrint.push_back("$t6");
	regPrint.push_back("$t7");
	regPrint.push_back("$s0");
	regPrint.push_back("$s1");
	regPrint.push_back("$s2");
	regPrint.push_back("$s3");
	regPrint.push_back("$s4");
	regPrint.push_back("$s5");
	regPrint.push_back("$s6");
	regPrint.push_back("$s7");
	regPrint.push_back("$t8");
	regPrint.push_back("$t9");
	regPrint.push_back("$k0");
	regPrint.push_back("$k1");
	regPrint.push_back("$gp");
	regPrint.push_back("$sp");
	regPrint.push_back("$s8");
	regPrint.push_back("$ra");
}

// Filling operations in the Operations map and initialised to zero. 
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

// Parser executes the required functions
void parser(vector<string> tokens){
	int m=tokens.size();
	string s0=tokens[0];
	if(m>4){
		cout<<"Syntax Error on line "<<(++itr)<<endl;
		throwError =1;
		return;
	}
	if(m!=1){
		if(operations.find(s0)!=operations.end()){
			if(m==2){
				string s1=tokens[1];
				if (s0!="j"){
					cout <<"Invalid instruction on line "<<(++itr)<<endl;
					throwError =1;
					return;
				}
				if(labels.find(s1)!=labels.end()){
					itr=labels[s1];
					printRegisters();
				}
				else{
					cout<<"Such a label doesn't exist on line "<<(++itr)<<endl;
					throwError=1;
					return;
				}
			}
			else if(m==3){
				string s1=tokens[1];
				string s2=tokens[2];
				if(registers.find(s1)==registers.end()){
					cout<<"Invalid register\n";
					throwError=1;
					return;
				}
				if (s1== "$zero"){
					cout << "value of $zero cannot be changed on line "<<(++itr)<<endl;
					throwError=1;
					return;
				}
				if(s0=="li"){
					if(!check_number(s2)){
						cout<<"Please provide a number as a second argument in li on line "<<(++itr)<<endl;
						throwError=1;
						return;
					}
					registers[s1]=stoi(s2);
					printRegisters();
				}
				else{
					if(!checkAddress(s2)){
						cout<<"Invalid format of memory address. on line "<<(++itr)<<endl;
						throwError=1;
						return;
					}
					int address=locateAddress(s2);
					if(s0=="lw"){
						if (address==-2){
							cout <<"Only 2^20 Bytes of memory could be used on line "<<(++itr)<<endl;
							throwError=1;
							return;
						}
						if(address<0){
							cout<<"Unaligned memory address on line "<<(++itr)<<endl;
							throwError=1;
							return;
						}
						registers[s1]=memory[address];
						printRegisters();
					}
					else if(s0=="sw"){
						if (address==-2){
							cout <<"Only 2^20 Bytes of memory could be used on line "<<(++itr)<<endl;
							throwError=1;
							return;
						}
						if(address<0){
							cout<<"Unaligned memory address on line "<<(++itr)<<endl;
							throwError=1;
							return;
						}
						else{
							memory[address]=registers[s1];
							printRegisters();
						}		
					}
					else{
						cout<<"Invalid instruction on line "<<(++itr)<<endl;
						throwError=1;
						return;
					}
				}
			}
			else if(m==4){
				string s1=tokens[1];
				string s2=tokens[2];
				string s3=tokens[3];
				if(registers.find(s1)!=registers.end() && registers.find(s2)!=registers.end()){
					if(s0=="beq"){
						if(registers[s1]==registers[s2]){
							if(labels.find(s3)!=labels.end()){
								itr=labels[s3];
								printRegisters();
							}
							else{
								cout<<"Invalid label on line "<<(++itr)<<endl;
								throwError=1;
								return;
							}
						}
					}
					else if(s0=="bne"){
						if(registers[s1]!=registers[s2]){
							if(labels.find(s3)!=labels.end()){
								itr=labels[s3];
								printRegisters();
							}
							else{
								cout<<"Invalid label on line "<<(++itr)<<endl;
								throwError=1;
								return;
							}
						}
					}
					else{
						// Because in the further operations we are going to change the value stored in regoister s1 
						if (s1== "$zero"){
							cout << "value of $zero cannot be changed on line "<<(++itr)<<endl;
							throwError=1;
							return;
						}
						if(s0=="add"){
							if(registers.find(s3)!=registers.end()){
								registers[s1]=registers[s2]+registers[s3];
								printRegisters();
							}
							else{
								cout<<"Invalid register on line "<<(++itr)<<endl;
								throwError=1;
								return;
							}
						}
						else if(s0=="sub"){
							if(registers.find(s3)!=registers.end()){
								registers[s1]=registers[s2]-registers[s3];
								printRegisters();
							}
							else{
								cout<<"Invalid register on line "<<(++itr)<<endl;
								throwError=1;
								return;
							}
						}
						else if(s0=="mul"){
							if(registers.find(s3)!=registers.end()){
								registers[s1]=registers[s2]*registers[s3];
								printRegisters();
							}
							else{
								cout<<"Invalid register on line "<<(++itr)<<endl;
								throwError=1;
								return;
							}
						}
						else if(s0=="slt"){
							if(registers.find(s3)!=registers.end()){
								if(registers[s2]<registers[s3]){
									registers[s1]=1;
								}
								else{
									registers[s1]=0;
								}
								printRegisters();
							}
							else{
								cout<<"Invalid register on line "<<(++itr)<<endl;
								throwError=1;
								return;
							}
						}
						else if(s0=="addi"){
							if(check_number(s3)){
								registers[s1]=registers[s2]+stoi(s3);
								printRegisters();
							}
							else{
								cout<<"Immediate value is not an integer on line "<<(++itr)<<endl;
								throwError=1;
								return;
							}
						}
					}					
				}
				else{
					cout<<"Invalid register on line "<<(++itr)<<endl;
					throwError=1;
					return;
				}
			}
			itr++;
			if(m!=1){
				operations[s0]++;
			}
		}
		else{
			cout<<"Invalid Instruction on line "<<(++itr)<<endl;
			throwError=1;
			return;
		}
	}
	else{
		itr++;
	}
	
}

// Lexer splits the string into tokens such that first 2 were space delimited in the input string and the remaining were space or tab delimited
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
						}
						else{
							break;
						}
					}
				}
				else{
					if (line[i]!=' ' && line[i]!='\t') s+=line[i];
					i++;
				}
			}
			else{
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
				}
				else{
					s+=line[i];
					i++;    
				}
			}
		}
		else{
			if(line[i]!=' ' && line[i]!='\t'){
				first=true;
				s+=line[i];
			}
			i++;
		}
	}
	if (s!="") v.push_back(s);
	return v;
}

int main(int argc, char** argv)
{
	string fileName = argv[1];
	ifstream myFile(fileName);
	string line;
	fillRegs();
	fillregPrint();
	fillOpers();

	while(getline(myFile,line)){
		vector <string> strings;
		strings = lexer(line);
		if(strings.size()!=0){
			int x = line.find(':');
			if (x!=string::npos){
				instructions.push_back(line.substr(0,x+1));
				line = line.substr(x+1);
			}
			strings = lexer(line);
			if (strings.size()!=0) instructions.push_back(line);
		}
	}
	int n=instructions.size();

	for(int i=0;i<n;i++){
		string currentLine = instructions[i];
		vector<string> strings;

		strings=lexer(currentLine);

		if(strings.size()==1){
			int l=strings[0].size();
			if(strings[0][l-1]==':'){
				// make sure it ain't the name of some operation or register
				if (operations.find(strings[0].substr(0,l-1))!=operations.end() || registers.find(strings[0].substr(0,l-1))!=registers.end()){
					cout << "A label name can't be reserved keyword on line "<<(++i)<<endl;
					throwError =1;
					return 0;
				}
				// Make sure we are not re-definig it
				if (labels.find(strings[0].substr(0,l-1))!=labels.end()){
					cout << "You cannot provide more than 1 set of instructions for same label on line "<<(++i)<<endl;
					throwError =1;
					return 0;
				}
				labels[strings[0].substr(0,l-1)]=i;
			}
			else{
				cout<<"Colon required at the end of label on line "<<(++i)<<endl;
				throwError=1;
				return 0;
			}
		}

		if(strings.size()==2 && strings[1]==":"){
			// make sure it ain't the name of some operation or register
			if (operations.find(strings[0])!=operations.end() || registers.find(strings[0])!=registers.end()){
				cout << "A label name can't be reserved keyword on line "<<(++i)<<endl;
				throwError =1;
				return 0;
			}
			// Make sure we are not re-definig it
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
		// printRegisters();
	}

	printOperations();
	cout<<"No. of clock cycles: "<<clockCycles<<"\n";

	return 0;
}
