// Tipo de número
typedef enum {
    NUM_TYPE_NAN,
    NUM_TYPE_INVALID,
    NUM_TYPE_NUM
} NUM_TYPE;

// Check if it's a blank char
bool is_blank(char str) {
    return str == ' ';
}

// Verifica se é um caractere de nova linha
bool is_lf(char str) {
    return str == '\n';
}

// Verifica se é um caractere de fim de texto
bool is_etx(char str) {
    return str == '\x03' || str == '\0';
}

// Verifica se é um caractere terminal
bool is_ending_char(char str) {
    return is_etx(str) || is_lf(str) || is_blank(str);
}

// Verifica se é um caractere de letra
bool is_letter_char(char str) {
    return str >= 'a' && str <= 'z';
}

// Avança ao próximo caractere
void next(char **str) {
    (*str)++;
    cursor.column++;
}

// Avança até a próxima token
void advance_token(char **str) {
    while(!is_ending_char(**str)) next(str);
}

// Avança espaços em branco
void advance_blank(char **str) {
    while(is_blank(**str)) next(str);
}

// Verifica se é uma sequência numérica válida
NUM_TYPE is_number(char *str) {
    bool isNumber = false;
    bool hasMinus = *str == '-';
    if(hasMinus) str++;
    while(!is_ending_char(*str)) {
        if(*str < '0' || *str > '9') {
            if(isNumber) return NUM_TYPE_INVALID;
            return NUM_TYPE_NAN;
        }
        else isNumber = true;
        str++;
    }
    return isNumber ? NUM_TYPE_NUM : NUM_TYPE_NAN;
}

// Verifica se é uma sequência de letras válida
bool is_word(char *str) {
    bool isWord = false;
    while(!is_ending_char(*str)) {
        if(*str < 'a' || *str > 'z') return false;
        else isWord = true;
        str++;
    }
    return isWord;
}

// Verifica se é uma variável válida
bool is_var(char *str) {
    return is_letter_char(str[0]) && is_ending_char(str[1]);
}

// Executa a análise léxica no código
void *parse(char *text) {
    // Lê token a token
    while(true) {
        // Pula espaços em branco
        while(is_blank(*text)) next(&text);

        // Checa se token é quebra de linha
        if(*text == '\n') {
            push_token(TOKEN_CODE_LF, 0);
            cursor.line++;
            cursor.column = 1;
            break;
        }
        // Checa se token é fim de arquivo
        else if(*text == '\0'
            || *text == '\x03') {
            push_token(TOKEN_CODE_ETX, 0);
            break;
        }

        // Lê token individualmente
        uint32_t i = 0;
        while(!is_ending_char(text[i])) i++;
        char *currentToken = malloc(sizeof(char) * (i + 1));
        for(uint32_t j = 0; j < i; j++) currentToken[j] = text[j];
        currentToken[i] = '\0';

        // Checa se token é um número
        NUM_TYPE num = is_number(currentToken);
        if(num == NUM_TYPE_NUM) {
            push_token(TOKEN_CODE_NUM, atoi(currentToken));
        } else if (num == NUM_TYPE_INVALID)
            throw_lexical_error(LEXICAL_ERROR_CODE_INVALID_NUMBER);
        
        // Checa se token é variável
        else if(is_var(currentToken)) push_token(TOKEN_CODE_VAR, currentToken[0]);
        // Checa se token é palavra reservada
        else if(is_word(currentToken)) {
            if(!strcmp(currentToken, "rem")) {
                push_token(TOKEN_CODE_REM, 0);
                while(!is_etx(*text) && !is_lf(*text)) next(&text);
            }
            else if (!strcmp(currentToken, "input"))   push_token(TOKEN_CODE_IN, 0);
            else if (!strcmp(currentToken, "let"))     push_token(TOKEN_CODE_LET, 0);
            else if (!strcmp(currentToken, "print"))   push_token(TOKEN_CODE_OUT, 0);
            else if (!strcmp(currentToken, "goto"))    push_token(TOKEN_CODE_GOTO, 0);
            else if (!strcmp(currentToken, "if"))      push_token(TOKEN_CODE_IF, 0);
            else if (!strcmp(currentToken, "end"))     push_token(TOKEN_CODE_END, 0);
            else                                throw_lexical_error(LEXICAL_ERROR_CODE_INVALID_WORD);
        }
        // Checa se token é operador
        else if(!strcmp(currentToken, "="))            push_token(TOKEN_CODE_SET, 0);
        else if(!strcmp(currentToken, "+"))            push_token(TOKEN_CODE_ADD, 0);
        else if(!strcmp(currentToken, "-"))            push_token(TOKEN_CODE_SUB, 0);
        else if(!strcmp(currentToken, "*"))            push_token(TOKEN_CODE_MUL, 0);
        else if(!strcmp(currentToken, "/"))            push_token(TOKEN_CODE_DIV, 0);
        else if(!strcmp(currentToken, "%"))            push_token(TOKEN_CODE_MOD, 0);
        else if(!strcmp(currentToken, "=="))           push_token(TOKEN_CODE_EQU, 0);
        else if(!strcmp(currentToken, "!="))           push_token(TOKEN_CODE_NEQU, 0);
        else if(!strcmp(currentToken, ">"))            push_token(TOKEN_CODE_GT, 0);
        else if(!strcmp(currentToken, "<"))            push_token(TOKEN_CODE_LT, 0);
        else if(!strcmp(currentToken, ">="))           push_token(TOKEN_CODE_GTE, 0);
        else if(!strcmp(currentToken, "<="))           push_token(TOKEN_CODE_LTE, 0);
        else                                    throw_lexical_error(LEXICAL_ERROR_CODE_UNKNOWN);

        // Desaloca token
        free(currentToken);

        // Avança para a próxima token
        while(!is_ending_char(*text)) next(&text);
    }
}