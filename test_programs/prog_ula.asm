WORD val1 0
WORD val2 2

W A0
W A1
ST A0 val1
ST A1 val2

ADD A2 A0 A1
SUB A3 A0 A1
ST A2 add_res
ST A3 sub_res

MUL A2 A0 A1
DIV A3 A0 A1
ST A2 mul_res
ST A3 div_res