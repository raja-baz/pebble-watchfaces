#include "num2words.h"
#include "string.h"

static const char* const HOURS[] = {
    "tna3esh",
    "we7de",
    "tenten",
    "tlete",
    "arb3a",
    "khamse",
    "sette",
    "sab3a",
    "tmene",
    "tes3a",
    "3ashra",
    "7da3esh"
};

static const char * const OFFSETS[] = {
    "",
    "khamse",
    "3ashra",
    "rebe3",
    "telet",
    "noss ella khamse",
    "noss",
    "noss w khamse",
    "telet",
    "rebe3",
    "3ashra",
    "khamse"
};

static const char* STR_MINUS = "ella";
static const char* STR_AND = "w";

static size_t append_string(char* buffer, const size_t length, const char* str) {
    strncat(buffer, str, length);

    size_t written = strlen(str);
    return (length > written) ? written : length;
}

void fuzzy_time_to_words(int hours, int minutes, char* words, size_t length) {
    int fuzzy_hours = hours;
    int fuzzy_minutes = ((minutes + 2) / 5) * 5;

    int positive_offset = 1;
    if (fuzzy_minutes > 35){
	positive_offset = 0;
	fuzzy_hours += 1;
    }

    if (fuzzy_minutes > 55){
	fuzzy_minutes = 0;
    }

    fuzzy_hours %= 12;
    fuzzy_minutes /= 5;

    int remaining = length;
    memset(words, 0, length);

    remaining -= append_string(words, remaining, HOURS[fuzzy_hours]);
    remaining -= append_string(words, remaining, " ");
    if (fuzzy_minutes > 0){
	if (positive_offset){
	    remaining -= append_string(words, remaining, STR_AND);
	} else {
	    remaining -= append_string(words, remaining, STR_MINUS);
	}
	remaining -= append_string(words, remaining, " ");
	remaining -= append_string(words, remaining, OFFSETS[fuzzy_minutes]);
    }
}
