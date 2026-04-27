.CODE

     ORG 0

     LDA _C1
     ADD _C2
     STA _Result_Sum_0
     LDA _C0
     STA _MULTIPLICAND_2
     LDA _Result_Sum_0
     STA _MULTIPLIER_3
     LDA ZERO
     STA _Result_Mult_1
     _MULT_LOOP0:
     LDA _MULTIPLIER_3
     JZ _MULT_END0
     LDA _Result_Mult_1
     ADD _MULTIPLICAND_2
     STA _Result_Mult_1
     LDA _MULTIPLIER_3
     ADD MINUS_1
     STA _MULTIPLIER_3
     JMP _MULT_LOOP0
     _MULT_END0:
     LDA _Result_Mult_1
     HLT

.DATA

     ZERO DATA 0
     MINUS_1 DATA 255
     _C0 DATA 2
     _C1 DATA 3
     _C2 DATA 1
     _Result_Sum_0 DATA 0
     _Result_Mult_1 DATA 0
     _MULTIPLICAND_2 DATA 0
     _MULTIPLIER_3 DATA 0
