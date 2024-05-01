#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string>

#define TODO(text)        \
    printf("%s\n", text); \
    exit(255);

#define UNREACHABLE                       \
    printf("Unreachable code reached\n"); \
    exit(255);

enum TokenType
{
    OPEN_PAREN,
    CLOSE_PAREN,
    ATOM,
};

std::string TokenTypeName[3] = {
    "OPEN_PAREN",
    "CLOSE_PAREN",
    "ATOM",
};

class Location
{
public:
    int row;
    int col;

    Location(int row, int col)
    {
        this->row = row;
        this->col = col;
    }

    std::string dump()
    {
        return std::to_string(this->row + 1) + ":" + std::to_string(this->col + 1);
    }
};

class Token
{
public:
    TokenType type;
    std::string value;
    Location *loc;

    Token(TokenType type, std::string value, Location *loc)
    {
        this->type = type;
        this->value = value;
        this->loc = loc;
    }
};

class Lexer
{
public:
    std::string text;
    size_t loc = 0;
    size_t bol = 0;
    size_t row = 0;

    Lexer(std::string text)
    {
        this->text = text;
    }

    ~Lexer()
    {
        delete &this->text;
    }

    void chop_char()
    {
        if (this->is_not_empty())
        {
            char curr_char = this->text.at(this->loc);
            this->loc += 1;

            if (curr_char == '\n')
            {
                this->bol = this->loc;
                this->row += 1;
            }
        }
    }

    void trim_left()
    {
        while (this->is_not_empty() && isspace(this->text.at(this->loc)))
        {
            this->chop_char();
        }
    }

    Token *next_token()
    {
        if (!this->is_not_empty())
        {
            throw -1;
        }

        this->trim_left();

        Location *loc = new Location(this->row, this->loc - this->bol);

        char first_char = this->text.at(this->loc);

        if (first_char == '(')
        {
            this->chop_char();
            return new Token(TokenType::OPEN_PAREN, "(", loc);
        }
        else if (first_char == ')')
        {
            this->chop_char();
            return new Token(TokenType::CLOSE_PAREN, ")", loc);
        }
        else if (isalnum(first_char))
        {
            int start_index = this->loc;
            int end_index = 0;

            while (this->is_not_empty() && isalnum(this->text.at(this->loc)))
            {
                this->chop_char();
                end_index += 1;
            }
            std::string value = this->text.substr(start_index, end_index);

            return new Token(TokenType::ATOM, value, loc);
        }
        else
        {
            UNREACHABLE;
        }
    }

    bool is_not_empty()
    {
        return this->loc + 1 <= this->text.length();
    }
};

int main(int argc, char **argv)
{

    std::ifstream t("seq.txt");
    std::stringstream buffer;
    buffer << t.rdbuf();

    const std::string seq = buffer.str();

    Lexer *lex = new Lexer(seq);

    while (lex->is_not_empty())
    {
        Token *token = lex->next_token();
        std::cout << token->loc->dump() << ": " << token->value << " is " << TokenTypeName[token->type] << std::endl;
    }
}