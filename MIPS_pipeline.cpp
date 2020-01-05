#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;
#define MemSize 1000 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

struct IFStruct {
    bitset<32>  PC;
    bool        nop=0;  
};

struct IDStruct {
    bitset<32>  Instr;
    bool        nop=1;  
};

struct EXStruct {
    bitset<32>  Read_data1; 
    bitset<32>  Read_data2;
    bitset<16>  Imm;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        is_I_type=0;
    bool        rd_mem=0;
    bool        wrt_mem=0; 
    bool        alu_op=1;
    bool        wrt_enable=0;
    bool        nop=1;  
};

struct MEMStruct {
    bitset<32>  ALUresult;
    bitset<32>  Store_data;
    bitset<5>   Rs;
    bitset<5>   Rt;    
    bitset<5>   Wrt_reg_addr;
    bool        rd_mem=0;
    bool        wrt_mem=0; 
    bool        wrt_enable=0;    
    bool        nop=1;    
};

struct WBStruct {
    bitset<32>  Wrt_data;
    bitset<5>   Rs;
    bitset<5>   Rt;     
    bitset<5>   Wrt_reg_addr;
    bool        wrt_enable=0;
    bool        nop=1;     
};

struct stateStruct {
    IFStruct    IF;
    IDStruct    ID;
    EXStruct    EX;
    MEMStruct   MEM;
    WBStruct    WB;
};

class RF
{
    public: 
        bitset<32> Reg_data;
     	RF()
    	{ 
			Registers.resize(32);  
			Registers[0] = bitset<32> (0);  
        }
	
        bitset<32> readRF(bitset<5> Reg_addr)
        {   
            Reg_data = Registers[Reg_addr.to_ulong()];
            return Reg_data;
        }
    
        void writeRF(bitset<5> Reg_addr, bitset<32> Wrt_reg_data)
        {
            Registers[Reg_addr.to_ulong()] = Wrt_reg_data;
        }
		 
		void outputRF()
		{
			ofstream rfout;
			rfout.open("RFresult.txt",std::ios_base::app);
			if (rfout.is_open())
			{
				rfout<<"State of RF:\t"<<endl;
				for (int j = 0; j<32; j++)
				{        
					rfout << Registers[j]<<endl;
				}
			}
			else cout<<"Unable to open file";
			rfout.close();               
		} 
			
	private:
		vector<bitset<32> >Registers;	
};

class INSMem
{
	public:
        bitset<32> Instruction;
        INSMem()
        {       
			IMem.resize(MemSize); 
            ifstream imem;
			string line;
			int i=0;
			imem.open("imem.txt");
			if (imem.is_open())
			{
				while (getline(imem,line))
				{      
					IMem[i] = bitset<8>(line);
					i++;
				}                    
			}
            else cout<<"Unable to open file";
			imem.close();                     
		}
                  
		bitset<32> readInstr(bitset<32> ReadAddress) 
		{    
			string insmem;
			insmem.append(IMem[ReadAddress.to_ulong()].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+1].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+2].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+3].to_string());
			Instruction = bitset<32>(insmem);		//read instruction memory
			return Instruction;     
		}     
      
    private:
        vector<bitset<8> > IMem;     
};
      
class DataMem    
{
    public:
        bitset<32> ReadData;  
        DataMem()
        {
            DMem.resize(MemSize); 
            ifstream dmem;
            string line;
            int i=0;
            dmem.open("dmem.txt");
            if (dmem.is_open())
            {
                while (getline(dmem,line))
                {      
                    DMem[i] = bitset<8>(line);
                    i++;
                }
            }
            else cout<<"Unable to open file";
                dmem.close();          
        }
		
        bitset<32> readDataMem(bitset<32> Address)
        {	
			string datamem;
            datamem.append(DMem[Address.to_ulong()].to_string());
            datamem.append(DMem[Address.to_ulong()+1].to_string());
            datamem.append(DMem[Address.to_ulong()+2].to_string());
            datamem.append(DMem[Address.to_ulong()+3].to_string());
            ReadData = bitset<32>(datamem);		//read data memory
            return ReadData;               
		}
            
        void writeDataMem(bitset<32> Address, bitset<32> WriteData)            
        {
            DMem[Address.to_ulong()] = bitset<8>(WriteData.to_string().substr(0,8));
            DMem[Address.to_ulong()+1] = bitset<8>(WriteData.to_string().substr(8,8));
            DMem[Address.to_ulong()+2] = bitset<8>(WriteData.to_string().substr(16,8));
            DMem[Address.to_ulong()+3] = bitset<8>(WriteData.to_string().substr(24,8));  
        }   
                     
