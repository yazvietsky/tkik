#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

enum TokenCode {
    NUMBER,
    ID,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    LPAREN,
    RPAREN,
    WHITESPACE,
    UNKNOWN,
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
    "WHITESPACE",
    "UNKNOWN",
    "EOF_"
}; 

class Token {
    TokenCode code;
    string value;
public:
    Token(TokenCode code, string value) {
        this->code = code;
        this->value = value;
    }
    
    void print() {
        if (code != WHITESPACE) {
            cout << names[code] << " " << value << endl;
        }
    }

    TokenCode getCode() const { return code; }
    string getValue() const { return value; }
};

class Scaner {
    string input;
    vector<Token> exp;

    string escapeHTML(const string& data) {
        string buffer;
        buffer.reserve(data.size());
        for (char c : data) {
            switch (c) {
                case '&':  buffer.append("&amp;"); break;
                case '<':  buffer.append("&lt;"); break;
                case '>':  buffer.append("&gt;"); break;
                default:   buffer.append(1, c); break;
            }
        }
        return buffer;
    }

    bool get_token(char*& next) {
        if (*next == '\0') {
            exp.push_back(Token(TokenCode::EOF_, "\\0"));
            return false; 
        }

        if (isspace(*next)) {
            string ws_value = "";
            while (isspace(*next)) {
                ws_value += *next;
                next++;
            }
            exp.push_back(Token(TokenCode::WHITESPACE, ws_value));
            return true;
        }

        if (isalpha(*next)) {
            string id_value = "";
            while (isalnum(*next)) {
                id_value += *next;
                next++;
            }
            exp.push_back(Token(ID, id_value));
            return true;
        }

        if (isdigit(*next)) {
            string value = "";
            while (isdigit(*next)) {
                value += *next;
                next++;
            }
            exp.push_back(Token(TokenCode::NUMBER, value));
            return true;
        }

        switch (*next) {
            case '+':
                exp.push_back(Token(TokenCode::PLUS, "+"));
                next++;
                return true;
            case '-':
                exp.push_back(Token(TokenCode::MINUS, "-"));
                next++;
                return true;
            case '*':
                exp.push_back(Token(TokenCode::MULTIPLY, "*"));
                next++;
                return true;
            case '/':
                exp.push_back(Token(TokenCode::DIVIDE, "/"));
                next++;
                return true;
            case '(':
                exp.push_back(Token(TokenCode::LPAREN, "("));
                next++;
                return true;
            case ')':
                exp.push_back(Token(TokenCode::RPAREN, ")"));
                next++;
                return true;
            default:
                string unk = "";
                unk += *next;
                exp.push_back(Token(TokenCode::UNKNOWN, unk));
                std::cerr << "Blad: Nierozpoznany znak '" << *next << "'" << std::endl;
                next++;
                return true; 
        }
    }
public:
    Scaner(string input) {
        this->input = input;
    }

    void scan() {
        if (input.empty()) return;
        char* next = &input[0];
        while(get_token(next));    
    }

    void print() {
        cout << "Scanned tokens:" << endl;
        for (auto &token : exp) {
            token.print();
        }
    }

    void exportToHTML(const string& filename) {
        ofstream outFile(filename);
        if (!outFile.is_open()) {
            cerr << "Nie udalo sie otworzyc pliku do zapisu: " << filename << endl;
            return;
        }

        outFile << "<!DOCTYPE html>\n<html>\n<head>\n"
                << "<style>\n"
                << "  body { background-color: #1e1e1e; color: #d4d4d4; font-family: Consolas, monospace; font-size: 16px; padding: 20px; }\n"
                << "  .number { color: #b5cea8; }\n"
                << "  .id { color: #9cdcfe; }\n"
                << "  .operator { color: #c586c0; }\n"
                << "  .paren { color: #ffd700; }\n"
                << "  .unknown { color: #f44747; text-decoration: underline; }\n"
                << "</style>\n"
                << "</head>\n<body>\n<pre>";

        for (const auto& token : exp) {
            if (token.getCode() == EOF_) break;

            string safeValue = escapeHTML(token.getValue());

            switch (token.getCode()) {
                case NUMBER:
                    outFile << "<span class=\"number\">" << safeValue << "</span>";
                    break;
                case ID:
                    outFile << "<span class=\"id\">" << safeValue << "</span>";
                    break;
                case PLUS: case MINUS: case MULTIPLY: case DIVIDE:
                    outFile << "<span class=\"operator\">" << safeValue << "</span>";
                    break;
                case LPAREN: case RPAREN:
                    outFile << "<span class=\"paren\">" << safeValue << "</span>";
                    break;
                case UNKNOWN:
                    outFile << "<span class=\"unknown\">" << safeValue << "</span>";
                    break;
                case WHITESPACE:
                    outFile << safeValue;
                    break;
                default:
                    break;
            }
        }

        outFile << "</pre>\n</body>\n</html>";
        outFile.close();
        cout << "\nPomyslnie wygenerowano plik HTML: " << filename << endl;
    }
};

int main() {
    string inputFilename = "kod.txt";
    string outputFilename = "wynik.html";

    ifstream inFile(inputFilename);
    if (!inFile.is_open()) {
        cerr << "Nie znaleziono pliku: " << inputFilename << endl;
        return 1;

    }

    stringstream buffer;
    buffer << inFile.rdbuf();
    string code = buffer.str();
    inFile.close();

    Scaner scaner(code);
    scaner.scan();
    
    scaner.print();
    scaner.exportToHTML(outputFilename);

    return 0;
}