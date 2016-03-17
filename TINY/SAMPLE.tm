* TINY Compilation to TM Code
* File: D:\code\cplusplus\tiny\SAMPLE.tm
* Standard prelude:
  0:     LD  6,0(0) 	load maxaddress from location 0
  1:     ST  0,0(0) 	clear location 0
* End of standard prelude.
  2:     IN  0,0,0 	read integer value
  3:     ST  0,1199(5) 	read: store value
* -> if
* -> LogicOp
* -> Op
* -> Id
  4:     LD  0,1199(5) 	load id value
* <- Id
  5:     ST  0,0(6) 	op: push left
* -> Const
  6:    LDC  0,0(0) 	load const
* <- Const
  7:     LD  1,0(6) 	op: load left
  8:    SUB  0,1,0 	op >
  9:    JGT  0,2(7) 	br if true
 10:    LDC  0,0(0) 	false case
 11:    LDA  7,1(7) 	unconditional jmp
 12:    LDC  0,1(0) 	true case
* <- Op
 13:     ST  0,0(6) 	op: push left
* -> Op
* -> Id
 14:     LD  0,1199(5) 	load id value
* <- Id
 15:     ST  0,-1(6) 	op: push left
* -> Const
 16:    LDC  0,100(0) 	load const
* <- Const
 17:     LD  1,-1(6) 	op: load left
 18:    SUB  0,1,0 	op <
 19:    JLT  0,2(7) 	br if true
 20:    LDC  0,0(0) 	false case
 21:    LDA  7,1(7) 	unconditional jmp
 22:    LDC  0,1(0) 	true case
* <- Op
 23:     LD  1,0(6) 	op: load left
* <- AND
 24:    JEQ  0,6(7) 	left is false
 25:    LDC  0,1(0) 	left is true
 26:    JEQ  1,2(7) 	right is false
 27:    LDC  1,1(1) 	right is true
 28:    LDA  7,1(7) 	conditional jmp
 29:    LDC  0,0(0) 	right is false case
 30:    LDA  7,1(7) 	jmp to avoid left_false
 31:    LDC  0,0(0) 	leftis false case
* -> AND
* <- LogicOpK
* if: jump to else belongs here
* -> assign
* -> Const
 33:    LDC  0,1(0) 	load const
* <- Const
 34:     ST  0,1689(5) 	assign: store value
* <- assign
* -> while
* while: test in here
* -> Op
* -> Id
 35:     LD  0,1199(5) 	load id value
* <- Id
 36:     ST  0,0(6) 	op: push left
* -> Const
 37:    LDC  0,0(0) 	load const
* <- Const
 38:     LD  1,0(6) 	op: load left
 39:    SUB  0,1,0 	op >
 40:    JGT  0,2(7) 	br if true
 41:    LDC  0,0(0) 	false case
 42:    LDA  7,1(7) 	unconditional jmp
 43:    LDC  0,1(0) 	true case
* <- Op
* -> assign
* -> Op
* -> Id
 45:     LD  0,1689(5) 	load id value
* <- Id
 46:     ST  0,0(6) 	op: push left
* -> Id
 47:     LD  0,1199(5) 	load id value
* <- Id
 48:     LD  1,0(6) 	op: load left
 49:    MUL  0,1,0 	op *
* <- Op
 50:     ST  0,1689(5) 	assign: store value
* <- assign
* -> assign
* -> Op
* -> Id
 51:     LD  0,1199(5) 	load id value
* <- Id
 52:     ST  0,0(6) 	op: push left
* -> Const
 53:    LDC  0,1(0) 	load const
* <- Const
 54:     LD  1,0(6) 	op: load left
 55:    SUB  0,1,0 	op -
* <- Op
 56:     ST  0,1199(5) 	assign: store value
* <- assign
 57:    LDA  7,-23(7) 	while jmp to end
 44:    JEQ  0,13(7) 	while: jmp back to body0
* <- while
* -> Id
 58:     LD  0,1689(5) 	load id value
* <- Id
 59:    OUT  0,0,0 	write ac
* if: jump to end belongs here
 32:    JEQ  0,28(7) 	if: jmp to else
 60:    LDA  7,0(7) 	jmp to end
* <- if
* End of execution.
 61:   HALT  0,0,0 	
