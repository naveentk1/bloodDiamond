//c++ tokenizer , performs lexical analysis on c++ source code
#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <unordered_set>

enum TokenType {
    KEYWORD,
    IDENTIFIER,
    NUMBER,
    OPERATOR,
    PUNCTUATION,
    STRING_LITERAL,
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;

    Token(TokenType t, const std::string& v, int l, int c)
        : type(t), value(v), line(l), column(c) {}
};

class Tokenizer {
private:
    std::string source;
    size_t pos;
    int line;
    int column;

    std::unordered_set<std::string> keywords = {
        "auto", "break", "case", "char", "const", "continue", "default",
        "do", "double", "else", "enum", "extern", "float", "for", "goto",
        "if", "int", "long", "register", "return", "short", "signed",
        "sizeof", "static", "struct", "switch", "typedef", "union",
        "unsigned", "void", "volatile", "while", "class", "namespace",
        "public", "private", "protected", "virtual", "bool", "true", "false"
    };

    char peek(int offset = 0) {
        if (pos + offset >= source.length()) return '\0';
        return source[pos + offset];
    }

    char advance() {
        if (pos >= source.length()) return '\0';
        char c = source[pos++];
        if (c == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        return c;
    }

    void skipWhitespace() {
        while (pos < source.length() && std::isspace(peek())) {
            advance();
        }
    }

    void skipComments() {
        if (peek() == '/' && peek(1) == '/') {
            // Single-line comment
            while (peek() != '\n' && peek() != '\0') {
                advance();
            }
        } else if (peek() == '/' && peek(1) == '*') {
            // Multi-line comment
            advance(); // consume '/'
            advance(); // consume '*'
            while (!(peek() == '*' && peek(1) == '/') && peek() != '\0') {
                advance();
            }
            if (peek() != '\0') {
                advance(); // consume '*'
                advance(); // consume '/'
            }
        }
    }

    Token readIdentifierOrKeyword() {
        int startLine = line;
        int startCol = column;
        std::string value;

        while (std::isalnum(peek()) || peek() == '_') {
            value += advance();
        }

        TokenType type = keywords.count(value) ? KEYWORD : IDENTIFIER;
        return Token(type, value, startLine, startCol);
    }

    Token readNumber() {
        int startLine = line;
        int startCol = column;
        std::string value;

        while (std::isdigit(peek()) || peek() == '.') {
            value += advance();
        }

        return Token(NUMBER, value, startLine, startCol);
    }

    Token readString() {
        int startLine = line;
        int startCol = column;
        std::string value;

        advance(); // consume opening quote

        while (peek() != '"' && peek() != '\0') {
            if (peek() == '\\') {
                advance();
                if (peek() != '\0') {
                    value += advance();
                }
            } else {
                value += advance();
            }
        }

        if (peek() == '"') {
            advance(); // consume closing quote
        }

        return Token(STRING_LITERAL, value, startLine, startCol);
    }

    Token readOperator() {
        int startLine = line;
        int startCol = column;
        std::string value;

        char c = peek();
        char next = peek(1);

        // Two-character operators
        if ((c == '=' && next == '=') ||
            (c == '!' && next == '=') ||
            (c == '<' && next == '=') ||
            (c == '>' && next == '=') ||
            (c == '+' && next == '+') ||
            (c == '-' && next == '-') ||
            (c == '&' && next == '&') ||
            (c == '|' && next == '|') ||
            (c == '-' && next == '>') ||
            (c == ':' && next == ':')) {
            value += advance();
            value += advance();
        } else {
            value += advance();
        }

        return Token(OPERATOR, value, startLine, startCol);
    }

public:
    Tokenizer(const std::string& src)
        : source(src), pos(0), line(1), column(1) {}

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;

        while (pos < source.length()) {
            skipWhitespace();
            skipComments();

            if (pos >= source.length()) break;

            char c = peek();
            int startLine = line;
            int startCol = column;

            if (std::isalpha(c) || c == '_') {
                tokens.push_back(readIdentifierOrKeyword());
            } else if (std::isdigit(c)) {
                tokens.push_back(readNumber());
            } else if (c == '"') {
                tokens.push_back(readString());
            } else if (c == ';' || c == ',' || c == '(' || c == ')' ||
                       c == '{' || c == '}' || c == '[' || c == ']') {
                std::string value(1, advance());
                tokens.push_back(Token(PUNCTUATION, value, startLine, startCol));
            } else if (c == '+' || c == '-' || c == '*' || c == '/' ||
                       c == '=' || c == '<' || c == '>' || c == '!' ||
                       c == '&' || c == '|' || c == ':') {
                tokens.push_back(readOperator());
            } else {
                std::string value(1, advance());
                tokens.push_back(Token(UNKNOWN, value, startLine, startCol));
            }
        }

        tokens.push_back(Token(END_OF_FILE, "", line, column));
        return tokens;
    }
};

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case KEYWORD: return "KEYWORD";
        case IDENTIFIER: return "IDENTIFIER";
        case NUMBER: return "NUMBER";
        case OPERATOR: return "OPERATOR";
        case PUNCTUATION: return "PUNCTUATION";
        case STRING_LITERAL: return "STRING";
        case END_OF_FILE: return "EOF";
        case UNKNOWN: return "UNKNOWN";
        default: return "?";
    }
}

void printTokens(const std::vector<Token>& tokens) {
    for (const auto& token : tokens) {
        std::cout << "[" << tokenTypeToString(token.type) << "] "
                  << "'" << token.value << "' "
                  << "(line " << token.line << ", col " << token.column << ")\n";
    }
}

int main() {
    // Test the tokenizer
    std::string code = R"(int main() {
            int x = 42;
            if (x > 0) {
                std::cout << "Hello, World!" << std::endl;
            }
            return 0;
        }
    )";

    Tokenizer tokenizer(code);
    std::vector<Token> tokens = tokenizer.tokenize();

    std::cout << "Tokenization Result:\n";
    std::cout << "====================\n";
    printTokens(tokens);

    return 0;
}