.CODE

     ORG 0

     LDA _C0
     STA _MULTIPLICAND_0
     LDA _C1
     STA _MULTIPLIER_0
     LDA ZERO
     STA _Result_Mult_0
     _MULT_LOOP0:
     LDA _MULTIPLIER_0
     JZ _MULT_END0
     LDA _Result_Mult_0
     ADD _MULTIPLICAND_0
     STA _Result_Mult_0
     LDA _MULTIPLIER_0
     ADD MINUS_1
     STA _MULTIPLIER_0
     JMP _MULT_LOOP0
     _MULT_END0:
     LDA _Result_Mult_0
     STA A
     LDA _C2
     STA B
     LDA A
     STA _MULTIPLICAND_1
     LDA B
     STA _MULTIPLIER_1
     LDA ZERO
     STA _Result_Mult_1
     _MULT_LOOP1:
     LDA _MULTIPLIER_1
     JZ _MULT_END1
     LDA _Result_Mult_1
     ADD _MULTIPLICAND_1
     STA _Result_Mult_1
     LDA _MULTIPLIER_1
     ADD MINUS_1
     STA _MULTIPLIER_1
     JMP _MULT_LOOP1
     _MULT_END1:
     LDA _Result_Mult_1
     STA C
     HLT

.DATA

     ZERO DATA 0
     MINUS_1 DATA 255
     ONE DATA 1
     _C0 DATA 2
     _C1 DATA 3
     _Result_Mult_0 DATA 0
     _MULTIPLICAND_0 DATA 0
     _MULTIPLIER_0 DATA 0
     A DATA 0
     _C2 DATA 12
     B DATA 0
     _Result_Mult_1 DATA 0
     _MULTIPLICAND_1 DATA 0
     _MULTIPLIER_1 DATA 0
     C DATA 0
