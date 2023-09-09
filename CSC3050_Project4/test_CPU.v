`timescale 1ns/1ps

// general register
`define gr0  	5'b00000
`define gr1  	5'b00001
`define gr2  	5'b00010
`define gr3 	5'b00011
`define gr4  	5'b00100
`define gr5  	5'b00101
`define gr6  	5'b00110
`define gr7  	5'b00111

module CPU_test;
reg clock;
reg start;
reg [319:0] instr;

CPU test1(.clock(clock),.start(start),.instr(instr));

initial begin
    clock=0;
    start=0;
    instr[319:288]={6'b001000,`gr0,`gr1,16'b11};
    instr[287:256]={6'b000000,`gr1,`gr1,`gr2,5'b0,6'b100000};
    instr[255:224]={6'b001000,`gr2,`gr3,16'b1};
    instr[223:192]={6'b000000,`gr2,`gr3,`gr4,5'b0,6'b100000};
    instr[191:160]={6'b000010,26'b1};
    instr[159:128]={6'b001000,`gr1,`gr5,16'b1};
    instr[127:96]={6'b001000,`gr1,`gr5,16'b11};
    instr[95:64]={6'b000000,`gr2,`gr3,`gr6,5'b0,6'b101010};
    instr[63:32]={6'b000000,`gr4,`gr6,`gr7,5'b0,6'b100011};

    #period $display("instruction=%b | gr_0=%h gr_1=%h gr_2=%h gr_3=%h gr_4=%h gr_5=%h gr_6=%h gr_7=%h pc=%h\n",test1.ins,test1.gr[0],test1.gr[1],test1.gr[2],test1.gr[3],test1.gr[4],test1.gr[5],test1.gr[6],test1.gr[7],test1.pc);
    #period $display("instruction=%b | gr_0=%h gr_1=%h gr_2=%h gr_3=%h gr_4=%h gr_5=%h gr_6=%h gr_7=%h pc=%h\n",test1.ins,test1.gr[0],test1.gr[1],test1.gr[2],test1.gr[3],test1.gr[4],test1.gr[5],test1.gr[6],test1.gr[7],test1.pc);
    #period $display("instruction=%b | gr_0=%h gr_1=%h gr_2=%h gr_3=%h gr_4=%h gr_5=%h gr_6=%h gr_7=%h pc=%h\n",test1.ins,test1.gr[0],test1.gr[1],test1.gr[2],test1.gr[3],test1.gr[4],test1.gr[5],test1.gr[6],test1.gr[7],test1.pc);
    #period $display("instruction=%b | gr_0=%h gr_1=%h gr_2=%h gr_3=%h gr_4=%h gr_5=%h gr_6=%h gr_7=%h pc=%h\n",test1.ins,test1.gr[0],test1.gr[1],test1.gr[2],test1.gr[3],test1.gr[4],test1.gr[5],test1.gr[6],test1.gr[7],test1.pc);
    #period $display("instruction=%b | gr_0=%h gr_1=%h gr_2=%h gr_3=%h gr_4=%h gr_5=%h gr_6=%h gr_7=%h pc=%h\n",test1.ins,test1.gr[0],test1.gr[1],test1.gr[2],test1.gr[3],test1.gr[4],test1.gr[5],test1.gr[6],test1.gr[7],test1.pc);
    #period $display("instruction=%b | gr_0=%h gr_1=%h gr_2=%h gr_3=%h gr_4=%h gr_5=%h gr_6=%h gr_7=%h pc=%h\n",test1.ins,test1.gr[0],test1.gr[1],test1.gr[2],test1.gr[3],test1.gr[4],test1.gr[5],test1.gr[6],test1.gr[7],test1.pc);
    #period $display("instruction=%b | gr_0=%h gr_1=%h gr_2=%h gr_3=%h gr_4=%h gr_5=%h gr_6=%h gr_7=%h pc=%h\n",test1.ins,test1.gr[0],test1.gr[1],test1.gr[2],test1.gr[3],test1.gr[4],test1.gr[5],test1.gr[6],test1.gr[7],test1.pc);
    #period $display("instruction=%b | gr_0=%h gr_1=%h gr_2=%h gr_3=%h gr_4=%h gr_5=%h gr_6=%h gr_7=%h pc=%h\n",test1.ins,test1.gr[0],test1.gr[1],test1.gr[2],test1.gr[3],test1.gr[4],test1.gr[5],test1.gr[6],test1.gr[7],test1.pc);
    #period $display("instruction=%b | gr_0=%h gr_1=%h gr_2=%h gr_3=%h gr_4=%h gr_5=%h gr_6=%h gr_7=%h pc=%h\n",test1.ins,test1.gr[0],test1.gr[1],test1.gr[2],test1.gr[3],test1.gr[4],test1.gr[5],test1.gr[6],test1.gr[7],test1.pc);
    #period $display("instruction=%b | gr_0=%h gr_1=%h gr_2=%h gr_3=%h gr_4=%h gr_5=%h gr_6=%h gr_7=%h pc=%h\n",test1.ins,test1.gr[0],test1.gr[1],test1.gr[2],test1.gr[3],test1.gr[4],test1.gr[5],test1.gr[6],test1.gr[7],test1.pc);
    #period $display("instruction=%b | gr_0=%h gr_1=%h gr_2=%h gr_3=%h gr_4=%h gr_5=%h gr_6=%h gr_7=%h pc=%h\n",test1.ins,test1.gr[0],test1.gr[1],test1.gr[2],test1.gr[3],test1.gr[4],test1.gr[5],test1.gr[6],test1.gr[7],test1.pc);
    #period $display("instruction=%b | gr_0=%h gr_1=%h gr_2=%h gr_3=%h gr_4=%h gr_5=%h gr_6=%h gr_7=%h pc=%h\n",test1.ins,test1.gr[0],test1.gr[1],test1.gr[2],test1.gr[3],test1.gr[4],test1.gr[5],test1.gr[6],test1.gr[7],test1.pc);
    #period $display("instruction=%b | gr_0=%h gr_1=%h gr_2=%h gr_3=%h gr_4=%h gr_5=%h gr_6=%h gr_7=%h pc=%h\n",test1.ins,test1.gr[0],test1.gr[1],test1.gr[2],test1.gr[3],test1.gr[4],test1.gr[5],test1.gr[6],test1.gr[7],test1.pc);
    #period $display("instruction=%b | gr_0=%h gr_1=%h gr_2=%h gr_3=%h gr_4=%h gr_5=%h gr_6=%h gr_7=%h pc=%h\n",test1.ins,test1.gr[0],test1.gr[1],test1.gr[2],test1.gr[3],test1.gr[4],test1.gr[5],test1.gr[6],test1.gr[7],test1.pc);
    #period $finish;
end

parameter period=10;
always #5clock=~clock;

endmodule