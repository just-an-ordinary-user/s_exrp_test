#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <variant>

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

enum AtomType
{
    SYMBOL,
    STRING,
    NUMBER,
};

std::string TokenTypeName[3] = {
    "OPEN_PAREN",
    "CLOSE_PAREN",
    "ATOM",
};

std::string AtomTypeName[3] = {
    "SYMBOL",
    "STRING",
    "NUMBER",
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

class Atom
{
public:
    AtomType type;
    std::variant<std::string, float> payload;

    Atom(AtomType type, std::variant<std::string, float> payload)
    {
        this->type = type;
        this->payload = payload;
    }

    template <typename T>
    T get_payload()
    {
        if (auto value = std::get_if<T>(&this->payload))
        {
            return *value;
        }
        UNREACHABLE;
    }
};

class Token
{
public:
    TokenType type;
    std::string value;
    Location *loc;
    Atom *atom;

    Token(TokenType type, std::string value, Location *loc, Atom *atom)
    {
        this->type = type;
        this->value = value;
        this->loc = loc;

        if (atom != NULL)
        {
            this->atom = atom;
        }
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
            return new Token(TokenType::OPEN_PAREN, "(", loc, NULL);
        }
        else if (first_char == ')')
        {
            this->chop_char();
            return new Token(TokenType::CLOSE_PAREN, ")", loc, NULL);
        }
        else if (isdigit(first_char) || first_char == '-')
        {
            bool is_negative = false;

            if (first_char == '-')
            {
                this->chop_char();
                is_negative = true;
            }

            int start_index = this->loc;
            int end_index = 0;

            while (this->is_not_empty() && isdigit(this->text.at(this->loc)) || this->text.at(this->loc) == '.')
            {
                this->chop_char();
                end_index += 1;
            }
            std::string num = this->text.substr(start_index, end_index);

            std::string value = is_negative ? "-" + num : num;
            float parsed_value = this->string_to_float(value);

            Atom *atom = new Atom(AtomType::NUMBER, parsed_value);

            return new Token(TokenType::ATOM, value, loc, atom);
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

            Atom *atom = new Atom(AtomType::SYMBOL, value);

            return new Token(TokenType::ATOM, value, loc, atom);
        }
        else if (first_char == '"')
        {
            this->chop_char();
            int start_index = this->loc;
            int end_index = 0;

            while (this->is_not_empty() && this->text.at(this->loc) != '"')
            {
                this->chop_char();
                end_index += 1;
            }

            this->chop_char();
            std::string value = this->text.substr(start_index, end_index);

            Atom *atom = new Atom(AtomType::STRING, value);

            return new Token(TokenType::ATOM, value, loc, atom);
        }
        else
        {
            UNREACHABLE;
        }
    }

    bool is_not_empty()
    {
        return this->loc + 1 < this->text.length();
    }

private:
    float string_to_float(std::string input)
    {
        return ::atof(input.c_str());
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
        if (token->type == TokenType::ATOM && token->atom != NULL)
        {
            if (token->atom->type == AtomType::STRING || token->atom->type == AtomType::SYMBOL)
            {
                std::cout << token->loc->dump() << ": \"" << token->value << "\" is " << TokenTypeName[token->type] << " of type " << AtomTypeName[token->atom->type] << " with value " << token->atom->get_payload<std::string>() << std::endl;
            }
            else if (token->atom->type == AtomType::NUMBER)
            {
                std::cout << token->loc->dump() << ": \"" << token->value << "\" is " << TokenTypeName[token->type] << " of type " << AtomTypeName[token->atom->type] << " with value " << token->atom->get_payload<float>() << std::endl;
            }
        }
        else
        {
            std::cout << token->loc->dump() << ": \"" << token->value << "\" is " << TokenTypeName[token->type] << std::endl;
        }
    }
}
