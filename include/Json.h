#pragma once

#include "Dictionary.h"
#include "StringUtils.h"

/* Creates a json text from given dictionary. */
String JsonCreate(Dictionary* dict);

/* Creates a dictionary from given json text. */
Dictionary* JsonParse(String jsonString);
