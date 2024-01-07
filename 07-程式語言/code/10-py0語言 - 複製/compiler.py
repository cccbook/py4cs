# prog = stmts
def compile(code):
    lex(code)
	


# stmts = stmt*

# block = : <begin> stmts <end>

"""
stmt = block                     |
       function                  |
       while expr: stmt           | 
       if expr: stmt (elif stmt)* (else stmt)? |
       return expr               |
       assign
"""

# assign = term = expr

# params = param*

# function = def id(params) block

# expr = bexpr (? expr : expr)

# bexpr = item (op2 expr)?

# item = str | array | map | factor

# factor = (!-~) factor | num | ( expr ) | term

# term = id ( [expr] | . id | args )*

# array = [ expr* ]

# map = { (str:expr)* }

# args  = ( expr* ','? )

# bool: True | False
# num : integer | float
# str : '...'
# id  : [a-zA-Z_][a-zA-Z_0-9]*
