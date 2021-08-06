#include<bits/stdc++.h>
#include "core.hpp"
using namespace std;

string priority[MAX_CORE] ={""};

// Run the MRM fo once
void runMRM(){
	if (waitingList.empty()){
		isReady =0;
		command = {-1,-1,-1,"",-1};
		return;
	}

	int i = currCore;
	int row = currRow;
	bool check3 = (currCount>=5);
	bool check4 = (waitingList.find(currCore)== waitingList.end());
	bool check5 = (!check4 && waitingList[currCore].find(currRow)== waitingList[currCore].end());
	if (check3 || check4 || check5){
		bool done=false;
		// Changing the Row according to core need
		for(int j=1;j<=MAX_CORE;j++){
			if (priority[(currCore+j)%N]!=""){
				i = (currCore+j)%N;
				if (waitingList.find(i)== waitingList.end()) continue;
				if (priority[i]=="do"){
					// We just need to make some space in the waiting queue
					row = (*waitingList[i].begin()).first;
				}
				else{
					// Need priority for some register
					int address  = cores[i]->registerUpdate[priority[i]].second;
					row = address/1024;
				}
				done = true;
				break;
			}
		}
		// Changing the Row if all cores are happy
		if (!done){
			for (int j=1; j<=MAX_CORE;j++){
				if (waitingList.find((currCore+j)%N) != waitingList.end()){
					i= (currCore+j)%N;
					break;
				}
			}
			row = (*waitingList[i].begin()).first;
		}
	}

	tuple<int, string, int> comm = waitingList[i][row].front();

	int count = get<0>(comm);
	string reg = get<1>(comm);
	int col = get<2>(comm);
	bool check_lw = (!check_number(reg));	// true if "lw" operation
	bool check1 = check_lw && (cores[i]->registerUpdate.find(reg)== cores[i]->registerUpdate.end());	// means load on that register is not required, hence redundant operation
	bool check2 = (check_lw && (!check1)) && (cores[i]->registerUpdate[reg].first != count);	// Update on register is required but not here, hence redundant operation

	stringstream buffercout;
	buffercout<< "(Core "<< i+1<<") " << cores[i]->insCounter[count-1]+1 <<" => " <<cores[i]->instructions[cores[i]->insCounter[count-1]];
	if (check_lw && (check1 || check2)){
		totalInstructions++;
		waitingList[i][row].pop();
		queueSize[i]--;
		if (waitingList[i][row].empty() ) waitingList[i].erase(row);
		if (waitingList[i].empty()) waitingList.erase(i);
		buffercout <<" is scrapped\n";
		print[{DRAMclock, -1*DRAMclock}][-1] = buffercout.str();
		isReady =0;
		command = {-1,-1,-1,"",-1};
	}
	else{
		isReady=1;
		if (time_req == DRAMclock || time_req==-1){
			buffercout <<" is sent to DRAM\n";
			print[{DRAMclock, -1*DRAMclock}][-1] = buffercout.str();
		}
		command = {row, col, count, reg, i};
	}
	
}

// This should never be passed any redundant instruction
void processCommand(tuple <int, int, int, string, int> command){
	// As you process, don't forget to remove from the thing from Register Update
	int row = get<0>(command);
	int col = get<1>(command);
	int count = get<2>(command);
	string reg = get<3>(command);
	int i = get<4>(command);
	if (row==-1 && col==-1 && count==-1 && reg=="" && i==-1) return;

	int address = 1024*row + 4*col;

	time_req+=DRAMclock;

	if (currRow!= row){
		if (currRow != -1 && dirty){
			DRAM[currRow]  = buffer;
			time_req+= row_access_delay;
		}
		currRow = row;
		currCore = i;
		currCount=1;
		buffer = DRAM[currRow];
		dirty = false;
		row_buffer_updates++;
		time_req+= row_access_delay;
	}
	else{
		currCount++;
	}
	if (priority[i]=="do") priority[i] ="";
	if (check_number(reg)){
		// Means this was a sw operation, and we are not ignoring any sw operation
		dirty = true;
		buffer[col] = stoi(reg);
		row_buffer_updates++;
		time_req+= col_access_delay;
		store = {"sw",to_string(DRAMclock),to_string(address) + "-" + to_string(address+3),to_string(stoi(reg)), count, i};
	}
	else{
		// means this is a lw operation
		cores[i]->registers[reg] = buffer[col];
		if (cores[i]->registerUpdate[reg].first == count){
			cores[i]->registerUpdate.erase(reg);
			if (priority[i]==reg) priority[i]= "";
		}
		time_req+= col_access_delay;
		store  = {"lw", to_string(DRAMclock), reg, to_string(buffer[col]), count, i};
	}
	waitingList[i][row].pop();
	if (waitingList[i][row].empty() ) waitingList[i].erase(row);
	if (waitingList[i].empty()) waitingList.erase(i);
}