        void outputDataMem()
        {
            ofstream dmemout;
            dmemout.open("dmemresult.txt");
            if (dmemout.is_open())
            {
                for (int j = 0; j< 1000; j++)
                {     
                    dmemout << DMem[j]<<endl;
                }
                     
            }
            else cout<<"Unable to open file";
            dmemout.close();               
        }             
      
    private:
		vector<bitset<8> > DMem;      
};  

void printState(stateStruct state, int cycle)
{
    ofstream printstate;
    printstate.open("stateresult.txt", std::ios_base::app);
    if (printstate.is_open())
    {
        printstate<<"State after executing cycle:\t"<<cycle<<endl; 
        
        printstate<<"IF.PC:\t"<<state.IF.PC.to_ulong()<<endl;        
        printstate<<"IF.nop:\t"<<state.IF.nop<<endl; 
        
        printstate<<"ID.Instr:\t"<<state.ID.Instr<<endl; 
        printstate<<"ID.nop:\t"<<state.ID.nop<<endl;
        
        printstate<<"EX.Read_data1:\t"<<state.EX.Read_data1<<endl;
        printstate<<"EX.Read_data2:\t"<<state.EX.Read_data2<<endl;
        printstate<<"EX.Imm:\t"<<state.EX.Imm<<endl; 
        printstate<<"EX.Rs:\t"<<state.EX.Rs<<endl;
        printstate<<"EX.Rt:\t"<<state.EX.Rt<<endl;
        printstate<<"EX.Wrt_reg_addr:\t"<<state.EX.Wrt_reg_addr<<endl;
        printstate<<"EX.is_I_type:\t"<<state.EX.is_I_type<<endl; 
        printstate<<"EX.rd_mem:\t"<<state.EX.rd_mem<<endl;
        printstate<<"EX.wrt_mem:\t"<<state.EX.wrt_mem<<endl;        
        printstate<<"EX.alu_op:\t"<<state.EX.alu_op<<endl;
        printstate<<"EX.wrt_enable:\t"<<state.EX.wrt_enable<<endl;
        printstate<<"EX.nop:\t"<<state.EX.nop<<endl;        

        printstate<<"MEM.ALUresult:\t"<<state.MEM.ALUresult<<endl;
        printstate<<"MEM.Store_data:\t"<<state.MEM.Store_data<<endl; 
        printstate<<"MEM.Rs:\t"<<state.MEM.Rs<<endl;
        printstate<<"MEM.Rt:\t"<<state.MEM.Rt<<endl;   
        printstate<<"MEM.Wrt_reg_addr:\t"<<state.MEM.Wrt_reg_addr<<endl;              
        printstate<<"MEM.rd_mem:\t"<<state.MEM.rd_mem<<endl;
        printstate<<"MEM.wrt_mem:\t"<<state.MEM.wrt_mem<<endl; 
        printstate<<"MEM.wrt_enable:\t"<<state.MEM.wrt_enable<<endl;         
        printstate<<"MEM.nop:\t"<<state.MEM.nop<<endl;        

        printstate<<"WB.Wrt_data:\t"<<state.WB.Wrt_data<<endl;
        printstate<<"WB.Rs:\t"<<state.WB.Rs<<endl;
        printstate<<"WB.Rt:\t"<<state.WB.Rt<<endl;        
        printstate<<"WB.Wrt_reg_addr:\t"<<state.WB.Wrt_reg_addr<<endl;
        printstate<<"WB.wrt_enable:\t"<<state.WB.wrt_enable<<endl;        
        printstate<<"WB.nop:\t"<<state.WB.nop<<endl; 
    }
    else cout<<"Unable to open file";
    printstate.close();
}

bitset<32> signextend (bitset<16> imm) {
    string sestring;
    if (imm[15] == 0) {
        sestring = "0000000000000000" + imm.to_string<char, std::string::traits_type, std::string::allocator_type>();
    } else {
        sestring = "1111111111111111" + imm.to_string<char, std::string::traits_type, std::string::allocator_type>();
    }
    return (bitset<32> (sestring));
}

unsigned long shiftbits(bitset<32> inst, int start) {
    unsigned long ulonginst;
    return ((inst.to_ulong())>>start);
}



