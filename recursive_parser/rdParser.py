import re
from functools import *

class recDescent:
    #token position tracker
    tracker = 0

    #tracker for un-paired parentheses 
    priopen_tracker = False

    # relational (unary) operators (prefix)
    relop = ['<', '>', '<=', '>=', '=', '!=', 'not']
    
    # binary operators (infix)
    dashop = ['-', '–']
    logicop = ['and', 'or']

    # tokens for manipulating priority
    priopen = '('
    priclose = ')'

    # constructor to initialize and set class level variables
    def __init__(self, expr = ""):

        # string to be parsed
        self.expr = expr

        # tokens from lexer tokenization of the expression
        self.tokens = []

    # lexer - tokenize the expression into a list of tokens
    # the tokens are stored in an list which can be accessed by self.tokens
    # do not edit any piece of code in this function
    def lex(self):
        self.tokens = re.findall("[-\(\)=]|[!<>]=|[<>]|\w+|[^ +]\W+", self.expr)
        # filter out the possible spaces in the elements
        self.tokens = list(filter((lambda x: len(x)), 
                           list(map((lambda x: x.strip().lower()), self.tokens)))) 

    #functions checks if the iterator is stepping out of bounds
    def atTheEnd(self):
        if self.tracker >= len(self.tokens) - 1:
            return True
        else:
            return False

    #function checks if the token is a relational operator
    def relopTest(self):
        if self.tokens[self.tracker] in self.relop:
            return True
        else:
            return False

    #function checks if the token is a logical operator
    def op(self):
        if self.tokens[self.tracker] in self.logicop:
            return True
        else:
            return False

    #function checks if the next sequence is an int - int sequence
    def dash(self):
        found = False
        if self.tokens[self.tracker].isnumeric():
            if not self.atTheEnd():
                self.tracker += 1
                if self.tokens[self.tracker] in self.dashop:
                    if not self.atTheEnd():
                        self.tracker += 1
                        if self.tokens[self.tracker].isnumeric():
                            found = True
        if found:
            return True
        else:
            return False

    #function checks if the next sequence is considered a term
    def term(self):
        found = False
        if not self.atTheEnd():
            if self.relopTest():
                if not self.atTheEnd():
                    self.tracker += 1
                    if self.tokens[self.tracker].isnumeric():
                        found = True
            elif self.dash():
                found = True
            elif self.tokens[self.tracker] == self.priopen:
                self.priopen_tracker = True
                """
                    <exp> recursion
                """
                self.tracker += 1
                if self.exp():
                    if not self.atTheEnd():
                        self.tracker += 1
                        if self.tokens[self.tracker] == self.priclose:
                            found = True

        if found:
            return True
        else:
            return False
    
    #function checks if the next sequence is considered an expression
    def exp(self):
        found = False
        """
            Checks for <term> { <op> <term> } sequences
        """
        if self.term():
            found = True
            if not self.atTheEnd() and not self.tokens[self.tracker + 1] == self.priclose:
                found = False
                self.tracker += 1
                while self.op():
                    if not self.atTheEnd():
                        self.tracker += 1
                        if self.term():
                            found = True
                            if not self.atTheEnd() and not self.tokens[self.tracker + 1] == self.priclose:
                                self.tracker += 1
                        else:
                            found = False
                            break
                    else:
                        found = False
                        break
            #checks for un-paired priclose boolean
            elif not self.atTheEnd() and not self.priopen_tracker and self.tokens[self.tracker + 1] == self.priclose:
                found = False

        if found:
            return True
        else:
            return False
    # parser - determine if the input expression is valid or not
    
    # validate() function will return True if the expression is valid, False otherwise 
    # do not change the method signature as this function will be called by the autograder
    def validate(self):
    # use your parsing procedures below to validate
        self.lex()
        if self.exp():
            return True
        else:
            return False

    # parsing procedures corresponding to the grammar rules - follow Figure 5.17