// This marks the end of a process running in the DRAM 
void processCompletion(){
	totalInstructions++;
	stringstream buffercout;
    int count = get<4>(store);
	int i  = get<5>(store);
	queueSize[i]--;
	string reg = get<2>(store);
	if (count == get<2>(cores[i]->last_sw) ) cores[i]->last_sw = {-1,0,-1};
	buffercout << to_string(cores[i]->insCounter[count-1]+1) +" => " +cores[i]->instructions[cores[i]->insCounter[count-1]]+"; ";
	if (get<0>(store)=="lw"){
		if (cores[i]->forRefusing[reg] > count) buffercout << "Rejected by processor, "<< reg<< " already updated in procesor\n";
		else  buffercout << reg + "= " + get<3>(store) + "\n";
	}
	else{
		buffercout << "Memory address " + reg + "= " + get<3>(store) + "\n";
	}
	print[{stoi(to_string(time_req)),-1*stoi(get<1>(store))}][i] = buffercout.str();
	time_req =-1;
	store = tuple <string ,string, string, string, int, int>();
}

// To check whether the register is complete or not, returns true is it is not an unsafe instruction
bool checkComplete(string name, int i){
	if (cores[i]->registerUpdate.find(name)!= cores[i]->registerUpdate.end()){
		priority[i] = name;
		return false;
	}
	if (get<2>(store) == name && get<5>(store) == i) return false;
	return true;
}