int main()
{
    // init
    RF myRF;
    INSMem myInsMem;
    DataMem myDataMem;
    stateStruct state, newState;
	int cycle = 0;
	bool stall = 0;
	bool branch = 0;
	bitset<32> branch_address = bitset<32> (0);

	state.IF.nop = 0;
    state.IF.PC = bitset<32> (0);

    state.ID.Instr = bitset<32> (0);
    state.ID.nop = 1;

    state.EX.Read_data1 = bitset<32> (0);
    state.EX.Read_data2 = bitset<32> (0);
    state.EX.Imm = bitset<16> (0);
    state.EX.Rs = bitset<5> (0);
    state.EX.Rt = bitset<5> (0);
    state.EX.Wrt_reg_addr = bitset<5> (0);
    state.EX.alu_op = 1;
    state.EX.is_I_type = 0;
    state.EX.wrt_enable = 0;
    state.EX.rd_mem = 0;
    state.EX.wrt_mem = 0;
    state.EX.nop = 1;

    state.MEM.ALUresult = bitset<32> (0);
    state.MEM.Store_data = bitset<32> (0);
    state.MEM.Rs = bitset<5> (0);
    state.MEM.Rt = bitset<5> (0);
    state.MEM.Wrt_reg_addr = bitset<5> (0);
    state.MEM.wrt_enable = 0;
    state.MEM.rd_mem = 0;
    state.MEM.wrt_mem = 0;
    state.MEM.nop = 1;

    state.WB.Wrt_data = bitset <32> (0);
    state.WB.Rs = bitset <5> (0);
    state.WB.Rt = bitset <5> (0);
    state.WB.Wrt_reg_addr = bitset <5> (0);
    state.WB.wrt_enable = 0;
    state.WB.nop = 1;

    while (1) {

    	// reset braching bool
		branch = 0;

        /* --------------------- WB stage --------------------- */
	
		if (state.WB.nop == 0) {
			if (state.WB.wrt_enable == 1) {
				myRF.writeRF(state.WB.Wrt_reg_addr, state.WB.Wrt_data);
			}
		}

        /* --------------------- MEM stage --------------------- */
		
		if (state.MEM.nop == 0) {

			// inherite unchang parameters
			newState.WB.Rs = state.MEM.Rs;
			newState.WB.Rt = state.MEM.Rt;
			newState.WB.Wrt_reg_addr = state.MEM.Wrt_reg_addr;
			newState.WB.wrt_enable = state.MEM.wrt_enable;

			// read from memory with lw 
			if (state.MEM.rd_mem) {
				newState.WB.Wrt_data = myDataMem.readDataMem(state.MEM.ALUresult);
			}
			else {
				newState.WB.Wrt_data = state.MEM.ALUresult;
			}

			// write into memory with sw 
			if (state.MEM.wrt_mem) {
				if (state.WB.Wrt_reg_addr == state.MEM.Rt) {
					state.MEM.Store_data = state.WB.Wrt_data;
				}
				myDataMem.writeDataMem(state.MEM.ALUresult, state.MEM.Store_data);
			}
		}

		newState.WB.nop = state.MEM.nop;

        /* --------------------- EX stage --------------------- */
		
		if (state.EX.nop == 0) {

			// inherite unchang parameters
			newState.MEM.Rs = state.EX.Rs;
			newState.MEM.Rt = state.EX.Rt;
			newState.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr;
			newState.MEM.wrt_enable = state.EX.wrt_enable;
			newState.MEM.wrt_mem = state.EX.wrt_mem;
			newState.MEM.rd_mem = state.EX.rd_mem;

			//dependencies & forwardings
			if (state.MEM.wrt_enable && state.MEM.Wrt_reg_addr == state.EX.Rs) {
			    state.EX.Read_data1 = state.MEM.ALUresult;
			}

			if (state.WB.wrt_enable && state.WB.Wrt_reg_addr == state.EX.Rs) {
			    state.EX.Read_data1 = state.WB.Wrt_data;
			}

			if (state.MEM.wrt_enable && state.MEM.Wrt_reg_addr == state.EX.Rt) {
			    state.EX.Read_data2 = state.MEM.ALUresult;
			}

			if (state.WB.wrt_enable && state.WB.Wrt_reg_addr == state.EX.Rt) {
			    state.EX.Read_data2 = state.WB.Wrt_data;
			}

			// ALU ops
			int op1;
			int op2;
			op1 = state.EX.Read_data1.to_ulong();
			if (state.EX.is_I_type) {
				op2 = signextend(state.EX.Imm).to_ulong();
			}
			else {
				op2 = state.EX.Read_data2.to_ulong(); 
			}
			if (state.EX.alu_op == 1)
				newState.MEM.ALUresult = bitset<32>(op1 + op2);
			else {
				newState.MEM.ALUresult = bitset<32>(op1 - op2);
			}
			
			// sw op
			newState.MEM.Store_data = state.EX.Read_data2;
		}

		newState.MEM.nop = state.EX.nop;

        /* --------------------- ID stage --------------------- */
		
		if (state.ID.nop == 0) {

			newState.EX.Rs = bitset<5> (shiftbits(state.ID.Instr, 21)); // Rs
            newState.EX.Rt = bitset<5> (shiftbits(state.ID.Instr, 16)); // Rt
          	newState.EX.Imm = bitset<16> (shiftbits(state.ID.Instr, 0)); // Immdiate
         	newState.EX.Read_data1 = myRF.readRF(newState.EX.Rs); // read Rs in RF
         	newState.EX.Read_data2 = myRF.readRF(newState.EX.Rt); // read Rt in RF

         	bitset<6> opcode = bitset<6> (shiftbits(state.ID.Instr, 26));

			if (opcode == bitset<6> (0)) {
				newState.EX.Wrt_reg_addr = bitset<5> (state.ID.Instr.to_string().substr(16, 5)); // Rt
				newState.EX.is_I_type = 0; // R
			}

			else {
				newState.EX.Wrt_reg_addr = bitset<5> (state.ID.Instr.to_string().substr(11, 5)); // Rd
				newState.EX.is_I_type = 1; // I
			}

			// alu_op
			if (opcode == bitset<6> (0) && state.ID.Instr.to_string().substr(26, 6) == "100011") {
				newState.EX.alu_op = 0; // sub
			}
			else {
				newState.EX.alu_op = 1; // add
 			}

			//wrt-enable
			if (opcode == bitset<6> (0) || state.ID.Instr.to_string().substr(0, 6) == "100011") {
				newState.EX.wrt_enable = 1; // R + load
			}
			else { 
				newState.EX.wrt_enable = 0; // sw + beq
			}

			//rd_mem, wrt_mem
			if (state.ID.Instr.to_string().substr(0, 6) == "100011") { // lw: 0x23 == '100011'
				newState.EX.rd_mem = 1; newState.EX.wrt_mem = 0; 
				}
			else if (state.ID.Instr.to_string().substr(0, 6) == "101011") { // sw: 0x2B = '101011'
				newState.EX.wrt_mem = 1; newState.EX.rd_mem = 0; 
			}
			else {
				newState.EX.rd_mem = 0; newState.EX.wrt_mem = 0; // R: no mem access
			}

			// stall detection
			if (newState.MEM.wrt_enable) {
				if (state.EX.Wrt_reg_addr == newState.EX.Rs && state.EX.rd_mem == 1) {
					stall = 1;
				}
				if (state.EX.Wrt_reg_addr == newState.EX.Rt && state.EX.rd_mem == 1 && newState.EX.is_I_type == 0) {
					stall = 1;
				}
			}

			//branching
			if (state.ID.Instr.to_string().substr(0, 6) == "000100") {
				if (newState.EX.Read_data1 != newState.EX.Read_data2) {
					branch_address= bitset<32> ((state.IF.PC.to_ulong()+(signextend(newState.EX.Imm)<<2).to_ulong()));
					branch = 1;
				}
			}
		}

		newState.EX.nop = state.ID.nop;

        /* --------------------- IF stage --------------------- */

		if (stall == 1) {
			newState.IF = state.IF;
			newState.ID = state.ID;
			newState.EX.nop = 1;
		}
		else {
			if (state.IF.nop == 0) {
				newState.ID.Instr = myInsMem.readInstr(state.IF.PC);

				if (branch == 0){ 
					newState.IF.PC = bitset<32> (state.IF.PC.to_ulong() + 4); 
				}
				else { 
					newState.IF.PC = bitset<32> (branch_address); 
				}
				if (newState.ID.Instr.to_string() == "11111111111111111111111111111111") {
					state.IF.nop = 1;
				}
			}
			newState.IF.nop = state.IF.nop;
			newState.ID.nop = state.IF.nop;
		}

		//delay IF with braching
		if (branch == 1) {
			newState.ID.nop = 1;
			newState.EX.nop = 1;
		}

		// reset stall at clean instruction
		stall = 0;
		
		// termination check
        if (state.IF.nop && state.ID.nop && state.EX.nop && state.MEM.nop && state.WB.nop) 
            break;
        
        printState(newState, cycle); //print states after executing cycle 0, cycle 1, cycle 2 ... 
       
        state = newState; /*The end of the cycle and updates the current state with the values calculated in this cycle */ 
		cycle++;
    }
    
    myRF.outputRF(); // dump RF;	
	myDataMem.outputDataMem(); // dump data mem 

	cout << "testing complete." << endl;
	
	return 0;
}