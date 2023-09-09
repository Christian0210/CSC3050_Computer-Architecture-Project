`timescale 1ns / 1ps
module CPU (
    input wire clock,
    input wire start,
    input [319:0] instr
    );

    reg [5:0] op;
    reg [5:0] func;
    reg [25:0] sec1;
    reg [4:0] sec2;
    reg [4:0] sec3;
    reg [15:0] sec4;
    reg [31:0] sec5;
    reg [4:0] sec6;

    reg[31:0] reg_A;
    reg[31:0] reg_B;
    reg[31:0] reg_C;

    reg [7:0] mem[1023:0];
    reg [7:0] dat[1023:0];
    reg [32:0] pc=32'h0000_0000;
    reg [31:0] gr[31:0];
    reg [31:0] ins;
    reg of1;
    reg of2;
    reg of3;

    reg r;
    reg rw;
    reg ga;
    reg sr;
    reg mr;
    reg ts;
    reg mw;
    reg mt;
    reg ctr=1'b1;

    reg [63:0] id;
    reg [129:0] ex;
    reg [53:0] exm;
    reg [78:0] wbm; 
    reg[31:0] in;
    reg[31:0] adm;
    reg[31:0] acc;

always @(start)
    begin
        gr[0]=32'h0000_0000;
        mem[0]=instr[319:312];
        mem[1]=instr[311:304];
        mem[2]=instr[303:296];
        mem[3]=instr[295:288];
        mem[4]=instr[287:280];
        mem[5]=instr[279:272];
        mem[6]=instr[271:264];
        mem[7]=instr[263:256];
        mem[8]=instr[255:248];
        mem[9]=instr[247:240];
        mem[10]=instr[239:232];
        mem[11]=instr[231:224];
        mem[12]=instr[223:216];
        mem[13]=instr[215:208];
        mem[14]=instr[207:200];
        mem[15]=instr[199:192];
        mem[16]=instr[191:184];
        mem[17]=instr[183:176];
        mem[18]=instr[175:168];
        mem[19]=instr[167:160];
        mem[20]=instr[159:152];
        mem[21]=instr[151:144];
        mem[22]=instr[143:136];
        mem[23]=instr[135:128];
        mem[24]=instr[127:120];
        mem[25]=instr[119:112];
        mem[26]=instr[111:104];
        mem[27]=instr[103:96];
        mem[28]=instr[95:88];
        mem[29]=instr[87:80];
        mem[30]=instr[79:72];
        mem[31]=instr[71:64];
        mem[32]=instr[63:56];
        mem[33]=instr[55:48];
        mem[34]=instr[47:40];
        mem[35]=instr[39:32];
        mem[36]=instr[31:24];
        mem[37]=instr[23:16];
        mem[38]=instr[15:8];
        mem[39]=instr[7:0];
    end

always @(posedge clock)
	begin
        if (ctr!=1'b0)
        begin
            ins={mem[pc],mem[pc+1],mem[pc+2],mem[pc+3]};
            op = ins[31:26];
            func = ins[5:0];
            sec1 = ins[25:0];
            sec2 = ins[25:21];
            sec3 = ins[20:16];
            sec4 = ins[15:0];
           
            case(op)
                6'b000010:
                begin
                    pc<=pc+sec1*4+4;
                end
                6'b000011:
                begin
                    gr[31]<=pc+4;
                    pc<=pc+sec1*4+4;
                end
                6'b000000:
                begin
                    if (func==6'b001000)
                        pc<=pc+gr[sec2]*4+4;
                    else
                        pc<=pc+4;
                end
                6'b000100:
                begin
                    if (sec2==sec3)
                        pc<=pc+sec4*4+4;
                    else
                        pc<=pc+4;
                end
                6'b000101:
                begin
                    if (sec2!=sec3)
                        pc<=pc+sec4*4+4;
                    else
                        pc<=pc+4;
                end
                default:
                begin
                    pc<=pc+4;
                end
            endcase

        end
        else
        begin
            ins={mem[pc-8],mem[pc-7],mem[pc-6],mem[pc-5]};
            pc<=pc-4;
        end
        id[31:0]<=ins;
        id[63:32]<=pc;
    end


always @(posedge clock)
	begin
    op=id[31:26];
    func=id[5:0];
    sec2=id[25:21];
    sec3=id[20:16];
    sec4=id[15:0];
    sec6=id[15:11];
    ex[63:32]<=gr[id[20:16]];
    ex[95:64]<=gr[sec2];
    case(op)
        6'b001000:
            ex[31:0]<={{16{id[15]}},sec4};
        6'b001001:
            ex[31:0]<={{16{id[15]}},sec4};
        6'b100011:
            ex[31:0]<={{16{id[15]}},sec4};
        6'b101011:
            ex[31:0]<={{16{id[15]}},sec4};
        default:
            ex[31:0]<={16'b0000_0000_0000_0000,id[15:0]};
    endcase
    if (mr==1'b1 && (ex[124:120]==sec2 || ex[124:120]==sec3))
    begin
        r=1'b0;
        rw=1'b0;
        ga=1'b0;
        sr=1'b0;
        mr=1'b0;
        mw=1'b0;
        mt=1'b0;
        ctr<=1'b0;
    end
    else
    begin
        if (op==6'b000000 || op==6'b001000 || op==6'b001001 || op==6'b001100 || op==6'b001101 || op==6'b100011)
        begin
            if (op==6'b000000 && func==6'b001000)
                rw=1'b0;
            else
                rw=1'b1;
        end
        else
            rw=1'b0;

        if (op==6'b000000 || op==6'b001000 || op==6'b001001 || op==6'b001100 || op==6'b001101 || op==6'b100011 || op==6'b101011)
        begin
            if (op==6'b000000 && func==6'b001000)
                ga=1'b0;
            else
                ga=1'b1;
        end
        else
            ga=1'b0;
        
        if (op==6'b000000)
            r=1'b1;
        if (op==6'b001000)
            r=1'b0;
        if (op==6'b001001)
            r=1'b0;
        if (op==6'b001100)
            r=1'b0;
        if (op==6'b001101)
            r=1'b0;
        if (op==6'b100011)
            r=1'b0;

        mr=1'b0;
        mw=1'b0;
        mt=1'b0;
        if (op==6'b100011)
            mr=1'b1;
        if (op==6'b101011)
            mw=1'b1;
        if (op==6'b100011)
            mt=1'b1;
        
        if (op==6'b000000)
            sr=1'b0;
        if (op==6'b001000)
            sr=1'b1;
        if (op==6'b001001)
            sr=1'b1;
        if (op==6'b001100)
            sr=1'b1;
        if (op==6'b001101)
            sr=1'b1;
        if (op==6'b100011)
            sr=1'b1;
        if (op==6'b101011)
            sr=1'b1;

        ctr<=1'b1;
    end
    ex[114]<=r;
    ex[113]<=rw;
    ex[112]<=ga;
    ex[111]<=sr;
    ex[110]<=mr;
    ex[109]<=mw;
    ex[108]<=mt;
    ex[107:102]<=op;
    ex[101:96]<=func;
    ex[129:125]<=sec2;
    ex[124:120]<=sec3;
    ex[119:115]<=sec6;
    end

always @(posedge clock)
    begin
    of1=1'b0;
    if (ex[112]==1'b1)
    begin
        reg_A=gr[ex[129:125]];
        if (ex[111]==1'b1)
            reg_B=ex[31:0];
        else
            reg_B=gr[ex[124:120]];
        if (exm[52]==1'b0)
        begin
            if (exm[41:37]==ex[129:125])
                reg_A=exm[31:0];
            if (exm[41:37]==ex[124:120] && ex[111]==1'b0)
                reg_B=exm[31:0];
        end
        else
        begin
            if (exm[46:42]==ex[129:125])
                reg_A=exm[31:0];
            if (exm[46:42]==ex[124:120] && ex[111]==1'b0)
                reg_B=exm[31:0];
        end
        if (wbm[77]==1'b0)
        begin
            if (wbm[71:67]==ex[129:125])
                reg_A=wbm[31:0];
            if (wbm[71:67]==ex[124:120] && ex[111]==1'b0)
                reg_B=wbm[31:0];
        end       
        else
        begin
            if (wbm[64]==1'b1)
            begin
                if (wbm[76:72]==ex[129:125])
                    reg_A=wbm[63:32];
                if (wbm[76:72]==ex[124:120] && ex[111]==1'b0)
                    reg_B=wbm[63:32];
            end
            else
            begin
                if (wbm[76:72]==ex[129:125])
                    reg_A=wbm[31:0];
                if (wbm[76:72]==ex[124:120] && ex[111]==1'b0)
                    reg_B=wbm[31:0];
            end
        end
    end

    op=ex[107:102];
    func=ex[101:96];
    case(op)
        6'b100011:
            reg_C=reg_A+reg_B;
        6'b101011:
            reg_C=reg_A+reg_B;
        6'b001000:
        begin
            reg_C=$signed(reg_A)+$signed(reg_B);
            if ((reg_A[31]==reg_B[31]) && (reg_A[31]!=reg_C[31]))
                of1=1'b1;
            else of1=1'b0;
        end
        6'b001001:
            reg_C=$unsigned(reg_A)+$unsigned(reg_B);
        6'b001100:
            reg_C=reg_A & reg_B;
        6'b001101:
            reg_C=reg_A | reg_B;
        6'b000000:
        begin
            case(func)
                6'b100000:
                begin
                    reg_C=$signed(reg_A)+$signed(reg_B);
                    if ((reg_A[31]==reg_B[31]) && (reg_A[31]!=reg_C[31]))
                        of1=1'b1;
                    else 
                        of1=1'b0;
                end
                6'b100001:
                    reg_C=$unsigned(reg_A)+$unsigned(reg_B);
                6'b100010:
                begin
                    reg_C=$signed(reg_A)-$signed(reg_B);
                    if ((reg_A[31]==reg_B[31]) && (reg_A[31]!=reg_C[31]))
                        of1=1'b1;
                    else 
                        of1=1'b0;
                end
                6'b100011:
                    reg_C=$unsigned(reg_A)-$unsigned(reg_B);
                6'b100100:
                    reg_C=reg_A & reg_B;
                6'b100101:
                    reg_C=reg_A | reg_B;
                6'b100110:
                    reg_C=reg_A ^ reg_B;
                6'b100111:
                    reg_C=~(reg_A | reg_B);
                6'b101010:
                begin
                    if ($signed(reg_A)>=$signed(reg_B))
                        reg_C=32'b0;
                    else 
                        reg_C=32'b1;
                end
            endcase
        end
    endcase
    exm[31:0]<=reg_C;
    exm[36]<=ex[114];
    exm[35]<=ex[113];
    exm[34]<=ex[110];
    exm[33]<=ex[109];
    exm[32]<=ex[108];
    exm[53]<=ex[112];
    exm[52]<=ex[111];
    exm[51:47]<=ex[129:125];
    exm[46:42]<=ex[124:120];
    exm[41:37]<=ex[119:115];
    end

always @(posedge clock)
    begin
    wbm[31:0]<=exm[31:0];
    wbm[66]<=exm[36];
    wbm[65]<=exm[35];
    wbm[64]<=exm[32];
    wbm[78]<=exm[53];
    wbm[77]<=exm[52];
    wbm[76:72]<=exm[46:42];
    wbm[71:67]<=exm[41:37];
    if (exm[34]==1'b1)
    begin
        adm=exm[31:0];
        in={dat[adm],dat[adm+1],dat[adm+2],dat[adm+3]};
    end
    if (exm[33]==1'b1)
    begin
        adm=exm[31:0];
        dat[adm]<=gr[exm[46:42]][31:24];
        dat[adm+1]<=gr[exm[46:42]][23:16];
        dat[adm+2]<=gr[exm[46:42]][15:8];
        dat[adm+3]<=gr[exm[46:42]][7:0];
    end
    wbm[63:32]<=in;
    end

always @(posedge clock)
    begin
        if (wbm[65]==1'b1)
        begin
            if (wbm[64]==1'b1)
            begin
                if (wbm[66]==1'b1)
                    gr[wbm[71:67]]<=wbm[63:32];
                else
                    gr[wbm[76:72]]<=wbm[63:32];
            end
            else
            begin
                if (wbm[66]==1'b1)
                    gr[wbm[71:67]]<=wbm[31:0];
                else 
                    gr[wbm[76:72]]<=wbm[31:0];
            end
        end
    end
endmodule