module alu(i_datain,gr1,gr2,c,imm,zero,neg,overflow);

output signed[31:0] c;
output zero;
output overflow;
output neg;

input signed[31:0] i_datain,gr1,gr2,imm;

reg[5:0] opcode, func;

reg[31:0] reg_C;
reg zf = 0;
reg nf = 0;
reg of = 0;
reg[31:0] reg_A, reg_B;
reg[31:0] Imm;
reg[63:0] ex;

always @(i_datain,gr1,gr2)
begin

opcode = i_datain[31:26];
func = i_datain[5:0];

case(opcode)
6'b001000:   // addu: Addition immediate (with overflow)
    begin
        reg_A = gr1;
        reg_B = {16'b0000_0000_0000_0000, i_datain[15:0]};
        reg_C = $signed(reg_A) + $signed(reg_B[15:0]);
        if (~(reg_A[31] ^ reg_B[15]) && (reg_C[31] ^ reg_A[31])) 
            of = 1;
        else
            of = 0;
        if (reg_C == 0) 
            zf = 1;
        else
            zf = 0;
        if (reg_C[31] == 1)
            nf = 1;
        else
            nf = 0;
    end
6'b001001:   // addiu: Addition immediate (without overflow)
    begin
        reg_A = gr1;
        reg_B = {16'b0000_0000_0000_0000, i_datain[15:0]};
        reg_C = $unsigned(reg_A) + $unsigned(reg_B[15:0]);
        of = 0;
        if (reg_C == 0) 
            zf = 1;
        else
            zf = 0;
        if (reg_C[31] == 1)
            nf = 1;
        else
            nf = 0;
    end
6'b001100:   // andi: AND immediate
    begin
        reg_A = gr1;
        reg_B = {16'b0000_0000_0000_0000, i_datain[15:0]};
        reg_C = reg_A & reg_B;
        of = 0;
        zf = 0;
        nf = 0;
    end
6'b001101:   // ori: OR immediate
    begin
        reg_A = gr1;
        reg_B = {16'b0000_0000_0000_0000, i_datain[15:0]};
        reg_C = reg_A | reg_B;
        of = 0;
        zf = 0;
        nf = 0;
    end
6'b001110:   // xori: XOR immediate
    begin
        reg_A = gr1;
        reg_B = {16'b0000_0000_0000_0000, i_datain[15:0]};
        reg_C = reg_A ^ reg_B;
        of = 0;
        zf = 0;
        nf = 0;
    end
6'b000100:   // beq: Branch on equal
    begin
        reg_A = gr1;
        reg_B = gr2;
        if (reg_A == reg_B)
            reg_C = {16'b0000_0000_0000_0000, i_datain[15:0]};
        else
            reg_C = 32'b0;
        of = 0;
        zf = 0;
        nf = 0;
    end
6'b000101:   // bne: Branch on not equal
    begin
        reg_A = gr1;
        reg_B = gr2;
        if (reg_A != reg_B)
            reg_C = {16'b0000_0000_0000_0000, i_datain[15:0]};
        else
            reg_C = 32'b0;
        of = 0;
        zf = 0;
        nf = 0;
    end
6'b100011:   // lw: Load word
    begin
        reg_A = gr1;
        reg_B = gr2;
        reg_C = {16'b0000_0000_0000_0000, i_datain[15:0]};
    end
6'b101011:   // sw: Store word
    begin
        reg_A = gr1;
        reg_B = gr2;
        reg_C = {16'b0000_0000_0000_0000, i_datain[15:0]};
    end
6'b001010:   // slti: Set less than immediate
    begin
        reg_A = gr1;
        reg_B = {16'b0000_0000_0000_0000, i_datain[15:0]};
        if (reg_A[31] && reg_B[15])
            reg_C = (reg_A[30:0] >= reg_B[14:0]);
        else if (~(reg_A[31] || reg_B[15]))
            reg_C = (reg_A[30:0] < reg_B[14:0]);
        else if (reg_A[31])
            reg_C = 32'b1;
        else
            reg_C = 32'b0;
        of = 0;
        zf = 0;
        nf = 0;
    end
6'b001011:   // sltiu: Set less than unsigned immediate
    begin
        reg_A = gr1;
        reg_B = {16'b0000_0000_0000_0000, i_datain[15:0]};
        reg_C = $unsigned(reg_A) < $unsigned(reg_B);
        of = 0;
        zf = 0;
        nf = 0;
    end
6'b000000:
    begin
        case(func)
        6'b000000:   // sll: Shift left logical
            begin
                reg_A = gr2;
                reg_B = i_datain[10:6];
                reg_C = reg_A << reg_B;
                of = 0;
                zf = 0;
                nf = 0;
            end
        6'b000100:   // sllv: Shift left logical variable
            begin
                reg_A = gr2;
                reg_B = gr1;
                reg_C = reg_A << reg_B;
                of = 0;
                zf = 0;
                nf = 0;
            end
        6'b000011:   // sra: Shift right arithmetic
            begin
                reg_A = gr2;
                reg_B = i_datain[10:6];
                reg_C = reg_A >> reg_B;
                reg_C = {reg_A[31:31],reg_C[30:0]};
                of = 0;
                zf = 0;
                nf = 0;
            end
        6'b000111:   // srav: Shift right arithmetic variable
            begin
                reg_A = gr2;
                reg_B = gr1;
                reg_C = reg_A >> reg_B;
                reg_C = {reg_A[31:31],reg_C[30:0]};
                of = 0;
                zf = 0;
                nf = 0;
            end
        6'b000010:   // srl: Shift right logical
            begin
                reg_A = gr2;
                reg_B = i_datain[10:6];
                reg_C = reg_A >> reg_B;
                of = 0;
                zf = 0;
                nf = 0;
            end
        6'b000110:   // srlv: Shift right logical variable
            begin
                reg_A = gr2;
                reg_B = gr1;
                reg_C = reg_A >> reg_B;
                of = 0;
                zf = 0;
                nf = 0;
            end
        6'b100000:   // add: Addition (with overflow)
            begin
                reg_A = gr1;
                reg_B = gr2;
                reg_C = $signed(reg_A) + $signed(reg_B);
                if ((~(reg_A[31] ^ reg_B[31])) && (reg_C[31] ^ reg_A[31])) 
                    of = 1;
                else
                    of = 0;
                if (reg_C == 0) 
                    zf = 1;
                else
                    zf = 0;
                if (reg_C[31] == 1)
                    nf = 1;
                else
                    nf = 0;
            end
        6'b100001:   // addu: Addition (without overflow)
            begin
                reg_A = gr1;
                reg_B = gr2;
                reg_C = $unsigned(reg_A) + $unsigned(reg_B);
                of = 0;
                if (reg_C == 0) 
                    zf = 1;
                else
                    zf = 0;
                if (reg_C[31] == 1)
                    nf = 1;
                else
                    nf = 0;
            end
        6'b100010:   // sub: Subtract (with overflow)
            begin
                reg_A = gr1;
                reg_B = gr2;
                reg_C = $signed(reg_A) - $signed(reg_B);
                if ((reg_A[31] ^ reg_B[31]) && (reg_A[31] ^ reg_C[31]))
                    of = 1;
                else
                    of = 0;
                if (reg_C == 0) 
                    zf = 1;
                else
                    zf = 0;
                if (reg_C[31] == 1)
                    nf = 1;
                else
                    nf = 0;
            end
        6'b100011:   // subu: Subtract (without overflow)
            begin
                reg_A = gr1;
                reg_B = gr2;
                reg_C = $unsigned(reg_A) - $unsigned(reg_B);
                of = 0;
                if (reg_C == 0) 
                    zf = 1;
                else
                    zf = 0;
                if (reg_C[31] == 1)
                    nf = 1;
                else
                    nf = 0;
            end
        6'b011010:   // div: Divide (with overflow)
            begin
                reg_A = gr1;
                reg_B = gr2;
                reg_C = $signed(reg_A)/ $signed(reg_B);
                of = 0;
                if (reg_C == 0) 
                    zf = 1;
                else
                    zf = 0;
                if (reg_C[31] == 1)
                    nf = 1;
                else
                    nf = 0;
            end
        6'b011011:   // divu: Divide (without overflow)
            begin
                reg_A = gr1;
                reg_B = gr2;
                reg_C = $unsigned(reg_A) / $unsigned(reg_B);
                of = 0;
                if (reg_C[31] == 1) 
                    nf = 1;
                else
                    nf = 0;
                if (reg_C == 0)
                    zf = 1;
                else
                    zf = 0;
            end
        6'b011000:   // mult: Multiply (with overflow)
            begin
                reg_A = gr1;
                reg_B = gr2;
                reg_C = reg_A * reg_B;
                ex = reg_A * reg_B; 
                if (ex == reg_C)
                begin
                    of = 0;
                    if (reg_C[31] == 1) 
                        nf = 1;
                    else
                        nf = 0;
                    if (reg_C == 0)
                        zf = 1;
                    else
                        zf = 0;
                end
                else 
                    of=1;
            end
        6'b011001:   // multu: Multiply (without overflow)
            begin
                reg_A = gr1;
                reg_B = gr2;
                reg_C = $unsigned(reg_A) * $unsigned(reg_B);
                if (reg_C[31] == 1) 
                    nf = 1;
                else
                    nf = 0;
                if (reg_C == 0)
                    zf = 1;
                else
                    zf = 0;
                of = 0;
            end
        6'b100100:   // and: AND
            begin
                reg_A = gr1;
                reg_B = gr2;
                reg_C = reg_A & reg_B;
                of = 0;
                zf = 0;
                nf = 0;
            end
        6'b100101:   // or: OR
            begin
                reg_A = gr1;
                reg_B = gr2;
                reg_C = reg_A | reg_B;
                of = 0;
                zf = 0;
                nf = 0;
            end
        6'b100110:   // xor: XOR
            begin
                reg_A = gr1;
                reg_B = gr2;
                reg_C = reg_A ^ reg_B;
                of = 0;
                zf = 0;
                nf = 0;
            end
        6'b100111:   // nor: NOR
            begin
                reg_A = gr1;
                reg_B = gr2;
                reg_C = ~(reg_A | reg_B);
                of = 0;
                zf = 0;
                nf = 0;
            end
        6'b101010:   // slt: Set less than
            begin
                reg_A = gr1;
                reg_B = gr2;
                if (reg_A[31] && reg_B[31])
                    reg_C = (reg_A >= reg_B);
                else if (~(reg_A[31] || reg_B[31]))
                    reg_C = (reg_A < reg_B);
                else if (reg_A[31])
                    reg_C = 32'b1;
                else
                    reg_C = 32'b0;
                of = 0;
                zf = 0;
                nf = 0;
            end
        6'b101011:   // sltu: Set less than unsigned
            begin
                reg_A = gr1;
                reg_B = gr2;
                reg_C = reg_A < reg_B;
                of = 0;
                zf = 0;
                nf = 0;
            end
        endcase
    end
endcase

end

assign c = reg_C[31:0];
assign zero = zf;
assign overflow = of;
assign neg = nf;

endmodule
