.CODE

     ORG 0

     LDA _C0
     ADD _C1
     STA _Result_Sum_0
     LDA _C2
     STA _MULTIPLICAND_0
     LDA _C3
     STA _MULTIPLIER_0
     LDA ZERO
     STA _Result_Mult_1
     _MULT_LOOP0:
     LDA _MULTIPLIER_0
     JZ _MULT_END0
     LDA _Result_Mult_1
     ADD _MULTIPLICAND_0
     STA _Result_Mult_1
     LDA _MULTIPLIER_0
     ADD MINUS_1
     STA _MULTIPLIER_0
     JMP _MULT_LOOP0
     _MULT_END0:
     LDA _C4
     ADD _C5
     STA _Result_Sum_2
     LDA _C7
     NOT
     ADD ONE
     STA _Result_Sum_3
     LDA _C6
     ADD _Result_Sum_3
     STA _Result_Sum_3
     LDA _Result_Sum_3
     NOT
     ADD ONE
     STA _Result_Sum_4
     LDA _Result_Sum_2
     ADD _Result_Sum_4
     STA _Result_Sum_4
     LDA _Result_Mult_1
     STA _MULTIPLICAND_1
     LDA _Result_Sum_4
     STA _MULTIPLIER_1
     LDA ZERO
     STA _Result_Mult_5
     _MULT_LOOP1:
     LDA _MULTIPLIER_1
     JZ _MULT_END1
     LDA _Result_Mult_5
     ADD _MULTIPLICAND_1
     STA _Result_Mult_5
     LDA _MULTIPLIER_1
     ADD MINUS_1
     STA _MULTIPLIER_1
     JMP _MULT_LOOP1
     _MULT_END1:
     LDA _Result_Sum_0
     ADD _Result_Mult_5
     STA _Result_Sum_6
     LDA _Result_Sum_6
     HLT

.DATA

     ZERO DATA 0
     MINUS_1 DATA 255
     ONE DATA 1
     _C0 DATA 5
     _C1 DATA 1
     _Result_Sum_0 DATA 0
     _C2 DATA 5
     _C3 DATA 2
     _Result_Mult_1 DATA 0
     _MULTIPLICAND_0 DATA 0
     _MULTIPLIER_0 DATA 0
     _C4 DATA 4
     _C5 DATA 2
     _Result_Sum_2 DATA 0
     _C6 DATA 3
     _C7 DATA 2
     _Result_Sum_3 DATA 0
     _Result_Sum_4 DATA 0
     _Result_Mult_5 DATA 0
     _MULTIPLICAND_1 DATA 0
     _MULTIPLIER_1 DATA 0
     _Result_Sum_6 DATA 0
