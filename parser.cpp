#include <iostream>
#include <vector>
#include <vector>

using namespace std;

enum TokenCode{
    NUMBER,
    ID,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    LPAREN,
    RPAREN,
    EOF_
};

string names[] = {
    "NUMBER",
    "ID",
    "PLUS",
    "MINUS",
    "MULTIPLY",
    "DIVIDE",
    "LPAREN",
    "RPAREN",
    "EOF_"
}; 

class Token{
    TokenCode code;
    string value;
public:
    Token(TokenCode code, string value){
        this->code = code;
        this->value = value;
    }
    void print(){
        cout<<names[code]<<" "<<value<<endl;
    }
};

class Scaner{
    string input;
    vector<Token> exp;

    bool get_token(char*& next) {
        while (isspace(*next)) {
            next++;
        }

        if (*next == '\0') {
            exp.push_back(Token(TokenCode::EOF_, "\\0"));
            std::cout << "Token: EOF, Wartosc: \\0" << std::endl;
            return false; 
        }
        if (isalpha(*next)) {
            std::string id_value = "";
            
            while (isalnum(*next)) {
                id_value += *next;
                next++;
            }
            
            exp.push_back(Token(ID, id_value));
            std::cout << "Token: ID, Wartosc: " << id_value << std::endl;
            return true;
        }

        if (isdigit(*next)) {
            std::string value = "";
            while (isdigit(*next)) {
                value += *next;
                next++;
            }
            exp.push_back(Token(TokenCode::NUMBER, value));
            std::cout << "Token: NUMBER, Wartosc: " << value << std::endl;
            return true;
        }

        switch (*next) {
            case '+':
                exp.push_back(Token(TokenCode::PLUS, "+"));
                std::cout << "Token: PLUS, Wartosc: +" << std::endl;
                next++;
                return true;
                
            case '-':
                exp.push_back(Token(TokenCode::MINUS, "-"));
                std::cout << "Token: MINUS, Wartosc: -" << std::endl;
                next++;
                return true;
                
            case '*':
                exp.push_back(Token(TokenCode::MULTIPLY, "*"));
                std::cout << "Token: MULTIPLY, Wartosc: *" << std::endl;
                next++;
                return true;
                
            case '/':
                exp.push_back(Token(TokenCode::DIVIDE, "/"));
                std::cout << "Token: DIVIDE, Wartosc: /" << std::endl;
                next++;
                return true;
                
            case '(':
                exp.push_back(Token(TokenCode::LPAREN, "("));
                std::cout << "Token: LPAREN, Wartosc: (" << std::endl;
                next++;
                return true;
                
            case ')':
                exp.push_back(Token(TokenCode::RPAREN, ")"));
                std::cout << "Token: RPAREN, Wartosc: )" << std::endl;
                next++;
                return true;
                
            default:
                std::cerr << "Blad: Nierozpoznany znak '" << *next << "'" << std::endl;
                next++;
                return true; 
        }
    }
public:
    Scaner(string input){
        this-> input = input;
    }
    void scan(){
        char* next = &input[0];
        while(get_token(next));    
    }
    void print(){
        cout<<"Scanned tokens:"<<endl;
        for (auto &token : exp){
            token.print();
        }
    }
};

int main(){
    string s = "zmienna123 + 456 * (78 / x) - 9  $%$%$%%%^^";
    Scaner scaner= Scaner(s);
    scaner.scan();
    scaner.print();
    return 0;
}