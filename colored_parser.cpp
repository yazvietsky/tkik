#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

// 1. Rozbudowana lista tokenów
enum TokenCode {
    KEYWORD,
    ID,
    NUMBER,
    STRING,
    COMMENT,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    ASSIGN,
    EQUALS,
    LESS,
    GREATER,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    SEMICOLON,
    WHITESPACE,
    UNKNOWN,
    EOF_
};

string names[] = {
    "KEYWORD", "ID", "NUMBER", "STRING", "COMMENT",
    "PLUS", "MINUS", "MULTIPLY", "DIVIDE",
    "ASSIGN", "EQUALS", "LESS", "GREATER",
    "LPAREN", "RPAREN", "LBRACE", "RBRACE", "SEMICOLON",
    "WHITESPACE", "UNKNOWN", "EOF_"
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

        // Białe znaki
        if (isspace(*next)) {
            string ws_value = "";
            while (isspace(*next)) {
                ws_value += *next;
                next++;
            }
            exp.push_back(Token(TokenCode::WHITESPACE, ws_value));
            return true;
        }

        // Identyfikatory i słowa kluczowe (rozszerzone o znak '_')
        if (isalpha(*next) || *next == '_') {
            string id_value = "";
            while (isalnum(*next) || *next == '_') {
                id_value += *next;
                next++;
            }
            
            // Sprawdzanie czy identyfikator to słowo kluczowe
            if (id_value == "int" || id_value == "if" || id_value == "else" || 
                id_value == "while" || id_value == "return" || id_value == "string" || id_value == "float" ) {
                exp.push_back(Token(TokenCode::KEYWORD, id_value));
            } else {
                exp.push_back(Token(ID, id_value));
            }
            return true;
        }

        // Liczby
        if (isdigit(*next)) {
            string value = "";
            while (isdigit(*next)) {
                value += *next;
                next++;
            }
            exp.push_back(Token(TokenCode::NUMBER, value));
            return true;
        }

        // Ciągi znaków (Stringi)
        if (*next == '"') {
            string str_val = "\"";
            next++;
            while (*next != '"' && *next != '\0') {
                str_val += *next;
                next++;
            }
            if (*next == '"') {
                str_val += '"';
                next++;
            }
            exp.push_back(Token(TokenCode::STRING, str_val));
            return true;
        }

        // Operatory, komentarze i interpunkcja
        switch (*next) {
            case '+': exp.push_back(Token(TokenCode::PLUS, "+")); next++; return true;
            case '-': exp.push_back(Token(TokenCode::MINUS, "-")); next++; return true;
            case '*': exp.push_back(Token(TokenCode::MULTIPLY, "*")); next++; return true;
            case '/':
                // Wykrywanie komentarzy jednolinijkowych (//)
                if (*(next + 1) == '/') {
                    string comment = "//";
                    next += 2;
                    while (*next != '\n' && *next != '\0') {
                        comment += *next;
                        next++;
                    }
                    exp.push_back(Token(TokenCode::COMMENT, comment));
                    return true;
                } else {
                    exp.push_back(Token(TokenCode::DIVIDE, "/"));
                    next++;
                    return true;
                }
            case '=':
                // Wykrywanie operatora przyrównania (==) vs przypisania (=)
                if (*(next + 1) == '=') {
                    exp.push_back(Token(TokenCode::EQUALS, "=="));
                    next += 2;
                } else {
                    exp.push_back(Token(TokenCode::ASSIGN, "="));
                    next++;
                }
                return true;
            case '<': exp.push_back(Token(TokenCode::LESS, "<")); next++; return true;
            case '>': exp.push_back(Token(TokenCode::GREATER, ">")); next++; return true;
            case '(': exp.push_back(Token(TokenCode::LPAREN, "(")); next++; return true;
            case ')': exp.push_back(Token(TokenCode::RPAREN, ")")); next++; return true;
            case '{': exp.push_back(Token(TokenCode::LBRACE, "{")); next++; return true;
            case '}': exp.push_back(Token(TokenCode::RBRACE, "}")); next++; return true;
            case ';': exp.push_back(Token(TokenCode::SEMICOLON, ";")); next++; return true;
            
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

        // Dodano nowe klasy CSS dla słów kluczowych, stringów, komentarzy i nawiasów
        outFile << "<!DOCTYPE html>\n<html>\n<head>\n"
                << "<style>\n"
                << "  body { background-color: #1e1e1e; color: #d4d4d4; font-family: Consolas, monospace; font-size: 16px; padding: 20px; }\n"
                << "  .keyword { color: #569cd6; font-weight: bold; }\n"
                << "  .number { color: #b5cea8; }\n"
                << "  .string { color: #ce9178; }\n"
                << "  .comment { color: #6a9955; font-style: italic; }\n"
                << "  .id { color: #9cdcfe; }\n"
                << "  .operator { color: #c586c0; }\n"
                << "  .punctuation { color: #ffd700; }\n"
                << "  .unknown { color: #f44747; text-decoration: underline; }\n"
                << "</style>\n"
                << "</head>\n<body>\n<pre>";

        for (const auto& token : exp) {
            if (token.getCode() == EOF_) break;

            string safeValue = escapeHTML(token.getValue());

            switch (token.getCode()) {
                case KEYWORD:
                    outFile << "<span class=\"keyword\">" << safeValue << "</span>";
                    break;
                case NUMBER:
                    outFile << "<span class=\"number\">" << safeValue << "</span>";
                    break;
                case STRING:
                    outFile << "<span class=\"string\">" << safeValue << "</span>";
                    break;
                case COMMENT:
                    outFile << "<span class=\"comment\">" << safeValue << "</span>";
                    break;
                case ID:
                    outFile << "<span class=\"id\">" << safeValue << "</span>";
                    break;
                case PLUS: case MINUS: case MULTIPLY: case DIVIDE: case ASSIGN: case EQUALS: case LESS: case GREATER:
                    outFile << "<span class=\"operator\">" << safeValue << "</span>";
                    break;
                case LPAREN: case RPAREN: case LBRACE: case RBRACE: case SEMICOLON:
                    outFile << "<span class=\"punctuation\">" << safeValue << "</span>";
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