// Parser executes the required functions (tokens and core_num as arguments)
void parser(vector<string> tokens, int i){
	cores[i]->insCounter.push_back(cores[i]->itr);
	cores[i]->counter++;
	int m=tokens.size();
	string s0=tokens[0];
	if(m>4){
		cout<<"Core "<<i+1<<": Syntax Error on line "<<(++cores[i]->itr)<<endl;
		cores[i]->error =1;
		return;
	}
	if (m==1){
		cores[i]->itr++;
        if (cores[i]->itr < cores[i]->instructions.size()) parser(lexer(cores[i]->instructions[cores[i]->itr]), i);
		return;
	}
	if(cores[i]->operations.find(s0)==cores[i]->operations.end()){
		cout<<"Core "<<i+1<<": Invalid instruction on line "<<(++cores[i]->itr)<<endl;
		cores[i]->error=1;
		return;
	}

	if(m==2){
		string s1=tokens[1];
		if (s0!="j"){
			cout<<"Core "<<i+1<<": Invalid instruction on line "<<(++cores[i]->itr)<<endl;
			cores[i]->error =1;
			return;
		}
		if(cores[i]->labels.find(s1)==cores[i]->labels.end()){
			cout<<"Core "<<i+1<<": Invalid label on line "<<(++cores[i]->itr)<<endl;
			cores[i]->error=1;
			return;
		}
		stringstream buffercout;
		buffercout << cores[i]->itr+1<<" => "<<cores[i]->instructions[cores[i]->itr]<<"; ";
		buffercout <<"Jumping to "<<s1<<"\n";
		cores[i]->itr=cores[i]->labels[s1];
		print[{DRAMclock, -1*DRAMclock}][i] = buffercout.str();
		totalInstructions++;
	}
	else if(m==3){
		if (s0 != "lw" && s0!="sw"){
			cout<<"Core "<<i+1<<": Invalid instruction on line "<<(++cores[i]->itr)<<endl;
			cores[i]->error=1;
			return;
		}
		string s1=tokens[1];
		string s2=tokens[2];
		if(cores[i]->registers.find(s1)==cores[i]->registers.end()){
			cout<<"Core "<<i+1<<": Invalid register on line "<<(++cores[i]->itr)<<"\n";
			cores[i]->error=1;
			return;
		}
		if (s1== "$zero" && s0!="sw"){
			cout<<"Core "<<i+1<<": Value of $zero cannot be changed on line "<<(++cores[i]->itr)<<endl;
			cores[i]->error=1;
			return;
		}
		if(!checkAddress(s2)){
			cout<<"Core "<<i+1<<": Invalid format of memory address on line "<<(++cores[i]->itr)<<endl;
			cores[i]->error=1;
			return;
		}

		if (!checkComplete(extract_reg(s2), i) || (s0== "sw" && !checkComplete(s1, i))) return;
		if (s0== "sw" && checkComplete(s1,i) && just_did == make_tuple("lw", i, s1)) return;
		if (checkComplete(extract_reg(s2),i) && just_did == make_tuple("lw", i, extract_reg(s2))) return;

		int address=locateAddress(s2, i);
		if (address==-2){
			cout<<"Core "<<i+1<<": Only 2^20 Bytes of memory could be used on line "<<(++cores[i]->itr)<<endl;
			cores[i]->error=1;
			return;
		}
		if(address<0){
			cout<<"Core "<<i+1<<": Unaligned memory address on line "<<(++cores[i]->itr)<<endl;
			cores[i]->error=1;
			return;
		}
		int row = address/1024;
		int col = (address%1024)/4;

		if ( address_core.find(row)!= address_core.end() && address_core[row] != i+1){
			cout<<"Core "<<i+1<<": Memory address "<<address<<" already accessed in core "<<address_core[row]<<", error on line"<<(++cores[i]->itr)<<endl;
			cores[i]->error=1;
			return;
		}
		address_core[row] =i+1;

		// Incase the lw gets handled by forwarding
		if (s0=="lw" && address == get<0>(cores[i]->last_sw)){
			if (get<0>(just_did) == "lw" && get<1>(just_did) ==i) return;
			cores[i]->registers[s1] = get<1>(cores[i]->last_sw);
			cores[i]->forRefusing[s1] = cores[i]->counter;
			cores[i]->registerUpdate.erase(s1);
			stringstream buffercout;
			buffercout << cores[i]->itr+1<<" => "<<cores[i]->instructions[cores[i]->itr]<<"; ";
			buffercout << s1<<"= "<<get<1>(cores[i]->last_sw)<<"; Due to forwarding"<<"\n";
			print[{DRAMclock, -1*DRAMclock}][i] = buffercout.str();
			totalInstructions++;
		}
		// Normal lw and sw instruction 
		else{
			if (queueSize[i] == MAX_SIZE){
				priority[i]= "do";
				return;
			}
			else priority[i] = "";
			if (queueSize[i] == MAX_SIZE-1 && get<0>(just_did) !="" && get<1>(just_did)==i) return;
			stringstream buffercout;
			buffercout << cores[i]->itr+1<<" => "<<cores[i]->instructions[cores[i]->itr]<<"; ";
			buffercout << "DRAM request issued"<<"\n";
			print[{DRAMclock, -1*DRAMclock}][i] = buffercout.str();
			
			queueSize[i]++;
			if(s0=="lw"){
				waitingList[i][row].push({cores[i]->counter, s1, col});
				cores[i]->registerUpdate[s1] = {cores[i]->counter, address};
			}
			else if(s0=="sw"){
				waitingList[i][row].push({cores[i]->counter, to_string(cores[i]->registers[s1]), col});
				cores[i]->last_sw = {address, cores[i]->registers[s1], cores[i]->counter};
			}
		}
	}
	else if(m==4){
		string s1=tokens[1];
		string s2=tokens[2];
		string s3=tokens[3];
		if (cores[i]->registers.find(s1)==cores[i]->registers.end() || cores[i]->registers.find(s2)==cores[i]->registers.end()){
			cout<<"Core "<<i+1<<": Invalid register on line "<<(++cores[i]->itr)<<endl;
			cores[i]->error=1;
			return;
		}
		if(s0=="beq" || s0=="bne"){
			if(cores[i]->labels.find(s3)==cores[i]->labels.end()){
				cout<<"Core "<<i+1<<": Invalid label on line "<<(++cores[i]->itr)<<endl;
				cores[i]->error=1;
				return;
			}
			if (!checkComplete(s1,i) || !checkComplete(s2,i)) return;
			if (checkComplete(s1,i) && just_did == make_tuple("lw", i, s1)) return;
			if (checkComplete(s2,i) && just_did == make_tuple("lw", i, s2)) return;
			int toJump = 0;
			if (s0 == "beq" && cores[i]->registers[s1]==cores[i]->registers[s2]){
				toJump = 1;
			}
			else if (s0 == "bne" && cores[i]->registers[s1]!=cores[i]->registers[s2]){
				toJump = 1;
			}
			stringstream buffercout;
			buffercout << cores[i]->itr+1<<" => "<<cores[i]->instructions[cores[i]->itr]<<"; ";
			if(toJump==1){
				cores[i]->itr=cores[i]->labels[s3];
				buffercout <<"Jumping to "<<s3<<"\n";
			}
			else{
				buffercout <<"No jump required to "<< s3 <<"\n";
			}
			print[{DRAMclock, -1*DRAMclock}][i] = buffercout.str();
			totalInstructions++;
		}
		else if (s0=="add" || s0=="sub" || s0=="mul" || s0 == "slt" || s0=="addi"){

			if (s1== "$zero"){
				cout<<"Core "<<i+1<<": Value of $zero cannot be changed on line "<<(++cores[i]->itr)<<endl;
				cores[i]->error=1;
				return;
			}
			if(s0!= "addi" && cores[i]->registers.find(s3)==cores[i]->registers.end()){
				cout<<"Core "<<i+1<<": Invalid register on line "<<(++cores[i]->itr)<<endl;
				cores[i]->error=1;
				return;
			}
			if(s0=="addi" && !check_number(s3)){
				cout<<"Core "<<i+1<<": Immediate value is not an integer on line "<<(++cores[i]->itr)<<endl;
				cores[i]->error=1;
				return;
			}

			if (!checkComplete(s2,i) || (s0 != "addi" && !checkComplete(s3,i))) return;
			if (checkComplete(s2,i) && just_did == make_tuple("lw", i, s2)) return;
			if (s0 != "addi" && checkComplete(s3,i) && just_did == make_tuple("lw", i, s3)) return;

			if (get<0>(just_did) == "lw" && get<1>(just_did) ==i) return;

			if (s0 == "add") cores[i]->registers[s1]=cores[i]->registers[s2]+cores[i]->registers[s3];
			else if (s0=="sub") cores[i]->registers[s1]=cores[i]->registers[s2]-cores[i]->registers[s3];
			else if (s0 == "mul") cores[i]->registers[s1]=cores[i]->registers[s2]*cores[i]->registers[s3];
			else if (s0 == "slt") cores[i]->registers[s1]= (cores[i]->registers[s2]<cores[i]->registers[s3]);
			else if (s0=="addi") cores[i]->registers[s1]=cores[i]->registers[s2]+stoi(s3);

			cores[i]->forRefusing[s1] = cores[i]->counter;
			cores[i]->registerUpdate.erase(s1);	
			stringstream buffercout;
			buffercout << cores[i]->itr+1<<" => "<<cores[i]->instructions[cores[i]->itr]<<"; ";
			buffercout <<s1<<"= "<<cores[i]->registers[s1]<<"\n";
			print[{DRAMclock, -1*DRAMclock}][i] = buffercout.str();
			totalInstructions++;
		}
		
		else{
			cout<<"Core "<<i+1<<": Invalid instruction on line "<<(++cores[i]->itr)<<endl;
			cores[i]->error=1;
			return;
		}
	}
	cores[i]->itr++;
	if(m!=1){
		cores[i]->operations[s0]++;
	}
	
}

int main(int argc, char** argv){
	initialize(argc, argv);
	if(throwError==1){
		return 0;
	}

	// Start running the file untill we reach the max time
	while(DRAMclock <= MAX_TIME){

		if (time_req==-1 && isReady) processCommand(command);

		runMRM();
		
		just_did = {"", -1, ""};
        if (time_req == DRAMclock){
			get<0>(just_did) = get<0>(store);
			get<1>(just_did) = get<5>(store);
			get<2>(just_did) = get<2>(store);
            processCompletion();
        }

		for (int i=0;i<N;i++){
			if (cores[i]->error==0 && cores[i]->itr < cores[i]->instructions.size()) parser(lexer(cores[i]->instructions[cores[i]->itr]), i);
		}
		
		DRAMclock++;
	}

	if (currRow!=-1 && dirty) DRAM[currRow] = buffer;

	print_stats();

	return 0;
}
